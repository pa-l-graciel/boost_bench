/*
   Simple JSON test.
   youngsu_heo@tmax.co.kr
*/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
namespace pt = boost::property_tree;
using namespace std;
using namespace boost;

int main() {
  pt::ptree config;
  pt::read_json("input.json", config);

  cout << "bitrate = " << config.get<int>("data.bitrate", 12) << "\n";
  cout << "qp = " << config.get<int>("data.qp", 25) << "\n";

  pt::write_json("output.json", config);

  return 0;
}
