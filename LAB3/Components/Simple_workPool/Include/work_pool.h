#ifndef WORK_POOL
#define WORK_POLL
#include <stdio.h>
#include <stdint.h>
#include <mutex>
#include <queue>
#include <thread>
#include <functional>
#include <atomic>
#include <condition_variable>
struct Task
{
    std::function<void(void* parameters)> func;
    void* parameters;
    Task (    std::function<void(void* parameters)> func,
    void* parameters) : func (func), parameters (parameters){}
};
class TaskQueue {
    private:  
        std::mutex mtx;
        std::queue<Task*> queue;
        static TaskQueue* instance;
        std::atomic<int> running_tasks_count = 0;  // Tracks how many tasks are currently running or waiting to run in the queue.
        std::condition_variable completion_cv; // Used by the main thread to sleep until the count drops to zero.
    public:
        void enqueue(Task* task);
        Task* dequeue();
        static TaskQueue* get();
        void wait_for_batch_completion(); 
        void notify_task_finished(); 
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