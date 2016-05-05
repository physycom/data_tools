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


#define MAJOR_VERSION 1
#define MINOR_VERSION 3
#define SEPARATORS       " \t"
#define COMMENTS         "#"


void prepare_gnuplot_script_1D_twoplots(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t x, size_t y, size_t z, size_t t, size_t q, std::string t_key, std::string q_key, std::string x_key, std::string y_key, std::string title_key) {
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

void prepare_gnuplot_script_1D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t x, size_t y, std::string x_key, std::string y_key, std::string plot_key, std::string title_key) {
  output_file << "#!/gnuplot\n";
  output_file << "FILE_IN='" << data_file << "'\n";
  output_file << "FILE_OUT='" << plot_file << "'\n";
  output_file << "set terminal pngcairo size " << Xres << ',' << Yres << " font \"," << fontsize << "\"\n";
  output_file << "set output FILE_OUT\n";
  output_file << "set title '" << title_key << "'\n";
  output_file << "set xlabel '" << x_key << "'\n";
  output_file << "set ylabel '" << y_key << "'\n";
  output_file << "plot FILE_IN u " << x << ":" << y << " w lines lt 1 lc rgb 'blue' lw 3 t '" << plot_key << "',\\" << std::endl;
  output_file << "\n";
}

bool Belongs_to(char c, std::string s) {
  for (size_t i = 0; i < s.size(); i++) { if (c == s.at(i)) return true; }
  return false;
}


std::vector< std::vector<double> > Convert_to_double_vector(std::vector< std::vector<std::string> > parsed_file) {
  std::vector<double> doubled_line;
  std::vector< std::vector<double> > doubled_file;

  for (auto &i : parsed_file) {
    doubled_line.clear();
    doubled_line.resize(i.size());
    for (size_t j = 0; j < i.size(); j++) doubled_line[j] = atof(i[j].c_str());
    doubled_file.push_back(doubled_line);
  }
  return doubled_file;
}


std::vector< std::vector<std::string> > Parse_file(std::string file_name, std::string separators, std::string comment) {
  // Safe file opening
  std::ifstream file_to_parse(file_name, std::ios::in);
  if (!file_to_parse) {
    std::cout << "Cannot open " << file_name << ". Quitting..." << std::endl;
    exit(12);
  }
  // Internal variables
  std::string line;
  std::vector<std::string> tokens;
  std::vector< std::vector<std::string> > parsed;
  while (getline(file_to_parse, line)) {
    boost::algorithm::trim(line);  // remove leading/trailing spaces
    if (Belongs_to(line[0], comment) || !line.size()) continue;
    boost::algorithm::split(tokens, line, boost::algorithm::is_any_of(separators), boost::token_compress_on);
    //std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);
    for (size_t i = 0; i < tokens.size(); i++) {  // remove inline comments
      if (Belongs_to(tokens[i][0], comment)) { tokens.erase(tokens.begin() + i, tokens.end()); }
    }
    if (tokens.size()) {
      parsed.push_back(tokens);
    }
    line.clear(); tokens.clear();
  }
  file_to_parse.close();
  return parsed;
}

double Calculate_average(std::vector< std::vector<double> > doubled_file, int col_number) {
  double average = 0;
  for (auto &i : doubled_file) {
    average += i[col_number];
  }
  average /= (double)(doubled_file.size());
  return average;
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

  std::cout << std::endl << "Averaging your files with period " << period << " s" << std::endl;

  for (auto file : input_files) {
    std::vector< std::vector<std::string> > parsed_file = Parse_file(file, SEPARATORS, COMMENTS);
    std::vector< std::vector<double> > doubled_file = Convert_to_double_vector(parsed_file);
    double average_ax = Calculate_average(doubled_file, 2);
    double average_ay = Calculate_average(doubled_file, 3);
    double average_az = Calculate_average(doubled_file, 4);
    double forzante = std::stod(file.substr(file.size() - 17, 3));

    std::vector<double> averaged_value;
    std::vector< std::vector<double> > averaged_values;
    size_t col_num = 0, line_cnt = 0, period_cnt = 0;
    double start_time = -1.0;
    double first_time = -1.0;

    for (auto line : doubled_file) {
      line_cnt++;
      period_cnt++;

      if (start_time < 0.0) start_time = first_time = line[0];

      if (col_num == 0) {
        col_num = line.size() + 2; // the last column contains again the timestamp, but starting from zero for the first row
        averaged_value.resize(col_num);
        for (size_t i = 0; i < col_num; i++) {
          averaged_value[i] = 0.0;
        }
      }

      for (size_t i = 0; i < col_num - 2; i++) {
        averaged_value[i] += line[i];
      }
      averaged_value[col_num - 2] += (line[2] - average_ax) * (line[2] - average_ax) + (line[3] - average_ay) * (line[3] - average_ay) + (line[4] - average_az) * (line[4] - average_az);

      if (line[0] - start_time > period) {
        start_time = line[0];
        for (size_t i = 0; i < col_num - 1; i++) {
          averaged_value[i] /= period_cnt;
        }
        averaged_value[col_num - 2] *= (2.0/forzante);
        averaged_value[col_num - 1] = averaged_value[0] - first_time;
        averaged_values.push_back(averaged_value);
        for (size_t i = 0; i < col_num; i++) {
          averaged_value[i] = 0.0;
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
    prepare_gnuplot_script_1D(file_out, file.substr(0, file.size() - 4) + "_ave.txt", file.substr(0, file.size() - 4) + "_ave.png", 1280, 720, 20, col_num, col_num - 1, "average of modulus", "t (s)", "2*osc^2/g_0 (g)", file.substr(0, file.size() - 13));
  }

  return 0;
}



