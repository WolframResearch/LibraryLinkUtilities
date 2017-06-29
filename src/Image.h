/** 
 * @file	Image.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Templated C++ wrapper for MImage
 *
 */
#ifndef LLUTILS_IMAGE_H_
#define LLUTILS_IMAGE_H_

#include "WolframLibrary.h"
#include "WolframImageLibrary.h"


#include "LibraryLinkError.hpp"
#include "MArray.hpp"

namespace LibraryLinkUtils {

	/**
	 * @class Image
	 * @brief This is a class template, where template parameter T is the type of data elements. Image is derived from MArray.
	 *
	 * Image<> classes automate creation and deletion of MImages.
	 * They are strongly typed (no void* to underlying memory) and almost all functions from \<algorithms\> can be used on Image.
	 *
	 * @tparam	T - type of underlying data
	 */
	template<typename T>
	class Image: public MArray<T> {

		/// Functions from WolframImageLibrary
		using MArray<T>::imgFuns;

		///	Short name for exception class
		using LibraryLinkError = LibraryLinkUtils::LibraryLinkError<LLErrorCode>;

	public:
		/**
		 *   @brief         Constructs new 2D Image
		 *   @param[in]     w - Image width (number of columns)
		 *   @param[in]     h - Image height (number of rows)
		 *   @param[in]     channels - number of channels
		 *   @param[in]     cs - color space
		 *   @param[in]		interleavingQ - whether Image data should be interleaved
		 *   @throws		see Image<T>::Image(mint nFrames, mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ)
		 **/
		Image(mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ);

		/**
		 *   @brief         Constructs new 3D Image
		 *   @param[in]     nFrames - number of 2D frames/slices
		 *   @param[in]     w - Image width (number of columns)
		 *   @param[in]     h - Image height (number of rows)
		 *   @param[in]     channels - number of channels
		 *   @param[in]     cs - color space
		 *   @param[in]     interleavingQ - whether Image data should be interleaved
		 *   @throws        LLErrorCode::ImageInitError - if structure with WolframImageLibrary functions is not initialized
		 *   @throws		LLErrorCode::ImageNewError - if internal MImage creation failed
		 **/
		Image(mint nFrames, mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ);

		/**
		 *   @brief         Constructs Image based on MImage
		 *   @param[in]     mi - LibraryLink structure to be wrapped
		 *   @throws        LLErrorCode::ImageInitError - if structure with WolframImageLibrary functions is not initialized
		 *   @throws		LLErrorCode::ImageTypeError - if template parameter \b T does not match MImage data type
		 *   @throws		LLErrorCode::ImageSizeError - if constructor failed to calculate image dimensions properly
		 **/
		Image(const MImage mi);

		/**
		 *   @brief	Free internal MImage if necessary
		 **/
		virtual ~Image();

		/**
		 *   @brief Get colorspace of internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColorSpace.html>
		 **/
		colorspace_t colorspace() const noexcept {
			return imgFuns->MImage_getColorSpace(internalMI);
		}

		/**
		 *   @brief Get number of rows in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRowCount.html>
		 **/
		mint rows() const noexcept {
			return imgFuns->MImage_getRowCount(internalMI);
		}

		/**
		 *   @brief Get number of columns in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColumnCount.html>
		 **/
		mint columns() const noexcept {
			return imgFuns->MImage_getColumnCount(internalMI);
		}

		/**
		 *   @brief Get number of slices in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getSliceCount.html>
		 **/
		mint slices() const noexcept {
			return imgFuns->MImage_getSliceCount(internalMI);
		}

		/**
		 *   @brief Get number of channels in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getChannels.html>
		 **/
		mint channels() const noexcept {
			return imgFuns->MImage_getChannels(internalMI);
		}

		/**
		 *   @brief Check if there is an alpha channel in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_alphaChannelQ.html>
		 **/
		bool alphaChannelQ() const noexcept {
			return imgFuns->MImage_alphaChannelQ(internalMI);
		}

		/**
		 *   @brief Check if internal MImage is interleaved
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_interleavedQ.html>
		 **/
		bool interleavedQ() const noexcept {
			return imgFuns->MImage_interleavedQ(internalMI);
		}

		/**
		 *   @brief Check if internal MImage is 3D
		 **/
		bool is3D() const noexcept {
			return allowSlices;
		}

	private:
		/**
		 *   @brief Return raw pointer to underlying image data
		 *   @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRawData.html>
		 **/
		T* getData() const noexcept override {
			return static_cast<T*>(imgFuns->MImage_getRawData(internalMI));
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::indexError()
		 *   @throws 	LibraryLinkError(LLErrorCode::ImageIndexError)
		 **/
		void indexError() const override {
			throw LibraryLinkError(LLErrorCode::ImageIndexError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::initError()
		 *   @throws 	LibraryLinkError(LLErrorCode::ImageInitError)
		 **/
		void initError() const override {
			throw LibraryLinkError(LLErrorCode::ImageInitError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::sizeError()
		 *   @throws 	LibraryLinkError(LLErrorCode::ImageSizeError)
		 **/
		void sizeError() const override {
			throw LibraryLinkError(LLErrorCode::ImageSizeError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::createInternal()
		 *   @note		This is currently not supported so it always throws an exception
		 *   @throws 	LibraryLinkError(LLErrorCode::ImageNewError)
		 *
		 *   @todo		Implement creating new MImage via Image<T>::createInternal()
		 **/
		void createInternal() override {
			throw LibraryLinkError(LLErrorCode::ImageNewError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::freeInternal(). It frees internal MImage.
		 **/
		void freeInternal() noexcept override {
			this->imgFuns->MImage_free(internalMI);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::passInternal(MArgument&). Passes internal MImage as result to LibraryLink.
		 **/
		void passInternal(MArgument& res) noexcept override {
			MArgument_setMImage(res, internalMI);
		}

		void initDataMembers();

		/// Image data type matching template parameter T
		static const imagedata_t type;

		/// Internal container
		MImage internalMI = nullptr;

		/// Whether internal image is 2D or 3D
		bool allowSlices;
	};

	template<typename T>
	Image<T>::Image(mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ) : Image<T>(0, w, h, channels, cs, interleavingQ) {
		allowSlices = false;
	}

	template<typename T>
	Image<T>::Image(mint nFrames, mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ) {
		if (!imgFuns)
			this->initError();
		int error = (nFrames > 0) ? imgFuns->MImage_new3D(nFrames, w, h, channels, type, cs, interleavingQ, &internalMI) :
				imgFuns->MImage_new2D(w, h, channels, type, cs, interleavingQ, &internalMI);
		if (error)
			throw LibraryLinkError(LLErrorCode::ImageNewError);
		this->arrayOwnerQ = true;
		initDataMembers();
	}

	template<typename T>
	Image<T>::Image(const MImage mi) {
		if (!imgFuns)
			this->initError();
		if (type != imgFuns->MImage_getDataType(mi))
			throw LibraryLinkError(LLErrorCode::ImageTypeError);
		this->arrayOwnerQ = false;
		internalMI = mi;
		initDataMembers();
	}

	template<typename T>
	Image<T>::~Image() {
		if (this->arrayOwnerQ)
			this->freeInternal();
	}

	template<typename T>
	void Image<T>::initDataMembers() {
		allowSlices = imgFuns->MImage_getRank(internalMI) == 3;
		this->depth = imgFuns->MImage_getRank(internalMI) + (channels() == 1? 0 : 1);
		this->flattenedLength = imgFuns->MImage_getFlattenedLength(internalMI);
		if (allowSlices)
			this->dims = { slices(), rows(), columns() };
		else
			this->dims = { rows(), columns() };
		if (channels() > 1) {
			if (interleavedQ())
				this->dims.push_back(channels());
			else
				this->dims.insert(this->dims.begin(), channels());
		}
		if (this->dims.size() != static_cast<std::make_unsigned<mint>::type>(this->depth))
			this->sizeError();
		this->fillOffsets();
	}
} /* namespace LibraryLinkUtils */


#endif /* LLUTILS_IMAGE_H_ */
