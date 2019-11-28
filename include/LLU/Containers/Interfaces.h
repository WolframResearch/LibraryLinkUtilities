/**
 * @file	Interfaces.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	November 28, 2019
 * @brief
 */
#ifndef LLU_CONTAINERS_INTERFACES_H
#define LLU_CONTAINERS_INTERFACES_H

#include "LLU/LibraryData.h"

namespace LLU {

	/**
 *  @brief Abstract class that defines a basic set of operations on an image
	 */
	struct ImageInterface {
		/**
		 *   @brief Get colorspace which describes how colors are represented as numbers
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColorSpace.html>
		 **/
		virtual colorspace_t colorspace() const = 0;

		/**
		 *   @brief Get number of rows
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRowCount.html>
		 **/
		virtual mint rows() const = 0;

		/**
		 *   @brief Get number of columns
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColumnCount.html>
		 **/
		virtual mint columns() const = 0;

		/**
		 *   @brief Get number of slices
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getSliceCount.html>
		 **/
		virtual mint slices() const = 0;

		/**
		 *   @brief Get number of channels
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getChannels.html>
		 **/
		virtual mint channels() const = 0;

		/**
		 *   @brief Check if there is an alpha channel in the image
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_alphaChannelQ.html>
		 **/
		virtual bool alphaChannelQ() const = 0;

		/**
		 *   @brief Check if the image is interleaved
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_interleavedQ.html>
		 **/
		virtual bool interleavedQ() const = 0;

		/**
		 *   @brief Check if the image is 3D
		 **/
		virtual bool is3D() const = 0;

		/**
		 * @brief   Get rank
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRank.html>
		 */
		virtual mint getRank() const = 0;

		/**
		 * @brief   Get the total number of pixels in the image
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getFlattenedLength.html>
		 */
		virtual mint getFlattenedLength() const = 0;

		/**
		 * @brief   Get the data type of the image
		 * @return  type of elements (see definition of \c imagedata_t)
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getDataType.html>
		 */
		virtual imagedata_t type() const = 0;

		/**
		 * @brief   Get access to raw image data. Use with caution.
		 * @return  pointer to the raw data
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRawData.html>
		 */
		virtual void* rawData() const = 0;
	};
}

#endif	  // LLU_CONTAINERS_INTERFACES_H
