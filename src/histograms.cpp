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

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "jsoncons/json.hpp"

#define MAJOR_VERSION 0
#define MINOR_VERSION 3

void prepare_gnuplot_script_1D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t min_bin_col, size_t max_bin_col, size_t data_col, std::string data_key) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN='" << data_file << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo size " << Xres << ',' << Yres << " font \"," << fontsize << "\"\n";
  output_file << "set output FILE_OUT\n";
  output_file << "set xlabel 'a (m/s^2)' \n";
  output_file << "set ylabel 'dN/da'\n";
  output_file << "plot FILE_IN u ($" << min_bin_col << "+$" << max_bin_col << ")/2:" << data_col << " w histeps lt 1 lc rgb 'red' lw 3 t '" << data_key << "'\n";
  output_file << "\n";
}

void prepare_gnuplot_script_2D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t min_bin_x_col, size_t max_bin_x_col, size_t min_bin_y_col, size_t max_bin_y_col, size_t data_col) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN='" << data_file << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo size " << Xres << ',' << Yres << " font \"," << fontsize << "\"\n";
  output_file << "set output FILE_OUT\n";
  output_file << "set xlabel 'a_x (m/s^2)' \n";
  output_file << "set ylabel 'a_y (m/s^2)' \n";
  output_file << "set palette rgbformulae 22,13,10\n";
  output_file << "plot FILE_IN u ($" << min_bin_x_col << "+$" << max_bin_x_col << ")/2:($" << min_bin_y_col << "+$" << max_bin_y_col << ")/2:" << data_col << " w image notitle\n";
  output_file << "\n";
}

void bin_data_2D(std::vector<std::vector <double>> &input_data, size_t which_x, size_t which_y, double min_x, double max_x, double min_y, double max_y, std::vector<std::vector <size_t>> &binned_data)
{
  size_t whichbin_x, whichbin_y;
  size_t nbin_x = binned_data.size();
  size_t nbin_y = binned_data.front().size();
  double bin_width_x = (max_x - min_x) / (nbin_x - 2);
  double bin_width_y = (max_y - min_y) / (nbin_y - 2);

  for (auto i : input_data) {
    if (i[which_x] < min_x) whichbin_x = 0;
    else if (i[which_x] > max_x) whichbin_x = nbin_x - 1;
    else whichbin_x = (int)(double(i[which_x] - min_x) / bin_width_x + 1.0);

    if (i[which_y] < min_y) whichbin_y = 0;
    else if (i[which_y] > max_y) whichbin_y = nbin_y - 1;
    else whichbin_y = (int)(double(i[which_y] - min_y) / bin_width_y + 1.0);

    binned_data[whichbin_x][whichbin_y] += 1;
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

void find_minmax_2D(std::vector<std::vector <double>> &input_data, size_t col_x, double &min_x, bool b_min_x, double &max_x, bool b_max_x, size_t col_y, double &min_y, bool b_min_y, double &max_y, bool b_max_y, double &min_tot, bool b_min_tot, double &max_tot, bool b_max_tot) {
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
    output_file << min_x << "\t" << i << std::endl;
    min_x += size_x;
  }
}

void print_histo_2D(std::ofstream &output_file, std::vector<std::vector <size_t>> &binned_data, double min_x, double min_y, double size_x, double size_y) {
  min_x -= size_x;
  min_y -= size_y;
  double min_y_stored = min_y;
  for (auto i : binned_data) {
    for (auto j : i) {
      output_file << min_x << "\t" << min_y << "\t" << j << std::endl;
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

  std::string input_file_name, parameter_file_name, output_file_name, output_gnuplot_file_name, output_image_file_name;
  bool twod = false;

  if (argc == 2) parameter_file_name = argv[1];
  else {
    std::cout << "ERROR: No flags specified. Read usage and relaunch properly." << std::endl;
    usage(argv[0]);
  }

  //Safety checks for file manipulations
  std::ofstream output_file;
  std::ofstream output_gnuplot_file;
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

  size_t col_acc_x, col_acc_y, nbin_x, nbin_y;
  double min_acc_x = std::numeric_limits<double>::max(), max_acc_x = std::numeric_limits<double>::lowest(), min_acc_y = std::numeric_limits<double>::max(), max_acc_y = std::numeric_limits<double>::lowest(), min_acc = std::numeric_limits<double>::max(), max_acc = std::numeric_limits<double>::lowest();
  bool b_min_acc_x = false, b_max_acc_x = false, b_min_acc_y = false, b_max_acc_y = false, b_min_acc = false, b_max_acc = false;
  bool enable_2D;

  input_file_name = parameters.has_member("input_file_name") ? parameters["input_file_name"].as<std::string>() : "acc.txt";
  input_file.open(input_file_name.c_str());
  if (!input_file.is_open()) {
    std::cout << "FAILED: Input file " << input_file_name << " could not be opened. Quitting..." << std::endl;
    exit(222);
  }
  else { std::cout << "SUCCESS: file " << input_file_name << " opened!\n"; }

  output_file_name = parameters.has_member("output_file_name") ? parameters["output_file_name"].as<std::string>() : "acc_bin.txt";
  output_file.open(output_file_name.c_str());
  if (!output_file.is_open()) {
    std::cout << "FAILED: Output file " << output_file_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_file_name << " opened!" << std::endl; }
  output_gnuplot_file_name = parameters.has_member("output_gnuplot_file_name") ? parameters["output_gnuplot_file_name"].as<std::string>() : "acc_bin.plt";
  output_gnuplot_file.open(output_gnuplot_file_name.c_str());
  if (!output_file.is_open()) {
    std::cout << "FAILED: Output file " << output_gnuplot_file_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_gnuplot_file_name << " opened!" << std::endl; }
  output_image_file_name = parameters.has_member("output_image_file_name") ? parameters["output_image_file_name"].as<std::string>() : "acc_bin.png";


  enable_2D = parameters.has_member("enable_2D") ? parameters["enable_2D"].as<bool>() : false;

  col_acc_x = parameters.has_member("col_acc_x") ? parameters["col_acc_x"].as<size_t>() - 1 : 1;
  col_acc_y = parameters.has_member("col_acc_y") ? parameters["col_acc_y"].as<size_t>() - 1 : 2;
  col_acc_x = (col_acc_x >= 0 ? col_acc_x : 1);
  col_acc_y = (col_acc_y >= 0 ? col_acc_y : 2);

  nbin_x = parameters.has_member("nbin_x") ? parameters["nbin_x"].as<size_t>() : 100;
  nbin_y = parameters.has_member("nbin_y") ? parameters["nbin_y"].as<size_t>() : 100;
  nbin_x = (nbin_x > 0 ? nbin_x : 100);
  nbin_y = (nbin_y > 0 ? nbin_y : 100);

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

  std::vector<std::vector<double>> input_data;
  parse_file(input_file, input_data);
  input_file.close();

  if (enable_2D) {
    find_minmax_2D(input_data, col_acc_x, min_acc_x, b_min_acc_x, max_acc_x, b_max_acc_x, col_acc_y, min_acc_y, b_min_acc_y, max_acc_y, b_max_acc_y, min_acc, b_min_acc, max_acc, b_max_acc);
    std::vector<std::vector<size_t>> binned_data(nbin_y, std::vector<size_t>(nbin_x));
    bin_data_2D(input_data, col_acc_x, col_acc_y, min_acc_x, max_acc_x, min_acc_y, max_acc_y, binned_data);
    print_histo_2D(output_file, binned_data, min_acc_x, min_acc_y, (max_acc_x - min_acc_x) / (nbin_x - 2), (max_acc_y - min_acc_y) / (nbin_y - 2));
    prepare_gnuplot_script_2D(output_gnuplot_file, output_file_name, output_image_file_name, 1280, 720, 25, 1, 2, 3, 4, 5);
  }
  else {
    find_minmax_1D(input_data, col_acc_x, min_acc, b_min_acc, max_acc, b_max_acc);
    std::vector<size_t> binned_data(nbin_x);
    bin_data_1D(input_data, col_acc_x, min_acc, max_acc, binned_data);
    print_histo_1D(output_file, binned_data, min_acc, (max_acc - min_acc) / (nbin_x - 2));
    prepare_gnuplot_script_1D(output_gnuplot_file, output_file_name, output_image_file_name, 1280, 720, 25, 1, 2, 3, "acc_x");
  }

  std::cout << "Done; please run: \n$gnuplot " << output_gnuplot_file_name << std::endl;
  output_file.close();
  output_gnuplot_file.close();

  return 0;

}


