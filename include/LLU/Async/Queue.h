/**
 * @file
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief   Definition and implementation of a thread-safe queue, taken from A. Williams "C++ Concurrency in Action" 2nd Edition, chapter 6.
 */
#ifndef LLU_ASYNC_QUEUE_H
#define LLU_ASYNC_QUEUE_H

#include <condition_variable>
#include <memory>
#include <mutex>

namespace LLU {

	template<typename T>
	class ThreadsafeQueue {
	public:
		using value_type = T;

	public:
		ThreadsafeQueue() : head(new Node), tail(head.get()) {}
		ThreadsafeQueue(const ThreadsafeQueue& other) = delete;
		ThreadsafeQueue& operator=(const ThreadsafeQueue& other) = delete;

		std::shared_ptr<value_type> tryPop();
		bool tryPop(value_type& value);
		std::shared_ptr<value_type> waitPop();
		void waitPop(value_type& value);
		void push(value_type new_value);
		bool empty();

	private:
		struct Node {
			std::shared_ptr<value_type> data;
			std::unique_ptr<Node> next;
		};

		std::mutex head_mutex;
		std::unique_ptr<Node> head;
		std::mutex tail_mutex;
		Node* tail;
		std::condition_variable data_cond;

		Node* getTail() {
			std::lock_guard<std::mutex> tail_lock(tail_mutex);
			return tail;
		}

		std::unique_ptr<Node> popHead() {
			std::unique_ptr<Node> old_head = std::move(head);
			head = std::move(old_head->next);
			return old_head;
		}

		std::unique_lock<std::mutex> waitForData() {
			std::unique_lock<std::mutex> head_lock(head_mutex);
			data_cond.wait(head_lock, [&] { return head.get() != getTail(); });
			return std::move(head_lock);
		}

		std::unique_ptr<Node> waitPopHead() {
			std::unique_lock<std::mutex> head_lock(waitForData());
			return popHead();
		}

		std::unique_ptr<Node> waitPopHead(value_type& value) {
			std::unique_lock<std::mutex> head_lock(waitForData());
			value = std::move(*head->data);
			return popHead();
		}

		std::unique_ptr<Node> tryPopHead() {
			std::lock_guard<std::mutex> head_lock(head_mutex);
			if (head.get() == getTail()) {
				return std::unique_ptr<Node>();
			}
			return popHead();
		}

		std::unique_ptr<Node> tryPopHead(value_type& value) {
			std::lock_guard<std::mutex> head_lock(head_mutex);
			if (head.get() == getTail()) {
				return std::unique_ptr<Node>();
			}
			value = std::move(*head->data);
			return popHead();
		}
	};

	template<typename T>
	void ThreadsafeQueue<T>::push(T new_value) {
		std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
		std::unique_ptr<Node> p(new Node);
		{
			std::lock_guard<std::mutex> tail_lock(tail_mutex);
			tail->data = new_data;
			Node* const new_tail = p.get();
			tail->next = std::move(p);
			tail = new_tail;
		}
		data_cond.notify_one();
	}

	template<typename T>
	std::shared_ptr<T> ThreadsafeQueue<T>::waitPop() {
		std::unique_ptr<Node> const old_head = waitPopHead();
		return old_head->data;
	}

	template<typename T>
	void ThreadsafeQueue<T>::waitPop(T& value) {
		std::unique_ptr<Node> const old_head = waitPopHead(value);
	}

	template<typename T>
	std::shared_ptr<T> ThreadsafeQueue<T>::tryPop() {
		std::unique_ptr<Node> const old_head = tryPopHead();
		return old_head ? old_head->data : std::shared_ptr<T>();
	}

	template<typename T>
	bool ThreadsafeQueue<T>::tryPop(T& value) {
		std::unique_ptr<Node> const old_head = tryPopHead(value);
		return static_cast<bool>(old_head);
	}

	template<typename T>
	bool ThreadsafeQueue<T>::empty() {
		std::lock_guard<std::mutex> head_lock(head_mutex);
		return (head.get() == getTail());
	}
}
#endif	  // LLU_ASYNC_QUEUE_H
