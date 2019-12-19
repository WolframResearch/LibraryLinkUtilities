/**
 * @file
 * @brief
 */
#include <numeric>
#include <thread>

#include <LLU/Async/GenericThreadPool.h>
#include <LLU/ErrorLog/Logger.h>
#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

using namespace std::chrono_literals;
using LLU::NumericArray;

namespace {
	std::mutex logMutex;
}

#define THREADSAFE_LOG(...) \
	{ \
		std::lock_guard mlg {logMutex}; \
		LLU_DEBUG(__VA_ARGS__); \
	}

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	return 0;
}

LLU_LIBRARY_FUNCTION(SleepyThreads) {
	auto numThreads = mngr.getInteger<mint>(0);
	if (numThreads <= 0) {
		numThreads = std::thread::hardware_concurrency() > 1? std::thread::hardware_concurrency() - 1 : 1;
	}
	THREADSAFE_LOG("Running on ", numThreads, " threads.")
	LLU::ThreadPool tp {static_cast<unsigned int>(numThreads)};
	const auto numJobs = mngr.getInteger<mint>(1);
	const auto time = mngr.getInteger<mint>(2);
	std::condition_variable allJobsDone;
	std::mutex jobCounterMutex;
	int completedJobs = 0;
	for (int i = 0; i < numJobs; ++i) {
		tp.submit([&, i] {
			std::this_thread::sleep_for(std::chrono::milliseconds(time));
			std::unique_lock lg {jobCounterMutex};
			if (++completedJobs == numJobs) {
				allJobsDone.notify_one();
			}
		});
	}
	THREADSAFE_LOG("Submitted ", numJobs, " jobs.")
	std::unique_lock lg {jobCounterMutex};
	allJobsDone.wait(lg, [&] { return completedJobs == numJobs; });
	THREADSAFE_LOG("All jobs finished.")
}

LLU_LIBRARY_FUNCTION(Accumulate) {
	auto data = mngr.getGenericNumericArray<LLU::Passing::Constant>(0);
	const auto numThreads = mngr.getInteger<mint>(1);
	const auto jobSize = mngr.getInteger<mint>(2);
	LLU::ThreadPool tp {static_cast<unsigned int>(numThreads)};

	const auto numJobs = (data.getFlattenedLength() + jobSize - 1) / jobSize;
	LLU::asTypedNumericArray(data, [&](auto&& typedNA) {
		using T = typename std::remove_reference_t<decltype(typedNA)>::value_type;
		std::vector<std::future<T>> partialResults {static_cast<size_t>(numJobs) - 1};
		auto blockBegin = std::cbegin(typedNA);
		for (int i = 0; i < numJobs - 1; ++i) {
			partialResults[i] = tp.submit(std::accumulate<typename NumericArray<T>::const_iterator, T>, blockBegin, blockBegin + jobSize, T {});
			blockBegin += jobSize;
		}
		T remainderSum = std::accumulate(blockBegin, std::cend(typedNA), T {});
		T totalSum =
			std::accumulate(std::begin(partialResults), std::end(partialResults), remainderSum, [](T currentSum, auto& fut) { return currentSum + fut.get(); });
		mngr.set(NumericArray<T> {totalSum});
	});
}