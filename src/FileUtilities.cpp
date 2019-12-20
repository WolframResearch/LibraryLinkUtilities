/**
 * @file	FileUtilities.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief
 */

#include <LLU/NoMinMaxWindows.h>

#include <LLU/FileUtilities.h>

#ifdef _WIN32
#include <share.h>
#endif

#include <LLU/ErrorLog/ErrorManager.h>
#include <LLU/LibraryData.h>

namespace LLU {

	namespace {
		std::string openModeString(std::ios::openmode mode) {
			using std::ios;
			bool isBinary = mode & ios::binary;
			mode &= ~ios::binary;
			std::string result;
			if (mode == ios::in) {
				result = "r";
			} else if (mode == ios::out || mode == (ios::out|ios::trunc)) {
				result = "w";
			} else if (mode == ios::app || mode == (ios::out|ios::app)) {
				result = "a";
			} else if (mode == (ios::out|ios::in)) {
				result = "r+";
			} else if (mode == (ios::out|ios::in|ios::trunc)) {
				result = "w+";
			} else if (mode == (ios::out|ios::in|ios::app) || mode == (ios::in|ios::app)) {
				result = "a+";
			} else {
				ErrorManager::throwException(ErrorName::InvalidOpenMode);
			}
			return isBinary? result + "b" : result;
		}
	}

	FilePtr claimFile(std::FILE* f) {
		return FilePtr(f, [](std::FILE* fp) { return fp ? std::fclose(fp) : 0; });
	}

	void validatePath(const std::string& fileName, std::ios::openmode mode) {
		char pathMode = (mode & std::ios::out) || (mode & std::ios::app)? 'W' : 'R';
		if (!LibraryData::API()->validatePath(const_cast<char*>(fileName.c_str()), pathMode)) {
			ErrorManager::throwException(ErrorName::PathNotValidated, fileName);
		}
	}

	FilePtr openFile(const std::string& fileName, std::ios::openmode mode) {
		validatePath(fileName, mode);

		FILE* file = nullptr;
		std::string modeStr = openModeString(mode);
#ifdef _WIN32
		std::wstring fileNameUTF16 = fromUTF8toUTF16<wchar_t>(fileName);
		std::wstring  modeWstr = fromUTF8toUTF16<wchar_t>(modeStr);
		int shflag = shareFlag(mode);
		file = _wfsopen(fileNameUTF16.c_str(), modeStr.c_str(), shflag);
#else
		file = std::fopen(fileName.c_str(), modeStr.c_str());
#endif /* _WIN32 */
		if (!file) {
			ErrorManager::throwException(ErrorName::OpenFileFailed, fileName);
		}
		return claimFile(file);
	}
}