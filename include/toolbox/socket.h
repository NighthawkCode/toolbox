#pragma once
#include <inttypes.h>

#include <string>

class Socket {
public:
  Socket();
  ~Socket();

  enum State {
    S_UNINITIALIZED,
    S_LISTENING,
    S_CONNECTING,
    S_CONNECTED_IDLE,
  };
  State GetState() { return state_; }

  bool StartServer(int port);
  bool PollServer();

  bool Connect(const std::string& server, int port);
  void Close();

  uint32_t Read(char* buf, uint32_t bufsize, int timeoutMilliseconds = 0);
  uint32_t Write(const char* buf, uint32_t bufsize);

private:
  int port_ = 1337;
  std::string server_;
  int sock_ = 0;
  int socklisten_ = 0;
  State state_ = S_UNINITIALIZED;
};
