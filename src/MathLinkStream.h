/** 
 * @file	MathLinkStream.h
 * @date	Nov 23, 2017
 * @author	rafalc
 * @brief	<brief description>
 */
#ifndef LLUTILS_MATHLINKSTREAM_H_
#define LLUTILS_MATHLINKSTREAM_H_

#include <functional>
#include <initializer_list>
#include <stack>
#include <type_traits>

#include "mathlink.h"

namespace LibraryLinkUtils {

	namespace LLErrorCode {
		// MathLink errors: [-501 : -600]
		constexpr int MLTestHeadError = -501; 		///< Image construction failed
		constexpr int MLPutSymbolError = -502; 		///<
		constexpr int MLPutFunctionError = -503; 	///<
	}

	namespace ML {
		struct Symbol {
			Symbol(std::string h) : head(std::move(h)) {};

			const std::string& getHead() const;
		private:
			std::string head;
		};

		struct Function : Symbol {
			Function(const std::string& h, int argCount) : Symbol(h), argc(argCount) {}

			int getArgc() const;
		private:
			int argc;
		};

		struct Association : Function {
			Association(int argCount) : Function("Association", argCount) {}
		};

		struct List : Function {
			List(int argCount) : Function("List", argCount) {}
		};
	}


	/** 
	 * TODO <comment>
	 */
	class MathLinkStream {
	public:
		MathLinkStream(MLINK mlp);

		MathLinkStream(MLINK mlp, const std::string& head, int argc);

		~MathLinkStream() = default;

		MLINK& get() noexcept {
			// The invariant is that "links" is never empty, so no need to check
			return links.top();
		}

		int testHead(const std::string& head);

		void testHead(const std::string& head, int argc);

		MathLinkStream& operator<<(std::function<MathLinkStream& (MathLinkStream&)> f);

		MathLinkStream& operator<<(const ML::Symbol& s);

		MathLinkStream& operator<<(const ML::Function& f);

		template<typename T>
		MathLinkStream& operator<<(std::initializer_list<T> l);

		template<typename T>
		MathLinkStream& operator<<(std::basic_string<T> s);

		template<typename T>
		MathLinkStream& operator<<(const T s[]);

		template<typename T, typename = typename std::enable_if_t<std::is_arithmetic<std::remove_reference_t<T>>::value>>
		MathLinkStream& operator<<(T value);


		template<typename T>
		MathLinkStream& operator>>(T value) {
			static_assert(sizeof(T) < 0, "Unsupported type to send via MathLink");
			return *this;
		}

	private:
//		template<typename T>
//		int MLPutListError(MLINK, const T*, int) {
//			static_assert(false, "sdsdsd"); //TODO
//			return 0;
//		}

		template<typename T>
		using MLPutListFunc = std::function<int(MLINK, const T*, int)>;

//		template<typename T>
//		static MLPutListFunc<T> MLPutList;
//
//		template<typename T>
//		static MLPutListFunc<T> MLPutStr;

		template<typename T>
		using MLPutScalarFunc = std::function<int(MLINK, T)>;

//		template<typename T>
//		static MLPutScalarFunc<T> MLPutScalar;

		template<typename T>
		struct MLPut {
			static MLPutListFunc<T> List;
			static MLPutListFunc<T> String;
			static MLPutScalarFunc<T> Scalar;
		};

		void checkMLError(int statusOk, int errorCode, const std::string& debugInfo = "");
		void checkMLError(int statusOk, const std::string& errorName, const std::string& debugInfo = "");
	private:
		std::stack<MLINK> links; //TODO change back to single MLINK
	};


	namespace ML {
		MathLinkStream& NewPacket(MathLinkStream& ms);

		MathLinkStream& EndPacket(MathLinkStream& ms);

		MathLinkStream& Flush(MathLinkStream& ms);

		MathLinkStream& Rule(MathLinkStream& ms);
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(std::initializer_list<T> l) {
		MLPut<T>::List(get(), l.begin(), l.size());
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(std::basic_string<T> s) {
		MLPut<T>::String(get(), s.c_str(), s.size());
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(const T s[]) {
		return *this << std::basic_string<T>(s);
	}

	template<typename T, typename>
	MathLinkStream& MathLinkStream::operator<<(T value) {
		MLPut<T>::Scalar(get(), value);
		return *this;
	}


} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_MATHLINKSTREAM_H_ */
