#include "work_pool.h"

/**
 * @brief Producer add a task into queue
 * @param task  A task needed to process
 */

void TaskQueue::enqueue (Task* task){
    std::lock_guard<std::mutex> lock (this->mtx);
    this->queue.push (task);
}

/**
 * @brief Worker pool get the task from queue
 * @return The task which is front of the queue
 */

Task* TaskQueue::dequeue (){
    std::lock_guard<std::mutex> lock (this->mtx);
    if (!this->queue.empty()){
        Task* task = queue.front();
        queue.pop();
        return task;
    }
    return nullptr;
}

/**
 * @brief Return the one and only instance of the task queue (be allocated if it is null)
 * @return The instance of the task queue
 */
TaskQueue* TaskQueue::get(){
    if (instance == nullptr)    instance = new TaskQueue(); // instance: static object
    return instance;
}



/**
 * @brief This function creates a new thread which is a instance for the run function of a specific worker
 */
Worker::Worker (int id) : id(id), stop (false) {
    this->t = std::thread (&Worker::run,this);
}

/**
 * @brief A Woker tries to get the task from the task queue if it is not empty
 */
void Worker::run() {
    while (!this->stop){
        Task* task = TaskQueue::get()->dequeue();   // Get the task fronted of the task queue
        if (task != nullptr){
            printf ("Worker %d executes task %d\n", this->id, task->task_id);
            delete task;
        }
        else {
            // Sleep briefly to avoid busy-waiting (optional)
            std::this_thread::sleep_for (std::chrono::milliseconds (10));
        }
    }
}

/**
 * @brief Terminate one worker
 */

 void Worker::exit() {
    this->stop = true;
    this->t.join();
 }
TaskQueue* TaskQueue::instance = nullptr;
