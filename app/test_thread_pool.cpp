#include "ra/thread_pool.hpp"

//test constructors and any getters.
void test_p1() {
  
  ra::concurrency::thread_pool tp0;

  for (auto i=0; i < 256; ++i) {
    tp0.schedule([]() {
      std::mutex mu;
      mu.lock();
      int x=0;
      //std::cout << x << std::endl;
      mu.unlock();
    });
  }


  ra::concurrency::thread_pool tp1;
  ra::concurrency::thread_pool tp2(4);
  ra::concurrency::thread_pool tp3(3);
  ra::concurrency::thread_pool tp4(16);
  //no way to test when std:;thread::hardware_concurrency returns 0.
  /*
  std::cout << "sizes" << std::endl;
  std::cout << "tp0.size: " << tp0.size() << std::endl;
  std::cout << "tp1.size: " << tp1.size() << std::endl;
  std::cout << "tp2.size: " << tp2.size() << std::endl;
  std::cout << "tp3.size: " << tp3.size() << std::endl;
  std::cout << "tp4.size: " << tp4.size() << std::endl;
  */
  tp2.shutdown();
  tp3.shutdown();
  /*
  std::cout << "is_shutdown, none are atm." << std::endl;
  std::cout << "tp0.is_shutdown: " << tp0.is_shutdown() << std::endl;
  std::cout << "tp1.is_shutdown: " << tp1.is_shutdown() << std::endl;
  std::cout << "tp2.is_shutdown: " << tp2.is_shutdown() << std::endl;
  std::cout << "tp3.is_shutdown: " << tp3.is_shutdown() << std::endl;
  std::cout << "tp4.is_shutdown: " << tp4.is_shutdown() << std::endl;
  */
  
  tp0.shutdown(); //size 12 on threads, taskqueue has none.
}

int main() {
  test_p1();
  return 0;
}
