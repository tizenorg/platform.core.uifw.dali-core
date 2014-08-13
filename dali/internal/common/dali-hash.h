#ifndef __DALI_HASH__
#define __DALI_HASH__

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

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"

#include <set>    // needed by boost hash.hpp
#include <map>    // needed by boost hash.hpp
#include <list>   // needed by boost hash.hpp
#include <vector> // needed by boost hash.hpp
#include <boost/functional/hash.hpp>

#pragma clang diagnostic pop
#else

#include <boost/functional/hash.hpp>

#endif // __clang__

typedef boost::hash<const std::string> StringHash;

#endif // header
