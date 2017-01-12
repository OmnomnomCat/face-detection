#include "locker.h"

using namespace std;

Locker::Locker(unsigned int initCounter):
    tpAvailableObjectCounter(initCounter),
    tpEmitMode(None)
{

}

void Locker::waitingForEvent()
{
    unique_lock<mutex> mutexLocker(tpAvailableObjectMutex);

    if(tpEmitMode == Empty)
        while(tpAvailableObjectCounter != 0)
            tpCondition.wait(mutexLocker);
    else while(tpAvailableObjectCounter == 0)
        tpCondition.wait(mutexLocker);
}

void Locker::emitEvent()
{
    tpCondition.notify_all();
}

Locker& Locker::operator ++()
{
    this->increaseCounter();
    return *this;
}

Locker& Locker::operator --()
{
    this->decreaseCounter();
    return *this;
}

void Locker::increaseCounter()
{
    ++this->tpAvailableObjectCounter;
    emitSelector(Increase);
}

void Locker::decreaseCounter()
{
    --this->tpAvailableObjectCounter;
    emitSelector(Decrease);

    if(tpAvailableObjectCounter == 0)
        emitSelector(Empty);
}

void Locker::setEmitMode(const EmitMode &value)
{
    tpEmitMode = value;
}

void Locker::emitSelector(EmitMode mode)
{
    if((tpEmitMode & mode) == mode)
        this->emitEvent();

}
