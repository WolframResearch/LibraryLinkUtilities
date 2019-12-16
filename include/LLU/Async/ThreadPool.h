/**
 * @file
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief   Definition of the ThreadPool class.
 */

#ifndef LLU_ASYNC_THREADPOOL_H
#define LLU_ASYNC_THREADPOOL_H

#include <atomic>
#include <deque>
#include <functional>
#include <future>
#include <thread>
#include <vector>

namespace LLU {

	class FunctionWrapper {
		struct TypeErasedCallableBase {
			virtual void call() = 0;
			virtual ~TypeErasedCallableBase() = default;
		};

		template<typename F>
		struct TypeErasedCallable : TypeErasedCallableBase {
			explicit TypeErasedCallable(F&& f) : callable(std::forward<F>(f)) {}

			void call() override {
				callable();
			}
			F callable;
		};

	public:
		template<typename F>
		explicit FunctionWrapper(F&& f) : impl{std::make_unique<TypeErasedCallable<F>>(std::forward<F>(f))} {}

		template<typename F, typename... Args>
		explicit FunctionWrapper(F&& f, Args&&... args) {
			auto boundF = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
			impl = std::make_unique<TypeErasedCallable<F>>(std::move(boundF));
		}

		FunctionWrapper() = default;
		FunctionWrapper(FunctionWrapper&& other) noexcept = default;
		FunctionWrapper& operator=(FunctionWrapper&& other) noexcept = default;
		FunctionWrapper(const FunctionWrapper&) = delete;
		FunctionWrapper& operator=(const FunctionWrapper&) = delete;

		void operator()() {
			impl->call();

		}

	private:
		std::unique_ptr<TypeErasedCallableBase> impl;
	};

	template<template<typename> class Queue = std::deque>
	class WorkStealingQueue {
	private:
		using data_type = FunctionWrapper;
		Queue<data_type> the_queue;
		mutable std::mutex the_mutex;

	public:
		WorkStealingQueue() = default;
		WorkStealingQueue(const WorkStealingQueue& other) = delete;
		WorkStealingQueue& operator=(const WorkStealingQueue& other) = delete;

		void push(data_type data) {
			std::lock_guard<std::mutex> lock(the_mutex);
			the_queue.push_front(std::move(data));
		}
		bool empty() const {
			std::lock_guard<std::mutex> lock(the_mutex);
			return the_queue.empty();
		}
		bool try_pop(data_type& res) {
			std::lock_guard<std::mutex> lock(the_mutex);
			if (the_queue.empty()) {
				return false;
			}
			res = std::move(the_queue.front());
			the_queue.pop_front();
			return true;
		}
		bool try_steal(data_type& res) {
			std::lock_guard<std::mutex> lock(the_mutex);
			if (the_queue.empty()) {
				return false;
			}
			res = std::move(the_queue.back());
			the_queue.pop_back();
			return true;
		}
	};

	template<typename LocalQueue>
	class ThreadPool {
	public:
		ThreadPool() : done(false), joiner(threads) {
			unsigned const thread_count = std::thread::hardware_concurrency();
			try {
				for (unsigned i = 0; i < thread_count; ++i) {
					queues.push_back(std::make_unique<LocalQueue>());
				}
				for (unsigned i = 0; i < thread_count; ++i) {
					threads.emplace_back(&ThreadPool::worker_thread, this, i);
				}
			} catch (...) {
				done = true;
				throw;
			}
		}
		~ThreadPool() {
			done = true;
		}
		template<typename FunctionType>
		std::future<std::result_of_t<FunctionType()>> submit(FunctionType f) {
			using result_type = std::result_of_t<FunctionType()>;
			std::packaged_task<result_type()> task(f);
			std::future<result_type> res(task.get_future());
			if (local_work_queue) {
				local_work_queue->push(std::move(task));
			} else {
				pool_work_queue.push(std::move(task));
			}
			return res;
		}
		void run_pending_task() {
			TaskType task;
			if (pop_task_from_local_queue(task) || pop_task_from_pool_queue(task) || pop_task_from_other_thread_queue(task)) {
				task();
			} else {
				std::this_thread::yield();
			}
		}

	private:
		using TaskType = FunctionWrapper;
		std::atomic_bool done;
		threadsafe_queue<TaskType> pool_work_queue;
		std::vector<std::unique_ptr<LocalQueue>> queues;
		std::vector<std::thread> threads;
		join_threads joiner;
		static thread_local LocalQueue* local_work_queue;
		static thread_local unsigned my_index;
		void worker_thread(unsigned my_index_) {
			my_index = my_index_;
			local_work_queue = queues[my_index].get();
			while (!done) {
				run_pending_task();
			}
		}
		bool pop_task_from_local_queue(TaskType& task) {
			return local_work_queue && local_work_queue->try_pop(task);
		}
		bool pop_task_from_pool_queue(TaskType& task) {
			return pool_work_queue.try_pop(task);
		}
		bool pop_task_from_other_thread_queue(TaskType& task) {
			for (unsigned i = 0; i < queues.size(); ++i) {
				unsigned const index = (my_index + i + 1) % queues.size();
				if (queues[index]->try_steal(task)) {
					return true;
				}
			}
			return false;
		}

	};
}

#endif	  // LLU_ASYNC_THREADPOOL_H
