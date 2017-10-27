// Copyr:ight Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

/* Shows how to use both command line and config file. */

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <fstream>
#include <iterator>
// using namespace std;

// A helper function to simplify the main part.
template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
  std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
  return os;
}

using std::string;
using std::ifstream;
using std::cout;
using std::vector;

int main(int ac, char* av[]) {
  try {
    int opt;
    string config_file;
    int maxCons;

    // Declare a group of options that will be
    // allowed only on command line
    po::options_description generic("Generic options");
    generic.add_options()
      ("version,v", "print version string")
      ("help", "produce help message")
      ("config,c", po::value<string>(&config_file)->default_value("multiple_sources.cfg"),
        "name of a file of a configuration.");

    // Declare a group of options that will be
    // allowed both on command line and in
    // config file
    po::options_description config("Configuration");
    config.add_options()
      ("optimization", po::value<int>(&opt)->default_value(10), "optimization level")
      ("include-path,I", po::value<vector<string> >()->composing(),"include path")
      ("max-connections,m", po::value<int>(&maxCons)->default_value(200), "maximum # of connections");

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()("input-file", po::value<std::vector<string> >(),
                         "input file");

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    po::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    po::options_description visible("Allowed options");
    visible.add(generic).add(config);

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    store(po::command_line_parser(ac, av)
              .options(cmdline_options)
              .positional(p)
              .run(),
          vm);
    notify(vm);

    ifstream ifs(config_file.c_str());
    if (!ifs) {
      cout << "can not open config file: " << config_file << "\n";
      return 0;
    } else {
      store(parse_config_file(ifs, config_file_options), vm);
      notify(vm);
    }

    if (vm.count("help")) {
      cout << visible << "\n";
      return 0;
    }

    if (vm.count("version")) {
      cout << "Multiple sources example, version 1.0\n";
      return 0;
    }

    if (vm.count("include-path")) {
      cout << "Include paths are: "
           << vm["include-path"].as<std::vector<string> >() << "\n";
    }

    if (vm.count("input-file")) {
      cout << "Input files are: " << vm["input-file"].as<std::vector<string> >()
           << "\n";
    }

    cout << "Optimization level is " << opt << "\n";
    cout << "Maximum # of connections is " << maxCons << "\n";
  } catch (std::exception& e) {
    cout << e.what() << "\n";
    return 1;
  }
  return 0;
}
