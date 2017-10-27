#ifndef __SERVERSESSION_H__
#define __SERVERSESSION_H__

#include <iostream>
#include <algorithm>
#include <string>
#include <list>
#include <deque>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "Protocol.h"

class ChatServer;

class Session {
 public:
  Session(int nSessionID,
          boost::asio::io_service& io_service,
          ChatServer* pServer);
  ~Session();
  int SessionID() { return m_nSessionID; }
  boost::asio::ip::tcp::socket& Socket() { return m_Socket; }
  void Init();

  // 클라이언트가 보내는 데이터 받기
  void PostReceive();
  void PostSend(const bool bImmediately, const int nSize, char* pData);
  void SetName(const char* pszName) { m_Name = pszName; }
  const char* GetName() { return m_Name.c_str(); }

 private:
  void handle_write(const boost::system::error_code& error,
                    size_t bytes_transferred);
  void handle_receive(const boost::system::error_code& error,
                      size_t bytes_transferred);
  int m_nSessionID;
  boost::asio::ip::tcp::socket m_Socket;  // 소켓 객체
  std::array<char, MAX_RECEIVE_BUFFER_LEN> m_ReceiveBuffer;
  int m_nPacketBufferMark;
  char m_PacketBuffer[MAX_RECEIVE_BUFFER_LEN * 2];

  // bool m_bCompletedWrite;

  std::deque<char*> m_SendDataQueue;
  std::string m_Name;
  ChatServer* m_pServer;
};

#endif
