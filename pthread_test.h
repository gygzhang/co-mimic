#ifndef PTHREAD_H
#define PTHREAD_H
#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<chrono>
#include<vector>
#include<queue>
#include<any>
 
std::mutex gMtx;
std::condition_variable gCv;
bool gProdReady = true;
bool gConsumReady = false;
bool done = false;
int gCousumIndex = 1;
const int nt = 30000;

// using namespace std;
 
void producer(std::any que)
{
    std::queue<int> *unBlockingQueue = std::any_cast<std::queue<int>*>(que);
    int start = 100;

    for (int i = 0; i < 5; ++i) {
        std::unique_lock<std::mutex> ul(gMtx);
        gCv.wait(ul, [&]{return unBlockingQueue->empty();});
        gProdReady = false;
        gConsumReady = true;
        for(int j = 0; j < nt; j++){
            unBlockingQueue->push(j + i*nt + start);
        }
        
        ul.unlock();
        gCv.notify_all();
        sched_yield();
        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    done = true;
}
 
void Consumer(int i, std::any que)
{
    std::queue<int> *unBlockingQueue = std::any_cast<std::queue<int>*>(que);
    while(!unBlockingQueue->empty())
    {
        std::unique_lock<std::mutex>ul(gMtx);
        if(done){
            ul.unlock();
            return;
        }
        gCv.wait(ul, [&]{return (!unBlockingQueue->empty());});
        gConsumReady = false;
        gProdReady = true;
        ++gCousumIndex;
        unBlockingQueue->pop();
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        ul.unlock();
        gCv.notify_all();
        sched_yield();
    }
}
 
 
int pthread_test()
{
    std::cout<<"pthread_test started"<<std::endl;
    std::queue<int> unBlockingQueue;
    std::thread T1(producer, &unBlockingQueue);

    std::vector<std::thread> ths;
    for(int i=0; i<nt; i++)
    {
        ths.push_back(std::thread(Consumer, i, &unBlockingQueue));
    }

    T1.join();
    for(auto &it : ths)
    {
        it.join();
    }

    std::cout<<"pthread_test finished"<<std::endl;
 
    return 0;
}

#endif // !PTHREAD_H
