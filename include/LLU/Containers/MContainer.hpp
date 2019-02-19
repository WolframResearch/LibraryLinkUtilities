/**
 * @file	MContainer.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	February 08, 2019
 * @brief	
 */
#ifndef LLU_MCONTAINER_HPP
#define LLU_MCONTAINER_HPP

#include <iostream>
#include <type_traits>
#include <LLU/Containers/Passing/Shared.hpp>

#include "LLU/Containers/LibDataHolder.h"
#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/LibraryLinkError.h"
#include "LLU/MArgument.h"
#include "LLU/Utilities.hpp"

namespace LibraryLinkUtils {

	template<MArgumentType Type, class PassingMode>
	class MContainerBase : public PassingMode {
	public:
		using Container = MType_t<Type>;

	public:
		MContainerBase() {
			static_assert(std::is_same<PassingMode, Passing::Manual>::value, "New MContainer can only be created with passing mode Manual.");
		}
		explicit MContainerBase(Container c) : container(c) {}

		template<class P>
		MContainerBase(const MContainerBase<Type, P>& mc) : PassingMode(mc), container(mc.clone()) {}

        template<class P>
		MContainerBase(MContainerBase<Type, P>&& mc) noexcept : PassingMode(std::move(mc)), container(mc.container) {
			mc.container = nullptr;
		}

        template<class P>
		MContainerBase& operator=(const MContainerBase<Type, P>& mc) {
			PassingMode::oparator=(mc);
			set(mc.clone());
			return *this;
		}

        template<class P>
		MContainerBase& operator=(MContainerBase<Type, P>&& mc) noexcept {
			PassingMode::oparator=(std::move(mc));
			set(mc.container);
			mc.container = nullptr;
			return *this;
		}

		virtual ~MContainerBase() = default;

		Container clone() const {
			if (container == nullptr) {
			// TODO
			}
			return cloneImpl();
		}

		/**
		 *   @brief 	Return share count of internal MTensor.
		 *   Use this to manually manage shared MTensors.
		 *
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_shareCount.html>
		 *   @note		LibraryLink does not provide any way to check whether MTensor was passed or will be returned as "Shared".
		 *   Therefore passing or returning MTensors as "Shared" is discouraged and if you do that you are responsible for managing MTensor memory.
		 **/
		mint shareCount() const noexcept {
			if (container) {
				return shareCountImpl();
			}
			return 0;
		}

		/**
		 *   @brief 	Disown internal MTensor that is shared with Mathematica.
		 *   Use this to manually manage shared MTensors.
		 *
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_disown.html>
		 *   @note		LibraryLink does not provide any way to check whether MTensor was passed or will be returned as "Shared".
		 *   Therefore passing or returning MTensors as "Shared" is discouraged and if you do that you are responsible for managing MTensor memory.
		 **/
		void disown() const noexcept override {
            std::cout << "Disowning a tensor" << std::endl;
			if (container) {
				disownImpl();
			}
		}

		void free() const noexcept override {
			if (container) {
				freeImpl();
			}
		}

		void pass(MArgument& res) const override {
			if (container) {
				passImpl(res);
			}
		}

		Container getContainer() const noexcept {
			return container;
		}

	protected:
		void setContainer(Container newCont) noexcept {
			container = newCont;
		}

	private:
		virtual Container cloneImpl() const = 0;

		virtual void freeImpl() const = 0;

		virtual void disownImpl() const = 0;

		virtual mint shareCountImpl() const = 0;

		virtual void passImpl(MArgument& res) const = 0;

		Container container {};
	};

	template<MArgumentType Type, class PassingMode = Passing::Manual>
	class MContainer {
		static_assert(dependent_false_v<Type>, "Trying to instantiate unspecialized MContainer template.");
	};

	template<class PassingMode>
	class MContainer<MArgumentType::Tensor, PassingMode> : public MContainerBase<MArgumentType::Tensor, PassingMode> {
	public:
		using Base = MContainerBase<MArgumentType::Tensor, PassingMode>;
		using RawContainer = typename Base::Container;

		MContainer(mint type, mint rank, const mint* dims) {
			RawContainer tmp;
			if (LibDataHolder::getLibraryData()->MTensor_new(type, rank, dims, &tmp)) {
				ErrorManager::throwException(LLErrorName::TensorNewError);
			}
			this->setContainer(tmp);
		}

		MContainer(RawContainer t) : Base(t) {}

        template<class P>
        MContainer(const MContainer<MArgumentType::Tensor, P>& mc) : Base(mc) {}

        template<class P>
        MContainer(MContainer<MArgumentType::Tensor, P>&& mc) noexcept : Base(std::move(mc)) {
        }

        template<class P>
        MContainer& operator=(const MContainer<MArgumentType::Tensor, P>& mc) {
            Base::operator=(mc);
            return *this;
        }

        template<class P>
        MContainer& operator=(MContainer<MArgumentType::Tensor, P>&& mc) noexcept {
            Base::operator=(mc);
            return *this;
        }

        ~MContainer() {
            this->cleanup();
        };

        mint rawRank() const noexcept {
            return LibDataHolder::getLibraryData()->MTensor_getRank(this->getContainer());
		}

        mint const* rawDimensions() const {
            return LibDataHolder::getLibraryData()->MTensor_getDimensions(this->getContainer());
        }

        mint rawFlattenedLength( ) const {
            return LibDataHolder::getLibraryData()->MTensor_getFlattenedLength(this->getContainer());
        }

        mint type() const {
            return LibDataHolder::getLibraryData()->MTensor_getType(this->getContainer());
        }

	private:

		mint shareCountImpl() const noexcept override {
			return LibDataHolder::getLibraryData()->MTensor_shareCount(this->getContainer());
		}

		void disownImpl() const noexcept override {
            std::cout << "Really disowning the tensor" << std::endl;
			LibDataHolder::getLibraryData()->MTensor_disown(this->getContainer());
		}

		void freeImpl() const noexcept override {
			LibDataHolder::getLibraryData()->MTensor_free(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMTensor(res, this->getContainer());
		}

		RawContainer cloneImpl() const override {
			RawContainer tmp;
			if(LibDataHolder::getLibraryData()->MTensor_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(LLErrorName::TensorCloneError);
			}
			return tmp;
		}
	};

	template<class PassingMode>
	class MContainer<MArgumentType::Image, PassingMode> : public MContainerBase<MArgumentType::Image, PassingMode> {
	public:
		using Base = MContainerBase<MArgumentType::Image, PassingMode>;
		using RawContainer = typename Base::Container;

		MContainer(mint width, mint height, mint channels, imagedata_t type, colorspace_t colorSpace, mbool interleaving) :
			MContainer(0, width, height, channels, type, colorSpace, interleaving) {}

		MContainer(mint slices, mint width, mint height, mint channels, imagedata_t type, colorspace_t colorSpace, mbool interleaving) : imgFuns(LibDataHolder::getImageFunctions()) {
			RawContainer tmp;
			if (slices ? imgFuns->MImage_new3D(slices, width, height, channels, type, colorSpace, interleaving, &tmp) :
					imgFuns->MImage_new2D(width, height, channels, type, colorSpace, interleaving, &tmp)) {
				ErrorManager::throwException(LLErrorName::ImageNewError);
			}
			this->setContainer(tmp);
		}

		MContainer(RawContainer i) : Base(i), imgFuns(LibDataHolder::getImageFunctions()) {}

        ~MContainer() {
            this->cleanup();
        };

		template<class P>
		MContainer<MArgumentType::Image, P> convert(imagedata_t t, mbool interleavingQ) const {
			auto newImage = imgFuns->MImage_convertType(this->getContainer(), t, interleavingQ);
			if (!newImage) {
				ErrorManager::throwException(LLErrorName::ImageNewError, "Conversion to type " + std::to_string(static_cast<int>(t)) + " failed.");
			}
			return newImage;
		}

		template<class P>
		MContainer<MArgumentType::Image, P> convert(imagedata_t t) const {
			return convert(t, interleavedQ());
		}

		/**
		 *   @brief Get colorspace of internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColorSpace.html>
		 **/
		colorspace_t colorspace() const noexcept {
			return imgFuns->MImage_getColorSpace(this->getContainer());
		}

		/**
		 *   @brief Get number of rows in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRowCount.html>
		 **/
		mint rows() const noexcept {
			return imgFuns->MImage_getRowCount(this->getContainer());
		}

		/**
		 *   @brief Get number of columns in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColumnCount.html>
		 **/
		mint columns() const noexcept {
			return imgFuns->MImage_getColumnCount(this->getContainer());
		}

		/**
		 *   @brief Get number of slices in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getSliceCount.html>
		 **/
		mint slices() const noexcept {
			return imgFuns->MImage_getSliceCount(this->getContainer());
		}

		/**
		 *   @brief Get number of channels in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getChannels.html>
		 **/
		mint channels() const noexcept {
			return imgFuns->MImage_getChannels(this->getContainer());
		}

		/**
		 *   @brief Check if there is an alpha channel in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_alphaChannelQ.html>
		 **/
		bool alphaChannelQ() const noexcept {
			return imgFuns->MImage_alphaChannelQ(this->getContainer());
		}

		/**
		 *   @brief Check if internal MImage is interleaved
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_interleavedQ.html>
		 **/
		bool interleavedQ() const noexcept {
			return imgFuns->MImage_interleavedQ(this->getContainer());
		}

		/**
		 *   @brief Check if internal MImage is 3D
		 **/
		bool is3D() const noexcept {
			return imgFuns->MImage_getRank(this->getContainer()) == 3;
		}

        imagedata_t type() const noexcept {
            return imgFuns->MImage_getDataType(this->getContainer());
        }

        void* rawData() const noexcept {
            return imgFuns->MImage_getRawData(this->getContainer());
        }
	private:

		RawContainer cloneImpl() const override {
			RawContainer tmp;
			if(LibDataHolder::getLibraryData()->imageLibraryFunctions->MImage_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(LLErrorName::ImageCloneError);
			}
			return tmp;
		}

		mint shareCountImpl() const noexcept override {
			return LibDataHolder::getLibraryData()->imageLibraryFunctions->MImage_shareCount(this->getContainer());
		}

		void disownImpl() const noexcept override {
			LibDataHolder::getLibraryData()->imageLibraryFunctions->MImage_disown(this->getContainer());
		}

		void freeImpl() const noexcept override {
			LibDataHolder::getLibraryData()->imageLibraryFunctions->MImage_free(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMImage(res, this->getContainer());
		}

	private:
		WolframImageLibrary_Functions imgFuns;
	};

	template<class PassingMode>
	class MContainer<MArgumentType::NumericArray, PassingMode> : public MContainerBase<MArgumentType::NumericArray, PassingMode> {
	public:
		using Base = MContainerBase<MArgumentType::NumericArray, PassingMode>;
		using RawContainer = typename Base::Container;

		MContainer(numericarray_data_t type, mint rank, const mint* dims) {
			RawContainer tmp;
			if (LibDataHolder::getLibraryData()->numericarrayLibraryFunctions->MNumericArray_new(type, rank, dims, &tmp)) {
				ErrorManager::throwException(LLErrorName::NumericArrayNewError);
			}
			this->setContainer(tmp);
		}

		MContainer(RawContainer t) : Base(t) {};

        template<class P>
        MContainer(const MContainer<MArgumentType::NumericArray, P>& mc) : Base(mc) {}

        template<class P>
        MContainer(MContainer<MArgumentType::NumericArray, P>&& mc) noexcept : Base(std::move(mc)) {
        }

        template<class P>
        MContainer& operator=(const MContainer<MArgumentType::NumericArray, P>& mc) {
            Base::operator=(mc);
            return *this;
        }

        template<class P>
        MContainer& operator=(MContainer<MArgumentType::NumericArray, P>&& mc) noexcept {
            Base::operator=(mc);
            return *this;
        }

        ~MContainer() {
            this->cleanup();
        };

		template<class P>
		MContainer<MArgumentType::NumericArray, P> convert(numericarray_data_t t, NA::ConversionMethod method) const {
			auto newNA = LibDataHolder::getLibraryData()->numericarrayLibraryFunctions->MNumericArray_convertType(this->getContainer(), t, static_cast<numericarray_convert_method_t>(method));
			if (!newNA) {
				ErrorManager::throwException(LLErrorName::NumericArrayConversionError, "Conversion to type " + std::to_string(static_cast<int>(t)) + " failed.");
			}
			return newNA;
		}
	private:

		RawContainer cloneImpl() const override {
			RawContainer tmp;
			if(LibDataHolder::getLibraryData()->numericarrayLibraryFunctions->MNumericArray_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(LLErrorName::NumericArrayCloneError);
			}
			return tmp;
		}

		mint shareCountImpl() const noexcept override {
			return LibDataHolder::getLibraryData()->numericarrayLibraryFunctions->MNumericArray_shareCount(this->getContainer());
		}

		void disownImpl() const noexcept override {
			LibDataHolder::getLibraryData()->numericarrayLibraryFunctions->MNumericArray_disown(this->getContainer());
		}

		void freeImpl() const noexcept override {
			LibDataHolder::getLibraryData()->numericarrayLibraryFunctions->MNumericArray_free(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMNumericArray(res, this->getContainer());
		}
	};

	template<class PassingMode>
	class MContainer<MArgumentType::DataStore, PassingMode> : public MContainerBase<MArgumentType::DataStore, PassingMode> {
		static_assert(!std::is_same<PassingMode, Passing::Shared>::value, "DataStore cannot be Shared.");
	public:

		using Base = MContainerBase<MArgumentType::DataStore, PassingMode>;
		using RawContainer = typename Base::Container;

		MContainer() : MContainer(LibDataHolder::getLibraryData()->ioLibraryFunctions->createDataStore()) {
		}

		MContainer(RawContainer t) : Base(t) {};

        ~MContainer() {
            this->cleanup();
        };

	private:

		RawContainer cloneImpl() const override {
			return LibDataHolder::getLibraryData()->ioLibraryFunctions->copyDataStore(this->getContainer());
		}

		mint shareCountImpl() const noexcept override {
			return 0;
		}

		void disownImpl() const noexcept override {
		}

		void freeImpl() const noexcept override {
			LibDataHolder::getLibraryData()->ioLibraryFunctions->deleteDataStore(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setDataStore(res, this->getContainer());
		}
	};

}

#endif //LLU_MCONTAINER_HPP
