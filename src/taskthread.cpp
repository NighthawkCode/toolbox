
#include "taskthread.h"

#include <pthread.h>  // POSIX threads

static std::atomic_bool _bTaskThreadGlobalQuit(false);

// Common Quit for all TaskThreads, causes all TaskThreads to exit their loop.
void TaskThread::SetGlobalQuit(bool bQuit) { _bTaskThreadGlobalQuit = bQuit; }
bool TaskThread::GetGlobalQuit() { return _bTaskThreadGlobalQuit; }

TaskThread::TaskThread(const char* szName)
    : m_strName(szName) {}

TaskThread::~TaskThread() {
  // TRACE( "TaskThread::~TaskThread()   %s \n", m_strName.empty()? "<unnamed task thread>" :
  // m_strName.c_str() );
}

// To override virtual Run(), construction does nothing, and you need a separate Start() function.
void TaskThread::Start(void* pExtra) {
  // TRACE( "TaskThread::Start()  %s \n", m_strName.empty()? "<unnamed thread>" : m_strName.c_str() );
  std::thread& t = *this;
  t = std::thread(&TaskThread::ThreadProc, this, pExtra);
  pthread_setname_np(t.native_handle(), m_strName.c_str());
}

void TaskThread::ThreadProc(void* pExtra) { this->Run(pExtra); }

void TaskThread::Run(void* pExtra) {
  // std::thread::id  thread_id = std::this_thread::get_id();
  // TRACE( "TaskThread::Run() ENTER  thread  %lu  is %s \n", THID(thread_id), m_strName.c_str()  );
  do {
    Poll(pExtra);
    if (GetGlobalQuit()) break;  //  quit out of thread loop.
  } while (true);
  // TRACE( "TaskThread::Run() EXIT  thread %lu  %s \n", THID(thread_id), m_strName.c_str()  );
}

// You make your own thread loop outside this class, and call Poll() yourself.
// e.g. an Application Main Thread can Poll() to execute tasks.
void TaskThread::Poll(void* pExtra) {
  AbstractTask* pTask = this->Pop();
  while (pTask && !GetGlobalQuit()) {
    pTask->Execute();  // Get the result in this thread
    pTask = this->Pop();
  }

  this->IdlePoll();
}

void TaskThread::IdlePoll() {}

// Push tasks for this thread to execute.
void TaskThread::Push(AbstractTask* pTask) {
  std::lock_guard<std::mutex> lock(m_mutex);  // lock the scope
  m_queue.push_back(pTask);
  m_condition.notify_one();
}

// This thread Pops tasks and executes them.
AbstractTask* TaskThread::Pop() {
  AbstractTask* pTask = NULL;
  std::lock_guard<std::mutex> lock(m_mutex);  // lock the scope
  if (m_queue.size() > 0) {
    pTask = m_queue.front();
    m_queue.pop_front();
  }
  return pTask;
}

void TaskThread::WaitForTask() {
  std::unique_lock<std::mutex> lock(m_mutex);  // lock the scope
  if (m_queue.size() == 0) {
    m_condition.wait(lock);
  }
}

void TaskThread::WaitForTaskTimeoutMs(int milliseconds) {
  std::unique_lock<std::mutex> lock(m_mutex);  // lock the scope
  if (m_queue.size() == 0) {
    m_condition.wait_for(lock, std::chrono::milliseconds(milliseconds));
  }
}

void WorkerThread::IdlePoll() { WaitForTaskTimeoutMs(100); }
