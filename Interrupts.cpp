#include "Helper.hpp"
#include <thread>
#include <mutex>

class InterruptHandle{
  bool switch;
  bool trigger;
  std::mutex mtx;
  std::thread& thread;
 public:
  InterruptHandle(std::thread& src):thread(src),mtx(),trigger(false),switch(false){}
};

void waitForInterrupt
