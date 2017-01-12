/*!
  * \brief Lock segment of code and synchronize data
  *
  * \details In fact, this is a handmade version of semaphores.
  * Event system looks like Windows system(WaitForSingleObject).
  * Doen't use active spinlock("while(1)")
  */
#ifndef LOCKER_H
#define LOCKER_H

#include <atomic>
#include <mutex>

class Locker
{
public:

    /*!
      * \brief Call emitEvent() on objectCounter event
      */
    enum EmitMode{
        Increase = 0x0001,
        Decrease = 0x0010,

        Empty = 0x0100,

        None = 0,
    };

    /*!
      * \brief Parameter initialize objectCounter
      */
    explicit Locker(unsigned int initCounter = 0);

    /*!
      * \brief waitingForEvent() wait for call of emitEvent(). Methods are thread-safe.
      */
    void waitingForEvent();
    void emitEvent();

    inline void increaseCounter();
    inline void decreaseCounter();

    void setEmitMode(const EmitMode &value);

protected:
    void emitSelector(EmitMode mode);

private:
    std::atomic_uint tpAvailableObjectCounter;
    std::mutex tpAvailableObjectMutex;
    std::condition_variable tpCondition;

    EmitMode tpEmitMode;

public:
    Locker& operator ++();
    Locker& operator --();

};

#endif // LOCKER_H
