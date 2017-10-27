// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

/* The simplest usage of the library.
 */

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>
using namespace std;

int main(int ac, char* av[]) {
  try {
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")(
        "compression", po::value<double>(), "set compression level")
        /// doc for po::value:
        /// http://www.boost.org/doc/libs/1_58_0/doc/html/boost/program_options/typed_value.html
        ("qp", po::value<int>()->default_value(25), "set qp");

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << desc << "\n";
      return 0;
    }

    if (vm.count("compression")) {
      cout << "Compression level was set to " << vm["compression"].as<double>()
           << ".\n";
    } else {
      cout << "Compression level was not set.\n";
    }

    if (vm.count("qp")) {
      cout << "qp was set to " << vm["qp"].as<int>() << ".\n";
    }

  } catch (exception& e) {
    cerr << "error: " << e.what() << "\n";
    return 1;
  } catch (...) {
    cerr << "Exception of unknown type!\n";
  }

  return 0;
}
