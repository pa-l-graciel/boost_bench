#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

const char SERVER_IP[] = "127.0.0.1";
const unsigned short PORT_NUMBER = 31400;

class TCP_Client {
 public:
  TCP_Client(boost::asio::io_service& io_service)
      : m_io_service(io_service), m_Socket(io_service), m_nSeqNumber(0) {}
  void Connect(boost::asio::ip::tcp::endpoint& endpoint) {
    m_Socket.async_connect(
        endpoint,                                 // 접속할 서버 주소
        boost::bind(&TCP_Client::handle_connect,  // 완료핸들러
                    this,  // 핸들러를 멤버로 갖는 클래스 인스턴스
                    boost::asio::placeholders::error)  // 에러코드
        );
  }

 private:
  void PostWrite() {
    if (m_Socket.is_open() == false)
      return;
    if (m_nSeqNumber > 7) {
      m_Socket.close();
      return;
    }
    ++m_nSeqNumber;

    char szMessage[128] = {
        0,
    };
    snprintf(szMessage, 128 - 1, "%d - Send Message", m_nSeqNumber);
    m_WriteMessage = szMessage;

    //  async_write는 완료함수 호출 시, 데이터가 100% 보내졌음을 보장한다
    //  반면에, async_write_some은 데이터를 다 못보내도 완료함수가 호출될 수
    //  있다(거의 이런일은 없지만, 네트워크 상황이 매우 안좋을 때 발생한다)
    boost::asio::async_write(
        m_Socket, boost::asio::buffer(m_WriteMessage),
        boost::bind(&TCP_Client::handle_write, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    PostReceive();
  }
  void PostReceive() {
    m_ReceiveBuffer.fill('\0');
    m_Socket.async_read_some(
        boost::asio::buffer(m_ReceiveBuffer),
        boost::bind(&TCP_Client::handle_receive, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }
  void handle_connect(const boost::system::error_code& error) {
    if (error) {
      std::cout << "connect failed : " << error.message() << std::endl;
    } else {
      std::cout << "connested" << std::endl;
      PostWrite();
    }
  }
  void handle_write(const boost::system::error_code& /* error */,
                    size_t /* bytes_transferred */) {}
  void handle_receive(const boost::system::error_code& error,
                      size_t bytes_transferred) {
    if (error) {
      if (error == boost::asio::error::eof) {
        std::cout << "서버와 연결이 끊어졌습니다" << std::endl;
      } else {
        std::cout << "error No: " << error.value()
                  << " error Message: " << error.message() << std::endl;
      }
    } else {
      const std::string strRecvMessage = m_ReceiveBuffer.data();
      std::cout << "서버에서 받은 메세지: " << strRecvMessage
                << ", 받은 크기: " << bytes_transferred << std::endl;
      PostWrite();
    }
  }
  boost::asio::io_service& m_io_service;
  boost::asio::ip::tcp::socket m_Socket;
  int m_nSeqNumber;
  std::array<char, 128> m_ReceiveBuffer;
  std::string m_WriteMessage;
};

int main() {
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::address::from_string(SERVER_IP), PORT_NUMBER);
  TCP_Client client(io_service);
  client.Connect(endpoint);
  io_service.run();
  std::cout << "네트워크 접속 종료" << std::endl;
  getchar();
  return 0;
}
