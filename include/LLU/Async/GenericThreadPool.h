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
#include <type_traits>
#include <vector>

#include "LLU/Async/Queue.h"

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

	template<typename BaseQueue>
	class WorkStealingQueue {
	private:
		using DataType = typename BaseQueue::value_type;
		BaseQueue theQueue;
		mutable std::mutex theMutex;

	public:
		WorkStealingQueue() = default;
		WorkStealingQueue(const WorkStealingQueue& other) = delete;
		WorkStealingQueue& operator=(const WorkStealingQueue& other) = delete;

		void push(DataType data) {
			std::lock_guard<std::mutex> lock(theMutex);
			theQueue.push_front(std::move(data));
		}
		bool empty() const {
			std::lock_guard<std::mutex> lock(theMutex);
			return theQueue.empty();
		}
		bool tryPop(DataType& res) {
			std::lock_guard<std::mutex> lock(theMutex);
			if (theQueue.empty()) {
				return false;
			}
			res = std::move(theQueue.front());
			theQueue.pop_front();
			return true;
		}
		bool trySteal(DataType& res) {
			std::lock_guard<std::mutex> lock(theMutex);
			if (theQueue.empty()) {
				return false;
			}
			res = std::move(theQueue.back());
			theQueue.pop_back();
			return true;
		}
	};

	class ThreadJoiner {
		std::vector<std::thread>& threads;

	public:
		explicit ThreadJoiner(std::vector<std::thread>& threadsToJoin) : threads(threadsToJoin) {}
		~ThreadJoiner() {
			for (auto& t : threads) {
				if (t.joinable()) {
					t.join();
				}
			}
		}
	};

	template<typename PoolQueue, typename LocalQueue>
	class GenericThreadPool {
	public:
		using TaskType = typename PoolQueue::value_type;

	public:
		GenericThreadPool() : GenericThreadPool(std::thread::hardware_concurrency()) {};

		explicit GenericThreadPool(unsigned threadCount) : done(false), joiner(threads) {
			try {
				queues.resize(threadCount);
				for (unsigned i = 0; i < threadCount; ++i) {
					threads.emplace_back(&GenericThreadPool::workerThread, this, i);
				}
			} catch (...) {
				done = true;
				throw;
			}
		}

		~GenericThreadPool() {
			done = true;
		}

		template<typename FunctionType>
		std::future<std::invoke_result_t<FunctionType>> submit(FunctionType&& f) {
			using result_type = std::invoke_result_t<FunctionType>;
			std::packaged_task<result_type()> task(std::forward<FunctionType>(f));
			std::future<result_type> res(task.get_future());
			if (localWorkQueue) {
				localWorkQueue->push(TaskType {std::move(task)});
			} else {
				poolWorkQueue.push(TaskType {std::move(task)});
			}
			return res;
		}

		void runPendingTask() {
			TaskType task;
			if (popTaskFromLocalQueue(task) || popTaskFromPoolQueue(task) || popTaskFromOtherThreadQueue(task)) {
				task();
			} else {
				std::this_thread::yield();
			}
		}

	private:
		std::atomic_bool done;
		PoolQueue poolWorkQueue;
		std::vector<std::unique_ptr<LocalQueue>> queues;
		std::vector<std::thread> threads;
		ThreadJoiner joiner;
		static thread_local LocalQueue* localWorkQueue;
		static thread_local unsigned myIndex;
		void workerThread(unsigned my_index_) {
			myIndex = my_index_;
			localWorkQueue = queues[myIndex].get();
			while (!done) {
				runPendingTask();
			}
		}
		bool popTaskFromLocalQueue(TaskType& task) {
			return localWorkQueue && localWorkQueue->tryPop(task);
		}
		bool popTaskFromPoolQueue(TaskType& task) {
			return poolWorkQueue.tryPop(task);
		}
		bool popTaskFromOtherThreadQueue(TaskType& task) {
			for (unsigned i = 0; i < queues.size(); ++i) {
				unsigned const index = (myIndex + i + 1) % queues.size();
				if (queues[index]->trySteal(task)) {
					return true;
				}
			}
			return false;
		}
	};

	template<typename PoolQueue, typename LocalQueue>
	thread_local LocalQueue* GenericThreadPool<PoolQueue, LocalQueue>::localWorkQueue = nullptr;

	template<typename PoolQueue, typename LocalQueue>
	thread_local unsigned GenericThreadPool<PoolQueue, LocalQueue>::myIndex = 0;

	using ThreadPool = GenericThreadPool<ThreadsafeQueue<FunctionWrapper>, WorkStealingQueue<std::deque<FunctionWrapper>>>;
}

#endif	  // LLU_ASYNC_THREADPOOL_H
