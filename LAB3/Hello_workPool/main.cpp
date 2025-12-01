#include "work_pool.h"
#include "main.h"
#include <thread>
#include <vector>
int main(int argc, char* argv[]){
    std::vector<Worker*> workers;
    for (int i = 0; i < NUM_THREAD; ++i)    workers.push_back (new Worker(i));
    for (int i = 0; i < NUM_TASK; ++i)      TaskQueue::get()->enqueue(new Task(i));

    // Sleep main thread to allow workers some time to deal with tasks
    std::this_thread::sleep_for (std::chrono::seconds(1));

    // Shutdown all workers
    for (Worker* worker : workers){
        worker->exit();
        delete worker;
    }
    return 0;
}