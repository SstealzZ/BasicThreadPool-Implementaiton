#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(int numThreads);
    ~ThreadPool();
    
    template<typename F, typename... Args>
    void enqueue(F&& function, Args&&... args);

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

#endif // THREADPOOL_HPP
