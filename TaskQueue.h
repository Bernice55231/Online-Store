#pragma once


#include "sthread.h"
#include <queue>
using namespace std;


typedef void (*handler_t) (void *); 

struct Task {
    handler_t handler;
    void* arg;
};

/*
 * ------------------------------------------------------------------
 * TaskQueue --
 * 
 *      A thread-safe task queue. This queue should be implemented
 *      as a monitor.
 *
 * ------------------------------------------------------------------
 */
class TaskQueue {
    private:
    // Synchronization variables:
    smutex_t mutex;
    scond_t nonempty;

    //State variables:
    int sz;
    std::queue<Task> tasklist;

    public:
    TaskQueue();
    ~TaskQueue();
    
    // no default copy constructor and assignment operators. this will prevent some
    // painful bugs by converting them into compiler errors.
    TaskQueue(const TaskQueue&) = delete; 
    TaskQueue& operator=(const TaskQueue &) = delete;

    void enqueue(Task task);
    Task dequeue();

    private:
    int size();
    bool empty();
};

