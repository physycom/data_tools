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

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "jsoncons/json.hpp"

#define MAJOR_VERSION 2
#define MINOR_VERSION 0

void prepare_gnuplot_script_1D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t min_bin_col, size_t max_bin_col, size_t data_col, std::string data_key) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN='" << data_file << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo size " << Xres << ',' << Yres << " font \"," << fontsize << "\"\n";
  output_file << "set output FILE_OUT\n";
  output_file << "set xlabel 'a (g)' \n";
  output_file << "set ylabel 'dN/da'\n";
  output_file << "plot FILE_IN u ($" << min_bin_col << "+$" << max_bin_col << ")/2:" << data_col << " w histeps lt 1 lc rgb 'red' lw 3 t '" << data_key << "'\n";
  output_file << "\n";
}

void prepare_gnuplot_script_cart_2D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t min_bin_x_col, size_t max_bin_x_col, size_t min_bin_y_col, size_t max_bin_y_col, size_t data_col, double xmin, double xmax, double ymin, double ymax, bool enable_ratio) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN='" << data_file << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo size " << Xres << ',' << Yres << " font \"," << fontsize << "\"\n";
  output_file << "set output FILE_OUT\n";
  output_file << "set xlabel 'a_x (g)' \n";
  output_file << "set ylabel 'a_y (g)' \n";
  output_file << "set xrange[" << xmin << ':' << xmax << "] \n";
  output_file << "set yrange[" << ymin << ':' << ymax << "] \n";
  if (enable_ratio) output_file << "set size ratio - 1 \n ";
  output_file << "set palette rgbformulae 22,13,10\n";
  output_file << "set logscale cb\n";
  output_file << "plot FILE_IN u ($" << min_bin_x_col << "+$" << max_bin_x_col << ")/2:($" << min_bin_y_col << "+$" << max_bin_y_col << ")/2:" << data_col << " w image notitle\n";
  output_file << "\n";
}

void prepare_gnuplot_script_polar_2D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t min_bin_x_col, size_t max_bin_x_col, size_t min_bin_y_col, size_t max_bin_y_col, size_t data_col, double rmin, double rmax) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN='" << data_file << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo size " << Xres << ',' << Yres << " font \"," << fontsize << "\"\n";
  output_file << "set output FILE_OUT\n";
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
  // Usage
  std::cout << std::endl << "\tUsage:\t" << progname << " -i [input_file.txt] -p [conf.json]" << std::endl << std::endl;
  std::cout << "This tool creates 1D or 2D histograms on input data, based on parameters given in a json config file" << std::endl;
  exit(1);
}

int main(int argc, char** argv) {
  std::cout << "Histogram calculator v" << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;

  std::string parameter_file_name;
  if (argc == 2) parameter_file_name = argv[1];
  else {
    std::cout << "ERROR: No flags specified. Read usage and relaunch properly." << std::endl;
    usage(argv[0]);
  }

  std::string input_file_name, output_file_histox_name, output_file_histoy_name, output_file_cart_name, output_file_polar_name, output_gnuplot_file_cart_name, output_gnuplot_file_polar_name, output_gnuplot_file_polar_name_exp,
    output_gnuplot_file_histox_name, output_gnuplot_file_histoy_name, output_image_file_cart_name, output_image_file_polar_name, output_image_file_polar_name_exp, output_image_file_histox_name, output_image_file_histoy_name;

  std::ofstream output_file_histox, output_file_histoy, output_file_cart, output_file_polar, output_gnuplot_file_cart, output_gnuplot_file_polar, output_gnuplot_file_polar_exp, output_gnuplot_file_histox, output_gnuplot_file_histoy;
  std::ifstream input_file;
  std::ifstream parameter_file;

  if (parameter_file_name.size() > 5) {
    if (parameter_file_name.substr(parameter_file_name.size() - 5, 5) != ".json") {
      std::cout << parameter_file_name << " is not a valid .json file. Quitting..." << std::endl;
      exit(2);
    }
  }
  else {
    std::cout << parameter_file_name << " is not a valid .json file. Quitting..." << std::endl;
    exit(22);
  }
  input_file.open(parameter_file_name.c_str());
  if (!input_file.is_open()) {
    std::cout << "FAILED: Input file " << parameter_file_name << " could not be opened. Quitting..." << std::endl;
    exit(222);
  }
  else { std::cout << "SUCCESS: file " << parameter_file_name << " opened!\n"; }
  input_file.close();

  jsoncons::json parameters = jsoncons::json::parse_file(parameter_file_name);

  size_t col_acc_x, col_acc_y, nbin_x, nbin_y, nbin_magn, nbin_angle;
  double min_acc_x = std::numeric_limits<double>::max(), max_acc_x = std::numeric_limits<double>::lowest(), min_acc_y = std::numeric_limits<double>::max(), max_acc_y = std::numeric_limits<double>::lowest(), min_acc = std::numeric_limits<double>::max(), max_acc = std::numeric_limits<double>::lowest();
  bool b_min_acc_x = false, b_max_acc_x = false, b_min_acc_y = false, b_max_acc_y = false, b_min_acc = false, b_max_acc = false;

  /* Parse input file */
  input_file_name = parameters.has_member("input_file_name") ? parameters["input_file_name"].as<std::string>() : "acc.txt";
  input_file.open(input_file_name.c_str());
  if (!input_file.is_open()) {
    std::cout << "FAILED: Input file " << input_file_name << " could not be opened. Quitting..." << std::endl;
    exit(222);
  }
  else { std::cout << "SUCCESS: file " << input_file_name << " opened!\n"; }
  std::vector<std::vector<double>> input_data;
  parse_file(input_file, input_data);
  input_file.close();

  /* Check all output files */
  // 1D histo_x
  output_file_histox_name = parameters.has_member("output_file_histox") ? parameters["output_file_histox"].as<std::string>() : "histox_bin.txt";
  output_file_histox.open(output_file_histox_name.c_str());
  if (!output_file_histox.is_open()) {
    std::cout << "FAILED: Output file " << output_file_histox_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_file_histox_name << " opened!" << std::endl; }

  output_gnuplot_file_histox_name = parameters.has_member("output_gnuplot_file_histox") ? parameters["output_gnuplot_file_histox"].as<std::string>() : "histox_bin.plt";
  output_gnuplot_file_histox.open(output_gnuplot_file_histox_name.c_str());
  if (!output_gnuplot_file_histox.is_open()) {
    std::cout << "FAILED: Output file " << output_gnuplot_file_histox_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_histox_name << " opened!" << std::endl; }
  output_image_file_histox_name = parameters.has_member("output_image_file_histox") ? parameters["output_image_file_histox"].as<std::string>() : "histox_bin.png";

  // 1D histo_y
  output_file_histoy_name = parameters.has_member("output_file_histoy") ? parameters["output_file_histoy"].as<std::string>() : "histoy_bin.txt";
  output_file_histoy.open(output_file_histoy_name.c_str());
  if (!output_file_histoy.is_open()) {
    std::cout << "FAILED: Output file " << output_file_histoy_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_file_histoy_name << " opened!" << std::endl; }

  output_gnuplot_file_histoy_name = parameters.has_member("output_gnuplot_file_histoy") ? parameters["output_gnuplot_file_histoy"].as<std::string>() : "histoy_bin.plt";
  output_gnuplot_file_histoy.open(output_gnuplot_file_histoy_name.c_str());
  if (!output_gnuplot_file_histoy.is_open()) {
    std::cout << "FAILED: Output file " << output_gnuplot_file_histoy_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_histoy_name << " opened!" << std::endl; }
  output_image_file_histoy_name = parameters.has_member("output_image_file_histoy") ? parameters["output_image_file_histoy"].as<std::string>() : "histoy_bin.png";

  // 2D cart
  output_file_cart_name = parameters.has_member("output_file_cart") ? parameters["output_file_cart"].as<std::string>() : "cart_bin.txt";
  output_file_cart.open(output_file_cart_name.c_str());
  if (!output_file_cart.is_open()) {
    std::cout << "FAILED: Output file " << output_file_cart_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_file_cart_name << " opened!" << std::endl; }

  output_gnuplot_file_cart_name = parameters.has_member("output_gnuplot_file_cart") ? parameters["output_gnuplot_file_cart"].as<std::string>() : "cart_bin.plt";
  output_gnuplot_file_cart.open(output_gnuplot_file_cart_name.c_str());
  if (!output_gnuplot_file_cart.is_open()) {
    std::cout << "FAILED: Output file " << output_gnuplot_file_cart_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_cart_name << " opened!" << std::endl; }
  output_image_file_cart_name = parameters.has_member("output_image_file_cart") ? parameters["output_image_file_cart"].as<std::string>() : "cart_bin.png";

  // 2D polar
  output_file_polar_name = parameters.has_member("output_file_polar") ? parameters["output_file_polar"].as<std::string>() : "polar_bin.txt";
  output_file_polar.open(output_file_polar_name.c_str());
  if (!output_file_polar.is_open()) {
    std::cout << "FAILED: Output file " << output_file_polar_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_file_polar_name << " opened!" << std::endl; }

  output_gnuplot_file_polar_name = parameters.has_member("output_gnuplot_file_polar") ? parameters["output_gnuplot_file_polar"].as<std::string>() : "polar_bin.plt";
  output_gnuplot_file_polar.open(output_gnuplot_file_polar_name.c_str());
  if (!output_gnuplot_file_polar.is_open()) {
    std::cout << "FAILED: Output file " << output_gnuplot_file_polar_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_polar_name << " opened!" << std::endl; }
  output_image_file_polar_name = parameters.has_member("output_image_file_polar") ? parameters["output_image_file_polar"].as<std::string>() : "polar_bin.png";

  // 2D polar experimental
  output_gnuplot_file_polar_name_exp = parameters.has_member("output_gnuplot_file_polar_exp") ? parameters["output_gnuplot_file_polar_exp"].as<std::string>() : "polar_bin_exp.plt";
  output_gnuplot_file_polar_exp.open(output_gnuplot_file_polar_name_exp.c_str());
  if (!output_gnuplot_file_polar_exp.is_open()) {
    std::cout << "FAILED: Output file " << output_gnuplot_file_polar_name_exp << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_polar_name_exp << " opened!" << std::endl; }
  output_image_file_polar_name_exp = parameters.has_member("output_image_file_polar_exp") ? parameters["output_image_file_polar_exp"].as<std::string>() : "polar_bin_exp.png";



  col_acc_x = parameters.has_member("col_acc_x") ? parameters["col_acc_x"].as<size_t>() - 1 : 1;
  col_acc_y = parameters.has_member("col_acc_y") ? parameters["col_acc_y"].as<size_t>() - 1 : 2;
  col_acc_x = (col_acc_x >= 0 ? col_acc_x : 1);
  col_acc_y = (col_acc_y >= 0 ? col_acc_y : 2);

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

  find_minmax(input_data, col_acc_x, min_acc_x, b_min_acc_x, max_acc_x, b_max_acc_x, col_acc_y, min_acc_y, b_min_acc_y, max_acc_y, b_max_acc_y, min_acc, b_min_acc, max_acc, b_max_acc);
  std::cout << min_acc_x << "\t" << max_acc_x << "\t" << min_acc_y << "\t" << max_acc_y << "\t" << min_acc << "\t" << max_acc << "\n";

  /* 2D analysis */
  std::vector<std::vector<size_t>> binned_cart_data(nbin_y, std::vector<size_t>(nbin_x));
  std::vector<std::vector<size_t>> binned_polar_data(nbin_angle, std::vector<size_t>(nbin_magn));
  bin_data_cart_2D(input_data, col_acc_x, col_acc_y, min_acc_x, max_acc_x, min_acc_y, max_acc_y, binned_cart_data);
  bin_data_polar_2D(input_data, col_acc_x, col_acc_y, min_acc, max_acc, 0, 360, binned_polar_data);
  print_histo_2D(output_file_cart, binned_cart_data, min_acc_x, min_acc_y, (max_acc_x - min_acc_x) / (nbin_x - 2), (max_acc_y - min_acc_y) / (nbin_y - 2));
  print_histo_2D(output_file_polar, binned_polar_data, 0, min_acc, 360.0 / double(nbin_angle - 2), (max_acc - min_acc) / (nbin_magn - 2));
  prepare_gnuplot_script_cart_2D(output_gnuplot_file_cart, output_file_cart_name, output_image_file_cart_name, 1280, 960, 25, 1, 2, 3, 4, 5, min_acc_x, max_acc_x, min_acc_y, max_acc_y, true);
  prepare_gnuplot_script_cart_2D(output_gnuplot_file_polar, output_file_polar_name, output_image_file_polar_name, 1280, 960, 25, 1, 2, 3, 4, 5, 0, 360, min_acc, max_acc, false);
  prepare_gnuplot_script_polar_2D(output_gnuplot_file_polar_exp, output_file_polar_name, output_image_file_polar_name_exp, 1280, 960, 25, 1, 2, 3, 4, 5, min_acc, max_acc);

  /* 1D analysis */
  std::vector<size_t> binned_histox_data(nbin_x);
  std::vector<size_t> binned_histoy_data(nbin_x);
  bin_data_1D(input_data, col_acc_x, min_acc_x, max_acc_x, binned_histox_data);
  bin_data_1D(input_data, col_acc_y, min_acc_y, max_acc_y, binned_histoy_data);
  print_histo_1D(output_file_histox, binned_histox_data, min_acc_x, (max_acc_x - min_acc_x) / (nbin_x - 2));
  print_histo_1D(output_file_histoy, binned_histoy_data, min_acc_y, (max_acc_y - min_acc_y) / (nbin_y - 2));
  prepare_gnuplot_script_1D(output_gnuplot_file_histox, output_file_histox_name, output_image_file_histox_name, 1280, 720, 25, 1, 2, 3, "a_x");
  prepare_gnuplot_script_1D(output_gnuplot_file_histoy, output_file_histoy_name, output_image_file_histoy_name, 1280, 720, 25, 1, 2, 3, "a_y");


  std::cout << "Done; please run: \n$for script in $(find . -name \"*.plt\") ; do gnuplot $script ; done\nin this folder to refresh png(s)" << std::endl;
  output_file_histox.close(), output_file_histoy.close(), output_file_cart.close(), output_file_polar.close(), output_gnuplot_file_cart.close(), output_gnuplot_file_polar.close(), output_gnuplot_file_histox.close(), output_gnuplot_file_histoy.close();

  return 0;

}


