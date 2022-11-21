
#include "TaskQueue.h"
#include <queue>
using namespace std;

TaskQueue::
TaskQueue()
{
   // initialize the variables:
   sz = 0;
   smutex_init(&mutex);
   scond_init(&nonempty);
}

TaskQueue::
~TaskQueue()
{
    // TODO: Your code here. 
    // destroy all the resources
    scond_destroy(&nonempty);
    smutex_destroy(&mutex);
}

/*
 * ------------------------------------------------------------------
 * size --
 *
 *      Return the current size of the queue.
 *
 * Results:
 *      The size of the queue.
 *
 * ------------------------------------------------------------------
 */
int TaskQueue::
size()
{
    // TODO: Your code here.
    smutex_lock(&mutex);
    sz = tasklist.size(); //?
    smutex_unlock(&mutex);
    return sz; // Keep compiler happy until routine done.
}

/*
 * ------------------------------------------------------------------
 * empty --
 *
 *      Return whether or not the queue is empty.
 *
 * Results:
 *      The true if the queue is empty and false otherwise.
 *
 * ------------------------------------------------------------------
 */
bool TaskQueue::
empty()
{
    // TODO: Your code here.
    smutex_lock(&mutex);
    bool result = tasklist.empty();
    smutex_unlock(&mutex);
    return result; // Keep compiler happy until routine done.
}

/*
 * ------------------------------------------------------------------
 * enqueue --
 *
 *      Insert the task at the back of the queue.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void TaskQueue::
enqueue(Task task)
{
    // TODO: Your code here.
    smutex_lock(&mutex);
    tasklist.push(task);
    ++sz;
    scond_signal(&nonempty, &mutex);
    smutex_unlock(&mutex);
}

/*
 * ------------------------------------------------------------------
 * dequeue --
 *
 *      Remove the Task at the front of the queue and return it.
 *      If the queue is empty, block until a Task is inserted.
 *
 * Results:
 *      The Task at the front of the queue.
 *
 * ------------------------------------------------------------------
 */
Task TaskQueue::
dequeue()
{
    // TODO: Your code here.
    smutex_lock(&mutex);
    while (tasklist.empty()) 
        scond_wait(&nonempty, &mutex);

    Task front = tasklist.front();
    tasklist.pop();
    --sz;
    smutex_unlock(&mutex);
    return front; // Keep compiler happy until routine done.
}

