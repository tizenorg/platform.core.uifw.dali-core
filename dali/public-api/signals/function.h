#ifndef __DALI_FUNCTION_H__
#define __DALI_FUNCTION_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// INTERNAL INCLUDES


/* #if defined(EMSCRIPTEN) */

/* // Emscripten is c++11 build and doesnt grok boost so using std */

/* #include <functional> */
/* namespace Dali DALI_IMPORT_API */
/* { */
/* typedef std::function function; */
/* } */

/* #else */

// Otherwise were using boost, but being deprecated

#include <boost/bind.hpp>
#include <boost/function.hpp>

/* #endif */

#endif // __DALI_FUNCTION_H__
