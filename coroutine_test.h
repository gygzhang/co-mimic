#ifndef COROUTINE_TEST_H
#define COROUTINE_TEST_H

#include "coroutine.h"
#include <iostream>
#include <queue>
#include <functional>

using namespace std;
using namespace coroutine_mimic;

constexpr int kLOOPTIMES = 5;
const int n = 30000;
/***
 * 生产者协程
 * @param schedule
 * @param args
 */
void coProducer(Scheduler* schedule, std::any args) {
  queue<int> *unBlockingQueue = any_cast<queue<int> *>(args);
  int start = 100;

  for (int i = 0; i < kLOOPTIMES; ++i) {
    for(int j = 0; j < n; j++){
        unBlockingQueue->push(j + i*n + start);
    }
	
	//cout << "producer push " << i + start << endl;
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
	//auto thing = unBlockingQueue->front();
	unBlockingQueue->pop();
	//cout << "consumer pop " << thing << endl;
	schedule->CoroutineYield();
  }
}

/***
 * 消费者协程
 * @param schedule
 * @param args
 */
void coConsumer2(Scheduler *schedule, std::any args) {
  char buf[1000];
  (void)buf;
  auto unBlockingQueue = any_cast<queue<int> *>(args);
  for (int i = 0; i < kLOOPTIMES; ++i) {
	//auto thing = unBlockingQueue->front();
	unBlockingQueue->pop();
	//cout << "consumer pop " << thing << endl;
	schedule->CoroutineYield();
  }
}

/***
 * 协程测试函数
 * @param schedule
 * @param unBlockingQueue
 */
void coroutineTest(Scheduler &schedule, queue<int> &unBlockingQueue) {
  // 创建协程对象，并返回协程对象ID
  auto producer = schedule.CoroutineCreate(coProducer, &unBlockingQueue);
  //auto consumer = schedule.CoroutineCreate(coConsumer, &unBlockingQueue);

  

  
  int ids1[n];  
  //int ids2[n];

  for (size_t i = 0; i < n-1; i++)
  {
    //ids1[i] = schedule.CoroutineCreate(coProducer, &unBlockingQueue);
    ids1[i] = schedule.CoroutineCreate(coConsumer, &unBlockingQueue);
  }
  ids1[n-1] = schedule.CoroutineCreate(coConsumer2, &unBlockingQueue);

  cout << "coroutine started" << endl;
  while (schedule.CoroutineState(producer)) {
    // 启动/调度到生产者协程
	schedule.CoroutineResume(producer);
	// 启动/调度到消费者协程
	//schedule.CoroutineResume(consumer);
	// 恢复到主协程
    for (size_t i = 0; i < n; i++)
    {
        if(schedule.CoroutineState(ids1[i])){
            schedule.CoroutineResume(ids1[i]);
        }
        // if(schedule.CoroutineState(ids1[i])){
        //     schedule.CoroutineResume(ids1[i]);
        // }  
    }
  }
  cout << "coroutine finished" << endl;
}

#endif