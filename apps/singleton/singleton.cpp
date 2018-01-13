#include <iostream>
#include <boost/serialization/singleton.hpp>

class Config : public boost::serialization::singleton<Config> {
 public:
  Config() { path_ = 100; }
  int getPath() const { return path_; }

 private:
  int path_;
};

int main() {
  std::cout << Config::get_const_instance().getPath() << std::endl;
  return 0;
}
