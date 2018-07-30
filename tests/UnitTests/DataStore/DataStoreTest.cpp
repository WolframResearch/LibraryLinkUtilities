/** 
 * @file	DataStoreTest.cpp
 * @date	June 7, 2018
 * @author	rafalc
 * @brief	Source code for DataStore unit tests.
 */

#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#include <tuple>

#include "WolframLibrary.h"
#include "WolframIOLibraryFunctions.h"

#include "LLU/LLU.h"


void pushSimulatedData(mint asyncObjID, void* taskData) {
	try {
		using TupleType = std::tuple<mint, bool>;
		std::unique_ptr<TupleType> td { static_cast<TupleType*>(taskData) };

		auto pauseTime = std::get<0>(*td);
		auto shouldCrashQ = std::get<1>(*td);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 255);

		auto* libData = LibraryLinkUtils::MArgumentManager::getLibraryData();
		while (libData->ioLibraryFunctions->asynchronousTaskAliveQ(asyncObjID)) {

			std::this_thread::sleep_for(std::chrono::milliseconds { pauseTime });

			int width = 640;
			int height = 480;
			LibraryLinkUtils::Image<uint8_t> newImage { width, height, 3, MImage_CS_RGB, true };

			for (auto&& channelValue : newImage) {
				channelValue = dis(gen);
			}

			DataStore asyncTaskDataStore = libData->ioLibraryFunctions->createDataStore();
			libData->ioLibraryFunctions->DataStore_addMImage(asyncTaskDataStore, newImage.getInternal());

			if (!shouldCrashQ) {
				newImage.setOwner(false);
			}

			char eventType[] = "currentImage";
			libData->ioLibraryFunctions->raiseAsyncEvent(asyncObjID, eventType, asyncTaskDataStore);
		}

	} catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

EXTERN_C DLLEXPORT int StartSimulatedInputStream(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);

		auto frameRate = mngr.getInteger<mint>(0);
		auto shouldCrash = mngr.getBoolean(1);
		auto pausems = 1000 / frameRate;

		auto taskData = std::make_unique<std::tuple<mint, bool>>(pausems, shouldCrash);
		auto asyncObjID = libData->ioLibraryFunctions->createAsynchronousTaskWithThread(pushSimulatedData, taskData.release());

		mngr.setInteger(asyncObjID);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

