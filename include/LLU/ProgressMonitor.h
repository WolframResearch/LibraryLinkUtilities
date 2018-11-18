/**
 * @file	ProgressMonitor.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	
 */
#ifndef LLU_PROGRESSMONITOR_H
#define LLU_PROGRESSMONITOR_H


#include "LLU/Containers/Tensor.h"

namespace LibraryLinkUtils {

	class ProgressMonitor {
	public:
		explicit ProgressMonitor(Tensor<double> sharedIndicator, double step = .1);

		ProgressMonitor(ProgressMonitor&) = delete;

		ProgressMonitor(ProgressMonitor&&) = default;

		~ProgressMonitor();

		double get() const;

		void set(double progressValue);

		double getStep() const;

		void setStep(double stepValue);

		void checkAbort() const;

		ProgressMonitor& operator++();

		ProgressMonitor& operator +=(double progress);

		ProgressMonitor& operator--();

		ProgressMonitor& operator -=(double progress);

	private:

		Tensor<double> sharedIndicator;

		double step;
	};

}
#endif //LLU_PROGRESSMONITOR_H
