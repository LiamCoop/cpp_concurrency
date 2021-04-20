#ifndef THREAD_POOL_CPP
#define THREAD_POOL_CPP
#include "ra/thread_pool.hpp"

namespace ra {
  namespace concurrency {

  thread_pool::thread_pool(): threadvec_(), taskq_(default_taskqSize),
    running(n_), n_(std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 2)
    {
      // cycle n_times (create n_ threads).
      for (auto i=0; i < n_; ++i) {
        threadvec_.push_back(std::thread([this] () {
          //create thread with an infinite job search function.
          std::unique_lock<std::mutex> lk(m_, std::defer_lock);
          for(;;) {
            lk.lock();
            //wake when the task queue has something, or we are shutting down.
            e_.wait(lk, [this] { return !taskq_.is_empty() || is_shutdown(); });

            //we're shutdown and no more functions to work on.
            if(is_shutdown() && taskq_.is_empty()) { break; }
            //checks done, pop & execute
            //arbitrary function instantiation.
            std::function<void()> fn;

            taskq_.pop(fn);// grab function from queue
            f_.notify_one();//grab task, tell scheduler that queue has room

            lk.unlock(); // remove lock
            fn();// execute the function (that we just grabbed)
          }
          --running;// one thread signed off.
          if(!running) { i_.notify_one(); }//all threads are done

        }));

      }
    }

    thread_pool::thread_pool(std::size_t num_threads) 
      : n_(num_threads), threadvec_(), taskq_(default_taskqSize), running(n_)
    {
      assert(num_threads > 0);
      //cycle n_ times (create n_ threads)
      for(auto i = 0; i < n_; ++i) {
        //create infinite job search function
        threadvec_.push_back(std::thread([this]() {
          std::unique_lock<std::mutex> lk(m_, std::defer_lock); 
          // infinite loop
          for(;;) {
            lk.lock();
            ///wake when the task queue has something, or we are shutting down.

            e_.wait(lk, [this] { return !taskq_.is_empty() || is_shutdown(); });

            //we're shutdown & no more funcs to work on.
            if(is_shutdown() && taskq_.is_empty()) { break; }
            //checks done, pop & execute
            std::function<void()> fn; // arbitrary function instantiation

            taskq_.pop(fn); // grab func from taskq.
            f_.notify_one(); //tell scheduler

            lk.unlock(); //remove lock
            fn(); // execute function
  
          }
          --running; //one thread signed off
          if (!running) { i_.notify_one(); }//all threads done


        }));

      }
    }

    thread_pool::~thread_pool()
    {
      shutdown();
      std::unique_lock<std::mutex> lk(m_);
      threadvec_.clear();
    }

    void thread_pool::schedule(std::function<void()>&& func) {
      assert(!is_shutdown());
      std::unique_lock<std::mutex> lk(m_);
      //block if task queue is full or closed
      f_.wait(lk, [this]() { return !taskq_.is_full()/* || taskq_.is_closed()*/; }); 
      //if we get past the wait & we're shutdown, no new tasks can be added to queue.
      if(taskq_.is_closed()) { return; }
      else {
        taskq_.push(std::move(func));// move func into the taskq
        e_.notify_one();//taskq is definitely not empty, notify
      }
    }

    void thread_pool::shutdown() {
      if(is_shutdown()) { return; }
      std::unique_lock<std::mutex> lk(m_);
      taskq_.close(); //close queue
      isShutdown = true; //make is_shutdown true
      e_.notify_all(); // all threads should know about this change
      f_.notify_all(); // all threads should know about this change
      i_.wait(lk, [this] { return (!running); }); // return if (number of threads) running is zero

      for(std::thread &t: threadvec_) { t.join(); }
    }

  }
}
#endif

