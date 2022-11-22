#ifndef COROUTINE_H
#define COROUTINE_H

#include<ucontext.h>
#include<memory>
#include<unordered_map>
#include<functional>
#include<stddef.h>
#include<any>

namespace coroutine_mimic
{

//status of a coroutine
enum STATUS
{
    COROUTINE_DEAD = 0,
    COROUTINE_READY,
    COROUTINE_RUNNING,
    COROUTINE_SUSPEND
};

class Coroutine;
//forward declartion of Scheduler for Coroutine using.
class Scheduler;

// the type using to create a coroutine
using CoroutineFunction = std::function<void(Scheduler *, std::any)>;

using  byte = unsigned char;

using CoroutineID = uint64_t;

using CoroutinePtr = std::shared_ptr<Coroutine>;
using CoroutineMap = std::unordered_map<CoroutineID, CoroutinePtr>;
using CoroutineMapIter = CoroutineMap::const_iterator;

// coroutine class
class Coroutine
{
    //status of a coroutine, public or private?
public:
    Coroutine(Scheduler* scheduler, CoroutineFunction fun, std::any args);
    ~Coroutine();



    STATUS getState() {return _state;}

private:
    Scheduler* _scheduler;
    ucontext_t _context;
    ptrdiff_t _stacksize;
    ptrdiff_t _capability;
    std::unique_ptr<byte> _stack;
    STATUS _state;

    CoroutineFunction _function;

    // std::any since c++ 17,  simlilar to void*/shared_ptr<void> 
    // and with safety and type infomation. see:
    // https://devblogs.microsoft.com/cppblog/stdany-how-when-and-why/
    std::any _args;
    static  int _id;
    std::string _name;
    friend class Scheduler;
};


// the coroutine scheduler class
class Scheduler
{
public:
    Scheduler();
    ~Scheduler();


    //create a coroutine
    CoroutineID CoroutineCreate(CoroutineFunction fun, std::any args);

    //yield a coroutine 
    void CoroutineYield();

    //resume a coroutine
    void CoroutineResume(CoroutineID id);

    void SaveCurrentStack(CoroutinePtr pco);

    //should be big enough, or munmap_chunk(): invalid pointer error will happen
    const static int STACKSIZE = 2<<20;

    static void Schedule(uint32_t lo32, uint32_t hi32);

    STATUS CoroutineState(CoroutineID id);

private:
    //map of coroutine
    CoroutineMap _map;
    std::unique_ptr<byte> _stack;
    CoroutineMapIter _currentRuningCoroutine;
    ucontext_t _schedulerContext;
    CoroutineID _nextID;
    ptrdiff_t _stackTop;
};




} //end of namespace coroutine_mimic


#endif 


