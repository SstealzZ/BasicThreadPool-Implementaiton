#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int numThreads) : stop(false) {
    for (int i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });

                    if (stop && tasks.empty())
                        return;

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();

    for (std::thread& worker : workers) {
        worker.join();
    }
}

template<typename F, typename... Args>
void ThreadPool::enqueue(F&& function, Args&&... args) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace([=] { function(args...); });
    }

    condition.notify_one();
}

// Main
void taskFunction(int taskId) {
    std::cout << "Task " << taskId << " executing in thread " << std::this_thread::get_id() << std::endl;
    // TODO: PLAZZA for example
}

int main() {
    ThreadPool pool(4);

    // Add tasks to the pool
    for (int i = 0; i < 8; ++i) {
        pool.enqueue(taskFunction, i);
    }

    // Safety sleep
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
