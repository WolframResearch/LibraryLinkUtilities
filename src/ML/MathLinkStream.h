/** 
 * @file	MathLinkStream.h
 * @date	Nov 23, 2017
 * @author	rafalc
 * @brief	<brief description>
 */
#ifndef LLUTILS_MATHLINKSTREAM_H_
#define LLUTILS_MATHLINKSTREAM_H_

#include <map>
#include <type_traits>
#include <vector>

#include "mathlink.h"

#include "Utilities.h"
#include "MLGet.h"
#include "MLPut.h"

namespace LibraryLinkUtils {


	/** 
	 * TODO <comment>
	 */
	class MathLinkStream {
	public:
		MathLinkStream(MLINK mlp);

		MathLinkStream(MLINK mlp, int argc);

		MathLinkStream(MLINK mlp, const std::string& head, int argc);

		~MathLinkStream() = default;

		MLINK& get() noexcept {
			// The invariant is that "links" is never empty, so no need to check
			return m;
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
		MathLinkStream& operator<<(const ML::ArrayData<T>& a);

		template<typename T>
		MathLinkStream& operator<<(const ML::ListData<T>& l);
	
		template<typename T>
		MathLinkStream& operator<<(const std::vector<T>& l);

		template<typename T>
		MathLinkStream& operator<<(const ML::StringData<T>& s);

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

		MathLinkStream& operator>>(ML::Symbol& s);

		MathLinkStream& operator>>(const ML::Function& f);

		MathLinkStream& operator>>(ML::Function& f);

		MathLinkStream& operator>>(bool& b);

		template<typename T>
		MathLinkStream& operator>>(ML::ArrayData<T>& s);

		template<typename T>
		MathLinkStream& operator>>(ML::ListData<T>& s);

		template<typename T>
		MathLinkStream& operator>>(std::vector<T>& l);

		template<typename T>
		MathLinkStream& operator>>(ML::StringData<T>& s);

		template<typename T>
		MathLinkStream& operator>>(std::basic_string<T>& s);

		template<typename K, typename V>
		MathLinkStream& operator>>(std::map<K, V>& s);

		template<typename T, typename = typename std::enable_if_t<std::is_arithmetic<std::remove_reference_t<T>>::value>>
		MathLinkStream& operator>>(T& value);

	private:
		void check(int statusOk, int errorCode, const std::string& debugInfo = "");
		void check(int statusOk, const std::string& errorName, const std::string& debugInfo = "");

		int testHead(const std::string& head);
		void testHead(const std::string& head, int argc);

	private:
		MLINK m;
	};


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

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(const std::vector<T>& l) {
		ML::PutList<T>::put(m, l.data(), l.size());
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(const ML::StringData<T>& s) {
		ML::PutString<T>::put(m, s.get(), s.get_deleter().getLength());
		return *this;
	}

	template<typename T>
	MathLinkStream& MathLinkStream::operator<<(const std::basic_string<T>& s) {
		ML::PutString<T>::put(m, s.c_str(), s.size());
		return *this;
	}

	template<typename T, std::size_t N>
	MathLinkStream& MathLinkStream::operator<<(const T (&s)[N]) {
		return *this << std::basic_string<T>(s);
	}

	template<typename T, typename>
	MathLinkStream& MathLinkStream::operator<<(T value) {
		ML::PutScalar<T>::put(m, value);
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
	MathLinkStream& MathLinkStream::operator>>(std::basic_string<T>& s) {
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

	template<typename T, typename>
	MathLinkStream& MathLinkStream::operator>>(T& value) {
		value = ML::GetScalar<T>::get(m);
		return *this;
	}


} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_MATHLINKSTREAM_H_ */
