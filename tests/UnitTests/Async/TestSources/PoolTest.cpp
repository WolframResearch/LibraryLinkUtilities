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

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	return 0;
}

EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {}

LLU_LIBRARY_FUNCTION(SleepyThreads) {
	auto numThreads = mngr.getInteger<mint>(0);
	if (numThreads <= 0) {
		numThreads = std::thread::hardware_concurrency();
	}
	LLU::ThreadPool tp {static_cast<unsigned int>(numThreads)};
	auto numJobs = mngr.getInteger<mint>(1);
	auto time = mngr.getInteger<mint>(2);
	for (int i = 0; i < numJobs; ++i) {
		tp.submit([=] { std::this_thread::sleep_for(std::chrono::milliseconds(time)); });
	}
}