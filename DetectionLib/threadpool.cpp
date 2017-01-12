#include "threadpool.h"

#include "worker.h"
#include "locker.h"

#include <algorithm>

using namespace std;

ThreadPool::ThreadPool(int poolSize):
    tpPoolLocker(new Locker()),
    tpFinalizeLocker(new Locker())
{
    tpPoolLocker->setEmitMode(Locker::Increase);
    tpFinalizeLocker->setEmitMode(Locker::Empty);

    this->setPoolSize(poolSize);
}

ThreadPool::~ThreadPool()
{
    while(tpPool.size() != 0)
        this->deleteWorker();

    delete tpPoolLocker;
}

void ThreadPool::processImage(std::string filepath)
{
    tpFinalizeLocker->increaseCounter();

    Worker* worker = this->waitForAvailableWorker();
    worker->process(filepath);
}

void ThreadPool::waitWhilePoolProcessing()
{
    tpFinalizeLocker->waitingForEvent();
}

void ThreadPool::createWorker()
{
    auto worker = new Worker(tpPoolLocker,tpFinalizeLocker);
    tpPoolLocker->increaseCounter();
    tpPool.push_back(worker);
}

void ThreadPool::deleteWorker()
{
    Worker* worker = *tpPool.begin();
    tpPool.remove(worker);
    delete worker;
}

int ThreadPool::getPoolDiff() const
{
    return tpPoolMaxSize - tpPool.size();
}

Worker *ThreadPool::tryGetAvailableWorker() const
{
    auto result = std::find_if(tpPool.begin(), tpPool.end(), [](Worker* ptr){
            return ptr->isAvailable() == true;
    });
    return result == std::end(tpPool)? nullptr: *(result);
}

Worker *ThreadPool::waitForAvailableWorker()
{
    Worker* worker = this->tryGetAvailableWorker();
    if(worker)
        return worker;

    tpPoolLocker->waitingForEvent();

    worker = this->waitForAvailableWorker();
    worker->wait();
    return worker;
}

void ThreadPool::setPoolSize(unsigned int value)
{
    tpPoolMaxSize = value;

    if(getPoolDiff() == 0)
        return;

    while(getPoolDiff() != 0)
        createWorker();
}
