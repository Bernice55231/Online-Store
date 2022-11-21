#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>

#include "EStore.h"
#include "TaskQueue.h"
#include "RequestGenerator.h"


class Simulation
{
    public:
    TaskQueue supplierTasks;
    TaskQueue customerTasks;
    EStore store;

    int maxTasks;
    int numSuppliers;
    int numCustomers;

    explicit Simulation(bool useFineMode) : store(useFineMode) { }
};

/*
 * ------------------------------------------------------------------
 * supplierGenerator --
 *
 *      The supplier generator thread. The argument is a pointer to
 *      the shared Simulation object.
 *
 *      Enqueue arg->maxTasks requests to the supplier queue, then
 *      stop all supplier threads by enqueuing arg->numSuppliers
 *      stop requests.
 *
 *      Use a Supplier to generate and enqueue
 *      requests.
 *
 *      This thread should exit when done.
 *
 * Results:
 *      Does not return. Exit instead.
 *
 * ------------------------------------------------------------------
 */
static void*
supplierGenerator(void* arg)
{
    // TODO: Your code here.
    Simulation* s = (Simulation *) arg;
    SupplierRequestGenerator* sg = new SupplierRequestGenerator(&(s->supplierTasks));
    sg->enqueueTasks(s->maxTasks, &(s->store));
    sg->enqueueStops(s->numSuppliers);
    
    sthread_exit();
    return NULL; // Keep compiler happy.
}

/*
 * ------------------------------------------------------------------
 * customerGenerator --
 *
 *      The customer generator thread. The argument is a pointer to
 *      the shared Simulation object.
 *
 *      Enqueue arg->maxTasks requests to the customer queue, then
 *      stop all customer threads by enqueuing arg->numCustomers
 *      stop requests.
 *
 *      Use a Customerx to generate and enqueue
 *      requests.  For the fineMode argument to the constructor
 *      of CustomerRequestGenerator, use the output of
 *      store.fineModeEnabled() method, where store is a field
 *      in the Simulation class.
 *
 *      This thread should exit when done.
 *
 * Results:
 *      Does not return. Exit instead.
 *
 * ------------------------------------------------------------------
 */
static void*
customerGenerator(void* arg)
{
    // TODO: Your code here.
    Simulation* s = (Simulation *) arg;
    CustomerRequestGenerator* cg = new CustomerRequestGenerator(&(s->customerTasks), s->store.fineModeEnabled());
    cg->enqueueTasks(s->maxTasks, &(s->store));
    cg->enqueueStops(s->numCustomers);
    
    sthread_exit();
    return NULL; // Keep compiler happy.
}

/*
 * ------------------------------------------------------------------
 * supplier --
 *
 *      The main supplier thread. The argument is a pointer to the
 *      shared Simulation object.
 *
 *      Dequeue Tasks from the supplier queue and execute them.
 *
 * Results:
 *      Does not return.
 *
 * ------------------------------------------------------------------
 */
static void*
supplier(void* arg)
{
    // TODO: Your code here.
    Simulation* s = (Simulation *) arg;
    for(;;) {
        Task toDo = s->supplierTasks.dequeue();
        toDo.handler(toDo.arg);
    }
    sthread_exit();
    return NULL; // Keep compiler happy.
}

/*
 * ------------------------------------------------------------------
 * customer --
 *
 *      The main customer thread. The argument is a pointer to the
 *      shared Simulation object.
 *
 *      Dequeue Tasks from the customer queue and execute them.
 *
 * Results:
 *      Does not return.
 *
 * ------------------------------------------------------------------
 */
static void*
customer(void* arg)
{
    // TODO: Your code here.
    Simulation* s = (Simulation *) arg;
    for(;;) {
        Task toDo = s->customerTasks.dequeue();
        toDo.handler(toDo.arg);
    }
    sthread_exit();
    return NULL; // Keep compiler happy.
}

/*
 * ------------------------------------------------------------------
 * startSimulation --
 *      Create a new Simulation object. This object will serve as
 *      the shared state for the simulation. 
 *
 *      Create the following threads:
 *          - 1 supplier generator thread.
 *          - 1 customer generator thread.
 *          - numSuppliers supplier threads.
 *          - numCustomers customer threads.
 *
 *      After creating the worker threads, the main thread
 *      should wait until all of them exit, at which point it
 *      should return.
 *
 *      Hint: Use sthread_join.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
static void
startSimulation(int numSuppliers, int numCustomers, int maxTasks, bool useFineMode)
{
    // TODO: Your code here.
    Simulation* s = new Simulation(useFineMode);
    s->maxTasks = maxTasks;
    s->numSuppliers = numSuppliers;
    s->numCustomers = numCustomers;

    // create threads:
    sthread_t sg_t;
    sthread_t cg_t;

    sthread_create(&sg_t, supplierGenerator, s); // s: simulation object
    sthread_create(&cg_t, customerGenerator, s);

    sthread_t st[numSuppliers];
    int st_count = 0;
    while(st_count < numSuppliers) {
        sthread_create(&st[st_count], supplier, s);
        st_count++;
    }

    sthread_t ct[numCustomers];
    int ct_count = 0;
    while(ct_count < numCustomers) {
        sthread_create(&ct[ct_count], customer, s);
        ct_count++;
    }

    // the main thread wait until all of them exit:
    sthread_join(sg_t);
    sthread_join(cg_t);

    // wait supplier threads and customer threads:
    for (int i = 0; i < numCustomers; i++) {
        sthread_join(ct[i]);
    }
    for (int i = 0; i < numSuppliers; i++) {
        sthread_join(st[i]);
    }
    
    


    return;

}

int main(int argc, char **argv)
{
    bool useFineMode = false;

    // Seed the random number generator.
    // You can remove this line or set it to some constant to get deterministic
    // results, but make sure you put it back before turning in.
    srand(time(NULL));

    if (argc > 1)
        useFineMode = strcmp(argv[1], "--fine") == 0;
    startSimulation(10, 10, 100, useFineMode);
    return 0;
}

