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

#include <array>

#include "LLU/Containers/MArray.hpp"
#include "LLU/Containers/Generic/Image.hpp"
#include "LLU/Containers/Passing/PassingPolicy.hpp"
#include "LLU/Containers/Passing/Automatic.hpp"
#include "LLU/Containers/Passing/Manual.hpp"

namespace LLU {


	template<typename T>
	class 	TypedImage : public MArray<T> {
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

	private:

		T* getData() const noexcept override {
			return static_cast<T*>(LibraryData::ImageAPI()->MImage_getRawData(this->getInternal()));
		}

		virtual MImage getInternal() const = 0;

		/**
		 *
		 */
		void indexError() const {
			ErrorManager::throwException(ErrorName::ImageIndexError);
		}

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
	class Image: public TypedImage<T>, public GenericImage<PassingMode> {
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
		 *   @throws		LLErrorName::ImageNewError - if internal MImage creation failed
		 **/
		Image(mint nFrames, mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ);


		/**
		 *   @brief
		 *   @param[in]
		 *   @throws		LLErrorName::ImageTypeError - if template parameter \b T does not match MImage data type
		 **/
		explicit Image(GenericImage<PassingMode> im);

		/**
		 *   @brief         Constructs Image based on MImage
		 *   @param[in]     mi - LibraryLink structure to be wrapped
		 *   @throws		LLErrorName::ImageTypeError - if template parameter \b T does not match MImage data type
		 *   @throws		LLErrorName::ImageSizeError - if constructor failed to calculate image dimensions properly
		 **/
		explicit Image(MImage mi);

		Image() = default;

		/**
		 *   @brief         Copy constructor
		 *   @param[in]     other - const reference to an Image
		 **/
		Image(const Image& other) = default;

		/**
		 *   @brief         Move constructor
		 *   @param[in]     other - rvalue reference to an Image
		 **/
		Image(Image&& other) noexcept = default;

		Image& operator=(const Image&) = default;

		Image& operator=(Image&&) noexcept = default;

		~Image() override = default;

		/**
		 *   @brief         Copy an Image with different passing policy
		 *   @param[in]     other - const reference to an Image
		 **/
		template<class P>
		Image(const Image<T, P> &other) : TypedImage<T>(other), GenericImage<PassingMode>(other) {}

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

	private:
		using GenericBase = MContainer<MArgumentType::Image, PassingMode>;

		MImage getInternal() const noexcept override {
			return this->getContainer();
		}

		/**
		 *
		 */
		[[noreturn]] static void sizeError() {
			ErrorManager::throwException(ErrorName::ImageSizeError);
		}

		static MArrayDimensions dimensionsFromGenericImage(const GenericBase& im);
	};

	template<typename T, class PassingMode>
	Image<T, PassingMode>::Image(mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ) : Image(0, w, h, channels, cs, interleavingQ) {
	}

	template<typename T, class PassingMode>
	Image<T, PassingMode>::Image(GenericBase im) : TypedImage<T>(dimensionsFromGenericImage(im)), GenericBase(std::move(im)) {
		if (TypedImage<T>::getType() != GenericBase::type()) {
			ErrorManager::throwException(ErrorName::ImageTypeError);
		}
	}

	template<typename T, class PassingMode>
	Image<T, PassingMode>::Image(mint nFrames, mint w, mint h, mint channels, colorspace_t cs, bool interleavingQ) :
			Image(GenericBase {nFrames, w, h, channels, this->getType(), cs, interleavingQ})  {
	}

	template<typename T, class PassingMode>
	Image<T, PassingMode>::Image(MImage mi) : Image(GenericBase {mi}) {
	}

    template<typename T, class PassingMode>
    template<typename U, class P>
    Image<T, PassingMode>::Image(const Image<U, P> &i2) : Image(i2, i2.interleavedQ()) {
    }

    template<typename T, class PassingMode>
    template<typename U, class P>
    Image<T, PassingMode>::Image(const Image<U, P> &i2, bool interleavedQ) : TypedImage<T>(i2), GenericBase(i2.convert(this->getType(),
    		interleavedQ)) {
    }

	template<typename T, class PassingMode>
	MArrayDimensions Image<T, PassingMode>::dimensionsFromGenericImage(const GenericBase &im) {
		std::vector<mint> dims;
		if (!im.getContainer()) {
			return dims;
		}
		mint depth = im.getRank() + (im.channels() == 1 ? 0 : 1);
		if (im.is3D()) {
			dims = {im.slices(), im.rows(), im.columns()};
		} else {
			dims = {im.rows(), im.columns()};
		}
        if (im.channels() > 1) {
            if (im.interleavedQ()) {
				dims.push_back(im.channels());
            } else {
				dims.insert(dims.begin(), im.channels());
            }
        }
		if (dims.size() != static_cast<std::make_unsigned_t<mint>>(depth)) {
			sizeError();
		}
		return MArrayDimensions {dims};
	}
} /* namespace LLU */


#endif /* LLUTILS_IMAGE_H_ */
