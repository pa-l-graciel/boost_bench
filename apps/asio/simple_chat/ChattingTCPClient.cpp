#include <iostream>
#include <deque>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <pthread.h>
#include "Protocol.h"

class ChatClient {
 public:
  ChatClient(boost::asio::io_service& io_service)
      : m_IOService(io_service), m_Socket(io_service) {
    m_bIsLogin = false;
    m_lock = PTHREAD_MUTEX_INITIALIZER;

  }

  ~ChatClient() {
    pthread_mutex_lock(&m_lock);

    while (m_SendDataQueue.empty() == false) {
      delete[] m_SendDataQueue.front();
      m_SendDataQueue.pop_front();
    }

    pthread_mutex_unlock(&m_lock);
    pthread_mutex_destroy(&m_lock);
  }

  bool IsConnecting() { return m_Socket.is_open(); }

  void LoginOK() { m_bIsLogin = true; }
  bool IsLogin() { return m_bIsLogin; }

  void Connect(boost::asio::ip::tcp::endpoint endpoint) {
    m_nPacketBufferMark = 0;

    m_Socket.async_connect(endpoint,
                           boost::bind(&ChatClient::handle_connect, this,
                                       boost::asio::placeholders::error));
  }

  void Close() {
    if (m_Socket.is_open()) {
      m_Socket.close();
    }
  }

  void PostSend(const bool bImmediately, const int nSize, char* pData) {
    char* pSendData = nullptr;

    pthread_mutex_lock(&m_lock);  // lock

    if (bImmediately == false) {  // main 스레드에서 PostSend를 호출한 케이스
      pSendData = new char[nSize];
      memcpy(pSendData, pData, nSize);

      m_SendDataQueue.push_back(pSendData);
    } else { // handle_write에서 m_SendDataQueue의 잔여데이터를 처리하는 케이스
      pSendData = pData;  
    }
    /* main에서 PostSend를 호출한 경우, 데이터가 2개이상 쌓여있는 경우는
     * async_write를 호출하지 않는다 (e.g.: handle_write가 호출되기 전에
     * main에서 PostSend를 연속호출한 경우임. */
    /*
     * PostSend는 main스레드에서 호출되고, handle_write는 io_service전용 스레드에서
     * 호출되기 때문에, async_write가 동시에 여러번 호출되는 경우가 발생할 수 있으며
     * 이런 상황이 발생하지 않도록 해야 한다.
     * 이를 위하여, handle_write가 호출한 경우에만 async_write를 무조건 실행하고,
     * main에서 호출한 경우, handle_write가 PostSend호출 없이 종료되어서 데이터사이즈가
     * 1로 된 경우만 async_write를 호출하도록 했다. */
    if (bImmediately || m_SendDataQueue.size() < 2) {
      boost::asio::async_write(
          m_Socket, boost::asio::buffer(pSendData, nSize),
          boost::bind(&ChatClient::handle_write, this,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
    }

    pthread_mutex_unlock(&m_lock);  // ulock
  }

 private:
  void PostReceive() {
    memset(&m_ReceiveBuffer, '\0', sizeof(m_ReceiveBuffer));

    m_Socket.async_read_some(
        boost::asio::buffer(m_ReceiveBuffer),
        boost::bind(&ChatClient::handle_receive, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred)

            );
  }

  void handle_connect(const boost::system::error_code& error) {
    if (!error) {
      std::cout << "서버 접속 성공" << std::endl;
      std::cout << "이름을 입력하세요!!" << std::endl;

      PostReceive();
    } else {
      std::cout << "서버 접속 실패. error No: " << error.value()
                << " error Message: " << error.message() << std::endl;
    }
  }

  void handle_write(const boost::system::error_code& error,
                    size_t bytes_transferred) {
    pthread_mutex_lock(&m_lock);  // lock

    delete[] m_SendDataQueue.front();
    m_SendDataQueue.pop_front();

    char* pData = nullptr;

    if (m_SendDataQueue.empty() == false) {
      pData = m_SendDataQueue.front();
    }

    pthread_mutex_unlock(&m_lock);  // unlock

    if (pData != nullptr) {
      PACKET_HEADER* pHeader = (PACKET_HEADER*)pData;
      PostSend(true, pHeader->nSize, pData);
    }
  }

  void handle_receive(const boost::system::error_code& error,
                      size_t bytes_transferred) {
    if (error) {
      if (error == boost::asio::error::eof) {
        std::cout << "서버와 연결이 끊어졌습니다" << std::endl;
      } else {
        std::cout << "error No: " << error.value()
                  << " error Message: " << error.message() << std::endl;
      }

      Close();
    } else {
      memcpy(&m_PacketBuffer[m_nPacketBufferMark], m_ReceiveBuffer.data(),
             bytes_transferred);

      int nPacketData = m_nPacketBufferMark + bytes_transferred;
      int nReadData = 0;

      while (nPacketData > 0) {
        if (nPacketData < sizeof(PACKET_HEADER)) {
          break;
        }

        PACKET_HEADER* pHeader = (PACKET_HEADER*)&m_PacketBuffer[nReadData];

        if (pHeader->nSize <= nPacketData) {
          ProcessPacket(&m_PacketBuffer[nReadData]);

          nPacketData -= pHeader->nSize;
          nReadData += pHeader->nSize;
        } else {
          break;
        }
      }

      if (nPacketData > 0) {
        char TempBuffer[MAX_RECEIVE_BUFFER_LEN] = {
            0,
        };
        memcpy(&TempBuffer[0], &m_PacketBuffer[nReadData], nPacketData);
        memcpy(&m_PacketBuffer[0], &TempBuffer[0], nPacketData);
      }

      m_nPacketBufferMark = nPacketData;

      PostReceive();
    } // else
  }

  void ProcessPacket(const char* pData) {
    PACKET_HEADER* pheader = (PACKET_HEADER*)pData;

    switch (pheader->nID) {
      case RES_IN: {
        PKT_RES_IN* pPacket = (PKT_RES_IN*)pData;

        LoginOK();

        std::cout << "클라이언트 로그인 성공 ?: " << pPacket->bIsSuccess
                  << std::endl;
      } break;
      case NOTICE_CHAT: {
        PKT_NOTICE_CHAT* pPacket = (PKT_NOTICE_CHAT*)pData;

        std::cout << pPacket->szName << ": " << pPacket->szMessage << std::endl;
      } break;
    }
  }

 private:
  boost::asio::io_service& m_IOService;
  boost::asio::ip::tcp::socket m_Socket;

  std::array<char, 512> m_ReceiveBuffer;

  int m_nPacketBufferMark;
  char m_PacketBuffer[MAX_RECEIVE_BUFFER_LEN * 2];

  pthread_mutex_t m_lock;
  std::deque<char*> m_SendDataQueue;

  bool m_bIsLogin;
};

int main() {
  boost::asio::io_service io_service;

  auto endpoint = boost::asio::ip::tcp::endpoint(
      boost::asio::ip::address::from_string("127.0.0.1"), PORT_NUMBER);

  ChatClient Cliet(io_service);
  Cliet.Connect(endpoint);

  // 워커 스레드를 만든 후 io_service.run() 호출
  // 채팅 입력이 가능하도록, io는 스레드에서 처리
  boost::thread thread(boost::bind(&boost::asio::io_service::run, &io_service));

  char szInputMessage[MAX_MESSAGE_LEN * 2] = {
      0,
  };

  while (std::cin.getline(szInputMessage, MAX_MESSAGE_LEN)) {
    if (strnlen(szInputMessage, MAX_MESSAGE_LEN) == 0) {
      break;
    }

    if (Cliet.IsConnecting() == false) {
      std::cout << "서버와 연결되지 않았습니다" << std::endl;
      continue;
    }

    if (Cliet.IsLogin() == false) {
      PKT_REQ_IN SendPkt;
      SendPkt.Init();
      strncpy(SendPkt.szName, szInputMessage, MAX_NAME_LEN - 1);

      // 채팅을 종료할 때까지 입력데이터를 서버에 보낸다
      Cliet.PostSend(false, SendPkt.nSize, (char*)&SendPkt);
    } else {
      PKT_REQ_CHAT SendPkt;
      SendPkt.Init();
      strncpy(SendPkt.szMessage, szInputMessage,
                MAX_MESSAGE_LEN - 1);

      Cliet.PostSend(false, SendPkt.nSize, (char*)&SendPkt);
    }
  }

  io_service.stop();

  Cliet.Close();

  thread.join();

  std::cout << "클라이언트를 종료해 주세요" << std::endl;

  return 0;
}
