#pragma once

#include <poll.h>
#include <sys/types.h>

#include <condition_variable>
#include <mutex>
#include <thread>

#include "task.h"

class TaskThread : public std::thread {
public:
  TaskThread(const char* szName = "<unnamed task thread>");
  virtual ~TaskThread();
  const char* GetName() const { return m_strName.c_str(); }

  // Start the thread
  virtual void Start(void* pExtra = nullptr);

  // Push tasks for this thread to execute.
  virtual void Push(AbstractTask* pTask);

  // Push lambdas for this thread to execute.
  template <typename FunctionType>
  void PushFunc(FunctionType func) {
    typedef Task<void> TASKTYPE;
    AbstractTask* pTask = new TASKTYPE(func);
    this->Push(pTask);
  }

  // Common Quit for all PrimaryThreads, causes all PrimaryThreads to exit their loop.
  static void SetGlobalQuit(bool bQuit = true);
  static bool GetGlobalQuit();

  // You make your own thread loop outside this class, and call Poll() yourself.
  // e.g. an Application Main Thread can Poll() to execute tasks.
  virtual void Poll(void* pExtra = nullptr);

protected:
  void ThreadProc(void* pExtra);

  // This thread Pops tasks and executes them.
  AbstractTask* Pop();

  // You override this to customize the thread loop.
  virtual void Run(void* pExtra);

  // You override this to implement the thread work.
  virtual void IdlePoll();

  void WaitForTask();
  void WaitForTaskTimeoutMs(int milliseconds);

  TaskThread(const char* szName, bool bKey)
      : m_strName(szName) {
    (void)bKey;
  }

protected:
  std::string m_strName;
  TASKQUEUE m_queue;
  std::mutex m_mutex;
  std::condition_variable m_condition;
};

typedef TaskThread AppMainThread;

class WorkerThread : public TaskThread {
public:
  WorkerThread(const char* szName = "workerthread")
      : TaskThread(szName) {}

protected:
  void IdlePoll() override;
};
