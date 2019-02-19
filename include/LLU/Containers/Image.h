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

#include "LLU/Containers/MArray.hpp"
#include "LLU/Containers/MContainer.hpp"
#include "LLU/Containers/Passing/PassingPolicy.hpp"
#include "LLU/Containers/Passing/Automatic.hpp"
#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/LibraryLinkError.h"

namespace LibraryLinkUtils {


	template<typename T>
	class TypedImage : public MArray<T> {
	public:
		using MArray<T>::MArray;

		/**
		 *   @brief         Get channel value at specified position in 2D image
		 *   @param[in]     row - pixel row (in Mathematica-style indexing - starting from 1)
		 *   @param[in]     col - pixel column (in Mathematica-style indexing - starting from 1)
		 *   @param[in]     channel - desired channel (in Mathematica-style indexing - starting from 1)
		 *   @throws		LLErrorName::ImageIndexError - if the specified coordinates are out-of-bound
		 **/
		T get(mint row, mint col, mint channel) const {
			std::array<mint, 2> pos { { row, col } };
			return getValueAt(pos.data(), channel);
		}

		/**
		 *   @brief         Get channel value at specified position in 3D image
		 *   @param[in]		slice - slice index (in Mathematica-style indexing - starting from 1)
		 *   @param[in]     row - pixel row (in Mathematica-style indexing - starting from 1)
		 *   @param[in]     col - pixel column (in Mathematica-style indexing - starting from 1)
		 *   @param[in]     channel - desired channel (in Mathematica-style indexing - starting from 1)
		 *   @throws		LLErrorName::ImageIndexError - if the specified coordinates are out-of-bound
		 **/
		T get(mint slice, mint row, mint col, mint channel) const {
			std::array<mint, 3> pos { { slice, row, col } };
			return getValueAt(pos.data(), channel);
		}

		/**
		 *   @brief         Set channel value at specified position in 2D image
		 *   @param[in]     row - pixel row (in Mathematica-style indexing - starting from 1)
		 *   @param[in]     col - pixel column (in Mathematica-style indexing - starting from 1)
		 *   @param[in]     channel - desired channel (in Mathematica-style indexing - starting from 1)
		 *   @param[in]		newValue - new channel value
		 *   @throws		LLErrorName::ImageIndexError - if the specified coordinates are out-of-bound
		 **/
		void set(mint row, mint col, mint channel, T newValue) {
			std::array<mint, 2> pos { { row, col } };
			setValueAt(pos.data(), channel, newValue);
		}

		/**
		 *   @brief         Set channel value at specified position in 3D image
		 *   @param[in]		slice - slice index (in Mathematica-style indexing - starting from 1)
		 *   @param[in]     row - pixel row (in Mathematica-style indexing - starting from 1)
		 *   @param[in]     col - pixel column (in Mathematica-style indexing - starting from 1)
		 *   @param[in]     channel - desired channel (in Mathematica-style indexing - starting from 1)
		 *   @param[in]		newValue - new channel value
		 *   @throws		LLErrorName::ImageIndexError - if the specified coordinates are out-of-bound
		 **/
		void set(mint slice, mint row, mint col, mint channel, T newValue) {
			std::array<mint, 3> pos { { slice, row, col } };
			setValueAt(pos.data(), channel, newValue);
		}

		static imagedata_t getType() noexcept {
			return type;
		}

	protected:
		/// Functions from WolframImageLibrary
		using MArray<T>::imgFuns;

	private:

		T* getData() const noexcept override {
			return static_cast<T*>(imgFuns->MImage_getRawData(this->getInternal()));
		}

		virtual void indexError() const = 0;

		virtual MImage getInternal() const = 0;

		T getValueAt(mint* pos, mint channel) const;

		void setValueAt(mint* pos, mint channel, T newValue);

		/// Image data type matching template parameter T
		static const imagedata_t type;
	};

	/**
	 * @class Image
	 * @brief This is a class template, where template parameter T is the type of data elements. Image is derived from MArray.
	 *
	 * Image<> classes automate creation and deletion of MImages.
	 * They are strongly typed (no void* to underlying memory) and almost all functions from \<algorithms\> can be used on Image.
	 *
	 * @tparam	T - type of underlying data
	 */
	template<typename T, class PassingMode = Passing::Manual>
	class Image: public TypedImage<T>, public MContainer<MArgumentType::Image, PassingMode> {
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
		 *   @throws        LLErrorName::ImageInitError - if structure with WolframImageLibrary functions is not initialized
		 *   @throws		LLErrorName::ImageNewError - if internal MImage creation failed
		 **/
		Image(mint nFrames, mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ);

		/**
		 *   @brief         Constructs Image based on MImage
		 *   @param[in]     mi - LibraryLink structure to be wrapped
		 *   @throws        LLErrorName::ImageInitError - if structure with WolframImageLibrary functions is not initialized
		 *   @throws		LLErrorName::ImageTypeError - if template parameter \b T does not match MImage data type
		 *   @throws		LLErrorName::ImageSizeError - if constructor failed to calculate image dimensions properly
		 **/
		Image(const MImage mi);


		/**
		 *   @brief         Copy constructor with type conversion
		 *   @tparam		U - any type that Image supports
		 *   @param[in]     t2 - const reference to an Image
		 **/
		template<typename U, class P>
		Image(const Image<U, P>& t2);

		/**
		 *   @brief         Copy constructor with type conversion and explicitly specified interleaving
		 *   @tparam		U - any type that Image supports
		 *   @param[in]     t2 - const reference to an Image
		 *   @param[in]		interleavedQ - whether the newly created Image should be interleaved
		 **/
		template<typename U, class P>
		Image(const Image<U, P>& t2, bool interleavedQ);

	protected:
		/// Functions from WolframImageLibrary
		using TypedImage<T>::imgFuns;

	private:
		using GenericImage = MContainer<MArgumentType::Image, PassingMode>;

		MImage getInternal() const noexcept override {
			return this->getContainer();
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::indexError()
		 *   @throws 	LibraryLinkError(LLErrorName::ImageIndexError)
		 **/
		void indexError() const override {
			ErrorManager::throwException(LLErrorName::ImageIndexError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::initError()
		 *   @throws 	LibraryLinkError(LLErrorName::ImageInitError)
		 **/
		void initError() const override {
			ErrorManager::throwException(LLErrorName::ImageInitError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::sizeError()
		 *   @throws 	LibraryLinkError(LLErrorName::ImageSizeError)
		 **/
		void sizeError() const override {
			ErrorManager::throwException(LLErrorName::ImageSizeError);
		}

		void initDataMembers();
	};

	template<typename T, class PassingMode>
	Image<T, PassingMode>::Image(mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ) : Image<T>(0, w, h, channels, cs, interleavingQ) {
	}

	template<typename T, class PassingMode>
	Image<T, PassingMode>::Image(mint nFrames, mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ) :
			GenericImage(nFrames, w, h, channels, this->getType(), cs, interleavingQ)  {
		if (!imgFuns)
			this->initError();
		initDataMembers();
	}

	template<typename T, class PassingMode>
	Image<T, PassingMode>::Image(const MImage mi) : GenericImage(mi) {
		if (!imgFuns)
			this->initError();
		if (TypedImage<T>::getType() != GenericImage::type())
			ErrorManager::throwException(LLErrorName::ImageTypeError);
		initDataMembers();
	}

    template<typename T, class PassingMode>
    template<typename U, class P>
    Image<T, PassingMode>::Image(const Image<U, P> &i2) : Image<T>(i2, i2.interleavedQ()) {
    }

    template<typename T, class PassingMode>
    template<typename U, class P>
    Image<T, PassingMode>::Image(const Image<U, P> &i2, bool interleavedQ) : TypedImage<T>(i2), GenericImage(i2.template convert<PassingMode>(this->getType(), interleavedQ)) {
    }


	template<typename T, class PassingMode>
	void Image<T, PassingMode>::initDataMembers() {
        this->depth = imgFuns->MImage_getRank(this->getInternal()) + (this->channels() == 1 ? 0 : 1);
        this->flattenedLength = imgFuns->MImage_getFlattenedLength(this->getInternal());
        if (this->is3D())
            this->dims = {this->slices(), this->rows(), this->columns()};
        else
            this->dims = {this->rows(), this->columns()};
        if (this->channels() > 1) {
            if (this->interleavedQ())
                this->dims.push_back(this->channels());
            else
                this->dims.insert(this->dims.begin(), this->channels());
        }
        if (this->dims.size() != static_cast<std::make_unsigned_t<mint>>(this->depth))
            this->sizeError();
        this->fillOffsets();
	}
} /* namespace LibraryLinkUtils */


#endif /* LLUTILS_IMAGE_H_ */
