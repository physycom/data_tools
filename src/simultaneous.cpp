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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

#define MAJOR_VERSION                       0
#define MINOR_VERSION                       1
#define NUMBER_OF_COLUMNS_IN_INERTIAL_FILES 10
#define TIMESTAMP_COLUMN                    1
#define TIMESTAMP_REL_COLUMN                10
#define XRES                                1280
#define YRES                                720
#define FONTSIZE                            10

double start_time;

void prepare_gnuplot_script(std::ofstream &output_file, std::string output_file_png, std::vector<std::string> file_names, std::vector<std::string> device_names, size_t Xres, size_t Yres, size_t fontsize, size_t column, std::string title_key) {
  output_file << "#!/gnuplot\n";
  for (size_t n = 0; n < file_names.size(); n++) {
    output_file << "FILE_IN_" << n << "='" << file_names[n] << "'\n";
  }
  output_file << "FILE_OUT='" << output_file_png << "'\n";
  output_file << "set terminal pngcairo dashed size " << Xres << ',' << Yres << " enhanced font 'Verdana," << fontsize << "'\n";
  output_file << "set output FILE_OUT\n";
  output_file << "# Styles\n";
  output_file << "linew = 1.2\n";
  output_file << "set style line  1  lc rgb '#0072bd' lt 7 lw linew  # blue\n";
  output_file << "set style line  2  lc rgb '#d95319' lt 7 lw linew  # orange\n";
  output_file << "set style line  3  lc rgb '#77ac30' lt 7 lw linew  # green\n";
  output_file << "set style line  4  lc rgb '#a2142f' lt 7 lw linew  # red\n";
  output_file << "set style line  5  lc rgb '#edb120' lt 7 lw linew  # yellow\n";
  output_file << "set style line  6  lc rgb '#7e2f8e' lt 7 lw linew  # purple\n";
  output_file << "set style line  7  lc rgb '#4dbeee' lt 7 lw linew  # lightblue\n";
  output_file << "set style line 100 lc rgb '#d6d7d9' lt 1 lw 1      # gray\n";
  output_file << "# Grid\n";
  output_file << "set grid xtics ytics back ls 100\n";
  output_file << "# Titles\n";
  output_file << "set key opaque\n";
  output_file << "set title '" << title_key << "'\n";
  output_file << "set xlabel 't_{rel} (s)' \n";
  output_file << "set ylabel '" << title_key << "'\n";
  output_file << "plot ";
  for (size_t n = 0; n < file_names.size(); n++) {
    //output_file << "FILE_IN_" << n << " u " << TIMESTAMP_REL_COLUMN << ':' << column << " w lines ls " << n + 1 << " t '" << device_names[n] << "'";
    output_file << "FILE_IN_" << n << " u ($" << TIMESTAMP_COLUMN << '-' << start_time << "):" << column << " w lines ls " << n + 1 << " t '" << device_names[n] << "'";
    if (n < file_names.size() - 1) output_file << ", \\\n";
  }
  output_file << "\n";
}


bool check_number_of_columns(std::ifstream& input_file) {
  std::string line;
  std::vector<std::string> tokens;
  std::getline(input_file, line);
  std::getline(input_file, line);
  std::getline(input_file, line);
  boost::algorithm::split(tokens, line, boost::algorithm::is_any_of("\t"), boost::algorithm::token_compress_off);
  if (tokens.size() != NUMBER_OF_COLUMNS_IN_INERTIAL_FILES) {
    std::cerr << "Non conforming file, wrong number of columns, found " << tokens.size() << '/' << NUMBER_OF_COLUMNS_IN_INERTIAL_FILES << " columns." << std::endl;
    return false;
  }
  else {
    double time = 0;
    try {
      time = std::stod(tokens[TIMESTAMP_COLUMN - 1]);
      if (time < start_time) start_time = time;
    }
    catch (std::exception &e) {
      std::cout << e.what() << std::endl;
      exit(-55);
    }
  }
  tokens.clear();
  return true;
}


void usage(char* progname) {
  std::cerr << "Usage: " << progname << " [plt_files_basename] [input_file_1.txt] [input_file_2.txt] [input_file_3.txt] [...]" << std::endl << std::endl;
  std::cerr << "This tool creates 1D plots overlapping data from same column on different inertial files, using the timestamp_rel column as the x axis" << std::endl;
  std::cerr << "and preparing multiple gnuplot scripts with the common basename plt_files_basename." << std::endl;
}


int main(int argc, char** argv) {
  std::cout << "simultaneous v" << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;
  start_time = 2500000000.0;

  if (argc < 3) {
    std::cerr << "At least two arguments are required: plt_files_basename and one inertial file is required." << std::endl;
    std::cerr << "The tool is meaningful anyway with 2+ inertial files." << std::endl;
    usage(argv[0]);
    exit(1);
  }

  if (argc > 9) {
    std::cerr << "A maximum of 7 files on the same plot is supported." << std::endl;
    std::cerr << "Not enough colours defined." << std::endl;
    usage(argv[0]);
    exit(1);
  }

  std::vector<std::string> input_file_names;
  for (size_t i = 2; i < (size_t)argc; i++) {
    input_file_names.push_back(argv[i]);
    std::ifstream file;
    file.open(argv[i]);
    if (!file.is_open()) {
      std::cerr << "FAILED: input file " << argv[i] << " couldn't be opened. Quitting..." << std::endl;
      exit(222);
    }
    std::cout << "File: " << argv[i] << ": ";
    if (check_number_of_columns(file)) {
      std::cout << "VALID" << std::endl;
    }
    else {
      exit(-56);
    }
    file.close();
  }

  std::string input_file_basename = argv[1];
  std::string output_file_name_plt_ax, output_file_name_plt_ay, output_file_name_plt_az, output_file_name_plt_gx, output_file_name_plt_gy, output_file_name_plt_gz, output_file_name_plt_amod;
  std::string output_file_name_png_ax, output_file_name_png_ay, output_file_name_png_az, output_file_name_png_gx, output_file_name_png_gy, output_file_name_png_gz, output_file_name_png_amod;
  std::ofstream output_file_plt_ax, output_file_plt_ay, output_file_plt_az, output_file_plt_gx, output_file_plt_gy, output_file_plt_gz, output_file_plt_amod;

  std::vector<std::string> device_names;
  for (auto file_name : input_file_names) {
    std::vector<std::string> tokens;
    boost::algorithm::split(tokens, file_name, boost::algorithm::is_any_of("_"), boost::algorithm::token_compress_off);
    device_names.push_back(tokens[0] + " " + tokens[1]);
  }

  output_file_name_png_ax = input_file_basename + "_ax.png";
  output_file_name_png_ay = input_file_basename + "_ay.png";
  output_file_name_png_az = input_file_basename + "_az.png";
  output_file_name_png_gx = input_file_basename + "_gx.png";
  output_file_name_png_gy = input_file_basename + "_gy.png";
  output_file_name_png_gz = input_file_basename + "_gz.png";
  output_file_name_png_amod = input_file_basename + "_amod.png";

  /* Check all output files */
  output_file_name_plt_ax = input_file_basename + "_ax.plt";
  output_file_name_plt_ay = input_file_basename + "_ay.plt";
  output_file_name_plt_az = input_file_basename + "_az.plt";
  output_file_name_plt_gx = input_file_basename + "_gx.plt";
  output_file_name_plt_gy = input_file_basename + "_gy.plt";
  output_file_name_plt_gz = input_file_basename + "_gz.plt";
  output_file_name_plt_amod = input_file_basename + "_amod.plt";

  output_file_plt_ax.open(output_file_name_plt_ax);
  if (!output_file_plt_ax.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_name_plt_ax << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }

  output_file_plt_ay.open(output_file_name_plt_ay);
  if (!output_file_plt_ay.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_name_plt_ay << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }

  output_file_plt_az.open(output_file_name_plt_az);
  if (!output_file_plt_az.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_name_plt_az << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }

  output_file_plt_gx.open(output_file_name_plt_gx);
  if (!output_file_plt_gx.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_name_plt_gx << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }

  output_file_plt_gy.open(output_file_name_plt_gy);
  if (!output_file_plt_gy.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_name_plt_gy << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }

  output_file_plt_gz.open(output_file_name_plt_gz);
  if (!output_file_plt_gz.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_name_plt_gz << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }

  output_file_plt_amod.open(output_file_name_plt_amod);
  if (!output_file_plt_amod.is_open()) {
    std::cerr << "FAILED: Output file " << output_file_name_plt_amod << " could not be opened. Quitting..." << std::endl;
    exit(333);
  }

  prepare_gnuplot_script(output_file_plt_ax, output_file_name_png_ax, input_file_names, device_names, XRES, YRES, FONTSIZE, 3, "a_x");
  prepare_gnuplot_script(output_file_plt_ay, output_file_name_png_ay, input_file_names, device_names, XRES, YRES, FONTSIZE, 4, "a_y");
  prepare_gnuplot_script(output_file_plt_az, output_file_name_png_az, input_file_names, device_names, XRES, YRES, FONTSIZE, 5, "a_z");
  prepare_gnuplot_script(output_file_plt_gx, output_file_name_png_gx, input_file_names, device_names, XRES, YRES, FONTSIZE, 6, "g_x");
  prepare_gnuplot_script(output_file_plt_gy, output_file_name_png_gy, input_file_names, device_names, XRES, YRES, FONTSIZE, 7, "g_y");
  prepare_gnuplot_script(output_file_plt_gz, output_file_name_png_gz, input_file_names, device_names, XRES, YRES, FONTSIZE, 8, "g_z");
  prepare_gnuplot_script(output_file_plt_amod, output_file_name_png_amod, input_file_names, device_names, XRES, YRES, FONTSIZE, 9, "|a|");

  output_file_plt_ax.close();
  output_file_plt_ay.close();
  output_file_plt_az.close();
  output_file_plt_gx.close();
  output_file_plt_gy.close();
  output_file_plt_gz.close();
  output_file_plt_amod.close();
  return 0;
}


