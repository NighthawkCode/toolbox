#include "asyncapp.h"

void AsyncApp::Cleanup() {
  // The base class by default quit all the threads.  If you have
  // more than one instance, you might not want to set the
  // global quit. So just override Cleanup().
  mainthread_.SetGlobalQuit(true);
  loadingthread_.SetGlobalQuit(true);
  loadingthread_.join();
}

// This is called in the application main loop.
void AsyncApp::Update() {
  // This calls task->Execute(),
  // to invoke the completion callback function.
  mainthread_.Poll();
}
