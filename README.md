[![Build Status Master](https://travis-ci.org/physycom/data_tools.png?branch=master)](https://travis-ci.org/physycom/data_tools "master")


### Installation
**Make** and a **C++11** compatible compiler are required. Clone the repo and type ``make all`` in your favourite shell.

Uses jsoncons library (https://github.com/danielaparker/jsoncons) and the boost C++ libraries (http://www.boost.org/).

json configuration example:
```
{
  "input_file_name"               : "a_acc.txt",
  "output_file_histox"            : "a_acc_histox_bin.txt",
  "output_gnuplot_file_histox"    : "a_acc_histox_bin.plt",
  "output_image_file_histox"      : "a_acc_histox_bin.png",
  "output_file_histoy"            : "a_acc_histoy_bin.txt",
  "output_gnuplot_file_histoy"    : "a_acc_histoy_bin.plt",
  "output_image_file_histoy"      : "a_acc_histoy_bin.png",
  "output_gnuplot_file_histoxy"   : "a_acc_histoxy_bin.plt",
  "output_image_file_histoxy"     : "a_acc_histoxy_bin.png",
  "output_file_cart"              : "a_acc_cart_bin.txt",
  "output_gnuplot_file_cart"      : "a_acc_cart_bin.plt",
  "output_image_file_cart"        : "a_acc_cart_bin.png",
  "output_file_polar"             : "a_acc_polar_bin.txt",
  "output_gnuplot_file_polar"     : "a_acc_polar_bin.plt",
  "output_image_file_polar"       : "a_acc_polar_bin.png",
  "output_gnuplot_file_polar_exp" : "a_acc_polar_bin_exp.plt",
  "output_image_file_polar_exp"   : "a_acc_polar_bin_exp.png",
  "col_acc_x"                     :  2,
  "col_acc_y"                     :  3,
  "nbin_x"                        :  500,
  "nbin_y"                        :  500,
  "nbin_magn"                     :  500,
  "nbin_angle"                    :  62
//  "min_acc_x"                   :  0.0,
//  "max_acc_x"                   :  0.0,
//  "min_acc_y"                   :  0.0,
//  "max_acc_y"                   :  0.0,
//  "min_acc"                     :  0.0,
//  "max_acc"                     :  0.0
 }
```


Brought to you by:

&copy; _Physics of Complex Systems Laboratory - Physics and Astronomy Department - University of Bologna_
