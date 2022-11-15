#include "coroutine.h"

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
:   _stack{},
    _map(),
    //_map(new CoroutineMap()),
    //_map(std::unique_ptr<CoroutineMap>()),
    //_map(std::make_unique<CoroutineMap>()),
    _currentRuningCoroutine(nullptr),
    _mainContext{},
    _nextID(0)
{

}

Scheduler::~Scheduler()
{

}

CoroutineID Scheduler::CoroutineCreate(CoroutineFunction fun, std::any args)
{
    CoroutineID id = _nextID++;
    CoroutinePtr ptr(std::make_shared<Coroutine>(fun, args));
    _map[id] = ptr;
    return id;
}

void Scheduler::CoroutineResume(CoroutineID id)
{
    assert(_map.find(id)!=_map.end());

    CoroutinePtr pco = _map[id];
    STATUS state = pco->getState();

}




