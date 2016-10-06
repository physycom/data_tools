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


#define MAJOR_VERSION    2
#define MINOR_VERSION    2
#define SEPARATORS       " \t"
#define COMMENTS         "#"


void prepare_gnuplot_script_1D_twoplots(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t x1, size_t y1, size_t x2, size_t y2, std::string plot1_key, std::string plot2_key, std::string x1_key, std::string y1_key, std::string x2_key, std::string y2_key, std::string title_key) {
  output_file << R"(#!/gnuplot
FILE_IN=')" << data_file << R"('
FILE_OUT=')" << plot_file << R"('
set terminal pngcairo dashed size )" << Xres << "," << Yres << " enhanced font 'Verdana," << fontsize << R"('
set output FILE_OUT
# Styles
linew = 1.2
set style line  21 lc rgb '#0072bd' lt 7 lw linew  # blue
set style line  22 lc rgb '#d95319' lt 7 lw linew  # orange
set style line  23 lc rgb '#77ac30' lt 7 lw linew  # green
set style line  24 lc rgb '#a2142f' lt 7 lw linew  # red
set style line 102 lc rgb '#d6d7d9' lt 1 lw 1      # gray
# Grid
set grid xtics ytics back ls 102
# Titles
set key opaque
set title 'Power Spectrum: )" << title_key << R"('
set xlabel ')" << x1_key << R"('
set ylabel ')" << y1_key << R"('
set x2label ')" << x2_key << R"('
set y2label ')" << y2_key << R"('
set ytics nomirror
set y2tics
plot FILE_IN u )" << x1 << ":" << y1 << " w lines ls 21 t '" << plot1_key << R"(' axes x1y1,\
     FILE_IN u )" << x2 << ":" << y2 << " w lines ls 24 t '" << plot2_key << "' axes x1y2\n";
}

void prepare_gnuplot_script_1D(std::ofstream &output_file, std::string data_file, std::string plot_file, size_t Xres, size_t Yres, size_t fontsize, size_t x, size_t y, std::string x_key, std::string y_key, std::string plot_key, std::string title_key) {
  output_file << R"(#!/gnuplot
FILE_IN=')" << data_file << R"('
FILE_OUT=')" << plot_file << R"('
set terminal pngcairo dashed size )" << Xres << "," << Yres << " enhanced font 'Verdana," << fontsize << R"('
set output FILE_OUT
# Styles
linew = 1.2
set style line  21 lc rgb '#0072bd' lt 7 lw linew  # blue
set style line  22 lc rgb '#d95319' lt 7 lw linew  # orange
set style line  23 lc rgb '#77ac30' lt 7 lw linew  # green
set style line  24 lc rgb '#a2142f' lt 7 lw linew  # red
set style line 102 lc rgb '#d6d7d9' lt 1 lw 1      # gray
# Grid
set grid xtics ytics back ls 102
# Titles
set key opaque
set title 'Power Spectrum: )" << title_key << R"('
set xlabel ')" << x_key << R"('
set ylabel ')" << y_key << R"('
set ytics nomirror
set y2tics
plot FILE_IN u )" << x << ":" << y << " w lines ls 21 t '" << plot_key << std::endl;
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
    std::cerr << "Cannot open " << file_name << ". Quitting..." << std::endl;
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

double Calculate_average(std::vector< std::vector<double> > doubled_file, size_t col_number) {
  double average = 0;
  for (auto &i : doubled_file) {
    average += i[col_number];
  }
  average /= (double)(doubled_file.size());
  return average;
}


void usage(char * progname) {
  std::cerr << "Usage: " << progname << " -period <time_interval_to_average[s]> -file filename1 -file filename2 ..." << std::endl;
  std::cerr << "If not defined, period by default is 1.0 s." << std::endl;
  std::cerr << "It is assumed that the first column represents timestamps. The second, third and fourth columns" << std::endl;
  std::cerr << "will be averaged if not specified with -ave i -ave j -ave k, repeating the -ave command as many times" << std::endl;
  std::cerr << "as necessary" << std::endl;
}

int main(int argc, char ** argv) {
  std::cout << "Average calculator v" << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;

  std::vector<std::string> input_files;
  std::vector<size_t> columns_to_be_averaged;
  double period = 1.0;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-period") {
      period = atof(argv[++i]);
    }
    else if (std::string(argv[i]) == "-ave") {
      columns_to_be_averaged.push_back(atoll(argv[++i]) - 1);
    }
    else if (std::string(argv[i]) == "-file") {
      input_files.push_back(argv[++i]);
    }
    else {
      std::cerr << "Unable to understand parameter " << argv[i] << std::endl;
    }
  }

  std::cout << "Averaging your files with period " << period << " s" << std::endl;

  if (columns_to_be_averaged.size() == 0) {
    columns_to_be_averaged.push_back(1);
    columns_to_be_averaged.push_back(2);
    columns_to_be_averaged.push_back(3);
  }
  std::cout << "Averaging columns # ";
  for (auto i : columns_to_be_averaged) std::cout << i << " ";
  std::cout << std::endl;

  for (auto file : input_files) {
    std::vector< std::vector<std::string> > parsed_file = Parse_file(file, SEPARATORS, COMMENTS);
    std::vector< std::vector<double> > doubled_file = Convert_to_double_vector(parsed_file);

    double normalization = 2.0;
    double driving = 1.0;
    size_t start_pos = 0;
    size_t end_pos = 0;
    try {
      start_pos = file.find("--");
      end_pos = file.find("g", start_pos);
      driving = std::stod(file.substr(start_pos + 2, file.size() - end_pos - 1));
    }
    catch (std::exception &e) {
      std::cerr << e.what() << " - the filename does not respect average.exe standard. Removing normalization" << std::endl;
      normalization = 1.0;
    }

    std::vector<double> average_columns;
    for (size_t i = 0; i < doubled_file.front().size(); i++) average_columns.push_back(Calculate_average(doubled_file, i));


    std::vector<double> averaged_value(doubled_file.front().size() + 3);
    std::vector< std::vector<double> > averaged_values;
    size_t line_cnt = 0, period_cnt = 0;
    double start_time = -1.0;
    double first_time = -1.0;

    for (auto line : doubled_file) {
      line_cnt++;
      period_cnt++;

      if (start_time < 0.0) start_time = first_time = line[0];

      for (size_t i = 0; i < doubled_file.front().size(); i++) {
        averaged_value[i] += line[i];
      }

      for (auto i : columns_to_be_averaged) averaged_value[doubled_file.front().size()] += (line[i] - average_columns[i]) / driving * (line[i] - average_columns[i]) / driving;

      if (line[0] - start_time > period) {
        start_time = line[0];
        for (size_t i = 0; i < doubled_file.front().size() + 1; i++) {
          averaged_value[i] /= period_cnt;
        }
        averaged_value[doubled_file.front().size()] *= normalization;
        averaged_value[doubled_file.front().size() + 1] = (double)period_cnt;
        averaged_value[doubled_file.front().size() + 2] = averaged_value[0] - first_time;
        averaged_values.push_back(averaged_value);
        for (size_t i = 0; i < doubled_file.front().size() + 3; i++) {
          averaged_value[i] = 0.0;
        }
        period_cnt = 0;
      }
    }

    std::cout << "Report \"" << file << "\" : lines " << line_cnt << " averaged " << averaged_values.size() << std::endl;

    std::string ave_data_name = file.substr(0, file.size() - 4) + "_ave.txt";
    std::ofstream file_out(ave_data_name);
    file_out << "#" << "In every column there's the average value of the same column in the original file" << std::endl;
    file_out << "#" << "First added column: squared average of the three defined columns (default 2-3-4, starting counting from 1)" << std::endl;
    file_out << "#" << "Second added column: number of rows used in averaging procedures" << std::endl;
    file_out << "#" << "Third added column: number of rows used in averaging procedures" << std::endl;

    for (auto av : averaged_values) {
      for (auto value : av) {
        file_out << std::fixed << std::setprecision(4) << value << "\t";
      }
      file_out << std::endl;
    }
    file_out.close();

    std::string plot_script_name = file.substr(0, file.size() - 4) + "_ave.plt",
      plot_image_name = file.substr(0, file.size() - 4) + "_ave.png",
      test_name = file.substr(0, file.size() - 4),
      escaped_filename = boost::replace_all_copy(file, "_", "\\_");

    file_out.open(plot_script_name);
    prepare_gnuplot_script_1D_twoplots(file_out, ave_data_name, plot_image_name, 1280, 720, 10, doubled_file.front().size() + 3, doubled_file.front().size() + 1, doubled_file.front().size() + 3, doubled_file.front().size() + 2, "average of magnitude", "acquisition frequency", "t (s)", "2*osc^2/g_0 (g)", "t (s)", "f (Hz)", escaped_filename);
    file_out.close();
  }

  return 0;
}



