#include <complex>
#include <chrono>
#include <algorithm>
#include <boost/multi_array.hpp>
#include "thread_pool.hpp"


namespace ra {
  namespace fractal {

    template <class Real>//real can be float, double, or long double
    void compute_julia_set(
        const std::complex<Real>& bottom_left, 
        const std::complex<Real>& top_right, 
        const std::complex<Real>& c, 
        int max_iters, 
        boost::multi_array<int, 2>& a, 
        int num_threads
      ) {

      //std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

      const Real u0 = bottom_left.real(), 
                 u1 = bottom_left.imag(), 
                 v0 = top_right.real(),
                 v1 = top_right.imag();
      const int H = a.shape()[0], W = a.shape()[1];

      ra::concurrency::thread_pool tp(num_threads);

      for(int l = 0; l < H; ++l) {
        tp.schedule([=, &a]() {
          for(int k=0; k < W; ++k) {
            std::complex<Real> zn(
              u0 + (static_cast<Real>(k) / static_cast<Real>(W - 1)) * (v0 - u0),
              u1 + (static_cast<Real>(l) / static_cast<Real>(H - 1)) * (v1 - u1)
            );
            int i;
            for(i=0; i < max_iters; ++i) {
              if (std::abs(zn) > 2) { break; }
              zn = zn * zn + c;
            }
            a[l][k] = i;

          }
        });

      }

      // std::chrono::time_point<std::chrono::system_clock> end=std::system_clock::now();
      // std::chrono::duration<double> elapsed = end - start;
      //std::cout << "That took: " << elapsed.count() << "s, given " << m_threads << "threads" << std::endl;

    }
        

  }
}
