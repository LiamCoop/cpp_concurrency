#include "ra/queue.hpp"
#include <iostream>
#include <thread>
#include <functional>

void CASE_JamLambda() {
  // a queue to fill.
  ra::concurrency::queue<std::function<void()>>* q1 = new ra::concurrency::queue<std::function<void()>>(16); 

  for (auto i = 0; i < 16; ++i) {
    std::cout << i << std::endl;
    q1->push([]() {
      std::cout << "Hello" << std::endl;
    });
    std::function<void()> func;
    q1->pop(func);
    func();
  }
}

void CASE_pushclosepush() {
  // a queue to fill.
  ra::concurrency::queue<int> *q1 = new ra::concurrency::queue<int>(2);
  std::cout << "max_size: " << q1->max_size() << std::endl;
  q1->push(1);
  q1->push(2);
  std::thread tp1(&ra::concurrency::queue<int>::push, q1, 3);
  q1->close();
  tp1.join();

  std::cout << "isclosed? " << q1->is_closed() << std::endl;
  std::cout << "size: " << q1->size() << std::endl;

}

void CASE_popclosepop() {
  // a queue to fill
  ra::concurrency::queue<int>* q1 = new ra::concurrency::queue<int>(2);
  std::cout << "max_size: " << q1->max_size() << std::endl;

  int b = 2;
  q1->push(2);
  q1->close();
  q1->pop(std::ref(b));
  std::cout << "b=2: " << b << std::endl;
  std::cout << "q1->is_empty(): " << q1->is_empty() << std::endl;
}

void CASE_pushclearpush() {
  // a queue to fill
  ra::concurrency::queue<int>* q1 = new ra::concurrency::queue<int>(2);
  std::cout << "max_size: " << q1->max_size() << std::endl;

  q1->push(2);
  q1->push(2);
  std::thread tp1(&ra::concurrency::queue<int>::push, q1, 2);
  std::thread tp2(&ra::concurrency::queue<int>::clear, q1);
  tp2.join();
  tp1.join();

  std::cout << "q1->size(): " << q1->size() << std::endl;

  // all get pushed, and then closed. Nice.
}

void CASE_pushfull_pop() {
  //a queue to fill
  ra::concurrency::queue<int>* q1 = new ra::concurrency::queue<int>(2);

  int x=0;
  // push to empty, then pop should occur
  std::thread tp1(&ra::concurrency::queue<int>::push, q1, x);
  std::thread tp2(&ra::concurrency::queue<int>::push, q1, x);
  tp1.join();
  tp2.join();

  int x2 = 1, x3;
  // push to empty, then pop should occur
  std::thread tp3(&ra::concurrency::queue<int>::push, q1, x2);
  //pop on empty
  std::thread tp4(&ra::concurrency::queue<int>::pop, q1, std::ref(x));

  tp3.join();
  tp4.join();
}

void CASE_popempty_push() {
  // a full queue
  ra::concurrency::queue<int>* q1 = new ra::concurrency::queue<int>(2);

  int x, x2=4;
  std::cout << q1->size() << std::endl;
  //pop on empty
  std::thread tp1(&ra::concurrency::queue<int>::pop, q1, std::ref(x));
  //push to empty, then pop should occur
  std::thread tp2(&ra::concurrency::queue<int>::push, q1, x2);
  tp2.join();
  tp1.join();
  std::cout << "x: " << x << std::endl;
  
}

void CASE_clear() {
  // a queue to fill
  ra::concurrency::queue<int>* q1 = new ra::concurrency::queue<int>(2);

  std::cout << "max_size: " << q1->max_size() << std::endl;

  int b=2;
  std::thread tp1(&ra::concurrency::queue<int>::push, q1, b);
  std::thread tp2(&ra::concurrency::queue<int>::push, q1, b);
  tp1.join();
  tp2.join();
  std::cout << q1->size() << std::endl;
  q1->clear();
  std::cout << q1->size() << std::endl;
  std::cout << "is_empty?: " << q1->is_empty() << std::endl;
}

void CASE_close() {
  // a queue to fill
  ra::concurrency::queue<int>* q1 = new ra::concurrency::queue<int>(2);
  q1->close();
  std::cout << q1->is_closed() << std::endl;
}

void test_queue_getters() {
  //a full queue. std::cout << "empty: " << std::endl;
  ra::concurrency::queue<int> q1(2);
  std::cout << "q1.is_full(): " << q1.is_full() << std::endl; std::cout << "q1.is_empty(): " << q1.is_empty() << std::endl; q1.push(2);
  std::cout << "!empty, !full: " << std::endl;
  std::cout << "q1.is_full(): " << q1.is_full() << std::endl; std::cout << "q1.is_empty(): " << q1.is_empty() << std::endl; q1.push(2);
  //q1.push(2);//hangs in single−threaded.
  std::cout << "full: " << std::endl;
  std::cout << "q1.is_full(): " << q1.is_full() << std::endl; int x;
  q1.pop(x);
  std::cout << "!empty, !full: " << std::endl;
  std::cout << "q1.is_full(): " << q1.is_full() << std::endl; std::cout << "q1.is_empty(): " << q1.is_empty() << std::endl; q1.pop(x);
  std::cout << "empty: " << std::endl;
  std::cout << "q1.is_full(): " << q1.is_full() << std::endl; std::cout << "q1.is_empty(): " << q1.is_empty() << std::endl;

}

int main() {
  test_queue_getters(); 
  CASE_popempty_push(); 
  CASE_pushfull_pop(); 
  CASE_clear(); 
  CASE_close(); 
  CASE_pushclearpush(); 
  CASE_pushclosepush(); 
  CASE_popclosepop(); 
  CASE_JamLambda(); 
  return 0;
}

