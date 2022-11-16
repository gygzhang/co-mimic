#include "coroutine.h"

#include<iostream>
#include<cstring>
#include<cassert>

using namespace coroutine_mimic;
using CoroutineID = coroutine_mimic::CoroutineID;

Coroutine::Coroutine(Scheduler* scheduler, CoroutineFunction fun, std::any args)
:   _scheduler(scheduler),
    _context{},
    _stacksize(0),
    _capability(0),
    _stack(nullptr),
    _state(STATUS::COROUTINE_READY),
    _function(fun),
    _args(args)
{

}


Coroutine::~Coroutine()
{

}

Scheduler::Scheduler()
:   _stack(new byte[STACKSIZE]),
    _map(),
    //_map(new CoroutineMap()),
    //_map(std::unique_ptr<CoroutineMap>()),
    //_map(std::make_unique<CoroutineMap>()),
    _currentRuningCoroutine(_map.end()),
    _schedulerContext{},
    _nextID(0),
    _stackTop(reinterpret_cast<ptrdiff_t>(_stack.get()+STACKSIZE))
{

}

Scheduler::~Scheduler()
{
    _map.clear();
}

CoroutineID Scheduler::CoroutineCreate(CoroutineFunction fun, std::any args)
{
    CoroutineID id = _nextID++;
    CoroutinePtr ptr(std::make_shared<Coroutine>(this, fun, args));
    _map[id] = ptr;
    return id;
}

void Scheduler::CoroutineResume(CoroutineID id)
{
    assert(_map.find(id)!=_map.end());

    CoroutinePtr pco = _map[id];
    STATUS state = pco->getState();
    switch (state)
    {
    case STATUS::COROUTINE_READY:
    {
        // get current context, put it into pco->_context
        getcontext(&pco->_context);

        // set a new coroutine's stack to scheduler's stack
        pco->_context.uc_stack.ss_sp = _stack.get();
        pco->_context.uc_stack.ss_size = STACKSIZE;
        //when the new coroutine ends, swtich context back to `_schedulerContext`
        pco->_context.uc_link = &_schedulerContext;
        pco->_state = STATUS::COROUTINE_RUNNING;
        this->_currentRuningCoroutine = _map.find(id);

        //why? see https://man7.org/linux/man-pages/man3/makecontext.3.html#DESCRIPTION 
        uint32_t lo32 = reinterpret_cast<uint64_t>(this)&0xffffffff;
        uint32_t hi32 = reinterpret_cast<uint64_t>(this)>>32;

        // modify the context which pointed by `pco->_context`, once later `swapcontext` was called,
        // the `Schedule` will be called imediately.
        makecontext(&pco->_context, (void(*)())Schedule, 2, lo32, hi32);
        
        std::cout<<"b make\n";
        // save current context to `_schedulerContext`, and switch to `pco->_context`,
        // so from above was saied, the function `Schedule` will be called.
        swapcontext(&_schedulerContext, &pco->_context);
        std::cout<<"a make\n";

        break;
    }
    // in this case, the 
    case STATUS::COROUTINE_SUSPEND:
    {
        //restore stack of suspend coroutine
        printf("%p, %ld\n", pco->_stack.get(), pco->_stacksize);
        memcpy(_stack.get() + STACKSIZE - pco->_stacksize, pco->_stack.get(), pco->_stacksize);
        _currentRuningCoroutine = _map.find(id);
        pco->_state = STATUS::COROUTINE_RUNNING;
        swapcontext(&_schedulerContext, &pco->_context);
    }

    
    default:
        break;
    }

}

void Scheduler::CoroutineYield()
{
    assert(_currentRuningCoroutine!=_map.end());
    CoroutinePtr pCurCoroutine = _currentRuningCoroutine->second;
    // this is the top of scheduler
    SaveCurrentStack(pCurCoroutine);
    pCurCoroutine->_state = STATUS::COROUTINE_SUSPEND;
    _currentRuningCoroutine = _map.end();
    swapcontext(&pCurCoroutine->_context, &_schedulerContext);   
}

void Scheduler::SaveCurrentStack(CoroutinePtr pco)
{
    byte guard = 0;
    printf("b SaveCurrentStack: %ld, %p, %ld\n", _stackTop, &guard, pco->_stacksize);
    if(pco->_capability < _stackTop - reinterpret_cast<ptrdiff_t>(&guard))
    {
        pco->_capability = _stackTop - reinterpret_cast<ptrdiff_t>(&guard);
        pco->_stack.reset(new byte[pco->_capability]);
    }
    pco->_stacksize = _stackTop - reinterpret_cast<ptrdiff_t>(&guard);
    printf("a SaveCurrentStack: %ld, %p, %ld\n", _stackTop, &guard, pco->_stacksize);
    //copy content from &guard to &guard + pco->_stacksize;
    memcpy(pco->_stack.get(), &guard, pco->_stacksize);

}

void Scheduler::Schedule(uint32_t lo32, uint32_t hi32)
{
    std::cout<<"b  Schedule"<<std::endl;
    // this function was enter from `swapcontext`
    // get the scheduler from two ints.
    Scheduler *sch = reinterpret_cast<Scheduler*>(lo32|((uint64_t)hi32<<32));
    //Scheduler* sch1 = reinterpret_cast<Scheduler*>(sch);
    CoroutineMapIter cur = sch->_currentRuningCoroutine;
    auto pco = cur->second;
    // call user provided function, in this project, it is either a cosumer or a producer. 
    pco->_function(sch, pco->_args);
    sch->_map.erase(cur);
    sch->_currentRuningCoroutine = sch->_map.end();
    std::cout<<"a  Schedule"<<std::endl;
    
}

STATUS Scheduler::CoroutineState(CoroutineID id)
{
    return _map.find(id) == _map.end() ? STATUS::COROUTINE_DEAD : _map[id]->getState();
}




