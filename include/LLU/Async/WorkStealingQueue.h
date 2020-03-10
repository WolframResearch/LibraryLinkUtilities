/**
 * @file	WorkStealingQueue.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief   Definition and implementation of a thread-safe queue, taken from A. Williams "C++ Concurrency in Action" 2nd Edition, chapter 6.
 */
#ifndef LLU_ASYNC_WORKSTEALINGQUEUE_H
#define LLU_ASYNC_WORKSTEALINGQUEUE_H

#include <mutex>

namespace LLU {
	template<typename BaseQueue>
	class WorkStealingQueue {
		using DataType = typename BaseQueue::value_type;

		BaseQueue theQueue;
		mutable std::mutex theMutex;

	public:
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
}

#endif	  // LLU_ASYNC_WORKSTEALINGQUEUE_H
