/**
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief
 */
#ifndef LLU_CONTAINERS_VIEWS_IMAGE_HPP
#define LLU_CONTAINERS_VIEWS_IMAGE_HPP

#include "LLU/Containers/Generic/Image.hpp"
#include "LLU/Containers/Interfaces.h"
#include "LLU/Containers/Iterators/IterableContainer.hpp"
#include "LLU/ErrorLog/ErrorManager.h"

namespace LLU {

	/**
	 * @brief   Simple, light-weight, non-owning wrappper over MImage.
	 *
	 * Intended for use in functions that only need to access MImage metadata, where it can alleviate the need for introducing template parameters
	 * for MImage passing mode (like in GenericImage) or data type (like in Image class).
	 */
	class ImageView : public ImageInterface {
	public:
		ImageView() = default;

		template<class Passing>
		/* implicit */ ImageView(const GenericImage<Passing>& gIm) : m {gIm.getContainer()} {}

		/* implicit */ ImageView(MImage mi) : m {mi} {}

		/// @copydoc ImageInterface::colorspace()
		colorspace_t colorspace() const override {
			return LibraryData::ImageAPI()->MImage_getColorSpace(m);
		}

		/// @copydoc ImageInterface::rows()
		mint rows() const override {
			return LibraryData::ImageAPI()->MImage_getRowCount(m);
		}

		/// @copydoc ImageInterface::columns()
		mint columns() const override {
			return LibraryData::ImageAPI()->MImage_getColumnCount(m);
		}

		/// @copydoc ImageInterface::slices()
		mint slices() const override {
			return LibraryData::ImageAPI()->MImage_getSliceCount(m);
		}

		/// @copydoc ImageInterface::channels()
		mint channels() const override {
			return LibraryData::ImageAPI()->MImage_getChannels(m);
		}

		/// @copydoc ImageInterface::alphaChannelQ()
		bool alphaChannelQ() const override {
			return LibraryData::ImageAPI()->MImage_alphaChannelQ(m);
		}

		/// @copydoc ImageInterface::interleavedQ()
		bool interleavedQ() const override {
			return LibraryData::ImageAPI()->MImage_interleavedQ(m);
		}

		/// @copydoc ImageInterface::is3D()
		bool is3D() const override {
			return LibraryData::ImageAPI()->MImage_getRank(m) == 3;
		}

		/// @copydoc ImageInterface::getRank()
		mint getRank() const override {
			return LibraryData::ImageAPI()->MImage_getRank(m);
		}

		/// @copydoc ImageInterface::getFlattenedLength()
		mint getFlattenedLength() const override {
			return LibraryData::ImageAPI()->MImage_getFlattenedLength(m);
		}

		/// @copydoc ImageInterface::type()
		imagedata_t type() const override {
			return LibraryData::ImageAPI()->MImage_getDataType(m);
		}

		/// @copydoc ImageInterface::rawData()
		void* rawData() const override {
			return LibraryData::ImageAPI()->MImage_getRawData(m);
		}

	private:
		MImage m = nullptr;
	};

	template<typename T>
	class ImageTypedView : public ImageView, public IterableContainer<T> {
	public:
		ImageTypedView() = default;

		template<class Passing>
		/* implicit */ ImageTypedView(const GenericImage<Passing>& gIm) : ImageView(gIm) {
			if (ImageType<T> != type()) {
				ErrorManager::throwException(ErrorName::ImageTypeError);
			}
		}

		/* implicit */ ImageTypedView(ImageView iv) : ImageView(std::move(iv)) {
			if (ImageType<T> != type()) {
				ErrorManager::throwException(ErrorName::ImageTypeError);
			}
		}

		/* implicit */ ImageTypedView(MImage mi) : ImageView(mi) {
			if (ImageType<T> != type()) {
				ErrorManager::throwException(ErrorName::ImageTypeError);
			}
		}

	private:
		T* getData() const noexcept override {
			return static_cast<T*>(rawData());
		}

		mint getSize() const noexcept override {
			return getFlattenedLength();
		}
	};

	template<typename ImageT, typename F>
	auto asTypedImage(ImageT&& img, F&& callable) {
		switch (img.type()) {
			case MImage_Type_Undef: ErrorManager::throwException(ErrorName::ImageTypeError);
			case MImage_Type_Bit: return std::forward<F>(callable)(ImageTypedView<std::int8_t>(std::forward<ImageT>(img)));
			case MImage_Type_Bit8: return std::forward<F>(callable)(ImageTypedView<std::uint8_t>(std::forward<ImageT>(img)));
			case MImage_Type_Bit16: return std::forward<F>(callable)(ImageTypedView<std::uint16_t>(std::forward<ImageT>(img)));
			case MImage_Type_Real32: return std::forward<F>(callable)(ImageTypedView<float>(std::forward<ImageT>(img)));
			case MImage_Type_Real: return std::forward<F>(callable)(ImageTypedView<double>(std::forward<ImageT>(img)));
		}
	}

	template<typename F>
	auto asTypedImage(MImage img, F&& callable) {
		return asTypedImage(ImageView {img}, std::forward<F>(callable));
	}
}

#endif	  // LLU_CONTAINERS_VIEWS_IMAGE_HPP
