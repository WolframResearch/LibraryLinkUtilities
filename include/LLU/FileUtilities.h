/**
 * @file	FileUtilities.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief
 */
#ifndef LLU_FILEUTILITIES_H
#define LLU_FILEUTILITIES_H

#include <cstdio>
#include <codecvt>
#include <fstream>
#include <ios>
#include <locale>
#include <memory>
#include <string>

namespace LLU {
	/// Smart pointer type around std::FILE
	using FilePtr = std::unique_ptr<std::FILE, int (*)(std::FILE*)>;

	/**
	 * Create a unique owning pointer of a FILE.
	 * @param   f - a stream pointer obtained from fopen
	 * @return  a smart pointer owning f that will call fclose on f in destructor
	 * @warning if f is not a result of fopen, the behavior is undefined
	 */
	FilePtr claimFile(std::FILE* f);

	/**
	 * Call a function called "validatePath" from a globally accessible WolframLibraryData. This function is not documented so what it does remains a mystery.
	 * @param fileName - file whose path is to be "validated", whatever that means
	 * @param mode - file open mode
	 */
	void validatePath(const std::string& fileName, std::ios::openmode mode);

	/**
	 * Convert string from UTF8 to UTF16.
	 * @tparam	T - character type for the result
	 * @param	source - string in UTF8 encoding
	 * @retun   copy of the input string converted to UTF16
	 */
	template <typename T>
	std::basic_string<T> fromUTF8toUTF16(const std::string& source) {
		std::wstring_convert<std::codecvt_utf8_utf16<T>, T> convertor;
		return convertor.from_bytes(source);
	}

	/**
	 * @struct
	 * @brief   Base class for shared access policies on Windows.
	 * @details Library users are encouraged to provide their own derived classes if needed.
	 * Implemented also on Mac and Linux to have uniform interface but does not make much sense on those platforms.
	 */
	struct SharePolicy {
		virtual ~SharePolicy() = default;

		/**
		 * Default share policy - shared read access when file opened readonly, exclusive access otherwise.
		 * @return _SH_SECURED on Windows and 0 on other platforms where it is not used anyway
		 */
		virtual int flag(std::ios::openmode) const;
	};

	/**
	 * Open given file with specified mode (read, write, append, etc.).
	 * Checks with WolframLibraryData if the path is "valid" (we don't know what that really means).
	 * Converts file name to UTF-16 wide string on Windows. Uses open modes from std::ios.
	 * @param   fileName - path to the input file
	 * @param   mode - file open mode
	 * @param   shp - shared access policy, only used on Windows
	 * @returns Unique pointer to opened file
	 */
	FilePtr openFile(const std::string& fileName, std::ios::openmode mode, const SharePolicy& shp = {});

	/**
	 * Open a file stream with specified mode (read, write, append, etc.).
	 * Checks with WolframLibraryData if the path is "valid" (we don't know what that really means).
	 * Converts file name to UTF-16 wide string on Windows.
	 * @param   fileName - path to the input file
	 * @param   mode - file open mode
	 * @return  Valid file stream
	 */
	std::fstream openFileStream(const std::string& fileName, std::ios::openmode mode);
}

#endif	  // LLU_FILEUTILITIES_H
