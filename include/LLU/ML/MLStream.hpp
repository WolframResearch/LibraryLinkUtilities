/** 
 * @file	MLStream.hpp
 * @date	Nov 23, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Header file for MLStream class.
 */
#ifndef LLUTILS_MLSTREAM_HPP_
#define LLUTILS_MLSTREAM_HPP_

#include <algorithm>
#include <iterator>
#include <map>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>

#include "mathlink.h"

#include "LLU/Error/Errors.h"
#include "LLU/Utilities.hpp"
#include "Utilities.h"
#include "UtilityTypeTraits.hpp"
#include "Get.h"
#include "Put.h"
#include "Strings.h"

namespace LibraryLinkUtils {


	/**
	 * @class 	MLStream
	 * @brief 	Wrapper class over MathLink with a stream-like interface.
	 *
	 * MLStream resides in LibraryLinkUtils namespace, whereas other MathLink-related classes can be found in LibraryLinkUtils::ML namespace.
	 *
	 * @tparam	EncodingIn - default encoding to use when reading strings from MathLink
	 * @tparam	EncodingOut - default encoding to use when writing strings to MathLink
	 */
	template<ML::Encoding EncodingIn, ML::Encoding EncodingOut = EncodingIn>
	class MLStream {
	public:

		/**
		 *   @brief			Constructs new MLStream
		 *   @param[in] 	mlp - low-level object of type MLINK received from LibraryLink
		 **/
		explicit MLStream(MLINK mlp);

		/**
		 *   @brief         Constructs new MLStream and checks whether there is a list of \c argc arguments on the LinkObject waiting to be read
		 *   @param[in]     mlp - low-level object of type MLINK received from LibraryLink
		 *   @param[in] 	argc - expected number of arguments
		 **/
		MLStream(MLINK mlp, int argc);

		/**
		 *   @brief         Constructs new MLStream and checks whether there is a function with head \c head and \c argc arguments on the LinkObject waiting to be read
		 *   @param[in]     mlp - low-level object of type MLINK received from LibraryLink\
		 *   @param[in]		head - expected head of expression on the Link
		 *   @param[in] 	argc - expected number of arguments
		 *   @throws 		see MLStream::testHead(const std::string&, int);
		 *
		 *   @note			arguments passed to the library function will almost always be wrapped in a List, so if not sure pass "List" as \c head
		 **/
		MLStream(MLINK mlp, const std::string& head, int argc);

		/**
		 *   @brief Default destructor
		 **/
		~MLStream() = default;

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
		using StreamToken = MLStream& (*)(MLStream&);

		/// Type of elements that can be either send or received via MathLink with no arguments, for example ML::Rule
		using BidirStreamToken = MLStream& (*)(MLStream&, ML::Direction);

		/// Type of data stored on the stack to facilitate sending expressions of a priori unknown length
		using LoopbackData = std::pair<std::string, MLINK>;

		//
		//	operator<<
		//

		/**
		 *   @brief			Sends a stream token via MathLink
		 *   @param[in] 	f - a stream token, i.e. an element that can be send via MathLink with no arguments, for example ML::Flush
		 **/
		MLStream& operator<<(StreamToken f);

		/**
		 *   @brief			Sends a bidirectional stream token via MathLink
		 *   @param[in] 	f - an element that can be either send or received via MathLink with no arguments, for example ML::Rule
		 **/
		MLStream& operator<<(BidirStreamToken f);

		/**
		 *   @brief			Sends a top-level symbol via MathLink
		 *   @param[in] 	s - a symbol
		 *   @see 			ML::Symbol
		 *   @throws 		LLErrorName::MLPutSymbolError
		 **/
		MLStream& operator<<(const ML::Symbol& s);

		/**
		 *   @brief			Sends a top-level function via MathLink, function arguments should be send immediately after
		 *   @param[in] 	f - a function
		 *   @see 			ML::Function
		 *   @throws 		LLErrorName::MLPutFunctionError
		 **/
		MLStream& operator<<(const ML::Function& f);

		/**
		 *   @brief			Sends a top-level expression of the form Missing["reason"]
		 *   @param[in] 	f - ML::Missing object with a reason
		 *   @see 			ML::Missing
		 *   @throws 		LLErrorName::MLPutFunctionError
		 **/
		MLStream& operator<<(const ML::Missing& f);

		/**
		 * @brief		Starts sending a new expression where the number of arguments is not known a priori
		 * @param[in]	expr - object of class BeginExpr that stores expression head as string
		 */
		MLStream& operator<<(const ML::BeginExpr& expr);

		/**
		 * @brief		Drops current expression that was initiated with BeginExpr
		 * @param[in]	expr - object of class DropExpr
		 **/
		MLStream& operator<<(const ML::DropExpr& expr);

		/**
		 * @brief		Ends current expression that was initiated with BeginExpr, prepends the head from BeginExpr and sends everything to the "parent" link
		 * @param[in]	expr - object of class EndExpr
		 **/
		MLStream& operator<<(const ML::EndExpr& expr);

		/**
		 *   @brief			Sends a boolean value via MathLink, it is translated to True or False in Mathematica
		 *   @param[in] 	b - a boolean value
		 *
		 *   @throws 		LLErrorName::MLPutSymbolError
		 **/
		MLStream& operator<<(bool b);

		/**
		 *   @brief			Sends a MathLink array
		 *   @tparam		T - array element type
		 *   @param[in] 	a - ArrayData to be sent
		 *   @see 			ML::ArrayData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingMultidimensionalArrays.html
		 *   @throws 		LLErrorName::MLPutArrayError
		 **/
		template<typename T>
		MLStream& operator<<(const ML::ArrayData<T>& a);

		/**
		 *   @brief			Sends a MathLink list
		 *   @tparam		T - list element type
		 *   @param[in] 	l - ListData to be sent
		 *   @see 			ML::ListData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingLists.html
		 *   @throws 		LLErrorName::MLPutListError
		 **/
		template<typename T>
		MLStream& operator<<(const ML::ListData<T>& l);
	
		/**
		 *   @brief			Sends an object owned by unique pointer
		 *   @tparam		T - list element type
		 *   @tparam		D - destructor type, not really relevant
		 *   @param[in] 	p - pointer to the object to be sent
		 **/
		template<typename T, typename D>
		MLStream& operator<<(const std::unique_ptr<T, D>& p);

		/**
		 *   @brief			Sends a std::vector via MathLink, it is interpreted as a List in Mathematica
		 *   @tparam		T - vector element type, it has to be a simple type that is supported in MLPut*List
		 *   @param[in] 	l - std::vector to be sent
		 *
		 *   @throws 		LLErrorName::MLPutListError
		 **/
		template<typename T>
		ML::ScalarSupportedTypeQ<T, MLStream&> operator<<(const std::vector<T>& l);

		/**
		 *   @brief			Sends a std::vector via MathLink, it is interpreted as a List in Mathematica
		 *   @tparam		T - vector element type, this overload will handle any type not supported in MLPut*List
		 *   @param[in] 	l - std::vector to be sent
		 *
		 *   @throws 		LLErrorName::MLPutListError
		 **/
		template<typename T>
		ML::NotScalarSupportedTypeQ<T, MLStream&> operator<<(const std::vector<T>& l);

		/**
		 *   @brief			Sends a MathLink string
		 *   @tparam		E - encoding of the string (it determines which function from MLPut*String family to use)
		 *   @param[in] 	s - ML::StringData to be sent
		 *   @see 			ML::StringData<E>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingStrings.html
		 *   @throws 		LLErrorName::MLPutStringError
		 **/
		template<ML::Encoding E>
		MLStream& operator<<(const ML::StringData<E>& s);

		/**
		 *   @brief			Sends all strings within a given object using specified character encoding.
		 *
		 *   Normally, when you send a string MLStream chooses the appropriate MathLink function based on the EncodingOut template parameter.
		 *   Sometimes you may want to locally override the output encoding and you can do this by wrapping the object with ML::PutAs<desired encoding, wrapped type>
		 *   (you can use ML::putAs function to construct ML::PutAs object without having to explicitly specify the second template parameter).
		 *
		 *   @code
		 *   	MLStream<ML::Encoding::UTF8> mls { mlink }; 		// By default use UTF8
		 *		std::vector<std::string> vecOfExpr = ....;  		// This is a vector of serialized Mathematica expressions,
		 *		ml << ML::putAs<ML::Encoding::Native>(vecOfExpr); 	// it should be sent with Native encoding
		 *   @endcode
		 *
		 *   @param[in] 	wrp - object to be sent
		 *
		 **/
		template<ML::Encoding E, typename T>
		MLStream& operator<<(const ML::PutAs<E, T>& wrp);

		/**
		 *   @brief			Sends std::basic_string
		 *   @tparam		T - string character type supported in any of MLPut*String
		 *   @param[in] 	s - std::basic_string<T> to be sent
		 *
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingStrings.html
		 *   @throws 		LLErrorName::MLPutStringError
		 *
		 **/
		template<typename T>
		ML::StringTypeQ<T, MLStream&> operator<<(const std::basic_string<T>& s);

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
		ML::StringTypeQ<T, MLStream&> operator<<(const T (&s)[N]);

		/**
		 *   @brief			Sends a C-string
		 *   @param[in] 	s - C-string to be sent
		 *
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingStrings.html
		 *   @throws 		LLErrorName::MLPutStringError
		 **/
		MLStream& operator<<(const char* s);

		/**
		 *   @brief			Sends a std::map via MathLink, it is translated to an Association in Mathematica
		 *   @tparam		K - map key type, must be supported in MLStream
		 *   @tparam		V - map value type, must be supported in MLStream
		 *   @param[in] 	m - map to be sent as Association
		 *
		 *   @throws 		LLErrorName::MLPutFunctionError plus whatever can be thrown sending keys and values
		 **/
		template<typename K, typename V>
		MLStream& operator<<(const std::map<K, V>& m);

		/**
		 *   @brief			Sends a scalar value (int, float, double, etc)
		 *   @tparam		T - scalar type
		 *   @param[in] 	value - numeric value to be sent
		 *
		 *   @throws 		LLErrorName::MLPutScalarError
		 **/
		template<typename T>
		ML::ScalarSupportedTypeQ<T, MLStream&> operator<<(T value);

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
		ML::ScalarNotSupportedTypeQ<T, MLStream&> operator<<(T value);

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
		auto operator<<(const Container& c) -> typename std::enable_if_t<sizeof(c.begin()) == sizeof(c.end()) && (sizeof(c.size()) > 0), MLStream&> {
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
		MLStream& operator>>(BidirStreamToken f);

		/**
		 *   @brief			Receives a symbol from MathLink.
		 *
		 *   Parameter \c s must have head specified and it has to match the head that was read from MathLink
		 *
		 *   @param[in] 	s - a symbol
		 *   @see 			ML::Symbol
		 *   @throws 		LLErrorName::MLGetSymbolError, LLErrorName::MLTestHeadError
		 **/
		MLStream& operator>>(const ML::Symbol& s);

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
		MLStream& operator>>(ML::Symbol& s);

		/**
		 *   @brief			Receives a function from MathLink.
		 *
		 *   Parameter \c f must have head and argument count specified and they need to match the head and argument count that was read from MathLink
		 *
		 *   @param[in] 	f - a function with head and argument count specified
		 *   @see 			ML::Function
		 *   @throws 		LLErrorName::MLGetFunctionError, LLErrorName::MLTestHeadError
		 **/
		MLStream& operator>>(const ML::Function& f);

		/**
		 *   @brief				Receives a function from MathLink.
		 *
		 *   If the parameter \c f has head or argument count set, than it has to match the head or argument count that was read from MathLink
		 *
		 *   @param[in, out] 	f - a function which may have head or argument count specified
		 *   @see 				ML::Function
		 *   @throws 			LLErrorName::MLGetFunctionError, LLErrorName::MLTestHeadError
		 **/
		MLStream& operator>>(ML::Function& f);

		/**
		 *   @brief			Receives a True or False symbol from Mathematica and converts it to bool
		 *   @param[out] 	b - argument to which the boolean received from MathLink will be assigned
		 *
		 *   @throws 		LLErrorName::MLGetSymbolError, LLErrorName::MLWrongSymbolForBool
		 **/
		MLStream& operator>>(bool& b);

		/**
		 *   @brief			Receives a MathLink array
		 *   @tparam		T - array element type
		 *   @param[out] 	a - argument to which the ML::ArrayData received from MathLink will be assigned
		 *   @see 			ML::ArrayData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingMultidimensionalArrays.html
		 *   @throws 		LLErrorName::MLGetArrayError
		 **/
		template<typename T>
		MLStream& operator>>(ML::ArrayData<T>& a);

		/**
		 *   @brief			Receives a MathLink list
		 *   @tparam		T - list element type
		 *   @param[out] 	l - argument to which the ML::ListData received from MathLink will be assigned
		 *   @see 			ML::ListData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingLists.html
		 *   @throws 		LLErrorName::MLGetListError
		 **/
		template<typename T>
		MLStream& operator>>(ML::ListData<T>& l);

		/**
		 *   @brief			Receives a List from MathLink and assigns it to std::vector
		 *   @tparam		T - vector element type, it has to be a simple type that is supported in MLGet*List
		 *   @param[out] 	l - argument to which the List received from MathLink will be assigned
		 *
		 *   @throws 		LLErrorName::MLGetListError
		 **/
		template<typename T>
		ML::ScalarSupportedTypeQ<T, MLStream&> operator>>(std::vector<T>& l);

		/**
		 *   @brief			Receives a List from MathLink and assigns it to std::vector
		 *   @tparam		T - vector element type, it can be any type supported by MathLink
		 *   @param[out] 	l - argument to which the List received from MathLink will be assigned
		 *
		 *   @throws 		LLErrorName::MLGetListError
		 **/
		template<typename T>
		ML::NotScalarSupportedTypeQ<T, MLStream&> operator>>(std::vector<T>& l);

		/**
		 *   @brief			Receives a MathLink string
		 *   @tparam		T - string character type
		 *   @param[out] 	s - argument to which the ML::StringData received from MathLink will be assigned
		 *   @see 			ML::StringData<T>
		 *   @see			http://reference.wolfram.com/language/guide/WSTPCFunctionsForExchangingStrings.html
		 *   @throws 		LLErrorName::MLGetStringError
		 **/
		template<ML::Encoding E = EncodingIn>
		MLStream& operator>>(ML::StringData<E>& s);

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
		ML::StringTypeQ<T, MLStream&> operator>>(std::basic_string<T>& s);

		/**
		 *	 @brief			Receives a value of type T
		 *	 @tparam		E - encoding to be used when reading value from MathLink
		 *	 @tparam		T - value type
		 *	 @param 		wrp - reference to object of type T wrapped in ML::GetAs structure
		 *
		 * 	 @note			There is a utility function ML::getAs for easier creation of ML::GetAs objects
		 */
		template<ML::Encoding E, typename T>
		MLStream& operator>>(ML::GetAs<E, T> wrp);

		/**
		 *   @brief			Receives a std::map via MathLink
		 *   @tparam		K - map key type, must be supported in MLStream
		 *   @tparam		V - map value type, must be supported in MLStream
		 *   @param[out] 	m - argument to which the std::map received from MathLink will be assigned
		 *
		 *   @throws 		LLErrorName::MLGetFunctionError plus whatever can be thrown receiving keys and values
		 *
		 *   @note			The top-level Association must have all values of the same type because this is how std::map works
		 **/
		template<typename K, typename V>
		MLStream& operator>>(std::map<K, V>& m);

		/**
		 *   @brief			Receives a scalar value (int, float, double, etc)
		 *   @tparam		T - scalar type
		 *   @param[out] 	value - argument to which the value received from MathLink will be assigned
		 *
		 *   @throws 		LLErrorName::MLGetScalarError
		 **/
		template<typename T>
		ML::ScalarSupportedTypeQ<T, MLStream&> operator>>(T& value);

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
		ML::ScalarNotSupportedTypeQ<T, MLStream&> operator>>(T& value);

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

		/**
		 *	@brief	Update the value of m to point to the top of loopbackStack.
		 */
		void refreshCurrentMLINK();

	private:
		/// Internal low-level handle to the currently active MathLink, it is assumed that the handle is valid.
		MLINK m;

		/// MathLink does not natively support sending expression of unknown length, so to simulate this behavior we can use a helper loopback link to store arguments
		/// until we know how many of them there are. But to be able to send nested expressions of unknown length we need more than one helper link. The data structure
		/// called stack seems to be the most reasonable choice.
		std::stack<LoopbackData> loopbackStack;

		/// Boolean flag to indicate if the current expression initiated with BeginExpr has been dropped. It is needed for EndExpr to behave correctly.
		bool currentExprDropped = false;
	};


	template<ML::Encoding EIn, ML::Encoding EOut>
	MLStream<EIn, EOut>::MLStream(MLINK mlp) : m(mlp), loopbackStack(std::deque<LoopbackData> {{"", mlp}}) {
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	MLStream<EIn, EOut>::MLStream(MLINK mlp, int argc) : MLStream(mlp, "List", argc) {
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	MLStream<EIn, EOut>::MLStream(MLINK mlp, const std::string& head, int argc) : MLStream(mlp) {
		testHead(head, argc);
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename Iterator, typename>
	void MLStream<EIn, EOut>::sendRange(Iterator begin, Iterator end) {
		sendRange(begin, end, "List");
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename Iterator, typename>
	void MLStream<EIn, EOut>::sendRange(Iterator begin, Iterator end, const std::string& head) {
		*this << ML::Function(head, static_cast<int>(std::distance(begin, end)));
		std::for_each(begin, end, [this](const auto& elem) { *this << elem; });
	}


	template<ML::Encoding EIn, ML::Encoding EOut>
	void MLStream<EIn, EOut>::check(int statusOk, const std::string& errorName, const std::string& debugInfo) {
		ML::checkError(m, statusOk, errorName, debugInfo);
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	int MLStream<EIn, EOut>::testHead(const std::string& head) {
		int argcount;
		check(
			MLTestHead(m, head.c_str(), &argcount),
			LLErrorName::MLTestHeadError,
			"Expected \"" + head + "\""
		);
		return argcount;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	void MLStream<EIn, EOut>::testHead(const std::string& head, int argc) {
		int argcount = testHead(head);
		if (argc != argcount) {
			ML::throwLLUException(LLErrorName::MLTestHeadError, "Expected " + std::to_string(argc) + " arguments but got " + std::to_string(argcount));
		}
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	void MLStream<EIn, EOut>::refreshCurrentMLINK() {
		if (loopbackStack.empty()) {
			ML::throwLLUException(LLErrorName::MLLoopbackStackSizeError, "Stack is empty in refreshCurrentMLINK()");
		}
		m = std::get<MLINK>(loopbackStack.top());
	}

	//
	//	Definitions of MLStream<EIn, EOut>::operator<<
	//

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator<<(StreamToken f) -> MLStream& {
		return f(*this);
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator<<(BidirStreamToken f) -> MLStream& {
		return f(*this, ML::Direction::Put);
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator<<(const ML::Symbol& s) -> MLStream& {
		check(
			MLPutSymbol(m, s.getHead().c_str()),
			LLErrorName::MLPutSymbolError,
			"Cannot put symbol: \"" + s.getHead() + "\""
		);
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator<<(const ML::Function& f) -> MLStream& {
		check(
			MLPutFunction(m, f.getHead().c_str(), f.getArgc()),
			LLErrorName::MLPutFunctionError,
			"Cannot put function: \"" + f.getHead() + "\" with " + std::to_string(f.getArgc()) + " arguments"
		);
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator<<(const ML::Missing& f) -> MLStream& {
		check(
			MLPutFunction(m, f.getHead().c_str(), 1), //f.getArgc() could be 0 but we still want to send f.reason, even if it's an empty string
			LLErrorName::MLPutFunctionError,
			"Cannot put function: \"" + f.getHead() + "\" with 1 argument"
		);
		*this << f.why();
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator<<(const ML::BeginExpr& expr) -> MLStream& {

		// reset dropped expression flag
		currentExprDropped = false;

		// create a new LoopbackLink for the expression
		auto loopback = ML::getNewLoopback(m);

		// store expression head together with the link on the stack
		loopbackStack.emplace(expr.getHead(), loopback);

		// active MLINK changes
		refreshCurrentMLINK();

		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator<<(const ML::DropExpr&) -> MLStream& {
		// check if the stack has reasonable size
		if (loopbackStack.size() < 2) {
			ML::throwLLUException(LLErrorName::MLLoopbackStackSizeError, "Trying to Drop expression with loopback stack size " + std::to_string(loopbackStack.size()));
		}
		// we are dropping the expression so just close the link and hope that MathLink will do the cleanup
		MLClose(std::get<MLINK>(loopbackStack.top()));
		loopbackStack.pop();
		refreshCurrentMLINK();

		// set the dropped expression flag
		currentExprDropped = true;

		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator<<(const ML::EndExpr&) -> MLStream& {

		// if the expression has been dropped at some point, then just reset the flag and do nothing as the loopback link no longer exists
		if (currentExprDropped) {
			currentExprDropped = false;
			return *this;
		}

		// check if the stack has reasonable size
		if (loopbackStack.size() < 2) {
			ML::throwLLUException(LLErrorName::MLLoopbackStackSizeError, "Trying to End expression with loopback stack size " + std::to_string(loopbackStack.size()));
		}

		// extract active loopback link and expression head
		auto currentPartialExpr = loopbackStack.top();
		loopbackStack.pop();

		// active MLINK changes
		refreshCurrentMLINK();

		// now count the expressions accumulated in the loopback link and send them to the parent link after the head
		MLINK& exprArgs = std::get<MLINK>(currentPartialExpr);
		auto argCnt = ML::countExpressionsInLoopbackLink(exprArgs);
		*this << ML::Function(std::get<std::string>(currentPartialExpr), argCnt);
		check(
			MLTransferToEndOfLoopbackLink(m, exprArgs),
			LLErrorName::MLTransferToLoopbackError,
			"Could not transfer " + std::to_string(argCnt) + " expressions from Loopback Link"
		);
		// finally, close the loopback link
		MLClose(exprArgs);

		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator<<(bool b) -> MLStream& {
		return *this << ML::Symbol(b? "True" : "False");
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator<<(const ML::ArrayData<T>& a) -> MLStream& {
		const auto& del = a.get_deleter();
		ML::PutArray<T>::put(m, a.get(), del.getDims(), del.getHeads(), del.getRank());
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator<<(const ML::ListData<T>& l) -> MLStream& {
		const auto& del = l.get_deleter();
		ML::PutList<T>::put(m, l.get(), del.getLength());
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T, typename D>
	auto MLStream<EIn, EOut>::operator<<(const std::unique_ptr<T, D>& p) -> MLStream& {
		if (p) {
			*this << *p;
		} else {
			*this << ML::Null;
		}
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator<<(const std::vector<T>& l) -> ML::ScalarSupportedTypeQ<T, MLStream&> {
		ML::PutList<T>::put(m, l.data(), static_cast<int>(l.size()));
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator<<(const std::vector<T>& l) -> ML::NotScalarSupportedTypeQ<T, MLStream&> {
		*this << ML::List(static_cast<int>(l.size()));
		for (const auto& elem : l) {
			*this << elem;
		}
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<ML::Encoding E>
	auto MLStream<EIn, EOut>::operator<<(const ML::StringData<E>& s) -> MLStream& {
		ML::String<E>::put(m, s.get(), s.get_deleter().getLength());
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator<<(const std::basic_string<T>& s) -> ML::StringTypeQ<T, MLStream&> {
		ML::String<EOut>::put(m, s.c_str(), static_cast<int>(s.size()));
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T, std::size_t N>
	auto MLStream<EIn, EOut>::operator<<(const T (&s)[N]) -> ML::StringTypeQ<T, MLStream&> {
		ML::String<EOut>::put(m, s, N);
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<ML::Encoding E, typename T>
	auto MLStream<EIn, EOut>::operator<<(const ML::PutAs<E, T>& wrp) -> MLStream& {
		MLStream<EIn, E> tmpMLS { m };
		tmpMLS << wrp.obj;
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator<<(const char* s) -> MLStream& {
		ML::String<EOut>::put(m, s, static_cast<int>(std::strlen(s)));
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator<<(T value) -> ML::ScalarSupportedTypeQ<T, MLStream&> {
		ML::PutScalar<T>::put(m, value);
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator<<(T) -> ML::ScalarNotSupportedTypeQ<T, MLStream&> {
		static_assert(dependent_false_v<T>, "Calling operator<< with unsupported type.");
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename K, typename V>
	auto MLStream<EIn, EOut>::operator<<(const std::map<K, V>& m) -> MLStream& {
		*this << ML::Association(static_cast<int>(m.size()));
		for (const auto& elem : m) {
			*this << ML::Rule << elem.first << elem.second;
		}
		return *this;
	}



	//
	//	Definitions of MLStream<EIn, EOut>::operator>>
	//

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator>>(BidirStreamToken f) -> MLStream& {
		return f(*this, ML::Direction::Get);
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator>>(const ML::Symbol& s) -> MLStream& {
		check(MLTestSymbol(m, s.getHead().c_str()), LLErrorName::MLTestSymbolError, "Cannot get symbol: \"" + s.getHead() + "\"");
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator>>(ML::Symbol& s) -> MLStream& {
		if (!s.getHead().empty()) {
			check(MLTestSymbol(m, s.getHead().c_str()), LLErrorName::MLTestSymbolError, "Cannot get symbol: \"" + s.getHead() + "\"");
		}
		else {
			const char* head;
			check(MLGetSymbol(m, &head), LLErrorName::MLGetSymbolError, "Cannot get symbol");
			s.setHead(head);
			MLReleaseSymbol(m, head);
		}
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator>>(const ML::Function& f) -> MLStream& {
		testHead(f.getHead().c_str(), f.getArgc());
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator>>(ML::Function& f) -> MLStream& {
		if (!f.getHead().empty()) {
			if (f.getArgc() < 0) {
				f.setArgc(testHead(f.getHead().c_str()));
			}
			else {
				testHead(f.getHead().c_str(), f.getArgc());
			}
		}
		else {
			const char* head;
			int argc;
			check(MLGetFunction(m, &head, &argc), LLErrorName::MLGetFunctionError, "Cannot get function");
			f.setHead(head);
			MLReleaseSymbol(m, head);
			f.setArgc(argc);
		}
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	auto MLStream<EIn, EOut>::operator>>(bool& b) -> MLStream& {
		ML::Symbol boolean;
		*this >> boolean;
		if (boolean.getHead() == "True") {
			b = true;
		}
		else if (boolean.getHead() == "False") {
			b = false;
		}
		else {
			ML::throwLLUException(LLErrorName::MLWrongSymbolForBool, R"(Expected "True" or "False", got )" + boolean.getHead());
		}
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<ML::Encoding E, typename T>
	auto MLStream<EIn, EOut>::operator>>(ML::GetAs<E, T> wrp) -> MLStream& {
		MLStream<E, EOut> tmpMLS { m };
		tmpMLS >> wrp.obj;
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator>>(ML::ArrayData<T>& a) -> MLStream& {
		a = ML::GetArray<T>::get(m);
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator>>(ML::ListData<T>& l) -> MLStream& {
		l = ML::GetList<T>::get(m);
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator>>(std::vector<T>& l) -> ML::ScalarSupportedTypeQ<T, MLStream&> {
		auto list = ML::GetList<T>::get(m);
		T* start = list.get();
		auto listLen = list.get_deleter().getLength();
		l = std::vector<T> { start, start + listLen };
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator>>(std::vector<T>& l) -> ML::NotScalarSupportedTypeQ<T, MLStream&> {
		ML::List inList;
		*this >> inList;
		std::vector<T> res(inList.getArgc());
		for(auto& elem : res) {
			*this >> elem;
		}
		l = std::move(res);
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<ML::Encoding E>
	auto MLStream<EIn, EOut>::operator>>(ML::StringData<E>& s) -> MLStream& {
		s = ML::String<E>::get(m);
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator>>(std::basic_string<T>& s) -> ML::StringTypeQ<T, MLStream&> {
		s = ML::String<EIn>::template getString<T>(m);
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename K, typename V>
	auto MLStream<EIn, EOut>::operator>>(std::map<K, V>& m) -> MLStream& {
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

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator>>(T& value) -> ML::ScalarSupportedTypeQ<T, MLStream&> {
		value = ML::GetScalar<T>::get(m);
		return *this;
	}

	template<ML::Encoding EIn, ML::Encoding EOut>
	template<typename T>
	auto MLStream<EIn, EOut>::operator>>(T&) -> ML::ScalarNotSupportedTypeQ<T, MLStream&> {
		static_assert(dependent_false_v<T>, "Calling operator>> with unsupported type.");
		return *this;
	}

} /* namespace LibraryLinkUtils */


#endif /* LLUTILS_MLSTREAM_HPP_ */
