/** 
 * @file	MathLinkStream.h
 * @date	Nov 23, 2017
 * @author	rafalc
 * @brief	<brief description>
 */
#ifndef LLUTILS_MATHLINKSTREAM_H_
#define LLUTILS_MATHLINKSTREAM_H_

#include <functional>
#include <iostream> //debug
#include <map>
#include <stack>
#include <type_traits>
#include <vector>

#include "mathlink.h"

#include "Utilities.h"

namespace LibraryLinkUtils {

	namespace LLErrorCode {
		// MathLink errors: [-501 : -600]
		constexpr int MLTestHeadError = -501; 		///< Image construction failed
		constexpr int MLPutSymbolError = -502; 		///<
		constexpr int MLPutFunctionError = -503; 	///<
		constexpr int MLTestSymbolError = -504;
		constexpr int MLWrongSymbolForBool = -505;
	}

	class MathLinkStream;

	namespace ML {
		struct Symbol {
			Symbol() = default;
			Symbol(std::string h) : head(std::move(h)) {};

			const std::string& getHead() const;
			void setHead(std::string h);
		private:
			std::string head;
		};

		struct Function : Symbol {
			Function() : Function("", -1) {};
			Function(const std::string& h) : Function(h, -1) {}
			Function(const std::string& h, int argCount) : Symbol(h), argc(argCount) {}

			int getArgc() const;
			void setArgc(int newArgc);
		private:
			int argc;
		};

		struct Association : Function {
			Association() : Function("Association") {}
			Association(int argCount) : Function("Association", argCount) {}
		};

		struct List : Function {
			List() : Function("List") {}
			List(int argCount) : Function("List", argCount) {}
		};


		enum class Direction : bool {
			Get,
			Put
		};


		MathLinkStream& NewPacket(MathLinkStream& ms);

		MathLinkStream& EndPacket(MathLinkStream& ms);

		MathLinkStream& Flush(MathLinkStream& ms);

		MathLinkStream& Rule(MathLinkStream& ms, Direction dir);
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

		using StreamToken = MathLinkStream& (*)(MathLinkStream&);
		using BidirStreamToken = MathLinkStream& (*)(MathLinkStream&, ML::Direction);

		//
		//	operator<<
		//

		MathLinkStream& operator<<(StreamToken f);

		MathLinkStream& operator<<(BidirStreamToken f);

		MathLinkStream& operator<<(const ML::Symbol& s);

		MathLinkStream& operator<<(const ML::Function& f);

		MathLinkStream& operator<<(bool b);

		template<typename T>
		MathLinkStream& operator<<(const std::vector<T>& l);

		template<typename T>
		MathLinkStream& operator<<(const std::basic_string<T>& s);

		template<typename T, std::size_t N>
		MathLinkStream& operator<<(const T (&s)[N]);

		template<typename K, typename V>
		MathLinkStream& operator<<(const std::map<K, V>& s);

		template<typename T, typename = typename std::enable_if_t<std::is_arithmetic<std::remove_reference_t<T>>::value>>
		MathLinkStream& operator<<(T value);

		//
		//	operator>>
		//

		MathLinkStream& operator>>(BidirStreamToken f);

		MathLinkStream& operator>>(const ML::Symbol& s);

		MathLinkStream& operator>>(const ML::Function& f);

		MathLinkStream& operator>>(ML::Function& f);

		MathLinkStream& operator>>(bool& b);

		template<typename T>
		MathLinkStream& operator>>(std::vector<T>& l);

		template<typename T>
		MathLinkStream& operator>>(std::basic_string<T>& s);

		template<typename K, typename V>
		MathLinkStream& operator>>(std::map<K, V>& s);

		template<typename T, typename = typename std::enable_if_t<std::is_arithmetic<std::remove_reference_t<T>>::value>>
		MathLinkStream& operator>>(T& value);

	private:
		template<typename T>
		using MLPutListFunc = std::function<int(MLINK, const T*, int)>;

		template<typename T>
		using MLPutScalarFunc = std::function<int(MLINK, T)>;

		template<typename T>
		struct MLPut {
			static MLPutListFunc<T> List;
			static MLPutListFunc<T> String;
			static MLPutScalarFunc<T> Scalar;
		};

		template<typename T>
		using MLGetListFunc = std::function<int(MLINK, T**, int*)>;

		template<typename T>
		using MLGetStringFunc = std::function<int(MLINK, const T**, int*, int*)>;

		template<typename T>
		using MLGetScalarFunc = std::function<int(MLINK, T*)>;

		template<typename T>
		struct MLGet {
			static MLGetListFunc<T> List;
			static MLGetStringFunc<T> String;
			static MLGetScalarFunc<T> Scalar;
		};

		template<typename T>
		using MLReleaseListFunc = std::function<void(MLINK, T*, int)>;

		template<typename T>
		using MLReleaseStringFunc = std::function<void(MLINK, const T*, int)>;

		template<typename T>
		struct MLRelease {
			static MLReleaseListFunc<T> List;
			static MLReleaseStringFunc<T> String;
		};

		void checkMLError(int statusOk, int errorCode, const std::string& debugInfo = "");
		void checkMLError(int statusOk, const std::string& errorName, const std::string& debugInfo = "");

		int testHead(const std::string& head);
		void testHead(const std::string& head, int argc);
	private:
		std::stack<MLINK> links; //TODO change back to single MLINK
	};


	//
	//	Definitions of MathLinkStream::operator<<
	//

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(const std::vector<T>& l) {
		MLPut<T>::List(get(), l.data(), l.size());
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(const std::basic_string<T>& s) {
		MLPut<T>::String(get(), s.c_str(), s.size());
		return *this;
	}

	template<typename T, std::size_t N>
	MathLinkStream& MathLinkStream::operator<<(const T (&s)[N]) {
		return *this << std::basic_string<T>(s);
	}

	template<typename T, typename>
	MathLinkStream& MathLinkStream::operator<<(T value) {
		MLPut<T>::Scalar(get(), value);
		return *this;
	}

	template<typename K, typename V>
	MathLinkStream& MathLinkStream::operator<<(const std::map<K, V>& s) {
		*this << ML::Association(s.size());
		for (const auto& elem : s) {
			*this << ML::Rule << elem.first << elem.second;
		}
		return *this;
	}

	//
	//	Definitions of MathLinkStream::operator>>
	//

	template<typename T>
	MathLinkStream& MathLinkStream::operator>>(std::vector<T>& l) {
		T* raw;
		int len;
		MLGet<T>::List(get(), &raw, &len);
		l = std::vector<T> { raw, len };
		MLRelease<T>::List(get(), raw, len);
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator>>(std::basic_string<T>& s) {
		const T* raw;
		int bytes, chars;
		MLGet<T>::String(get(), &raw, &bytes, &chars);
		std::cout << "Read string: " << raw << ", bytes: " << bytes << ", chars: " << chars << std::endl;
		s = (bytes < 0? std::basic_string<T> { raw } : std::basic_string<T> { raw, static_cast<typename std::basic_string<T>::size_type>(bytes) });
		std::cout << "Converted string size: " << s.size() << std::endl;
		MLRelease<T>::String(get(), raw, bytes);
		return *this;
	}

	template<typename K, typename V>
	MathLinkStream& MathLinkStream::operator>>(std::map<K, V>& m) {
		auto elemCount = testHead("Association");
		for (auto i = 0; i < elemCount; ++i) {
			*this >> ML::Rule;
			K key;
			*this >> key;
			V value;
			*this >> value;
			m.emplace(std::move(key), std::move(value));
		}
		return *this;
	}

	template<typename T, typename>
	MathLinkStream& MathLinkStream::operator>>(T& value) {
		MLGet<T>::Scalar(get(), value);
		return *this;
	}


} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_MATHLINKSTREAM_H_ */
