#ifndef QUEUE_HPP
#define QUEUE_HPP
#include <mutex>
#include <queue>
#include <cassert>
#include <condition_variable> 
#include <memory>
#include <thread>

namespace ra {
  namespace concurrency {
    //Concurrent bounded FIFO queue class
    template <class T> 
    class queue { 
    private:
      std::queue<T> q_;
      std::size_t max_size_;
      std::mutex m_; 
      std::condition_variable e_;//for empty 
      std::condition_variable f_;//for full 
      bool isClosed = false;
    
    public:

      //The type of each of the elements stored in the queue. using value_type = T;
      using value_type = T;

      //An unsigned integral type used to represent sizes.
      using size_type = std::size_t;
      //A type for the status of a queue operation. 
      enum class status {
        //operation successful
        success = 0,
        //queue is empty (not currently used)
        empty,
        // queue is full (not currently used)
        full,
        // queue is closed
        closed,
      };
      
      //A queue is not default constructible.
      queue() = delete;

      // Constructs a queue with a maximum size of max_size.
      // The queue is marked as open (i.e., not closed).
      // Precondition: The quantity max_size must be greater than zero 
      queue(size_type max_size)
        : q_(), max_size_(max_size)
      { assert(max_size > 0); }
      //
      // A queue is not movable or copyable.
      queue(const queue&) = delete;
      queue& operator=(const queue&) = delete; 
      queue(queue&&) = delete;
      queue& operator=(queue&&) = delete;

      // Destroys the queue after closing the queue (if not already closed)
      // and clearing the queue (if not already empty)
      ~queue() {
        close();
        clear();
      }

      // Inserts the value x at the end of the queue, blocking if necessary
      // If the queue is full, the thread will be blocked until the queue insertion
      // can be completed or the queue is closed.
      // If the value x is successfully inserted on the queue, the function 
      // returns status::success;
      // If the value x cannot be inserted on the queue (due to the queue being closed)
      // The function returns with a return value of status::closed.
      // This function is thread safe.
      // Note: the rvalue reference parameter is intentional and implies that
      // the push function is permitted to change the value of x (e.g. by moving from x).
      status push(value_type&& x) {
        //if closed, return status::closed.
        if(is_closed()) { return status::closed; }
        std::unique_lock<std::mutex> lk(m_);
        // wait for an opportunity when not full.
        f_.wait(lk, [this]() { return !is_full() || is_closed(); });
        //if closed after wa iting for a spot, return status::closed.
        if (is_closed()) { return status::closed; } 
        q_.push(std::move(x)); //push
        e_.notify_one();//definitely no longer empty, should notify. return status::success
        //if push was successful, return status::success;
        return status::success;
      }
      
      void hardpop(value_type& x) { 
        x = q_.front();
        q_.pop(); 
      }
      // Removes the value from the front of the queue and places it in x, blocking if necessary.
      // // If the queue is empty and not closed, the thread is blocked until
      // 1) A value can be removed from the queue OR
      // 2) The queue is closed.
      // If the queue is closed, the function does not block and either turns status::closed or
      // status::success, depending on whether a value can be successfully removed from the queue.
            // If a value is successfully removed from the queue, the value is p laced in x and
            // the function returns status::success.
            // If a value cannot be successfully removed from the queue (due to the queue being both
      // empty and closed), the function returns status::closed. // This function is thread safe.
      status pop(value_type& x) {
        if (is_empty() && is_closed()) { return status::closed; } //noth ing we can pop, return status::closed.
        std::unique_lock<std::mutex> lk(m_);
        e_.wait(lk, [this]() { return !is_empty() || is_closed(); });//b lock & wait for it to be not empty.
        //if closed at this point, try and pop, return either status::su ccess (if successful) or status::closed otherwise.
        hardpop(x);
        f_.notify_one(); //definitely no longer full, should notify. return status::success;
        return status::success;
      }

      // Closes the queue.
      // The queue is placed in the closed state.
      // The closed state prevents more items from being inserted on the q ueue, but it does
      // not clear the items that are already on the queue.
      // Invoking this function on a closed queue has no effect. // This function is thread safe.
      void close() {
        std::unique_lock<std::mutex> lk(m_); isClosed = true;
        f_.notify_all();
        e_.notify_all();
      }

      // Clears the queue.
      // All of the elements on the queue are discarded. // This function is thread safe.
      void clear() {
        std::unique_lock<std::mutex> lk(m_); 
        while(q_.size() != 0){
          T tmp;
          hardpop(tmp);
        }
        f_.notify_all();
      }

      // Returns if the queue is currently full (i.e., the number of eleme
      // in the queue equals the maximum queue size).
      // This function is not thread safe.
      bool is_full() const { return q_.size() == max_size_; }

      // Returns if the queue is currently empty.
      // This function is not thread safe.
      bool is_empty() const { return q_.size() == 0; }

      // Returns if the queue is closed (i.e., in the closed state). // This function is not thread safe.
      bool is_closed() const { return isClosed; }
      
      // Returns the maximum number of elements that can be held in the qu
      // This function is not thread safe.
      size_type max_size() const { return max_size_; } 
      size_type size() const { return q_.size(); }

    };
  }
}
#endif
