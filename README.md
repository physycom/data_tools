---
documentclass: physycomen
title:  "data_tools"
author: "Fabbri, Sinigardi"
---

<a href="http://www.physycom.unibo.it"> 
<div class="image">
<img src="https://cdn.rawgit.com/physycom/templates/697b327d/logo_unibo.png" width="90" height="90" alt="© Physics of Complex Systems Laboratory - Physics and Astronomy Department - University of Bologna"> 
</div>
</a>
<a href="https://travis-ci.org/physycom/data_tools"> 
<div class="image">
<img src="https://travis-ci.org/physycom/data_tools.svg?branch=master" width="90" height="20" alt="Build Status"> 
</div>
</a>
<a href="https://ci.appveyor.com/project/cenit/data-tools"> 
<div class="image">
<img src="https://ci.appveyor.com/api/projects/status/cf9icu8pp70hqwip?svg=true" width="90" height="20" alt="Build Status"> 
</div>
</a>


`data_tools` was created to easily calculate and plot some statistics on inertial and GNSS data, obtained during tests, developed and executed at the Physics of the City Laboratory.  

#### Synopsis
```
average.exe -period <time_interval_to_average[s]> -file filename1 -file filename2 ...
histograms [input_file.txt] OR [conf.json]
simultaneous [plt_files_basename] [input_file_1.txt] [input_file_2.txt] [input_file_3.txt] [...]
```

#### `average`
This tool produces an output file which has the same structure as the input one, but with values in every rows obtained averaging the input over a certain number of lines or over a time span (depending on user choice), in the latter case assuming that the timestamp is reported in the first column of the input file. User can define which columns should be averaged, if not the tool will run on columns #2, #3 and #4
Apart from the averaging procedure, in the output file there are three added columns:
- in the first added column, we can find the value of the cumulative squared average of the analysed columns, subtracted with their own column average;
- in the second added column, there's the number of rows used in averaging procedures (useful to plot the frequency in case the average is done over a time span of 1 second);
- in the third added column, there's the average relative timestamps.

#### `histograms`
This tool produces, as the name suggests, histograms, usually from inertial data. It is useful to extract particular patterns in the input data (an undocumented offset, a frozen device giving often same numbers, etc.). To run the program it is enough to give a data file on the command line, but if the user wants to define minimum and maximum values a .json configuration file like the following can be passed through the command line.

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

#### `simultaneous`
This tool prepares a gnuplot script to superimpose similar data from different files. It is useful, for example, to plot test results from impulse transfer tests, plotting acceleration and rotations on same axes and different devices in the same picture. 

### Installation
**CMake** and a **C++11** compatible compiler are required. To build the executable, clone the repo and then type  
```
mkdir build ; cd build ; cmake .. ; cmake --build . --target install
```
With CMake you can also deploy projects for the most common IDEs.  

`data_tools` contains [jsoncons](https://github.com/danielaparker/jsoncons) as a git submodule.   
`data_tools` uses the [boost libraries](http://www.boost.org/).   


