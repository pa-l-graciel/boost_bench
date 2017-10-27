#include <iostream>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/config.hpp>
#include <boost/program_options/environment_iterator.hpp>
#include <boost/program_options/eof_iterator.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/version.hpp>

namespace po = boost::program_options;

int main() {
  /// print version info
  std::cout << "BOOST_PROGRAM_OPTIONS_VERSION:\t"
            << BOOST_PROGRAM_OPTIONS_VERSION << std::endl;

  // Declare the supported options.
  po::options_description desc("Allowed options");

  desc.add_options()("help", "produce help message")(
      "compression", po::value<int>(), "set compression level");
  /*
    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
          cout << desc << "\n";
              return 1;
    }

    if (vm.count("compression")) {
          cout << "Compression level was set to "
             << vm["compression"].as<int>() << ".\n";
    } else {
          cout << "Compression level was not set.\n";
    }
  */
  return 0;
}
