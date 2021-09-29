#pragma once
#include "taskthread.h"

// Add this into an app, for async loading large document data files.
// 1. Application calls Update().
// 2. Application passes this to other components which use the threads.

class AsyncApp {
public:
  AsyncApp()
      : loadingthread_("file loading") {
    loadingthread_.Start();
  }
  virtual ~AsyncApp() { Cleanup(); }

  // Derived class could override.
  virtual void Cleanup();

  // Application call this in the main loop.
  virtual void Update();

  AppMainThread& GetMainThread() { return mainthread_; }
  WorkerThread& GetLoadingThread() { return loadingthread_; }

protected:
  AppMainThread mainthread_;
  WorkerThread loadingthread_;  // push tasks to worker thread, see examples.
};
