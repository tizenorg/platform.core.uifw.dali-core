#ifndef __DALI_DEMANGLER_H__
#define __DALI_DEMANGLER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace Internal
{

/**
 * Demangle a nested typeid name into its component parts.
 * A nested type name is one containing namespaces and class names only.
 *   eg DemangleNestedNames(typeid(Dali::Actor).name());
 * @param[in] typeIdName The type id name string to demangle.
 * @returns the demangled list of names ie ["Dali","Actor"] or an empty list
 */
std::vector<std::string> DemangleNestedNames(const char *typeIdName);

/**
 * Demangle a nested typeid name to its class name.
 * @param[in] typeIdName The type id name string to demangle.
 * @returns the class name ie "Actor" or an empty string
 */
const std::string DemangleClassName(const char *typeIdName);

}

}


#endif // header
