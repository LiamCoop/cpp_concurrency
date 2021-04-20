#include "ra/julia_set.hpp"
#include <boost/multi_array.hpp>
#include <iostream>
#include <fstream>
#include <complex>
#include <typeinfo>
#include <iomanip>

void test_julia_set() {
  using array2 = boost::multi_array<int, 2>;

  //ra::fractal::compute_julia_set<float> ();
  std::complex<double> bottom_left(-1.25, -1.25), top_right(1.25, 1.25), c(0.37, -0.16);
  int max_iters = 1023;
  array2 a(boost::extents[512][512]);
  ra::fractal::compute_julia_set<double>(bottom_left, top_right, c, max_iters, a, 1);
  ra::fractal::compute_julia_set<double>(bottom_left, top_right, c, max_iters, a, 2);
  ra::fractal::compute_julia_set<double>(bottom_left, top_right, c, max_iters, a, 4);
  ra::fractal::compute_julia_set<double>(bottom_left, top_right, c, max_iters, a, 8);

  std::ofstream julia;
  julia.open("julia.pnm");
  julia << "P2 " << a.shape()[1] << " " << a.shape()[0] << " " << 255 << std::endl;
  for (auto i=0; i < a.shape()]0]; ++i) {
    for(auto j=0; j < a.shape()[1]; ++j){
      julia << a[i][j] << " ";
    }
    julia << std::endl;
  }
  julia << std::endl;
  julia.close();

}

int main() {
  test_julia_set();
  return 0;
}
