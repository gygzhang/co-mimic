#include "coroutine_test.h"
#include "pthread_test.h"
#include <sys/time.h>

int main() {
  Scheduler schedule;
  queue<int> unBlockingQueue;
  struct timeval start,end;
       
  gettimeofday(&start , 0);
  coroutineTest(schedule, unBlockingQueue);
  gettimeofday(&end , 0);
  std::cout<<"30000 coroutines avg: "<<(end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000+start.tv_usec)/30000<<" us\n"<<std::endl;

  gettimeofday(&start , 0);
  pthread_test();
  gettimeofday(&end , 0);
  std::cout<<"30000 pthreads  avg: "<<(end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000+start.tv_usec)/30000<<" us\n"<<std::endl;

  return 0;
}

