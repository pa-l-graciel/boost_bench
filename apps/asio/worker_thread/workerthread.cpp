#include <sys/types.h>
#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

class BackGroundJobManager {
  boost::asio::io_service& m_io_service;
  boost::shared_ptr<boost::asio::io_service::work> m_Work;
  boost::thread_group m_Group;

 public:
  BackGroundJobManager(boost::asio::io_service& io_service, std::size_t size)
      : m_io_service(io_service) {

    // boost::asio::io_service::work를 사용하면, io_service에 비동기
    // 요청을 하기 전에 run 함수를 실행해도 run 함수가 종료되지 않음.
    m_Work.reset(new boost::asio::io_service::work(m_io_service));

    for (std::size_t i = 0; i < size; ++i) {
      // boost::thread_group를 사용하여 지정한 개수만큼 스레드 생성 후
      // io_service()를 실행.
      m_Group.create_thread(
          boost::bind(&boost::asio::io_service::run, &m_io_service));
    }
  }

  ~BackGroundJobManager() {
    m_Work.reset();     // io_service.run 종료
    m_Group.join_all(); // 생성한 스레드가 종료될때까지 대기
  }

  template <class F>
  void post(F f) {
    /* io_service에서는 비동기로 job를 처리 가능한 post 함수를 제공한다
     * template<typename CompletionHandler>
         void post(CompletionHandler handler);
       */
    m_io_service.post(f); // post 함수를 사용하여 비동기로 실행할 함수를
                          // io_service에 요청
  }
};

boost::mutex g_mutex;     // 콘솔 출력 시 동기화를 위해 사용

void Function(int nNumber) {
  // 함수 이름과 인자 값, 시간, 이 함수를 실행하는
  // 스레드의 ID를 콘솔에 출력
  char szMessage[128] = {
      0,
  };
  snprintf(szMessage, 128 - 1, "%s(%d) | time(%d)", __FUNCTION__, nNumber,
//  snprintf(szMessage, 128 - 1, "%s, line %d: %s(%d) | time(%d)", __FILE__, __LINE__, __FUNCTION__, nNumber,
            static_cast<int>(time(NULL)));
  {
    boost::mutex::scoped_lock lock(g_mutex);
    std::cout << "워커 스레드 ID: " << pthread_self() << ". "
             << szMessage << std::endl;
 }

//  sleep(1);
}

class TEST {
 public:
  TEST() {}

  void Function(int nNumber) { ::Function(nNumber); }
};

int main() {
  std::cout << "메인 스레드 ID: " << pthread_self() << std::endl;

  boost::asio::io_service io_service;
  BackGroundJobManager JobManager(io_service, 3);

  JobManager.post(boost::bind(Function, 11));
  JobManager.post(boost::bind(Function, 12));
  sleep(3);

  TEST test;
  JobManager.post(boost::bind(&TEST::Function, &test, 21));
  JobManager.post(boost::bind(&TEST::Function, &test, 22));
  sleep(3);

  return 0;
}
