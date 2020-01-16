/**
 * @file	Utilities.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief
 */
#ifndef LLU_ASYNC_UTILITIES_H
#define LLU_ASYNC_UTILITIES_H

#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <vector>

namespace LLU::Async {
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
		explicit FunctionWrapper(F&& f) : impl {std::make_unique<TypeErasedCallable<F>>(std::forward<F>(f))} {}

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
		std::unique_ptr<TypeErasedCallableBase> impl = nullptr;
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

	template<typename FunctionType, typename... Args>
	std::packaged_task<std::invoke_result_t<FunctionType, Args...>()> getPackagedTask(FunctionType&& f, Args&&... args) {
		using result_type = std::invoke_result_t<FunctionType, Args...>;
		auto boundF = std::bind(std::forward<FunctionType>(f), std::forward<Args>(args)...);
		return std::packaged_task<result_type()> {std::move(boundF)};
	}

	class Pausable {
		std::atomic_bool pausedQ = false;
		std::mutex workersMutex;
		std::condition_variable pausedWorkers;
	public:
		void checkPause() {
			if (pausedQ) {
				std::unique_lock lck {workersMutex};
				pausedWorkers.wait(lck, [&]() -> bool { return !pausedQ; });
			}
		}
		void pause() {
			pausedQ = true;
		}
		void resume() {
			pausedQ = false;
			pausedWorkers.notify_all();
		}
	};
}

#endif	  // LLU_ASYNC_UTILITIES_H
