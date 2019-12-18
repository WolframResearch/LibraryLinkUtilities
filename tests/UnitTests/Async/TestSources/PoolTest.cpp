/**
 * @file
 * @brief
 */
#include <thread>

#include <LLU/Async/GenericThreadPool.h>
#include <LLU/ErrorLog/Logger.h>
#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

using namespace std::chrono_literals;

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

EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {}

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
			THREADSAFE_LOG("Job ", i, " starts.")
			std::this_thread::sleep_for(std::chrono::milliseconds(time));
			std::unique_lock lg {jobCounterMutex};
			if (++completedJobs == numJobs) {
				//lg.unlock();
				allJobsDone.notify_one();
			}
		});
	}
	THREADSAFE_LOG("Submitted ", numJobs, " jobs.")
	std::unique_lock lg {jobCounterMutex};
	allJobsDone.wait(lg, [&]{ return completedJobs == numJobs;});
	THREADSAFE_LOG("All jobs finished.")
}