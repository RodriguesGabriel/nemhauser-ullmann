#pragma once

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

typedef std::uint_fast32_t ui32;

class thread_pool {
   public:
    thread_pool(const ui32 &_thread_count = std::thread::hardware_concurrency())
        : thread_count(_thread_count ? _thread_count : std::thread::hardware_concurrency()),
          threads(new std::thread[_thread_count ? _thread_count
                                                : std::thread::hardware_concurrency()]) {
        for (ui32 i = 0; i < thread_count; i++) {
            threads[i] = std::thread(&thread_pool::worker, this);
        }
    }

    ~thread_pool() {
        task_condition.notify_all();
        wait_condition.notify_all();
        for (ui32 i = 0; i < thread_count; i++) {
            threads[i].join();
        }
    }

    template <typename F>
    void push_task(const F &task) {
        tasks_total++;
        {
            const std::unique_lock lock(queue_mutex);
            tasks.push(std::function<void()>(task));
            task_condition.notify_one();
        }
    }

    void wait_for_tasks() {
        std::unique_lock lock(wait_mutex);
        wait_condition.wait(lock, [=] { return tasks_total == 0; });
    }

    template <typename T, typename F>
    void parallelize_loop(const T &first_index, const T &last_index, const F &loop,
                          ui32 num_tasks = 0) {
        if (num_tasks == 0) num_tasks = thread_count;
        assert(first_index < last_index);
        size_t total_size = last_index - first_index + 1;
        size_t block_size = total_size / num_tasks;
        if (block_size == 0) {
            block_size = 1;
            num_tasks = (ui32)total_size > 1 ? (ui32)total_size : 1;
        }
        for (ui32 t = 0; t < num_tasks; t++) {
            T start = (T)(t * block_size + first_index);
            T end = (t == num_tasks - 1) ? last_index : (T)((t + 1) * block_size + first_index);
            push_task(std::bind(loop, start, end));
        }
        wait_for_tasks();
    }

   private:
    std::mutex queue_mutex, wait_mutex;

    std::condition_variable task_condition, wait_condition;

    std::queue<std::function<void()>> tasks;

    ui32 thread_count;

    std::atomic<ui32> tasks_total = 0;

    std::unique_ptr<std::thread[]> threads;

    bool pop_task(std::function<void()> &task) {
        std::unique_lock lock(queue_mutex);
        task_condition.wait(lock);
        if (tasks.empty()) {
            return false;
        }
        task = std::move(tasks.front());
        tasks.pop();
        return true;
    }

    void worker() {
        std::function<void()> task;
        while (true) {
            if (!pop_task(task)) return;
            task();
            tasks_total--;
            {
                std::unique_lock lock(wait_mutex);
                wait_condition.notify_one();
            }
        }
        // std::cerr << "worker out!\n";
    }
};
