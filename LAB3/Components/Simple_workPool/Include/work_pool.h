#ifndef WORK_POOL
#define WORK_POLL
#include <stdio.h>
#include <stdint.h>
#include <mutex>
#include <queue>
#include <thread>

struct Task
{
    uint16_t task_id;
    Task (uint16_t id) : task_id(id) {};
};

class TaskQueue {
    private:  
        std::mutex mtx;
        std::queue<Task*> queue;
        static TaskQueue* instance;
    public:
        void enqueue(Task* task);
        Task* dequeue();
        static TaskQueue* get();
};

class Worker {
    private:
        bool stop;
        std::thread t;
        int id;
        void run();

    public:
        Worker (int id);
        void exit ();    
};

#endif