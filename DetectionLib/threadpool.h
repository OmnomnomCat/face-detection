/*!
  * \brief Main class of library.
*/


#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <atomic>
#include <mutex>

#include "sharedlibrary.h"

class Worker;
class Locker;

class SHAREDLIB_LIBRARY ThreadPool
{
public:
    /*!
      * \brief Ctor for class. Create Worker's objects pool
    */
    explicit ThreadPool(int poolSize);
    ~ThreadPool();

    /*!
      * \brief Initialize image processing
      *
      * \details Every call of this function start new thread in context
      * of existing Worker object
    */
    void processImage(std::string filepath);

    /*!
      * \brief Wait while images are processing.Prevent data losing and corruption
      *
      * \details See Locker.h description for details
    */
    void waitWhilePoolProcessing();

protected:
    void setPoolSize(unsigned int value);

    /*!
      * \brief Thread-safe creating and deleting of Worker objects
    */
    void createWorker();
    void deleteWorker();

    inline int getPoolDiff() const;

    /*!
      * \brief Thread-safe method to get available Worker object
    */
    inline Worker* tryGetAvailableWorker() const;
    inline Worker* waitForAvailableWorker();

private:
    unsigned int tpPoolMaxSize;

    /*!
      * \brief Lock pool if no one Worker available
    */
    Locker* tpPoolLocker;

    /*!
      * \brief Lock main thread while images are processing
    */
    Locker* tpFinalizeLocker;

    std::list<Worker*> tpPool;

};

#endif // THREADPOOL_H
