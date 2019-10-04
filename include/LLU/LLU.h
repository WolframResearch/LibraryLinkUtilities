/**
 * @file	LLU.h
 * @brief	Header file which includes all major parts of LLU.
 *
 * It's often convenient to use a single include file but bear in mind that if you only need a small subset of LLU in your project
 * you may get shorter compilation times by including only what you actually use.
 */

#ifndef LLUTILS_LLU_H_
#define LLUTILS_LLU_H_

/* Containers */
#include "LLU/Containers/DataList.h"
#include "LLU/Containers/Image.h"
#include "LLU/Containers/NumericArray.h"
#include "LLU/Containers/Passing/Automatic.hpp"
#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/Containers/Passing/Shared.hpp"
#include "LLU/Containers/Tensor.h"

/* Error reporting */
#include "LLU/ErrorLog/ErrorManager.h"
#include "LLU/ErrorLog/Errors.h"

/* Library function arguments */
#include "LLU/MArgumentManager.h"

/* MathLink */
#include "LLU/ML/MLStream.hpp"

#endif /* LLUTILS_LLU_H_ */
