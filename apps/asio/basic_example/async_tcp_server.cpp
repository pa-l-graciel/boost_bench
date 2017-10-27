#include <iostream>
#include <algorithm>
#include <string>
#include <list>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#define PORT_NUMBER 31400

/**********************************************************
Session: 서버에 접속한 클라이언트
 - 클라이언트와 데이터를 주고받는 기능
 - 소켓 객체 하나를 멤버로 들고있다
**********************************************************/
class Session {
 public:
  Session(boost::asio::io_service& io_service) : m_Socket(io_service) {}
  boost::asio::ip::tcp::socket& Socket() { return m_Socket; }

  // 클라이언트가 보내는 데이터 받기
  void PostReceive() {
    memset(&m_ReceiveBuffer, '\0', sizeof(m_ReceiveBuffer));

    // 비동기로 받기
    m_Socket.async_read_some(
        boost::asio::buffer(m_ReceiveBuffer),  // 데이터를 받을 버퍼
        boost::bind(
            &Session::handle_receive,  // 완료 시 호출함수(핸들러)
            this,  // 핸들러를 멤버로 가지는 클래스의 인스턴스
            boost::asio::placeholders::error,  // 핸들러에 넘길 첫번째 인자
            boost::asio::placeholders::bytes_transferred)  // 두번째 인자
        );
  }

 private:
  void handle_write(const boost::system::error_code& /*error*/,
                    size_t /* bytes_transferred */) {}
  void handle_receive(const boost::system::error_code& error,
                      size_t bytes_transferred) {
    if (error) {
      if (error == boost::asio::error::eof) {
        std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;
      } else {
        std::cout << "error No: " << error.value()
                  << " error Message: " << error.message() << std::endl;
      }
    } else {
      const std::string strRecvMessage = m_ReceiveBuffer.data();
      std::cout << "클라이언트에서 받은 메세지: " << strRecvMessage
                << ", 받은 크기: " << bytes_transferred << std::endl;

      char szMessage[128] = {
          0,
      };
      snprintf(szMessage, 128 - 1, "Re: %s", strRecvMessage.c_str());
      m_WriteMessage = szMessage;

      // 클라이언트로부터 받은 패킷을 다시 되돌려주는 echo 로직을 비동기로 구현
      boost::asio::async_write(
          m_Socket,                             // 클라이언트 소켓
          boost::asio::buffer(m_WriteMessage),  // 보낼 데이터 버퍼
          boost::bind(&Session::handle_write,   // 완료 시 핸들러함수
                      this, boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
      PostReceive();  // 비동기 받기작업을 반복하기 위하여 다시 호출함
    }
  }

  boost::asio::ip::tcp::socket m_Socket;  // 소켓 객체
  std::string m_WriteMessage;
  std::array<char, 128> m_ReceiveBuffer;
};

/**********************************************************
TCP_Server: Session 클래스를 활용하여 클라이언트의 접속을 받아들이는 기능

**********************************************************/
class TCP_Server {
 public:
  TCP_Server(boost::asio::io_service& io_service)
      : m_acceptor(io_service,
                   boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                                                  PORT_NUMBER)) {
    m_pSession = nullptr;
    StartAccept();
  }
  ~TCP_Server() {
    if (m_pSession != nullptr) {
      delete m_pSession;
    }
  }

 private:
  // 클라이언트 접속을 처리
  void StartAccept() {
    std::cout << "클라이언트 접속 대기 ...." << std::endl;
    m_pSession = new Session(m_acceptor.get_io_service());

    // 비동기 함수로 접속을 처리함
    // async_accept가 완료되면 handle_accept가 호출 됨
    // 즉, handl_accept가 호출되면 접속 처리작업이 완료되었다는 뜻이다
    m_acceptor.async_accept(
        m_pSession->Socket(),  // 접속한 클라이언트에 할당할 소켓
        boost::bind(&TCP_Server::handle_accept, this, m_pSession,
                    boost::asio::placeholders::error));
  }
  // 접속처리 완료 후 호출되는 핸들러
  void handle_accept(Session* pSession,
                     const boost::system::error_code& error) {
    if (!error) {
      std::cout << " 클라이언트 접속 성공" << std::endl;

      // 접속 성공 시, 클아이언트가 보내는 패킷을 받을 수 있도록, accept 된
      // 소켓에 받기 작업을 요청해야 한다
      pSession->PostReceive();
    }
  }
  int m_nSeqNumber;
  boost::asio::ip::tcp::acceptor m_acceptor;
  Session* m_pSession;
};

int main() {
  boost::asio::io_service io_service;
  TCP_Server server(io_service);

  // io_service.run()
  //  함수 호출 시, main함수는 무한대기 상태가 되고, 비동기 함수작업이 끝나면
  // 핸들러함수를 호출함.
  //  비동기 요청이 있을 경우 요청이 끝날 때까지 무한히 대기하다가, 다음 요청이
  // 있는지 보고, 없으면 종료됨.
  // 주의: 비동기 요청을 하기 전에 run()을 호출하면 곧바로 종료되버린다
  // 주의: 비동기 요청 완료 시, 새로운 비동기 요청을 하지 않으면 종료된다
  io_service.run();

  std::cout << "네트워크 접속 종료" << std::endl;
  getchar();
  return 0;
}
