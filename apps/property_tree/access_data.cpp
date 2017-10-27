/*
   test code of put and get method.
   youngsu_heo@tmax.co.kr
*/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
namespace pt = boost::property_tree;
using namespace std;

int main() {
  pt::ptree pt;

  // two ways of putting data
  pt.put("math.pi", 3.14159);  // 1. overwrite version
  pt.add("math.pi", 3.141);    // 2. add new node version

  // three versions of 'get' methods. only difference is the fail-handling
  double pi = pt.get<double>("math.pi");  // 1. throw version
  //  double pi = pt.get("math.pi", 3.14);    // 2. default-value version.
  //  useful for common situations
  //  boost::optional<double> pi = pt.get_optional<double>("math.pi");  // 3.
  //  optional version

  cout << "pi = " << pi << endl;
  return 0;
}
