/*!
  * \brief In fact Worker means thread. One worker means one available thread
*/

#ifndef WORKER_H
#define WORKER_H

#include "types.h"

#include <string>
#include <thread>
#include <map>
#include <opencv2/core.hpp>

class Locker;
class Worker;

/*!
  * \brief Function of image detection
  *
  * \details Function based on Haar cascade implemented in OpenCV library
  *
  * \badcode Haar cascade supports large images but simultaneous processing of several images
  * may call overhead of memory
*/

FaceMap<cv::Rect, std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>> threadFunction(std::string filepath, Worker *worker = nullptr);

class Worker
{
public:
    /*!
      * \brief  ThreadPool lockers as params
    */
    explicit Worker(Locker* workerLocker, Locker* finalizeLocker);
    ~Worker();

    /*!
      * \brief Thread creating and initialization of image precessing
      */
    void process(std::string filepath);

    /*!
      * \brief Is thread running?
      */
    bool isAvailable();

    /*!
      * \brief Wait while thread will terminated
      */
    void wait();

    /*!
      * \brief Switch of lockers
      */
    void done();

private:
    std::thread wThread;
    Locker* wWorkerLocker;
    Locker* wFinalizeLocker;

    bool wAvailable;
};



#endif // WORKER_H
