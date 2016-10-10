/* Copyright 2016 - Stefano Sinigardi, Alessandro Fabbri */

/***************************************************************************
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "jsoncons/json.hpp"

#define MAJOR_VERSION 2
#define MINOR_VERSION 9

#define XRES     1280
#define YRES     720
#define FONTSIZE 10

void prepare_gnuplot_script_1D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t min_bin_col, size_t max_bin_col, size_t data_col, std::string data_key, std::string title_key) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN='" << data_file << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo dashed size " << Xres << ',' << Yres << " enhanced font 'Verdana," << fontsize << "'\n";
  output_file << "set output FILE_OUT\n";
  output_file << "# Styles\n";
  output_file << "linew = 1.2\n";
  output_file << "set style line  21 lc rgb '#0072bd' lt 7 lw linew  # blue\n";
  output_file << "set style line  22 lc rgb '#d95319' lt 7 lw linew  # orange\n";
  output_file << "set style line  23 lc rgb '#77ac30' lt 7 lw linew  # green\n";
  output_file << "set style line  24 lc rgb '#a2142f' lt 7 lw linew  # red\n";
  output_file << "set style line 102 lc rgb '#d6d7d9' lt 1 lw 1      # gray\n";
  output_file << "# Grid\n";
  output_file << "set grid xtics ytics back ls 102\n";
  output_file << "# Titles\n";
  output_file << "set key opaque\n";
  output_file << "set title 'Power Spectrum: )" << title_key << "\n";
  output_file << "set xlabel 'a (g)' \n";
  output_file << "set ylabel 'dN/da'\n";
  output_file << "plot FILE_IN u ($" << min_bin_col << "+$" << max_bin_col << ")/2:" << data_col << " w histeps lt 1 lc rgb 'red' lw 3 t '" << data_key << "'\n";
  output_file << "\n";
}

void prepare_gnuplot_script_double_1D(std::ofstream &output_file, std::string data_file_1, std::string data_file_2, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t min_bin_col, size_t max_bin_col, size_t data_col, std::string data_key_1, std::string data_key_2, std::string title_key) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN_1='" << data_file_1 << "'\n";
  output_file << "FILE_IN_2='" << data_file_2 << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo dashed size " << Xres << ',' << Yres << " enhanced font 'Verdana," << fontsize << "'\n";
  output_file << "set output FILE_OUT\n";
  output_file << "# Styles\n";
  output_file << "linew = 1.2\n";
  output_file << "set style line  21 lc rgb '#0072bd' lt 7 lw linew  # blue\n";
  output_file << "set style line  22 lc rgb '#d95319' lt 7 lw linew  # orange\n";
  output_file << "set style line  23 lc rgb '#77ac30' lt 7 lw linew  # green\n";
  output_file << "set style line  24 lc rgb '#a2142f' lt 7 lw linew  # red\n";
  output_file << "set style line 102 lc rgb '#d6d7d9' lt 1 lw 1      # gray\n";
  output_file << "# Grid\n";
  output_file << "set grid xtics ytics back ls 102\n";
  output_file << "# Titles\n";
  output_file << "set key opaque\n";
  output_file << "set title 'Power Spectrum: )" << title_key << "\n";
  output_file << "set xlabel 'a (g)' \n";
  output_file << "set ylabel 'dN/da'\n";
  output_file << "plot FILE_IN_1 u ($" << min_bin_col << "+$" << max_bin_col << ")/2:" << data_col << " w histeps lt 1 lc rgb 'red' lw 3 t '" << data_key_1 << "', \\\n";
  output_file << "FILE_IN_2 u ($" << min_bin_col << "+$" << max_bin_col << ")/2:" << data_col << " w histeps lt 1 lc rgb 'blue' lw 3 t '" << data_key_2 << "'\n";
  output_file << "\n";
}

void prepare_gnuplot_script_cart_2D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t min_bin_x_col, size_t max_bin_x_col, size_t min_bin_y_col, size_t max_bin_y_col, size_t data_col, double xmin, double xmax, double ymin, double ymax, bool enable_ratio, std::string title_key) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN='" << data_file << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo dashed size " << Xres << ',' << Yres << " enhanced font 'Verdana," << fontsize << "'\n";
  output_file << "set output FILE_OUT\n";
  output_file << "# Styles\n";
  output_file << "linew = 1.2\n";
  output_file << "set style line  21 lc rgb '#0072bd' lt 7 lw linew  # blue\n";
  output_file << "set style line  22 lc rgb '#d95319' lt 7 lw linew  # orange\n";
  output_file << "set style line  23 lc rgb '#77ac30' lt 7 lw linew  # green\n";
  output_file << "set style line  24 lc rgb '#a2142f' lt 7 lw linew  # red\n";
  output_file << "set style line 102 lc rgb '#d6d7d9' lt 1 lw 1      # gray\n";
  output_file << "# Grid\n";
  output_file << "set grid xtics ytics back ls 102\n";
  output_file << "# Titles\n";
  output_file << "set key opaque\n";
  output_file << "set title 'Power Spectrum: )" << title_key << "\n";
  output_file << "set xlabel 'a_x (g)' \n";
  output_file << "set ylabel 'a_y (g)' \n";
  output_file << "set xrange[" << xmin << ':' << xmax << "] \n";
  output_file << "set yrange[" << ymin << ':' << ymax << "] \n";
  if (enable_ratio) output_file << "set size ratio - 1 \n";
  output_file << "set palette rgbformulae 22,13,10\n";
  output_file << "set logscale cb\n";
  output_file << "plot FILE_IN u ($" << min_bin_x_col << "+$" << max_bin_x_col << ")/2:($" << min_bin_y_col << "+$" << max_bin_y_col << ")/2:" << data_col << " w image notitle\n";
  output_file << "\n";
}

void prepare_gnuplot_script_polar_2D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t min_bin_x_col, size_t max_bin_x_col, size_t min_bin_y_col, size_t max_bin_y_col, size_t data_col, double rmin, double rmax, std::string title_key) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN='" << data_file << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo dashed size " << Xres << ',' << Yres << " enhanced font 'Verdana," << fontsize << "'\n";
  output_file << "set output FILE_OUT\n";
  output_file << "# Styles\n";
  output_file << "linew = 1.2\n";
  output_file << "set style line  21 lc rgb '#0072bd' lt 7 lw linew  # blue\n";
  output_file << "set style line  22 lc rgb '#d95319' lt 7 lw linew  # orange\n";
  output_file << "set style line  23 lc rgb '#77ac30' lt 7 lw linew  # green\n";
  output_file << "set style line  24 lc rgb '#a2142f' lt 7 lw linew  # red\n";
  output_file << "set style line 102 lc rgb '#d6d7d9' lt 1 lw 1      # gray\n";
  output_file << "# Grid\n";
  output_file << "set grid xtics ytics back ls 102\n";
  output_file << "# Titles\n";
  output_file << "set key opaque\n";
  output_file << "set title 'Power Spectrum: )" << title_key << "\n";
  output_file << "set xlabel 'a (g)' \n";
  output_file << "set rrange[" << rmin << ':' << rmax << "] \n";
  output_file << "set palette rgbformulae 22,13,10\n";
  output_file << "set logscale cb\n";
  output_file << "unset tics\n";
  output_file << "unset border\n";
  output_file << "set angles degrees\n";
  output_file << "set polar\n";
  output_file << "set grid polar 15.\n";
  output_file << "set pointsize 3\n";
  output_file << "plot FILE_IN u ($" << min_bin_x_col << "+$" << max_bin_x_col << ")/2:($" << min_bin_y_col << "+$" << max_bin_y_col << ")/2:" << data_col << " w points palette notitle\n";
  output_file << "\n";
}

void bin_data_cart_2D(std::vector<std::vector <double>> &input_data, size_t which_x, size_t which_y, double min_x, double max_x, double min_y, double max_y, std::vector<std::vector <size_t>> &binned_cart_data)
{
  size_t whichbin_x, whichbin_y;
  size_t nbin_x = binned_cart_data.size();
  size_t nbin_y = binned_cart_data.front().size();
  double bin_width_x = (max_x - min_x) / (nbin_x - 2);
  double bin_width_y = (max_y - min_y) / (nbin_y - 2);

  for (auto i : input_data) {
    if (i[which_x] < min_x) whichbin_x = 0;
    else if (i[which_x] > max_x) whichbin_x = nbin_x - 1;
    else whichbin_x = (int)(double(i[which_x] - min_x) / bin_width_x + 1.0);

    if (i[which_y] < min_y) whichbin_y = 0;
    else if (i[which_y] > max_y) whichbin_y = nbin_y - 1;
    else whichbin_y = (int)(double(i[which_y] - min_y) / bin_width_y + 1.0);

    binned_cart_data[whichbin_x][whichbin_y] += 1;
  }
}

void bin_data_polar_2D(std::vector<std::vector <double>> &input_data, size_t which_x, size_t which_y, double min_magn, double max_magn, double min_angle, double max_angle, std::vector<std::vector <size_t>> &binned_polar_data)
{
  size_t whichbin_magn, whichbin_angle;
  size_t nbin_magn = binned_polar_data.front().size();
  size_t nbin_angle = binned_polar_data.size();
  double bin_width_magn = (max_magn - min_magn) / (nbin_magn - 2);
  double bin_width_angle = (max_angle - min_angle) / (nbin_angle - 2);

  for (auto i : input_data) {
    double magnitude = sqrt(i[which_x] * i[which_x] + i[which_y] * i[which_y]);
    double angle = atan2(i[which_y], i[which_x]) / M_PI * 180.0 + 180.0;

    if (magnitude < min_magn) whichbin_magn = 0;
    else if (magnitude > max_magn) whichbin_magn = nbin_magn - 1;
    else whichbin_magn = (int)((magnitude - min_magn) / bin_width_magn + 1.0);

    if (angle < min_angle) whichbin_angle = 0;
    else if (angle > max_angle) whichbin_angle = nbin_angle - 1;
    else whichbin_angle = (int)(double(angle - min_angle) / bin_width_angle + 1.0);

    binned_polar_data[whichbin_angle][whichbin_magn] += 1;
  }
}

void bin_data_1D(std::vector<std::vector <double>> &input_data, size_t which_x, double min_x, double max_x, std::vector<size_t> &binned_data)
{
  size_t whichbin_x;
  double bin_width = (max_x - min_x) / (binned_data.size() - 2);
  for (auto i : input_data) {
    if (i[which_x] < min_x) whichbin_x = 0;
    else if (i[which_x] > max_x) whichbin_x = binned_data.size() - 1;
    else whichbin_x = (int)(double(i[which_x] - min_x) / bin_width + 1.0);

    binned_data[whichbin_x] += 1;
  }
}

void filter_data(std::vector<std::vector <double>> &input_data, size_t which_column, double min_value, double max_value)
{
  for (size_t i = 0; i < input_data.size(); i++) {
    if (input_data[i][which_column] < min_value || input_data[i][which_column] > max_value) {
      input_data.erase(input_data.begin() + i);
      i--;
    }
  }
}

void filter_data_modulus(std::vector<std::vector <double>> &input_data, size_t which_column_x, size_t which_column_y, size_t which_column_z, double min_value, double max_value)
{
  for (size_t i = 0; i < input_data.size(); i++) {
    if (
      sqrt(input_data[i][which_column_x] * input_data[i][which_column_x] + input_data[i][which_column_y] * input_data[i][which_column_y] + input_data[i][which_column_z] * input_data[i][which_column_z]) < min_value ||
      sqrt(input_data[i][which_column_x] * input_data[i][which_column_x] + input_data[i][which_column_y] * input_data[i][which_column_y] + input_data[i][which_column_z] * input_data[i][which_column_z]) > max_value) {

      input_data.erase(input_data.begin() + i);
      i--;
    }
  }
}

void parse_file(std::ifstream& input_file, std::vector<std::vector <double>> &input_data)
{
  std::string line;
  std::vector<std::string> tokens;
  while (std::getline(input_file, line)) {
    boost::algorithm::split(tokens, line, boost::algorithm::is_any_of(" \t"), boost::algorithm::token_compress_on);
    std::vector<double>data(tokens.size(), 0);
    for (size_t i = 0; i < tokens.size(); i++) data[i] = atof(tokens[i].c_str());
    input_data.push_back(data);
    tokens.clear();
    data.clear();
  }
}

void find_minmax_1D(std::vector<std::vector <double>> &input_data, size_t col_x, double &min_x, bool b_min_x, double &max_x, bool b_max_x) {
  for (auto i : input_data) {
    if (!b_min_x) min_x = (i[col_x] < min_x ? i[col_x] : min_x);
    if (!b_max_x) max_x = (i[col_x] > max_x ? i[col_x] : max_x);
  }
  if (min_x > max_x) {
    double temp = min_x;
    min_x = max_x;
    max_x = temp;
  }
}

void find_minmax(std::vector<std::vector <double>> &input_data, size_t col_x, double &min_x, bool b_min_x, double &max_x, bool b_max_x, size_t col_y, double &min_y, bool b_min_y, double &max_y, bool b_max_y, double &min_tot, bool b_min_tot, double &max_tot, bool b_max_tot) {
  for (auto i : input_data) {
    if (!b_min_x)   min_x = (i[col_x] < min_x ? i[col_x] : min_x);
    if (!b_max_x)   max_x = (i[col_x] > max_x ? i[col_x] : max_x);
    if (!b_min_y)   min_y = (i[col_y] < min_y ? i[col_y] : min_y);
    if (!b_max_y)   max_y = (i[col_y] > max_y ? i[col_y] : max_y);
    if (!b_min_tot) min_tot = (sqrt(i[col_x] * i[col_x] + i[col_y] * i[col_y]) < min_tot ? sqrt(i[col_x] * i[col_x] + i[col_y] * i[col_y]) : min_tot);
    if (!b_max_tot) max_tot = (sqrt(i[col_x] * i[col_x] + i[col_y] * i[col_y]) > max_tot ? sqrt(i[col_x] * i[col_x] + i[col_y] * i[col_y]) : max_tot);
  }
  if (min_x > max_x) {
    double temp = min_x;
    min_x = max_x;
    max_x = temp;
  }
  if (min_y > max_y) {
    double temp = min_y;
    min_y = max_y;
    max_y = temp;
  }
  if (min_tot > max_tot) {
    double temp = min_tot;
    min_tot = max_tot;
    max_tot = temp;
  }
}

void print_histo_1D(std::ofstream &output_file, std::vector<size_t> &binned_data, double min_x, double size_x) {
  min_x -= size_x;
  for (auto i : binned_data) {
    output_file << min_x << "\t" << min_x + size_x << "\t" << i << std::endl;
    min_x += size_x;
  }
}

void print_histo_2D(std::ofstream &output_file, std::vector<std::vector <size_t>> &binned_data, double min_x, double min_y, double size_x, double size_y) {
  min_x -= size_x;
  min_y -= size_y;
  double min_y_stored = min_y;
  for (auto i : binned_data) {
    for (auto j : i) {
      output_file << min_x << "\t" << min_x + size_x << "\t" << min_y << "\t" << min_y + size_y << "\t" << j << std::endl;
      min_y += size_y;
    }
    min_y = min_y_stored;
    min_x += size_x;
  }
}

void usage(char* progname) {
  std::cerr << "Usage: " << progname << " [input_file.txt] OR [conf.json]" << std::endl << std::endl;
  std::cerr << "This tool creates 1D and 2D histograms using inertial data." << std::endl;
  std::cerr << "Using the json you can fully customize how this program works." << std::endl;
  std::cerr << "If passing an inertial file, then all the plots will be done with automatic scalings." << std::endl;
}

void json_example() {
  std::cerr << R"({
  "input_file_name" : "inertial.txt",
  "output_file_histox" : "histox_bin.txt",
  "output_gnuplot_file_histox" : "histox_bin.plt",
  "output_image_file_histox" : "histox_bin.png",
  "output_file_histoy" : "histoy_bin.txt",
  "output_gnuplot_file_histoy" : "histoy_bin.plt",
  "output_gnuplot_file_histoxy" : "histoxy_bin.plt",
  "output_gnuplot_file_histoxy" : "histoxy_bin.plt",
  "output_image_file_histoxy" : "histoxy_bin.png",
  "output_file_cart" : "cart_bin.txt",
  "output_gnuplot_file_cart" : "cart_bin.plt",
  "output_image_file_cart" : "cart_bin.png",
  "output_file_polar" : "polar_bin.txt",
  "output_gnuplot_file_polar" : "polar_bin.plt",
  "output_image_file_polar" : "polar_bin.png",
  "output_gnuplot_file_polar_exp" : "polar_bin_exp.plt",
  "output_image_file_polar_exp" : "polar_bin_exp.png",
  "col_acc_x" : 1,
  "col_acc_y" : 2,
  "col_acc_z" : 3,
  "col_speed" : 4,
  "nbin_x" : 100,
  "nbin_y" : 100,
  "nbin_magn" : 100,
  "nbin_angle" : 36,
  "min_acc_x" : 0.0,
  "max_acc_x" : 1.0,
  "min_acc_y" : 0.0,
  "max_acc_y" : 1.0,
  "min_acc" : 0.0,
  "max_acc" : 1.0,
  "min_acc_filter" : 0.0,
  "max_acc_filter" : 1.0,
  "min_speed_filter" : 0.0,
  "max_speed_filter" : 1.0
}
)" << std::endl;
}

int main(int argc, char** argv) {
  std::cout << "histogram v" << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;

  std::string parameter_file_name;
  if (argc == 2) parameter_file_name = argv[1];
  else {
    std::cerr << "One argument is required: be it a .json conf file or a .txt inertial file." << std::endl;
    usage(argv[0]);
    std::cerr << "json example file:" << std::endl;
    json_example();
    exit(1);
  }

  bool we_have_json_conf = false;
  std::string input_file_name, input_file_basename, output_file_histox_name, output_file_histoy_name, output_file_cart_name, output_file_polar_name, output_gnuplot_file_cart_name, output_gnuplot_file_polar_name, output_gnuplot_file_polar_name_exp, output_gnuplot_file_histoxy_name,
    output_gnuplot_file_histox_name, output_gnuplot_file_histoy_name, output_image_file_cart_name, output_image_file_polar_name, output_image_file_polar_name_exp, output_image_file_histox_name, output_image_file_histoy_name, output_image_file_histoxy_name;

  std::ofstream output_file_histox, output_file_histoy, output_file_cart, output_file_polar, output_gnuplot_file_cart, output_gnuplot_file_polar, output_gnuplot_file_polar_exp, output_gnuplot_file_histox, output_gnuplot_file_histoy, output_gnuplot_file_histoxy;
  std::ifstream input_file;
  std::ifstream parameter_file;

  if (parameter_file_name.size() > 4) {
    if (parameter_file_name.substr(parameter_file_name.size() - 4, 4) != ".txt") {
      if (parameter_file_name.size() > 5) if (parameter_file_name.substr(parameter_file_name.size() - 5, 5) != ".json") {
        std::cerr << parameter_file_name << " is not a valid input file (not .json nor .txt). Quitting..." << std::endl;
        usage(argv[0]);
        std::cerr << "json example file:" << std::endl;
        json_example();
        exit(2);
      }
      else we_have_json_conf = true;
    }
    else {
      input_file_name = parameter_file_name;
      parameter_file_name.clear();
      we_have_json_conf = false;
    }
  }
  else {
    std::cerr << parameter_file_name << " is not a valid input file (not .json nor .txt). Quitting..." << std::endl;
    usage(argv[0]);
    std::cerr << "json example file:" << std::endl;
    json_example();
    exit(22);
  }

  jsoncons::json parameters;
  if (we_have_json_conf) {
    input_file.open(parameter_file_name.c_str());
    if (!input_file.is_open()) {
      std::cerr << "FAILED: Parameter file " << parameter_file_name << " could not be opened. Quitting..." << std::endl;
      exit(222);
    }
    else { std::cout << "SUCCESS: file " << parameter_file_name << " opened!\n"; }
    input_file.close();
    parameters = jsoncons::json::parse_file(parameter_file_name);
  }

  size_t col_speed, col_acc_x, col_acc_y, col_acc_z, nbin_x, nbin_y, nbin_magn, nbin_angle;
  double min_acc_x = std::numeric_limits<double>::max(), max_acc_x = std::numeric_limits<double>::lowest(), min_acc_y = std::numeric_limits<double>::max(), max_acc_y = std::numeric_limits<double>::lowest(), min_acc = std::numeric_limits<double>::max(), max_acc = std::numeric_limits<double>::lowest();
  double min_speed_filter = 0.0, max_speed_filter = std::numeric_limits<double>::max(), min_acc_filter = 0.0, max_acc_filter = std::numeric_limits<double>::max();
  bool b_min_acc_x = false, b_max_acc_x = false, b_min_acc_y = false, b_max_acc_y = false, b_min_acc = false, b_max_acc = false;

  /* Parse input file */
  if (we_have_json_conf) input_file_name = parameters.has_member("input_file_name") ? parameters["input_file_name"].as<std::string>() : "inertial.txt";
  input_file.open(input_file_name.c_str());
  if (!input_file.is_open()) {
    std::cerr << "FAILED: Input file " << input_file_name << " could not be opened. Quitting..." << std::endl;
    exit(222);
  }
  else { std::cout << "SUCCESS: file " << input_file_name << " opened!\n"; }
  std::vector<std::vector<double>> input_data;
  parse_file(input_file, input_data);
  input_file.close();

  input_file_basename = input_file_name.substr(0, input_file_name.size() - 4);

  /* Check all output files */
  // 1D histo_x
  output_file_histox_name = parameters.has_member("output_file_histox") ? parameters["output_file_histox"].as<std::string>() : input_file_basename + "_histox_bin.txt";
  output_file_histox.open(output_file_histox_name.c_str());
  if (!output_file_histox.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_histox_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_file_histox_name << " opened!" << std::endl; }

  output_gnuplot_file_histox_name = parameters.has_member("output_gnuplot_file_histox") ? parameters["output_gnuplot_file_histox"].as<std::string>() : input_file_basename + "_histox_bin.plt";
  output_gnuplot_file_histox.open(output_gnuplot_file_histox_name.c_str());
  if (!output_gnuplot_file_histox.is_open()) {
    std::cerr << "FAILED: Output file " << output_gnuplot_file_histox_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_histox_name << " opened!" << std::endl; }
  output_image_file_histox_name = parameters.has_member("output_image_file_histox") ? parameters["output_image_file_histox"].as<std::string>() : input_file_basename + "_histox_bin.png";

  // 1D histo_y
  output_file_histoy_name = parameters.has_member("output_file_histoy") ? parameters["output_file_histoy"].as<std::string>() : input_file_basename + "_histoy_bin.txt";
  output_file_histoy.open(output_file_histoy_name.c_str());
  if (!output_file_histoy.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_histoy_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_file_histoy_name << " opened!" << std::endl; }

  output_gnuplot_file_histoy_name = parameters.has_member("output_gnuplot_file_histoy") ? parameters["output_gnuplot_file_histoy"].as<std::string>() : input_file_basename + "_histoy_bin.plt";
  output_gnuplot_file_histoy.open(output_gnuplot_file_histoy_name.c_str());
  if (!output_gnuplot_file_histoy.is_open()) {
    std::cerr << "FAILED: Output file " << output_gnuplot_file_histoy_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_histoy_name << " opened!" << std::endl; }
  output_image_file_histoy_name = parameters.has_member("output_image_file_histoy") ? parameters["output_image_file_histoy"].as<std::string>() : input_file_basename + "_histoy_bin.png";

  // 1D histo_xy
  output_gnuplot_file_histoxy_name = parameters.has_member("output_gnuplot_file_histoxy") ? parameters["output_gnuplot_file_histoxy"].as<std::string>() : input_file_basename + "_histoxy_bin.plt";
  output_gnuplot_file_histoxy.open(output_gnuplot_file_histoxy_name.c_str());
  if (!output_gnuplot_file_histoxy.is_open()) {
    std::cerr << "FAILED: Output file " << output_gnuplot_file_histoxy_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_histoxy_name << " opened!" << std::endl; }
  output_image_file_histoxy_name = parameters.has_member("output_image_file_histoxy") ? parameters["output_image_file_histoxy"].as<std::string>() : input_file_basename + "_histoxy_bin.png";

  // 2D cart
  output_file_cart_name = parameters.has_member("output_file_cart") ? parameters["output_file_cart"].as<std::string>() : input_file_basename + "_cart_bin.txt";
  output_file_cart.open(output_file_cart_name.c_str());
  if (!output_file_cart.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_cart_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_file_cart_name << " opened!" << std::endl; }

  output_gnuplot_file_cart_name = parameters.has_member("output_gnuplot_file_cart") ? parameters["output_gnuplot_file_cart"].as<std::string>() : input_file_basename + "_cart_bin.plt";
  output_gnuplot_file_cart.open(output_gnuplot_file_cart_name.c_str());
  if (!output_gnuplot_file_cart.is_open()) {
    std::cerr << "FAILED: Output file " << output_gnuplot_file_cart_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_cart_name << " opened!" << std::endl; }
  output_image_file_cart_name = parameters.has_member("output_image_file_cart") ? parameters["output_image_file_cart"].as<std::string>() : input_file_basename + "_cart_bin.png";

  // 2D polar
  output_file_polar_name = parameters.has_member("output_file_polar") ? parameters["output_file_polar"].as<std::string>() : input_file_basename + "_polar_bin.txt";
  output_file_polar.open(output_file_polar_name.c_str());
  if (!output_file_polar.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_polar_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_file_polar_name << " opened!" << std::endl; }

  output_gnuplot_file_polar_name = parameters.has_member("output_gnuplot_file_polar") ? parameters["output_gnuplot_file_polar"].as<std::string>() : input_file_basename + "_polar_bin.plt";
  output_gnuplot_file_polar.open(output_gnuplot_file_polar_name.c_str());
  if (!output_gnuplot_file_polar.is_open()) {
    std::cerr << "FAILED: Output file " << output_gnuplot_file_polar_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_polar_name << " opened!" << std::endl; }
  output_image_file_polar_name = parameters.has_member("output_image_file_polar") ? parameters["output_image_file_polar"].as<std::string>() : input_file_basename + "_polar_bin.png";

  // 2D polar experimental
  output_gnuplot_file_polar_name_exp = parameters.has_member("output_gnuplot_file_polar_exp") ? parameters["output_gnuplot_file_polar_exp"].as<std::string>() : input_file_basename + "_polar_bin_exp.plt";
  output_gnuplot_file_polar_exp.open(output_gnuplot_file_polar_name_exp.c_str());
  if (!output_gnuplot_file_polar_exp.is_open()) {
    std::cerr << "FAILED: Output file " << output_gnuplot_file_polar_name_exp << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_polar_name_exp << " opened!" << std::endl; }
  output_image_file_polar_name_exp = parameters.has_member("output_image_file_polar_exp") ? parameters["output_image_file_polar_exp"].as<std::string>() : input_file_basename + "_polar_bin_exp.png";



  col_acc_x = parameters.has_member("col_acc_x") ? parameters["col_acc_x"].as<size_t>() - 1 : 2;
  col_acc_y = parameters.has_member("col_acc_y") ? parameters["col_acc_y"].as<size_t>() - 1 : 3;
  col_acc_z = parameters.has_member("col_acc_z") ? parameters["col_acc_z"].as<size_t>() - 1 : 4;
  col_speed = parameters.has_member("col_speed") ? parameters["col_speed"].as<size_t>() - 1 : 1;
  col_acc_x = (col_acc_x >= 0 ? col_acc_x : 2);
  col_acc_y = (col_acc_y >= 0 ? col_acc_y : 3);
  col_acc_z = (col_acc_z >= 0 ? col_acc_z : 4);
  col_speed = (col_speed >= 0 ? col_speed : 1);

  nbin_x = parameters.has_member("nbin_x") ? parameters["nbin_x"].as<size_t>() : 100;
  nbin_y = parameters.has_member("nbin_y") ? parameters["nbin_y"].as<size_t>() : 100;
  nbin_x = (nbin_x > 0 ? nbin_x : 100);
  nbin_y = (nbin_y > 0 ? nbin_y : 100);

  nbin_magn = parameters.has_member("nbin_magn") ? parameters["nbin_magn"].as<size_t>() : 100;
  nbin_angle = parameters.has_member("nbin_angle") ? parameters["nbin_angle"].as<size_t>() : 36 + 2;
  nbin_magn = (nbin_magn > 0 ? nbin_magn : 100);
  nbin_angle = (nbin_angle > 0 ? nbin_angle : 36 + 2);

  if (parameters.has_member("min_acc_x")) {
    min_acc_x = parameters["min_acc_x"].as<double>();
    b_min_acc_x = true;
  }
  else min_acc_x = std::numeric_limits<double>::max();

  if (parameters.has_member("max_acc_x")) {
    max_acc_x = parameters["max_acc_x"].as<double>();
    b_max_acc_x = true;
  }
  else max_acc_x = std::numeric_limits<double>::lowest();

  if (parameters.has_member("min_acc_y")) {
    min_acc_y = parameters["min_acc_y"].as<double>();
    b_min_acc_y = true;
  }
  else min_acc_y = std::numeric_limits<double>::max();

  if (parameters.has_member("max_acc_y")) {
    max_acc_y = parameters["max_acc_y"].as<double>();
    b_max_acc_y = true;
  }
  else max_acc_y = std::numeric_limits<double>::lowest();

  if (parameters.has_member("min_acc")) {
    min_acc = parameters["min_acc"].as<double>();
    b_min_acc = true;
  }
  else min_acc = std::numeric_limits<double>::max();

  if (parameters.has_member("max_acc")) {
    max_acc = parameters["max_acc"].as<double>();
    b_max_acc = true;
  }
  else max_acc = std::numeric_limits<double>::lowest();

  min_acc_filter = parameters.has_member("min_acc_filter") ? parameters["min_acc_filter"].as<double>() : min_acc_filter;
  max_acc_filter = parameters.has_member("max_acc_filter") ? parameters["max_acc_filter"].as<double>() : max_acc_filter;
  min_speed_filter = parameters.has_member("min_speed_filter") ? parameters["min_speed_filter"].as<double>() : min_speed_filter;
  max_speed_filter = parameters.has_member("max_speed_filter") ? parameters["max_speed_filter"].as<double>() : max_speed_filter;

  find_minmax(input_data, col_acc_x, min_acc_x, b_min_acc_x, max_acc_x, b_max_acc_x, col_acc_y, min_acc_y, b_min_acc_y, max_acc_y, b_max_acc_y, min_acc, b_min_acc, max_acc, b_max_acc);

  filter_data(input_data, col_speed, min_speed_filter, max_speed_filter);
  filter_data_modulus(input_data, col_acc_x, col_acc_y, col_acc_z, min_acc_filter, max_acc_filter);

  /* 2D analysis */
  std::vector<std::vector<size_t>> binned_cart_data(nbin_y, std::vector<size_t>(nbin_x));
  std::vector<std::vector<size_t>> binned_polar_data(nbin_angle, std::vector<size_t>(nbin_magn));
  bin_data_cart_2D(input_data, col_acc_x, col_acc_y, min_acc_x, max_acc_x, min_acc_y, max_acc_y, binned_cart_data);
  bin_data_polar_2D(input_data, col_acc_x, col_acc_y, min_acc, max_acc, 0, 360, binned_polar_data);

  std::string title_key, escaped_title_key;

  print_histo_2D(output_file_cart, binned_cart_data, min_acc_x, min_acc_y, (max_acc_x - min_acc_x) / (nbin_x - 2), (max_acc_y - min_acc_y) / (nbin_y - 2));
  print_histo_2D(output_file_polar, binned_polar_data, 0, min_acc, 360.0 / double(nbin_angle - 2), (max_acc - min_acc) / (nbin_magn - 2));

  title_key = "a_x vs a_y (cartesian): " + output_file_cart_name;
  escaped_title_key = boost::replace_all_copy(title_key, "_", "\\_");
  prepare_gnuplot_script_cart_2D(output_gnuplot_file_cart, output_file_cart_name, output_image_file_cart_name, XRES, YRES, FONTSIZE, 1, 2, 3, 4, 5, min_acc_x, max_acc_x, min_acc_y, max_acc_y, true, escaped_title_key);

  title_key = "a_x vs a_y (polar): " + output_file_polar_name;
  escaped_title_key = boost::replace_all_copy(title_key, "_", "\\_");
  prepare_gnuplot_script_cart_2D(output_gnuplot_file_polar, output_file_polar_name, output_image_file_polar_name, XRES, YRES, FONTSIZE, 1, 2, 3, 4, 5, 0, 360, min_acc, max_acc, false, escaped_title_key);

  title_key = "a_x vs a_y (polar_exp): " + output_file_polar_name;
  escaped_title_key = boost::replace_all_copy(title_key, "_", "\\_");
  prepare_gnuplot_script_polar_2D(output_gnuplot_file_polar_exp, output_file_polar_name, output_image_file_polar_name_exp, XRES, YRES, FONTSIZE, 1, 2, 3, 4, 5, min_acc, max_acc, escaped_title_key);

  /* 1D analysis */
  std::vector<size_t> binned_histox_data(nbin_x);
  std::vector<size_t> binned_histoy_data(nbin_x);


  bin_data_1D(input_data, col_acc_x, min_acc_x, max_acc_x, binned_histox_data);
  bin_data_1D(input_data, col_acc_y, min_acc_y, max_acc_y, binned_histoy_data);
  print_histo_1D(output_file_histox, binned_histox_data, min_acc_x, (max_acc_x - min_acc_x) / (nbin_x - 2));
  print_histo_1D(output_file_histoy, binned_histoy_data, min_acc_y, (max_acc_y - min_acc_y) / (nbin_y - 2));

  title_key = "a_x (histogram): " + output_file_histox_name;
  escaped_title_key = boost::replace_all_copy(title_key, "_", "\\_");
  prepare_gnuplot_script_1D(output_gnuplot_file_histox, output_file_histox_name, output_image_file_histox_name, XRES, YRES, FONTSIZE, 1, 2, 3, "a_x", escaped_title_key);

  title_key = "a_y (histogram): " + output_file_histoy_name;
  escaped_title_key = boost::replace_all_copy(title_key, "_", "\\_");
  prepare_gnuplot_script_1D(output_gnuplot_file_histoy, output_file_histoy_name, output_image_file_histoy_name, XRES, YRES, FONTSIZE, 1, 2, 3, "a_y", escaped_title_key);

  title_key = "a_x and a_y (histograms): " + output_file_histox_name + " and " + output_file_histoy_name;
  escaped_title_key = boost::replace_all_copy(title_key, "_", "\\_");
  prepare_gnuplot_script_double_1D(output_gnuplot_file_histoxy, output_file_histox_name, output_file_histoy_name, output_image_file_histoxy_name, XRES, YRES, FONTSIZE, 1, 2, 3, "a_x", "a_y", escaped_title_key);


  //std::cout << "Done; please run: \n$for script in $(find . -name \"*.plt\") ; do gnuplot $script ; done\nin this folder to refresh png(s)" << std::endl;
  output_file_histox.close(), output_file_histoy.close(), output_file_cart.close(), output_file_polar.close(), output_gnuplot_file_cart.close(), output_gnuplot_file_polar.close(), output_gnuplot_file_histox.close(), output_gnuplot_file_histoy.close();

  return 0;

}


