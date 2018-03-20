/** 
 * @file	MathLinkStream.h
 * @date	Nov 23, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Header file for MathLinkStream class.
 */
#ifndef LLUTILS_MATHLINKSTREAM_H_
#define LLUTILS_MATHLINKSTREAM_H_

#include <iterator>
#include <map>
#include <type_traits>
#include <vector>

#include "mathlink.h"

#include "../Utilities.hpp"
#include "Utilities.h"
#include "UtilityTypeTraits.hpp"
#include "MLGet.h"
#include "MLPut.h"

namespace LibraryLinkUtils {


	/**
	 * @class MathLinkStream
	 * @brief Wrapper class over MathLink with a stream-like interface. So far covers only the most important types.
	 *
	 * MathLinkStream resides in LibraryLinkUtils namespace, whereas other MathLink-related classes can be found in LibraryLinkUtils::ML namespace.
	 */
	class MathLinkStream {
	public:

		/**
		 *   @brief			Constructs new MathLinkStream
		 *   @param[in] 	mlp - low-level object of type MLINK received from LibraryLink
		 **/
		MathLinkStream(MLINK mlp);

		/**
		 *   @brief         Constructs new MathLinkStream and checks whether there is a list of \c argc arguments on the LinkObject waiting to be read
		 *   @param[in]     mlp - low-level object of type MLINK received from LibraryLink
		 *   @param[in] 	argc - expected number of arguments
		 **/
		MathLinkStream(MLINK mlp, int argc);

		/**
		 *   @brief         Constructs new MathLinkStream and checks whether there is a function with head \c head and \c argc arguments on the LinkObject waiting to be read
		 *   @param[in]     mlp - low-level object of type MLINK received from LibraryLink\
		 *   @param[in]		head - expected head of expression on the Link
		 *   @param[in] 	argc - expected number of arguments
		 *   @throws 		see MathLinkStream::testHead(const std::string&, int);
		 *
		 *   @note			arguments passed to the library function will almost always be wrapped in a List, so if not sure pass "List" as \c head
		 **/
		MathLinkStream(MLINK mlp, const std::string& head, int argc);

		/**
		 *   @brief Default destructor
		 **/
		~MathLinkStream() = default;

		/**
		 *   @brief Returns a reference to underlying low-level MathLink handle
		 **/
		MLINK& get() noexcept {
			return m;
		}

		/**
		 *   @brief			Sends any range as List
		 *   @tparam		InputIterator - type that is an iterator
		 *   @param[in] 	begin - iterator to the first element of the range
		 *	 @param[in] 	end - iterator past the last element of the range
		 *
		 **/
		template<typename Iterator, typename = enable_if_input_iterator<Iterator>>
		void sendRange(Iterator begin, Iterator end);

		/**
		 *   @brief			Sends a range of elements as top-level expression with arbitrary head
		 *   @tparam		InputIterator - type that is an iterator
		 *   @param[in] 	begin - iterator to the first element of the range
		 *	 @param[in] 	end - iterator past the last element of the range
		 *	 @param[in]		head - head of the top-level expression
		 *
		 **/
		template<typename Iterator, typename = enable_if_input_iterator<Iterator>>
		void sendRange(Iterator begin, Iterator end, const std::string& head);

	public:
		/// Type of elements that can be send via MathLink with no arguments, for example ML::Flush
		using StreamToken = MathLinkStream& (*)(MathLinkStream&);

		/// Type of elements that can be either send or received via MathLink with no arguments, for example ML::Rule
		using BidirStreamToken = MathLinkStream& (*)(MathLinkStream&, ML::Direction);


		//
		//	operator<<
		//

		/**
		 *   @brief			Sends a stream token via MathLink
		 *   @param[in] 	f - a stream token, i.e. an element that can be send via MathLink with no arguments, for example ML::Flush
		 **/
		MathLinkStream& operator<<(StreamToken f);

		/**
		 *   @brief			Sends a bidirectional stream token via MathLink
		 *   @param[in] 	f - an element that can be either send or received via MathLink with no arguments, for example ML::Rule
		 **/
		MathLinkStream& operator<<(BidirStreamToken f);

		/**
		 *   @brief			Sends a top-level symbol via MathLink
		 *   @param[in] 	s - a symbol
		 *   @see 			ML::Symbol
		 *   @throws 		LLErrorName::MLPutSymbolError
		 **/
		MathLinkStream& operator<<(const ML::Symbol& s);

		/**
		 *   @brief			Sends a top-level function via MathLink, function arguments should be send immediately after
		 *   @param[in] 	f - a function
		 *   @see 			ML::Function
		 *   @throws 		LLErrorName::MLPutFunctionError
		 **/
		MathLinkStream& operator<<(const ML::Function& f);

		/**
		 *   @brief			Sends a top-level expression of the form Missing["reason"]
		 *   @param[in] 	m - ML::Missing object with a reason
		 *   @see 			ML::Missing
		 *   @throws 		LLErrorName::MLPutFunctionError
		 **/
		MathLinkStream& operator<<(const ML::Missing& m);

		/**
		 *   @brief			Sends a boolean value via MathLink, it is translated to True or False in Mathematica
		 *   @param[in] 	b - a boolean value
		 *
		 *   @throws 		LLErrorName::MLPutSymbolError
		 **/
		MathLinkStream& operator<<(bool b);

		/**
		 *   @brief			Sends a MathLink array
		 *   @tparam		T - array element type
		 *   @param[in] 	a - ArrayData to be sent
		 *   @see 			ML::ArrayData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingMultidimensionalArrays.html
		 *   @throws 		LLErrorName::MLPutArrayError
		 **/
		template<typename T>
		MathLinkStream& operator<<(const ML::ArrayData<T>& a);

		/**
		 *   @brief			Sends a MathLink list
		 *   @tparam		T - list element type
		 *   @param[in] 	l - ListData to be sent
		 *   @see 			ML::ListData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingLists.html
		 *   @throws 		LLErrorName::MLPutListError
		 **/
		template<typename T>
		MathLinkStream& operator<<(const ML::ListData<T>& l);
	
		/**
		 *   @brief			Sends an object owned by unique pointer
		 *   @tparam		T - list element type
		 *   @tparam		D - destructor type, not really relevant
		 *   @param[in] 	p - pointer to the object to be sent
		 **/
		template<typename T, typename D>
		MathLinkStream& operator<<(const std::unique_ptr<T, D>& p);

		/**
		 *   @brief			Sends a std::vector via MathLink, it is interpreted as a List in Mathematica
		 *   @tparam		T - vector element type, it has to be a simple type that is supported in MLPut*List
		 *   @param[in] 	l - std::vector to be sent
		 *
		 *   @throws 		LLErrorName::MLPutListError
		 **/
		template<typename T>
		ML::ScalarSupportedTypeQ<T> operator<<(const std::vector<T>& l);

		/**
		 *   @brief			Sends a std::vector via MathLink, it is interpreted as a List in Mathematica
		 *   @tparam		T - vector element type, this overload will handle any type not supported in MLPut*List
		 *   @param[in] 	l - std::vector to be sent
		 *
		 *   @throws 		LLErrorName::MLPutListError
		 **/
		template<typename T>
		ML::NotScalarSupportedTypeQ<T> operator<<(const std::vector<T>& l);

		/**
		 *   @brief			Sends a MathLink string
		 *   @tparam		T - string character type
		 *   @param[in] 	s - ML::StringData to be sent
		 *   @see 			ML::StringData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingStrings.html
		 *   @throws 		LLErrorName::MLPutStringError
		 **/
		template<typename T>
		MathLinkStream& operator<<(const ML::StringData<T>& s);

		/**
		 *   @brief			Sends a string (with character type _const char_ or _const unsigned char_) using `MLPutUTF8String`.
		 *
		 *   Normally, when you send a string MathLinkStream chooses the appropriate MathLink function for the string you passed, depending on the type
		 *   of characters. Here you explicitly state that the string must be sent with `MLPutUTF8String`. It is your responsibility to make sure that
		 *   the string you want to send is encoded in UTF8.
		 *
		 *   @param[in] 	s - string you want send with `MLPutUTF8String` wrapped in a helper structure `ML::PutAsUTF8`
		 *
		 *   @throws 		LLErrorName::MLPutStringError
		 **/
		MathLinkStream& operator<<(const ML::PutAsUTF8& s);

		/**
		 *   @brief			Sends std::basic_string
		 *   @tparam		T - string character type supported in any of MLPut*String
		 *   @param[in] 	s - std::basic_string<T> to be sent
		 *
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingStrings.html
		 *   @throws 		LLErrorName::MLPutStringError
		 *
		 *   @note			std::string is just std::basic_string<char>
		 **/
		template<typename T>
		ML::StringTypeQ<T> operator<<(const std::basic_string<T>& s);

		/**
		 *   @brief			Sends a character array (or a string literal)
		 *   @tparam		T - character type supported in any of MLPut*String
		 *   @tparam		N - length of character array
		 *   @param[in] 	s - character array to be sent as String
		 *
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingStrings.html
		 *   @throws 		LLErrorName::MLPutStringError
		 **/
		template<typename T, std::size_t N>
		ML::StringTypeQ<T> operator<<(const T (&s)[N]);

		/**
		 *   @brief			Sends a C-string
		 *   @param[in] 	s - C-string to be sent
		 *
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingStrings.html
		 *   @throws 		LLErrorName::MLPutStringError
		 **/
		MathLinkStream& operator<<(const char* s);

		/**
		 *   @brief			Sends a std::map via MathLink, it is translated to an Association in Mathematica
		 *   @tparam		K - map key type, must be supported in MathLinkStream
		 *   @tparam		V - map value type, must be supported in MathLinkStream
		 *   @param[in] 	m - map to be sent as Association
		 *
		 *   @throws 		LLErrorName::MLPutFunctionError plus whatever can be thrown sending keys and values
		 **/
		template<typename K, typename V>
		MathLinkStream& operator<<(const std::map<K, V>& m);

		/**
		 *   @brief			Sends a scalar value (int, float, double, etc)
		 *   @tparam		T - scalar type
		 *   @param[in] 	value - numeric value to be sent
		 *
		 *   @throws 		LLErrorName::MLPutScalarError
		 **/
		template<typename T>
		ML::ScalarSupportedTypeQ<T> operator<<(T value);

		/**
		 *   @brief			Overload for scalar values that cannot be sent via MathLink without conversion
		 *   @tparam		T - scalar type
		 *   @param[in] 	value - numeric value
		 *
		 *   @note			Trying to use this overload will always trigger compilation error.
		 *   If you need to send value of type not supported by MathLink (like unsigned int) you must either explicitly cast
		 *   or provide your own overload.
		 *
		 **/
		template<typename T>
		ML::ScalarNotSupportedTypeQ<T> operator<<(T value);

		/**
		 *   @brief			Sends any container (a class with begin(), end() and size()) as List
		 *   @tparam		Container - type that is a collection of some elements
		 *   @param[in] 	c - container to be sent
		 *
		 *   @throws 		LLErrorName::MLPutContainerError
		 *
		 *   @note			Size() is not technically necessary, but needed for performance reason. Most STL containers have size() anyway.
		 **/
		template<typename Container>
		auto operator<<(const Container& c) -> typename std::enable_if_t<sizeof(c.begin()) == sizeof(c.end()) && (sizeof(c.size()) > 0), MathLinkStream&> {
			this->sendRange(c.begin(), c.end());
			return *this;
		}

		//
		//	operator>>
		//

		/**
		 *   @brief			Receives a bidirectional stream token via MathLink
		 *   @param[in] 	f - an element that can be either send or received via MathLink with no arguments, for example ML::Rule
		 **/
		MathLinkStream& operator>>(BidirStreamToken f);

		/**
		 *   @brief			Receives a symbol from MathLink.
		 *
		 *   Parameter \c s must have head specified and it has to match the head that was read from MathLink
		 *
		 *   @param[in] 	s - a symbol
		 *   @see 			ML::Symbol
		 *   @throws 		LLErrorName::MLGetSymbolError, LLErrorName::MLTestHeadError
		 **/
		MathLinkStream& operator>>(const ML::Symbol& s);

		/**
		 *   @brief				Receives a symbol from MathLink.
		 *
		 *   If the parameter \c s has head specified, than it has to match the head that was read from MathLink, otherwise the head read from MathLink
		 *   will be assigned to s
		 *
		 *   @param[in, out] 	s - a symbol
		 *   @see 				ML::Symbol
		 *   @throws 			LLErrorName::MLGetSymbolError, LLErrorName::MLTestHeadError
		 **/
		MathLinkStream& operator>>(ML::Symbol& s);

		/**
		 *   @brief			Receives a function from MathLink.
		 *
		 *   Parameter \c f must have head and argument count specified and they need to match the head and argument count that was read from MathLink
		 *
		 *   @param[in] 	f - a function with head and argument count specified
		 *   @see 			ML::Function
		 *   @throws 		LLErrorName::MLGetFunctionError, LLErrorName::MLTestHeadError
		 **/
		MathLinkStream& operator>>(const ML::Function& f);

		/**
		 *   @brief				Receives a function from MathLink.
		 *
		 *   If the parameter \c f has head or argument count set, than it has to match the head or argument count that was read from MathLink
		 *
		 *   @param[in, out] 	f - a function which may have head or argument count specified
		 *   @see 				ML::Function
		 *   @throws 			LLErrorName::MLGetFunctionError, LLErrorName::MLTestHeadError
		 **/
		MathLinkStream& operator>>(ML::Function& f);

		/**
		 *   @brief			Receives a True or False symbol from Mathematica and converts it to bool
		 *   @param[out] 	b - argument to which the boolean received from MathLink will be assigned
		 *
		 *   @throws 		LLErrorName::MLGetSymbolError, LLErrorName::MLWrongSymbolForBool
		 **/
		MathLinkStream& operator>>(bool& b);

		/**
		 *   @brief			Receives a MathLink array
		 *   @tparam		T - array element type
		 *   @param[out] 	a - argument to which the ML::ArrayData received from MathLink will be assigned
		 *   @see 			ML::ArrayData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingMultidimensionalArrays.html
		 *   @throws 		LLErrorName::MLGetArrayError
		 **/
		template<typename T>
		MathLinkStream& operator>>(ML::ArrayData<T>& a);

		/**
		 *   @brief			Receives a MathLink list
		 *   @tparam		T - list element type
		 *   @param[out] 	l - argument to which the ML::ListData received from MathLink will be assigned
		 *   @see 			ML::ListData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingLists.html
		 *   @throws 		LLErrorName::MLGetListError
		 **/
		template<typename T>
		MathLinkStream& operator>>(ML::ListData<T>& l);

		/**
		 *   @brief			Receives a List from MathLink and assigns it to std::vector
		 *   @tparam		T - vector element type
		 *   @param[out] 	l - argument to which the List received from MathLink will be assigned
		 *
		 *   @throws 		LLErrorName::MLGetListError
		 **/
		template<typename T>
		MathLinkStream& operator>>(std::vector<T>& l);

		/**
		 *   @brief			Receives a MathLink string
		 *   @tparam		T - string character type
		 *   @param[out] 	s - argument to which the ML::StringData received from MathLink will be assigned
		 *   @see 			ML::StringData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingStrings.html
		 *   @throws 		LLErrorName::MLGetStringError
		 **/
		template<typename T>
		MathLinkStream& operator>>(ML::StringData<T>& s);

		/**
		 *   @brief			Receives std::basic_string
		 *   @tparam		T - string character type supported in any of MLGet*String
		 *   @param[out] 	s - argument to which the std::basic_string<T> received from MathLink will be assigned
		 *
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingStrings.html
		 *   @throws 		LLErrorName::MLGetStringError
		 *
		 *   @note			std::string is just std::basic_string<char>
		 **/
		template<typename T>
		ML::StringTypeQ<T> operator>>(std::basic_string<T>& s);

		/**
		 *	 @brief			Receives a UTF8 encoded string via MathLink
		 *	 @param 		s - std::string reference wrapped in ML::GetAsUTF8 structure
		 *
		 * 	 @see			ML::GetAsUTF8
		 * 	 @throws 		LLErrorName::MLGetStringError
		 */
		MathLinkStream& operator>>(ML::GetAsUTF8 s);

		/**
		 *   @brief			Receives a std::map via MathLink
		 *   @tparam		K - map key type, must be supported in MathLinkStream
		 *   @tparam		V - map value type, must be supported in MathLinkStream
		 *   @param[out] 	m - argument to which the std::map received from MathLink will be assigned
		 *
		 *   @throws 		LLErrorName::MLGetFunctionError plus whatever can be thrown receiving keys and values
		 *
		 *   @note			The top-level Association must have all values of the same type because this is how std::map works
		 **/
		template<typename K, typename V>
		MathLinkStream& operator>>(std::map<K, V>& m);

		/**
		 *   @brief			Receives a scalar value (int, float, double, etc)
		 *   @tparam		T - scalar type
		 *   @param[out] 	value - argument to which the value received from MathLink will be assigned
		 *
		 *   @throws 		LLErrorName::MLGetScalarError
		 **/
		template<typename T>
		ML::ScalarSupportedTypeQ<T> operator>>(T& value);

		/**
		 *   @brief			Overload for scalar values that cannot be received via MathLink without conversion
		 *   @tparam		T - scalar type
		 *   @param[in] 	value - numeric value
		 *
		 *   @note			Trying to use this overload will always trigger compilation error.
		 *   If you need to receive value of type not supported by MathLink (like unsigned int) you must either explicitly cast
		 *   or provide your own overload.
		 *
		 **/
		template<typename T>
		ML::ScalarNotSupportedTypeQ<T> operator>>(T& value);

	private:

		/**
		 *   @brief			Check if the call to MathLink API succeeded, throw an exception otherwise
		 *   @param[in] 	statusOk - error code returned from MathLink API function, usually 0 means error
		 *   @param[in]		errorName - which exception to throw
		 *   @param[in]		debugInfo - additional information to include in the exception, should it be thrown
		 *
		 *   @throws 		errorName
		 **/
		void check(int statusOk, const std::string& errorName, const std::string& debugInfo = "");

		/**
		 * 	 @brief			Test if the next expression to be read from MathLink has given head
		 * 	 @param[in] 	head - expression head to test for
		 * 	 @return		Number of arguments for the next expression on the Link (only if head is correct)
		 *
		 * 	 @throws		LLErrorName::MLTestHeadError
		 */
		int testHead(const std::string& head);

		/**
		 * 	 @brief			Test if the next expression to be read from MathLink has given head and given number of arguments
		 * 	 @param[in] 	head - expression head to test for
		 * 	 @param[in]		argc - number of arguments to test for
		 *
		 * 	 @throws		LLErrorName::MLTestHeadError
		 */
		void testHead(const std::string& head, int argc);

	private:
		/// Internal low-level handle to MathLink, it is assumed that the handle is valid
		MLINK m;
	};


	template<typename Iterator, typename>
	void MathLinkStream::sendRange(Iterator begin, Iterator end) {
		sendRange(begin, end, "List");
	}

	template<typename Iterator, typename>
	void MathLinkStream::sendRange(Iterator begin, Iterator end, const std::string& head) {
		*this << ML::Function(head, std::distance(begin, end));
		std::for_each(begin, end, [this](const auto& elem) { *this << elem; });
	}

	//
	//	Definitions of MathLinkStream::operator<<
	//

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(const ML::ArrayData<T>& a) {
		const auto& del = a.get_deleter();
		ML::PutArray<T>::put(m, a.get(), del.getDims(), del.getHeads(), del.getRank());
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(const ML::ListData<T>& l) {
		const auto& del = l.get_deleter();
		ML::PutList<T>::put(m, l.get(), del.getLength());
		return *this;
	}

	template<typename T, typename D>
	MathLinkStream& MathLinkStream::operator<<(const std::unique_ptr<T, D>& p) {
		if (p) {
			*this << *p;
		} else {
			*this << ML::Null;
		}
		return *this;
	}

	template<typename T>
	ML::ScalarSupportedTypeQ<T> MathLinkStream::operator<<(const std::vector<T>& l) {
		ML::PutList<T>::put(m, l.data(), l.size());
		return *this;
	}

	template<typename T>
	ML::NotScalarSupportedTypeQ<T> MathLinkStream::operator<<(const std::vector<T>& l) {
		*this << ML::List(l.size());
		for (const auto& elem : l) {
			*this << elem;
		}
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(const ML::StringData<T>& s) {
		ML::PutString<T>::put(m, s.get(), s.get_deleter().getLength());
		return *this;
	}

	template<typename T>
	ML::StringTypeQ<T> MathLinkStream::operator<<(const std::basic_string<T>& s) {
		ML::PutString<T>::put(m, s.c_str(), static_cast<int>(s.size()));
		return *this;
	}

	template<typename T, std::size_t N>
	ML::StringTypeQ<T> MathLinkStream::operator<<(const T (&s)[N]) {
		ML::PutString<T>::put(m, s, N);
		return *this;
	}

	template<typename T>
	ML::ScalarSupportedTypeQ<T> MathLinkStream::operator<<(T value) {
		ML::PutScalar<T>::put(m, value);
		return *this;
	}

	template<typename T>
	ML::ScalarNotSupportedTypeQ<T> MathLinkStream::operator<<(T) {
		static_assert(sizeof(T) < 0, "Calling operator<< with unsupported type.");
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
	MathLinkStream& MathLinkStream::operator>>(ML::ArrayData<T>& a) {
		a = ML::GetArray<T>::get(m);
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator>>(ML::ListData<T>& l) {
		l = ML::GetList<T>::get(m);
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator>>(std::vector<T>& l) {
		auto list = ML::GetList<T>::get(m);
		T* start = list.get();
		auto listLen = list.get_deleter().getLength();
		l = std::vector<T> { start, start + listLen };
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator>>(ML::StringData<T>& s) {
		s = ML::GetString<T>::get(m);
		return *this;
	}

	template<typename T>
	ML::StringTypeQ<T> MathLinkStream::operator>>(std::basic_string<T>& s) {
		using StringType = std::basic_string<T>;

		auto rawString = ML::GetString<T>::get(m);
		auto bytes = rawString.get_deleter().getLength();
		s = (bytes < 0? StringType { rawString.get() } : StringType { rawString.get(), static_cast<typename StringType::size_type>(bytes) });

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

	template<typename T>
	ML::ScalarSupportedTypeQ<T> MathLinkStream::operator>>(T& value) {
		value = ML::GetScalar<T>::get(m);
		return *this;
	}

	template<typename T>
	ML::ScalarNotSupportedTypeQ<T> MathLinkStream::operator>>(T&) {
		static_assert(sizeof(T) < 0, "Calling operator>> with unsupported type.");
		return *this;
	}


} /* namespace LibraryLinkUtils */


#endif /* LLUTILS_MATHLINKSTREAM_H_ */
