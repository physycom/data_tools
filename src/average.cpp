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
#include <iomanip>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

//#include "jsoncons/json.hpp"

#define MAJOR_VERSION 1
#define MINOR_VERSION 2

void prepare_gnuplot_script_1D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t x, size_t y, size_t z, size_t t, size_t q, std::string t_key, std::string q_key, std::string x_key, std::string y_key, std::string title_key) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN='" << data_file << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo size " << Xres << ',' << Yres << " font \"," << fontsize << "\"\n";
  output_file << "set output FILE_OUT\n";
  output_file << "set title '" << title_key << "'\n";
  output_file << "set xlabel '" << x_key << "'\n";
  output_file << "set ylabel '" << y_key << "'\n";
  output_file << "plot FILE_IN u " << t << ":(sqrt($" << x << "*$" << x << "+$" << y << "*$" << y << "+$" << z << "*$" << z << ")) w lines lt 1 lc rgb 'blue' lw 3 t '" << t_key << "',\\" << std::endl;
  output_file << "     FILE_IN u " << t << ":" << q << " w lines lt 1 lc rgb 'red' lw 3 t '" << q_key << "'\n";
  output_file << "\n";
}

void usage(char * progname) {
  std::cout << "Usage: " << progname << " -period <time_interval_to_average[s]> file1 file2 ... fileN" << std::endl;
  std::cout << "or     " << progname << " -samples <number_of_lines_to_average> file1 file2 ... fileN" << std::endl;
  std::cout << "It is assumed that the first column represents timestamps. All the others will be averaged" << std::endl;
  exit(-3);
}

int main(int argc, char ** argv) {
  std::cout << "Average calculator v" << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;

  std::vector<std::string> input_files;
  double period;
  int samples;
  if (argc > 1) {
    if (std::string(argv[1]) == "-period") {
      period = atof(argv[2]);
    }
    else if (std::string(argv[1]) == "-samples") {
      samples = atoi(argv[2]);
    }

    for (int i = 3; i < argc; i++) {
      input_files.push_back(argv[i]);
    }
  }
  else {
    std::cout << "ERROR: Wrong command line parameters. Read usage and relaunch properly." << std::endl;
    usage(argv[0]);
  }

  for (auto file : input_files) {
    std::ifstream file_in(file);
    if (!file_in) {
      std::cout << "ERROR: unable to open " << file << std::endl;
      exit(-2);
    }
    else std::cout << std::endl << "Averaging \"" << file << "\" with period " << period << " s" << std::endl;

    std::string line;
    std::vector<std::string> tokens;
    std::vector<double> values;
    std::vector< std::vector<double> > averaged_values;
    int col_num = 0, line_cnt = 0, period_cnt = 0;
    double start_time = -1.0;
    double first_time = -1.0;
    while (std::getline(file_in, line)) {
      boost::split(tokens, line, boost::is_any_of(" \t"), boost::token_compress_on);
      if (tokens[0][0] == '#') continue;
      line_cnt++;
      period_cnt++;

      if (start_time < 0.0) start_time = first_time = std::stod(tokens[0]);

      if (col_num == 0) {
        col_num = (int)tokens.size()+1; // the last column contains again the timestamp, but starting from zero for the first row
        values.resize(col_num);
        for (int i = 0; i < col_num; i++) {
          values[i] = 0;
        }
      }

      for (int i = 0; i < col_num-1; i++) {
        values[i] += std::stod(tokens[i]);
      }

      if (std::stod(tokens[0]) - start_time > period) {
        start_time = std::stod(tokens[0]);
        for (int i = 0; i < col_num-1; i++) {
          values[i] /= period_cnt;
        }
        values[col_num - 1] = values[0] - first_time;
        averaged_values.push_back(values);
        for (int i = 0; i < col_num; i++) {
          values[i] = 0;
        }
        period_cnt = 0;
      }
    }

    std::cout << "Report \"" << file << "\" : lines " << line_cnt << " averaged " << averaged_values.size() << std::endl;

    std::ofstream file_out(file.substr(0, file.size() - 4) + "_ave.txt");
    for (auto av : averaged_values) {
      for (auto value : av) {
        file_out << std::fixed << std::setprecision(4) << value << "\t";
      }
      file_out << std::endl;
    }
    file_out.close();

    file_out.open(file.substr(0, file.size() - 4) + "_ave.plt");
    prepare_gnuplot_script_1D(file_out, file.substr(0, file.size() - 4) + "_ave.txt", file.substr(0, file.size() - 4) + "_ave.png", 1280, 720, 20, 3, 4, 5, col_num, col_num-1, "modulus of average", "average of modulus", "t (s)", "a (g)", file.substr(0, file.size() - 13));
  }

  return 0;
}



