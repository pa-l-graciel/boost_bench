/*
   Simple echo server example
*/

#include <iostream>
#include <boost/asio.hpp>

const char SERVER_IP[] = "127.0.0.1";
const unsigned short PORT_NUMBER = 31400;

int main() {
  // io_service: 커널에서 발생한 I/O 이벤트를 디스 패치해주는 클래스
  // 접속 받기/하기, 데이터 받기/보내기 이벤트를 알 수 있다
  // socket과 같은 객체 생성 시 io_service를 인자로 넘겨줌
  boost::asio::io_service io_service;

  // endpoint: 네트워크 주소를 설정한다(이 주소로 클라이언트가 접속)
  //
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(),
                                          PORT_NUMBER);

  // acceptor: 클라이언트의 접속을 받아들임 (io_service와 endpoint를 인자로
  // 받음)
  boost::asio::ip::tcp::acceptor acceptor(io_service, endpoint);

  // socket: 접속한 클라이언트에 할당할 소켓
  // 클라이언트와 메세지를 주고 받으므로, io_service를 인자로 넣음
  boost::asio::ip::tcp::socket socket(io_service);

  // 클라이언트의 접속을 기다림 (동기방식)
  acceptor.accept(socket);
  std::cout << "클라이언트 접속"
            << "\n";

  for (;;) {
    std::array<char, 128> buf;
    buf.fill(0);
    boost::system::error_code error;

    // read_some(): 클라이언트가 보낸 데이터를 받는다
    size_t len = socket.read_some(boost::asio::buffer(buf), error);

    if (error) {
      // 에러 발생 시, 진짜 에러인지, 접속이 끊긴문제인지를 꼭 구분해야 한다
      if (error == boost::asio::error::eof) {
        std::cout << "클라이언트와 연결이 끊어졌습니다"
                  << "\n";
      } else {
        std::cout << "error No: " << error.value()
                  << " error Message: " << error.message() << "\n";
      }
      break;
    }

    std::cout << "클라이언트에서 받은 메세지: " << &buf[0] << std::endl;
    char szMessage[128] = {
        0,
    };
    snprintf(szMessage, 128 - 1, "Re: %s", &buf[0]);
    int nMsgLen = strnlen(szMessage, 128 - 1);

    boost::system::error_code ignored_error;

    // write_some() : 클라이언트에 메세지를 보냄
    // asio:buffer는 char 뿐 아니라,  array, vector, string 등 STL과 호환이 잘
    // 된다
    socket.write_some(boost::asio::buffer(szMessage, nMsgLen), ignored_error);
    std::cout << "클라이언트에 보낸 메세지: " << szMessage << std::endl;
  }
  getchar();

  return 0;
}
