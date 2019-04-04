/**
 * @file	ProgressMonitor.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Implementation file for ProgressMonitor class
 */
#include "LLU/ProgressMonitor.h"

#include "LLU/Containers/LibDataHolder.h"
#include "LLU/ErrorLog/ErrorManager.h"

namespace LibraryLinkUtils {

	ProgressMonitor::ProgressMonitor(Tensor<double> sharedIndicator, double step) : sharedIndicator(std::move(sharedIndicator)), step(step) {}

	ProgressMonitor::~ProgressMonitor() {
		sharedIndicator.disown();
	}

	double ProgressMonitor::get() const {
		return sharedIndicator[0];
	}

	void ProgressMonitor::set(double progressValue) {
		sharedIndicator[0] = progressValue;
		checkAbort();
	}

	double ProgressMonitor::getStep() const {
		return step;
	}

	void ProgressMonitor::setStep(double stepValue) {
		step = stepValue;
		checkAbort();
	}

	void ProgressMonitor::checkAbort() {
		auto libData = LibDataHolder::getLibraryData();
		if (!libData) {
			ErrorManager::throwException(LLErrorName::MArgumentLibDataError);
		}
		if(libData->AbortQ()) {
			ErrorManager::throwException(LLErrorName::Aborted);
		}
	}

	ProgressMonitor& ProgressMonitor::operator++() {
		set(sharedIndicator[0] + step);
		return *this;
	}

	ProgressMonitor& ProgressMonitor::operator +=(double progress) {
		set(sharedIndicator[0] + progress);
		return *this;
	}

	ProgressMonitor& ProgressMonitor::operator--() {
		set(sharedIndicator[0] - step);
		return *this;
	}

	ProgressMonitor& ProgressMonitor::operator -=(double progress) {
		set(sharedIndicator[0] - progress);
		return *this;
	}

}