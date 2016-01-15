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
#define MINOR_VERSION 1


void bin_data_2D(std::vector<std::vector <double>> &input_data, size_t size_row, size_t size_col, size_t which_x, size_t which_y, double min_x, double max_x, double min_y, double max_y, std::vector<std::vector <size_t>> &binned_data, size_t nbin_x, size_t nbin_y)
{
  size_t whichbin_x, whichbin_y;

  for (size_t i = 0; i < size_row; i++) {
    if (input_data[i][which_x] < min_x) whichbin_x = 0;
    else if (input_data[i][which_x] > max_x) whichbin_x = nbin_x - 1;
    else whichbin_x = (int)(((input_data[i][which_x] - min_x) / (nbin_x - 2)) + 1.0);

    if (input_data[i][which_y] < min_y) whichbin_y = 0;
    else if (input_data[i][which_y] > max_y) whichbin_y = nbin_y - 1;
    else whichbin_y = (int)(((input_data[i][which_y] - min_y) / (nbin_y - 2)) + 1.0);

    binned_data[whichbin_x][whichbin_y] += 1;
  }
}

void bin_data_1D(std::vector<std::vector <double>> &input_data, size_t size_row, size_t size_col, size_t which_x, double min_x, double max_x, std::vector<size_t> &binned_data, size_t nbin_x)
{
  size_t whichbin_x;

  for (size_t i = 0; i < size_row; i++) {
    if (input_data[i][which_x] < min_x) whichbin_x = 0;
    else if (input_data[i][which_x] > max_x) whichbin_x = nbin_x - 1;
    else whichbin_x = (int)(((input_data[i][which_x] - min_x) / (nbin_x - 2)) + 1.0);

    binned_data[whichbin_x] += 1;
  }
}

void parse_file(std::ifstream& input_file, std::vector<std::vector <double>> &input_data, size_t &nrows, size_t &ncols)
{
  std::string line;
  std::vector<std::string> tokens;
  while (std::getline(input_file, line)) {
    boost::algorithm::split(tokens, line, boost::algorithm::is_any_of(" \t"), boost::algorithm::token_compress_off);
    std::vector<double>data(tokens.size(), 0);
    for (size_t i = 0; i < tokens.size(); i++) data[i] = atof(tokens[i].c_str());
    input_data.push_back(data);
    tokens.clear();
    data.clear();
  }
  nrows = input_data.size();
  ncols = input_data.front().size();
}

void find_minmax_1D(std::vector<std::vector <double>> &input_data, size_t col_x, double &min_x, double &max_x) {
  for (auto i : input_data) {
    min_x = (i[col_x] < min_x ? i[col_x] : min_x);
    max_x = (i[col_x] > max_x ? i[col_x] : max_x);
  }
}

void find_minmax_2D(std::vector<std::vector <double>> &input_data, size_t col_x, double &min_x, double &max_x, size_t col_y, double &min_y, double &max_y, double &min_tot, double &max_tot) {
  for (auto i : input_data) {
    min_x = (i[col_x] < min_x ? i[col_x] : min_x);
    max_x = (i[col_x] > max_x ? i[col_x] : max_x);
    min_y = (i[col_y] > min_y ? i[col_y] : min_y);
    max_y = (i[col_y] > max_y ? i[col_y] : max_y);
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
  for (auto i : binned_data) {
    for (auto j : i) {
      output_file << min_x << "\t" << min_y <<  "\t" << j << std::endl;
      min_y += size_y;
    }
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

  std::string input_file_name, parameter_file_name, output_name;
  bool twod = false;

  if (argc > 2) { /* Parse arguments, if there are arguments supplied */
    for (int i = 1; i < argc; i++) {
      if ((argv[i][0] == '-') || (argv[i][0] == '/')) {       // switches or options...
        switch (tolower(argv[i][1])) {
        case 'i':
          input_file_name = argv[++i];
          break;
        case 'p':
          parameter_file_name = argv[++i];
          break;
        default:    // no match...
          std::cout << "ERROR: Flag \"" << argv[i] << "\" not recognized. Quitting..." << std::endl;
          usage(argv[0]);
        }
      }
      else {
        std::cout << "ERROR: Flag \"" << argv[i] << "\" not recognized. Quitting..." << std::endl;
        usage(argv[0]);
      }
    }
  }
  else {
    std::cout << "ERROR: No flags specified. Read usage and relaunch properly." << std::endl;
    usage(argv[0]);
  }

  //Safety checks for file manipulations
  std::ofstream output_file;
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

  input_file.open(input_file_name.c_str());
  if (!input_file.is_open()) {
    std::cout << "FAILED: Input file " << input_file_name << " could not be opened. Quitting..." << std::endl;
    exit(222);
  }
  else { std::cout << "SUCCESS: file " << input_file_name << " opened!\n"; }
  std::vector<std::vector<double>> input_data;
  size_t nrows, ncols;
  parse_file(input_file, input_data, nrows, ncols);
  input_file.close();

  output_file.open(output_name.c_str());
  if (!output_file.is_open()) {
    std::cout << "FAILED: Output file " << output_name << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }
  else { std::cout << "SUCCESS: file " << output_name << " opened!" << std::endl; }

  size_t col_acc_x, col_acc_y, nbin_x, nbin_y;
  double min_acc_x = std::numeric_limits<double>::max(), max_acc_x = std::numeric_limits<double>::lowest(), min_acc_y = std::numeric_limits<double>::max(), max_acc_y = std::numeric_limits<double>::lowest();
  double min_acc = std::numeric_limits<double>::max(), max_acc = std::numeric_limits<double>::lowest();
  bool enable_find_minmax;
  bool enable_2D;
  
  enable_find_minmax = parameters.has_member("enable_find_minmax") ? parameters["enable_find_minmax"].as<bool>() : true;
  enable_2D = parameters.has_member("enable_2D") ? parameters["enable_2D"].as<bool>() : true;

  col_acc_x = parameters.has_member("col_acc_x") ? parameters["col_acc_x"].as<size_t>() - 1 : 0;
  col_acc_y = parameters.has_member("col_acc_y") ? parameters["col_acc_y"].as<size_t>() - 1 : 1;
  col_acc_x = (col_acc_x >= 0 ? col_acc_x : 0);
  col_acc_y = (col_acc_y >= 0 ? col_acc_y : 0);

  nbin_x = parameters.has_member("nbin_x") ? parameters["nbin_x"].as<size_t>() : 100;
  nbin_y = parameters.has_member("nbin_y") ? parameters["nbin_y"].as<size_t>() : 100;
  nbin_x = (nbin_x > 0 ? nbin_x : 100);
  nbin_y = (nbin_y > 0 ? nbin_y : 100);

  min_acc_x = parameters.has_member("min_acc_x") ? parameters["min_acc_x"].as<size_t>() : std::numeric_limits<double>::max();
  max_acc_x = parameters.has_member("max_acc_x") ? parameters["max_acc_x"].as<size_t>() : std::numeric_limits<double>::lowest();
  if (min_acc_x > max_acc_x) {
    double temp = min_acc_x;
    min_acc_x = max_acc_x;
    max_acc_x = temp;
  }

  min_acc_y = parameters.has_member("min_acc_y") ? parameters["min_acc_y"].as<size_t>() : std::numeric_limits<double>::max();
  max_acc_y = parameters.has_member("max_acc_y") ? parameters["max_acc_y"].as<size_t>() : std::numeric_limits<double>::lowest();
  if (min_acc_y > max_acc_y) {
    double temp = min_acc_y;
    min_acc_y = max_acc_y;
    max_acc_y = temp;
  }

  min_acc = parameters.has_member("min_acc") ? parameters["min_acc"].as<size_t>() : std::numeric_limits<double>::max();
  max_acc = parameters.has_member("max_acc") ? parameters["max_acc"].as<size_t>() : std::numeric_limits<double>::lowest();
  if (min_acc > max_acc) {
    double temp = min_acc;
    min_acc = max_acc;
    max_acc = temp;
  }


  if (enable_2D) {
    if (enable_find_minmax) find_minmax_2D(input_data, col_acc_x, min_acc_x, max_acc_x, col_acc_y, min_acc_y, max_acc_y, min_acc, max_acc);
    std::vector<std::vector<size_t>> binned_data(nbin_y, std::vector<size_t>(nbin_x));
    bin_data_2D(input_data, nrows, ncols, col_acc_x, col_acc_y, min_acc_x, max_acc_x, min_acc_y, max_acc_y, binned_data, nbin_x, nbin_y);
    print_histo_2D(output_file, binned_data, min_acc_x, min_acc_y, (max_acc_x - min_acc_x) / (nbin_x - 2), (max_acc_y - min_acc_y) / (nbin_y - 2));
  }
  else {
    if (enable_find_minmax) find_minmax_1D(input_data, col_acc_x, min_acc, max_acc);
    std::vector<size_t> binned_data(nbin_x);
    bin_data_1D(input_data, nrows, ncols, col_acc_x, min_acc_x, max_acc_x, binned_data, nbin_x);
    print_histo_1D(output_file, binned_data, min_acc_x, (max_acc_x-min_acc_x)/(nbin_x-2));
  }

  return 0;

}


