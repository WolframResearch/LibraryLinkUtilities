/** 
 * @file	LLU.h
 * @brief	Header file which includes all major parts of LLU.
 *
 * It's often convenient to use a single include file but bare in mind that if you only need a small subset of LLU i your project
 * you may get faster compilation times by including only what you actually use.
 */

#ifndef LLUTILS_LLU_H_
#define LLUTILS_LLU_H_

#include "LLU/Containers/DataList.h"
#include "LLU/Containers/Image.h"
#include "LLU/Containers/MContainer.hpp"
#include "LLU/Containers/NumericArray.h"
#include "LLU/Containers/Tensor.h"

#include "LLU/Containers/Passing/Automatic.hpp"
#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/Containers/Passing/Shared.hpp"

#include "LLU/ErrorLog/Errors.h"
#include "LLU/ErrorLog/ErrorManager.h"

#include "LLU/ML/MLStream.hpp"

#include "MArgumentManager.h"

#endif /* LLUTILS_LLU_H_ */
