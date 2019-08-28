/**
 * @file
 * @brief
 */
#ifndef LLU_INCLUDE_LLU_CONTAINERS_GENERIC_IMAGE
#define LLU_INCLUDE_LLU_CONTAINERS_GENERIC_IMAGE

#include "LLU/Containers/Generic/Base.hpp"

namespace LLU {

	template<class PassingMode>
	class MContainer<MArgumentType::Image, PassingMode>;

	template<class PassingMode>
	using GenericImage = MContainer<MArgumentType::Image, PassingMode>;


	template<class PassingMode>
	class MContainer<MArgumentType::Image, PassingMode> : public MContainerBase<MArgumentType::Image, PassingMode> {
	public:
		using Base = MContainerBase<MArgumentType::Image, PassingMode>;
		using RawContainer = typename Base::Container;

		using Base::Base;

		/**
		 * @brief   Default constructor, the MContainer does not manage any instance of MImage.
		 */
		MContainer() = default;

		MContainer(mint width, mint height, mint channels, imagedata_t type, colorspace_t colorSpace, mbool interleaving) :
				MContainer(0, width, height, channels, type, colorSpace, interleaving) {
		}

		MContainer(mint slices, mint width, mint height, mint channels, imagedata_t type, colorspace_t colorSpace, mbool interleaving) {
			RawContainer tmp {};
			if (slices ? LibraryData::ImageAPI()->MImage_new3D(slices, width, height, channels, type, colorSpace, interleaving, &tmp) :
			    LibraryData::ImageAPI()->MImage_new2D(width, height, channels, type, colorSpace, interleaving, &tmp)) {
				ErrorManager::throwException(ErrorName::ImageNewError);
			}
			this->setContainer(tmp);
		}

		template<class P>
		MContainer(const MContainer<MArgumentType::Image, P>& mc) : Base(mc) {
		}

		MContainer(const MContainer& mc) = default;

		MContainer(MContainer&& mc) noexcept = default;

		MContainer &operator=(const MContainer &) = default;

		MContainer &operator=(MContainer &&) noexcept = default;

		template<class P>
		MContainer& operator=(const MContainer<MArgumentType::Image, P>& mc) {
			Base::operator=(mc);
			return *this;
		}

		~MContainer() override {
			this->cleanup();
		};

		GenericImage<Passing::Manual> convert(imagedata_t t, mbool interleavingQ) const {
			auto newImage = LibraryData::ImageAPI()->MImage_convertType(this->getContainer(), t, interleavingQ);
			if (!newImage) {
				ErrorManager::throwException(ErrorName::ImageNewError, "Conversion to type " + std::to_string(static_cast<int>(t)) + " failed.");
			}
			return newImage;
		}

		GenericImage<Passing::Manual> convert(imagedata_t t) const {
			return convert(t, interleavedQ());
		}

		/**
		 *   @brief Get colorspace of internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColorSpace.html>
		 **/
		colorspace_t colorspace() const noexcept {
			return LibraryData::ImageAPI()->MImage_getColorSpace(this->getContainer());
		}

		/**
		 *   @brief Get number of rows in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRowCount.html>
		 **/
		mint rows() const noexcept {
			return LibraryData::ImageAPI()->MImage_getRowCount(this->getContainer());
		}

		/**
		 *   @brief Get number of columns in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColumnCount.html>
		 **/
		mint columns() const noexcept {
			return LibraryData::ImageAPI()->MImage_getColumnCount(this->getContainer());
		}

		/**
		 *   @brief Get number of slices in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getSliceCount.html>
		 **/
		mint slices() const noexcept {
			return LibraryData::ImageAPI()->MImage_getSliceCount(this->getContainer());
		}

		/**
		 *   @brief Get number of channels in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getChannels.html>
		 **/
		mint channels() const noexcept {
			return LibraryData::ImageAPI()->MImage_getChannels(this->getContainer());
		}

		/**
		 *   @brief Check if there is an alpha channel in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_alphaChannelQ.html>
		 **/
		bool alphaChannelQ() const noexcept {
			return LibraryData::ImageAPI()->MImage_alphaChannelQ(this->getContainer());
		}

		/**
		 *   @brief Check if internal MImage is interleaved
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_interleavedQ.html>
		 **/
		bool interleavedQ() const noexcept {
			return LibraryData::ImageAPI()->MImage_interleavedQ(this->getContainer());
		}

		/**
		 *   @brief Check if internal MImage is 3D
		 **/
		bool is3D() const noexcept {
			return LibraryData::ImageAPI()->MImage_getRank(this->getContainer()) == 3;
		}

		mint getRank() const noexcept {
			return LibraryData::ImageAPI()->MImage_getRank(this->getContainer());
		}

		mint getFlattenedLength() const {
			return LibraryData::ImageAPI()->MImage_getFlattenedLength(this->getContainer());
		}

		imagedata_t type() const noexcept {
			return LibraryData::ImageAPI()->MImage_getDataType(this->getContainer());
		}

		void* rawData() const noexcept {
			return LibraryData::ImageAPI()->MImage_getRawData(this->getContainer());
		}

	private:

		RawContainer cloneImpl() const override {
			RawContainer tmp {};
			if (LibraryData::ImageAPI()->MImage_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(ErrorName::ImageCloneError);
			}
			return tmp;
		}

		mint shareCountImpl() const noexcept override {
			return LibraryData::ImageAPI()->MImage_shareCount(this->getContainer());
		}

		void disownImpl() const noexcept override {
			LibraryData::ImageAPI()->MImage_disown(this->getContainer());
		}

		void freeImpl() const noexcept override {
			LibraryData::ImageAPI()->MImage_free(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMImage(res, this->abandonContainer());
		}

	};

}

#endif //LLU_INCLUDE_LLU_CONTAINERS_GENERIC_IMAGE
