#include "socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <resolv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Set options on socket.
int SocketSetNonBlocking(int sockfd, bool tcpnodelay /* = true  */) {
  int result = 0;
  int yes = 1;
  // Set NON-blocking socket.  connect() listen() accept() send() and recv() should not block execution.
  if (0 > fcntl(sockfd, F_SETFL, O_NONBLOCK)) {
    printf("SocketSetNonBlocking() could not set non-blocking \n");
    return -1;
  }
  if (tcpnodelay) {
    // Disable the Nagle.
    if (0 > setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(int))) {
      printf("SocketClient::Connect() could not set TCP_NODELAY option \n");
    }
  }
  return result;
}

// Initialize socketaddr structure.  port must be native byte order.
void InitSocketAddr(sockaddr_in* socketaddr, const char* hostname, const char* serveraddress,
                    unsigned short port) {
  socketaddr->sin_family = AF_INET;
  socketaddr->sin_port = htons(port);
  socketaddr->sin_addr.s_addr = 0;
  if (hostname != nullptr && hostname[0] != 0) {
    hostent* hostinfo = gethostbyname(hostname);  // "www.google.com"
    if (hostinfo == nullptr) {
      printf("InitSocketAddr() Unknown host %s.\n", hostname);
    } else {
      socketaddr->sin_addr = *((in_addr*)hostinfo->h_addr);
    }
  }
  if (socketaddr->sin_addr.s_addr == 0 && serveraddress != NULL && serveraddress[0] != 0) {
    socketaddr->sin_addr.s_addr = inet_addr(serveraddress);  //  "192.168.1.111"
  }
}

Socket::Socket() {}

Socket::~Socket() { Close(); }

bool Socket::StartServer(int port) {
  port_ = port;
  socklisten_ = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (0 > SocketSetNonBlocking(socklisten_, true)) {
    printf("Could not set non blocking \n");
  }
  sockaddr_in server_info;
  memset(&server_info, 0, sizeof(sockaddr_in));
  server_info.sin_family = AF_INET;
  server_info.sin_addr.s_addr = INADDR_ANY;
  server_info.sin_port = htons(port_);
  if (bind(socklisten_, (struct sockaddr*)&server_info, sizeof(server_info)) < 0) {
    printf("Could not bind using port %d\n", port_);
    return false;
  }
  if (listen(socklisten_, 1) < 0) {
    printf("listen failed.  errno=%d\n", errno);
    return false;
  }
  state_ = S_LISTENING;
  return true;
}

bool Socket::PollServer() {
  sockaddr_storage theiraddress;
  memset(&theiraddress, 0, sizeof(sockaddr_storage));
  socklen_t sinsize = sizeof(theiraddress);
  int s = accept(socklisten_, (struct sockaddr*)&theiraddress, (socklen_t*)&sinsize);
  if (s <= 0) {
    if (errno == EWOULDBLOCK || errno == EINPROGRESS) {
      return false;  // this is 'ok', since its a non-blocking socket
    } else {
      printf("accept failed.  errno=%d\n", errno);
    }
    return false;
  } else if (s > 0) {
    char buf[1024];
    strcpy(buf, inet_ntoa(((sockaddr_in*)&theiraddress)->sin_addr));
    printf("accepted connection from %s\n", buf);
    if (0 > SocketSetNonBlocking(s, true)) {
      printf("Could not set non blocking \n");
    }
    sock_ = s;
    state_ = S_CONNECTED_IDLE;
    return true;
  }

  return false;
}

bool Socket::Connect(const std::string& server, int port) {
  port_ = port;
  server_ = server;

  sock_ = ::socket(PF_INET, SOCK_STREAM, 0);
  if (sock_ < 0) {
    printf("\n Socket creation error \n");
    return false;
  }
  if (0 > SocketSetNonBlocking(sock_, true)) {
    printf("Could not set non blocking \n");
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(sockaddr_in));

  if (0 == strcmp(server_.c_str(), "localhost")) {
    InitSocketAddr(&serv_addr, server_.c_str(), nullptr, port);
  } else {
    InitSocketAddr(&serv_addr, nullptr, server_.c_str(), port);
  }

  if (::connect(sock_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    int err = errno;
    if (err == EINPROGRESS) {
      // non-blocking connection in progress
      state_ = S_CONNECTING;
    } else {
      printf("\nConnection Failed \n");
      return false;
    }
  }
  return true;
}

void Socket::Close() {
  if (sock_ != 0) {
    ::close(sock_);
  }
  if (socklisten_ != 0) {
    ::close(socklisten_);
  }
  state_ = S_UNINITIALIZED;
}

uint32_t Socket::Read(char* buf, uint32_t bufsize, int timeoutMilliseconds) {
  if (state_ == S_UNINITIALIZED || state_ == S_LISTENING) {
    return 0;
  } else if (state_ == S_CONNECTING) {
    int pollresult = 0;
    pollfd pfd;
    pfd.fd = sock_;
    pfd.events = POLLIN | POLLOUT;
    pfd.revents = 0;
    pollresult = poll(&pfd, 1, timeoutMilliseconds);
    if (pollresult == -1) {
      // not connected yet
      return 0;
    } else if (pollresult == 0) {
      // timeout
    } else {
      if (((pfd.revents & POLLOUT) != POLLOUT)) {
        //printf("Remote end hung up or connection failed\n");
        close(sock_);
        sock_ = 0;
        return 0;
      }
      state_ = S_CONNECTED_IDLE;
      ssize_t bytes = 0;
      if (pfd.revents & POLLIN) {
        bytes = ::recv(sock_, buf, bufsize, 0);
        if (bytes <= 0 && (errno == EWOULDBLOCK || errno == EINPROGRESS)) {
          return 0;  // this is 'ok', since its a non-blocking socket
        } else {
          if (bytes < 0) {
            //printf("recv() error  %zd  %d\n", bytes, errno);
          }
          if (bytes >= 0) {
            return (uint32_t)bytes;
          }
        }
      }
    }
  } else if (state_ == S_CONNECTED_IDLE) {
    ssize_t bytes = 0;
    bytes = ::recv(sock_, buf, bufsize, 0);
    if (bytes <= 0 && errno == EWOULDBLOCK) {  // || errno == EINPROGRESS
      return 0;                                // this is 'ok', since its a non-blocking socket
    } else {
      if (bytes < 0) {
        //printf("recv() error  %zd  %d\n", bytes, errno);
      } else if (bytes >= 0) {
        return (uint32_t)bytes;
      }
    }
  }
  return 0;
}

uint32_t Socket::Write(const char* buf, uint32_t bufsize) {
  if (state_ == S_UNINITIALIZED || state_ == S_LISTENING) {
    return 0;
  }
  ssize_t size = 0;
  size = ::send(sock_, buf, bufsize, MSG_DONTWAIT);
  if (size > 0) {
    return (uint32_t) size;
  }
  return 0;
}
