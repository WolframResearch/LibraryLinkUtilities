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

	/// MContainer specialization for MImage is called GenericImage
	template<class PassingMode>
	using GenericImage = MContainer<MArgumentType::Image, PassingMode>;

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
		explicit ImageView(const GenericImage<Passing>& gIm) : m{gIm.getContainer()} {}

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

	/**
	 *  @brief  MContainer specialization for MImage
	 *  @tparam PassingMode - passing policy
	 */
	template<class PassingMode>
	class MContainer<MArgumentType::Image, PassingMode> : public ImageInterface, public MContainerBase<MArgumentType::Image, PassingMode> {
	public:
		/// Inherit constructors from MContainerBase
		using MContainerBase<MArgumentType::Image, PassingMode>::MContainerBase;

		/// Default constructor, the MContainer does not manage any instance of MImage.
		MContainer() = default;

		/**
		 * @brief Create new 2D MImage based on given parameters
		 * @param width - image width in pixels (number of columns)
		 * @param height - image height in pixels (number of rows)
		 * @param channels - number of color channels (for example 3 for RGB, 1 for greyscale)
		 * @param type - image data type (1-bit, 8-bit, Real32, etc.)
		 * @param colorSpace - image color space
		 * @param interleaving - whether the image data should be interleaved or not
		 */
		MContainer(mint width, mint height, mint channels, imagedata_t type, colorspace_t colorSpace, mbool interleaving)
			: MContainer(0, width, height, channels, type, colorSpace, interleaving) {}

		/**
		 * @brief Create new 2D or 3D MImage based on given parameters
		 * @param slices - number of slices (0 for 2D Image, any positive number for Image3D)
		 * @param width - image width in pixels (number of columns)
		 * @param height - image height in pixels (number of rows)
		 * @param channels - number of color channels (for example 3 for RGB, 1 for greyscale)
		 * @param type - image data type (1-bit, 8-bit, Real32, etc.)
		 * @param colorSpace - image color space
		 * @param interleaving - whether the image data should be interleaved or not
		 */
		MContainer(mint slices, mint width, mint height, mint channels, imagedata_t type, colorspace_t colorSpace, mbool interleaving) {
			RawContainer tmp {};
			if (slices ? LibraryData::ImageAPI()->MImage_new3D(slices, width, height, channels, type, colorSpace, interleaving, &tmp)
					   : LibraryData::ImageAPI()->MImage_new2D(width, height, channels, type, colorSpace, interleaving, &tmp)) {
				ErrorManager::throwException(ErrorName::ImageNewError);
			}
			this->setContainer(tmp);
		}

		/**
		 * @brief   Create GenericImage from another GenericImage with different passing mode.
		 * @tparam  P - some passing mode
		 * @param   mc - different GenericImage
		 */
		template<class P>
		explicit MContainer(const MContainer<MArgumentType::Image, P>& mc) : Base(mc) {}

		MContainer(const MContainer& mc) = default;

		MContainer(MContainer&& mc) noexcept = default;

		MContainer& operator=(const MContainer&) = default;

		MContainer& operator=(MContainer&&) noexcept = default;

		/**
		 * @brief   Assign a GenericImage with different passing mode.
		 * @tparam  P - some passing mode
		 * @param   mc - different GenericImage
		 * @return  this
		 */
		template<class P>
		MContainer& operator=(const MContainer<MArgumentType::Image, P>& mc) {
			Base::operator=(mc);
			return *this;
		}

		/// Destructor which triggers the appropriate cleanup action which depends on the PassingMode
		~MContainer() override {
			this->cleanup();
		};

		/**
		 * @brief   Convert this object to a new GenericImage of given datatype, optionally changing interleaving
		 * @param   t - destination data type
		 * @param   interleavingQ - whether the converted GenericImage should be interleaved or not
		 * @return  converted GenericImage with Manual passing mode
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_convertType.html>
		 */
		GenericImage<Passing::Manual> convert(imagedata_t t, mbool interleavingQ) const {
			auto newImage = LibraryData::ImageAPI()->MImage_convertType(this->getContainer(), t, interleavingQ);
			if (!newImage) {
				ErrorManager::throwException(ErrorName::ImageNewError, "Conversion to type " + std::to_string(static_cast<int>(t)) + " failed.");
			}
			return newImage;
		}

		/**
		 * @brief   Convert this object to a new GenericImage of given datatype
		 * @param   t - destination data type
		 * @return  converted GenericImage with Manual passing mode
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_convertType.html>
		 */
		GenericImage<Passing::Manual> convert(imagedata_t t) const {
			return convert(t, interleavedQ());
		}

		/// @copydoc ImageInterface::colorspace()
		colorspace_t colorspace() const override {
			return LibraryData::ImageAPI()->MImage_getColorSpace(this->getContainer());
		}

		/// @copydoc ImageInterface::rows()
		mint rows() const override {
			return LibraryData::ImageAPI()->MImage_getRowCount(this->getContainer());
		}

		/// @copydoc ImageInterface::columns()
		mint columns() const override {
			return LibraryData::ImageAPI()->MImage_getColumnCount(this->getContainer());
		}

		/// @copydoc ImageInterface::slices()
		mint slices() const override {
			return LibraryData::ImageAPI()->MImage_getSliceCount(this->getContainer());
		}

		/// @copydoc ImageInterface::channels()
		mint channels() const override {
			return LibraryData::ImageAPI()->MImage_getChannels(this->getContainer());
		}

		/// @copydoc ImageInterface::alphaChannelQ()
		bool alphaChannelQ() const override {
			return LibraryData::ImageAPI()->MImage_alphaChannelQ(this->getContainer());
		}

		/// @copydoc ImageInterface::interleavedQ()
		bool interleavedQ() const override {
			return LibraryData::ImageAPI()->MImage_interleavedQ(this->getContainer());
		}

		/// @copydoc ImageInterface::is3D()
		bool is3D() const override {
			return LibraryData::ImageAPI()->MImage_getRank(this->getContainer()) == 3;
		}

		/// @copydoc ImageInterface::getRank()
		mint getRank() const override {
			return LibraryData::ImageAPI()->MImage_getRank(this->getContainer());
		}

		/// @copydoc ImageInterface::getFlattenedLength()
		mint getFlattenedLength() const override {
			return LibraryData::ImageAPI()->MImage_getFlattenedLength(this->getContainer());
		}

		/// @copydoc ImageInterface::type()
		imagedata_t type() const override {
			return LibraryData::ImageAPI()->MImage_getDataType(this->getContainer());
		}

		/// @copydoc ImageInterface::rawData()
		void* rawData() const override {
			return LibraryData::ImageAPI()->MImage_getRawData(this->getContainer());
		}
	private:
		using Base = MContainerBase<MArgumentType::Image, PassingMode>;
		using RawContainer = typename Base::Container;

		/**
		 *   @copydoc   MContainerBase::clone()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_clone.html>
		 **/
		RawContainer cloneImpl() const override {
			RawContainer tmp {};
			if (LibraryData::ImageAPI()->MImage_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(ErrorName::ImageCloneError);
			}
			return tmp;
		}

		/**
		 * @copydoc MContainerBase::shareCount()
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_shareCount.html>
		 */
		mint shareCountImpl() const noexcept override {
			return LibraryData::ImageAPI()->MImage_shareCount(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::disown()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_disown.html>
		 **/
		void disownImpl() const noexcept override {
			LibraryData::ImageAPI()->MImage_disown(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::free()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_free.html>
		 **/
		void freeImpl() const noexcept override {
			LibraryData::ImageAPI()->MImage_free(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::pass
		 **/
		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMImage(res, this->getContainer());
		}
	};

}

#endif	  // LLU_INCLUDE_LLU_CONTAINERS_GENERIC_IMAGE
