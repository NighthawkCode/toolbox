#pragma once
#include <deque>
#include <functional>
#include <future>
#include <string>

struct AbstractTask {
  virtual ~AbstractTask();
  virtual void Execute() = 0;
};
typedef std::deque<AbstractTask*> TASKQUEUE;

// Async Task with a result, but no promise and future.
// The Task can be re-queued back to the requesting thread in order to transport the result.
template <typename ResultType>
struct Task : public AbstractTask {
  typedef Task<ResultType> TASKTYPE;
  typedef std::function<void(TASKTYPE*)> TASKEXEC;

  explicit Task() {}
  template <typename FunctionType>
  Task(FunctionType execfn)
      : execution(execfn) {}
  ~Task() override {}

  void Execute() override {
    TASKEXEC temp = execution;
    temp(this);  // <-- This lambda call can change the execution function.  It can provide another lambda
                 // function, and re-queue this task to the calling thread.
  }

  ResultType GetResult() { return result; }

  TASKEXEC execution;
  ResultType result;
};

// Async Task template specialization has no result.
template <>
struct Task<void> : public AbstractTask {
  typedef Task<void> TASKTYPE;
  typedef std::function<void(TASKTYPE*)> TASKEXEC;
  explicit Task() {}
  template <typename FunctionType>
  Task(FunctionType execfn)
      : execution(execfn) {}
  ~Task() override;

  void Execute() override {
    TASKEXEC temp = execution;
    temp(this);
  }

  TASKEXEC execution;
};

// Task with a promise and a future.  The requeusting thread polls the result.
// What to name it: FuturisticTask?, PromiscuousTask?
template <typename ResultType>
struct WaitableTask : public AbstractTask {
  typedef WaitableTask<ResultType> TASKTYPE;
  typedef std::function<ResultType(TASKTYPE*)> TASKEXEC;
  typedef std::promise<ResultType> PROMISETYPE;
  typedef std::future<ResultType> FUTURETYPE;

  explicit WaitableTask() {}
  template <typename FunctionType>
  WaitableTask(FunctionType execfn)
      : execution(execfn) {}
  ~WaitableTask() override {}

  FUTURETYPE GetFuture() { return promise.get_future(); }

  void Execute() override {
    TASKEXEC temp = execution;
    ResultType result = temp(this);
    promise.set_value(result);
  }

  TASKEXEC execution;
  PROMISETYPE promise;
};

// Async Task template specialization has no result.
struct StringTask : public AbstractTask {
  typedef StringTask TASKTYPE;
  typedef std::function<void(TASKTYPE*)> TASKEXEC;

  template <typename FunctionType>
  StringTask(const char* szValue, FunctionType execfn)
      : execution(execfn)
      , strValue(szValue) {}
  ~StringTask() override;

  void Execute() override {
    TASKEXEC temp = execution;
    temp(this);
  }

  TASKEXEC execution;
  std::string strValue;
};
