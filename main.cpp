#include "coroutine.h"
#include <iostream>
#include <queue>
#include <functional>

using namespace std;
using namespace coroutine_mimic;

constexpr int kLOOPTIMES = 5;

/***
 * 生产者协程
 * @param schedule
 * @param args
 */
void coProducer(Scheduler* schedule, std::any args) {
  queue<int> *unBlockingQueue = any_cast<queue<int> *>(args);
  int start = 100;

  for (int i = 0; i < kLOOPTIMES; ++i) {
	unBlockingQueue->push(i + start);
	cout << "producer push " << i + start << endl;
	schedule->CoroutineYield();
  }
}

/***
 * 消费者协程
 * @param schedule
 * @param args
 */
void coConsumer(Scheduler *schedule, std::any args) {
  auto unBlockingQueue = any_cast<queue<int> *>(args);

  for (int i = 0; i < kLOOPTIMES; ++i) {
	auto thing = unBlockingQueue->front();
	unBlockingQueue->pop();
	cout << "consumer pop " << thing << endl;
	schedule->CoroutineYield();
  }
}

/***
 * 协程测试函数
 * @param schedule
 * @param unBlockingQueue
 */
void testFunc1(Scheduler &schedule, queue<int> &unBlockingQueue) {
  // 创建协程对象，并返回协程对象ID
  auto producer = schedule.CoroutineCreate(coProducer, &unBlockingQueue);
  auto consumer = schedule.CoroutineCreate(coConsumer, &unBlockingQueue);

//   int ids1[100];  
//   int ids2[100];

//   for (size_t i = 0; i < 100; i++)
//   {
//     ids1[i] = schedule.CoroutineCreate(coProducer, &unBlockingQueue);
//   }
  

  cout << "main coroutine start" << endl;
  while (schedule.CoroutineState(producer) && schedule.CoroutineState(consumer)) {
    // 启动/调度到生产者协程
	schedule.CoroutineResume(producer);
	// 启动/调度到消费者协程
	schedule.CoroutineResume(consumer);
	// 恢复到主协程
    // for (size_t i = 0; i < 100; i++)
    // {
    //     schedule.CoroutineState(producer)
    // }
  }
  cout << "main coroutine end" << endl;
}

int main() {
  Scheduler schedule;
  queue<int> unBlockingQueue;
  testFunc1(schedule, unBlockingQueue);
  return 0;
}

