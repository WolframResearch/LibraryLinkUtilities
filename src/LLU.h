/** 
 * @file	LLU.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	3/10/2017
 * @brief	Header file with common includes and definitions that will be used in most paclets.
 */

#ifndef LLUTILS_LLU_H_
#define LLUTILS_LLU_H_

#include "LibraryLinkError.h"
#include "MArgumentManager.h"

#include "RawArray.h"
#include "Tensor.h"
#include "Image.h"


// Import some of the LibraryLinkUtils symbols into the global namespace.
// You definitely should not include this in your .h files but it may save you some typing in .cpp files.

// MArgumentManager
using MArgumentManager = LibraryLinkUtils::MArgumentManager;

// Error-related
using LibraryLinkError = LibraryLinkUtils::LibraryLinkError;
using ErrorManager = LibraryLinkUtils::ErrorManager;
namespace LLErrorCode = LibraryLinkUtils::LLErrorCode;
namespace LLErrorName = LibraryLinkUtils::LLErrorName;

// Containers
template<typename T>
using RawArray = LibraryLinkUtils::RawArray<T>;

template<typename T>
using Tensor = LibraryLinkUtils::Tensor<T>;

template<typename T>
using Image = LibraryLinkUtils::Image<T>;


#endif /* LLUTILS_LLU_H_ */
