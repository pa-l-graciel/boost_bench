#include <iostream>
#include <boost/asio.hpp>

const char SERVER_IP[] = "127.0.0.1";
const unsigned short PORT_NUMBER = 31400;

int main() {
  boost::asio::io_service io_service;

  // 클라이언트를 위한 endpoint는, 접속할 서버의 IP주소도 인자로 줘야함
  boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::address::from_string(SERVER_IP), PORT_NUMBER);

  boost::system::error_code connect_error;
  boost::asio::ip::tcp::socket socket(io_service);

  // 서버로 접속을 시도하며, 접속이 성공 또는 실패할 때까지 대기상태가 됨
  socket.connect(endpoint, connect_error);

  if (connect_error) {
    std::cout << "연결 실패. error No: " << connect_error.value()
              << ", Message: " << connect_error.message() << std::endl;
    getchar();
    return 0;
  } else {
    std::cout << "서버에 연결 성공" << std::endl;
  }

  for (int i = 0; i < 7; ++i) {
    char szMessage[129] = {
        0,
    };
    snprintf(szMessage, 128 - 1, "%d - Send Message", i);
    int nMsgLen = strnlen(szMessage, 128 - 1);

    boost::system::error_code ignored_error;

    // 서버에 데이터 보내기
    socket.write_some(boost::asio::buffer(szMessage, nMsgLen), ignored_error);

    std::cout << "서버에 보낸 메세지: " << szMessage << std::endl;

    std::array<char, 128> buf;
    buf.fill(0);
    boost::system::error_code error;

    // 서버로부터 데이터 받기
    size_t len = socket.read_some(boost::asio::buffer(buf), error);

    if (error) {
      if (error == boost::asio::error::eof) {
        std::cout << "서버와 연결이 끊어졌습니다" << std::endl;
      } else {
        std::cout << "error No: " << error.value()
                  << " error Message: " << error.message().c_str() << std::endl;
      }
      break;
    }
    std::cout << "서버로부터 받은 메세지: " << &buf[0] << std::endl;
  }

  // 접속 끊기
  if (socket.is_open()) {
    socket.close();
  }
  getchar();
  return 0;
}
