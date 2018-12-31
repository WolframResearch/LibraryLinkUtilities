/**
 * @file	ProgressMonitor.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	
 */
#ifndef LLUTILS_PROGRESSMONITOR_H
#define LLUTILS_PROGRESSMONITOR_H


#include "LLU/Containers/Tensor.h"

namespace LibraryLinkUtils {

	/**
	 * @brief	Stores and updates current progress of computation in a location shared between the library and WL Kernel.
	 *
	 * ProgressMonitor receives an instance of a shared Tensor<double> in constructor and becomes the (shared) owner. Progress is
	 * a single number of type \c double between 0. and 1.
	 * This class offers an interface for modifying the progress value (increase/decrease by a given step or set to an arbitrary value) and
	 * one static function for checking if a user requested to abort the computation in WL Kernel.
	 **/
	class ProgressMonitor {
	public:
		/**
		 * @brief Construct a new ProgressMonitor
		 * @param sharedIndicator - shared Tensor of type \c double. If tensor length is smaller than 1, the behavior is undefined.
		 * @param step - by how much to modify the progress value in operator++ and operator--
		 */
		explicit ProgressMonitor(Tensor<double> sharedIndicator, double step = .1);

		/**
		 * @brief Copy-constructor is disabled because ProgressMonitor shares a Tensor with WL Kernel.
		 */
		ProgressMonitor(ProgressMonitor&) = delete;

		/**
		 * @brief Default move-constructor.
		 */
		ProgressMonitor(ProgressMonitor&&) = default;

		/**
		 * @brief Destructor will disown the shared Tensor.
		 */
		~ProgressMonitor();

		/**
		 * @brief Get current value of the progress.
		 * @return current value of the progress (a \c double between 0. and 1.)
		 */
		double get() const;

		/**
		 * @brief Set current progress value.
		 * @param progressValue - current progress (a \c double between 0. and 1.)
		 */
		void set(double progressValue);

		/**
		 * @brief Get current step value.
		 * @return current step value
		 */
		double getStep() const;

		/**
		 * @brief Change step value to a given number.
		 * @param stepValue - any real number between 0. and 1.
		 */
		void setStep(double stepValue);

		/**
		 * @brief Check whether user requested to abort the computation in WL Kernel.
		 */
		static void checkAbort();

		/**
		 * @brief Increment current progress value by \c step.
		 * @return self
		 */
		ProgressMonitor& operator++();

		/**
		 * @brief Increment current progress value by a given number.
		 * @param progress - a real number between 0. and (1 - get()). No validation is done.
		 * @return self
		 */
		ProgressMonitor& operator +=(double progress);

		/**
		 * @brief Decrement current progress value by \c step.
		 * @return self
		 */
		ProgressMonitor& operator--();

		/**
		 * @brief Decrement current progress value by a given number.
		 * @param progress - a real number between 0. and get(). No validation is done.
		 * @return self
		 */
		ProgressMonitor& operator -=(double progress);

	private:

		/// This tensor stores current progress as the first element.
		Tensor<double> sharedIndicator;

		/// Step determines by how much will ++ or -- operators modify the current progress.
		double step;
	};

}
#endif //LLUTILS_PROGRESSMONITOR_H
