#ifndef __DALI_FUNCTIONAL_H__
#define __DALI_FUNCTIONAL_H__

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

#include <dali/public-api/common/dali-common.h>

#if defined(_CPP11)

#include <functional>
#include <memory>

// emscripten build is cpp11
namespace Dali
{
  using std::bind;
  using std::cref;
  using std::function;
  using std::ref;
}

namespace {
  using namespace std::placeholders;
}

#else // !_CPP11

#if defined(HAVE_TR1_FUNCTIONAL)

#include <tr1/functional>
#include <tr1/memory>

namespace Dali
{
  using std::tr1::bind;
  using std::tr1::cref;
  using std::tr1::function;
  using std::tr1::ref;
}

namespace {
  using namespace std::tr1::placeholders;
}

#elif defined(HAVE_BOOST_FUNCTION_HPP)

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace Dali
{
  using boost::bind;
  using boost::cref;
  using boost::function;
  using boost::ref;
}

#else
#error "One of TR1 or Boost is required"
#endif // HAVE_TR1_FUNCTIONAL || HAVE_BOOST_FUNCTION_HPP

#endif // _CPP11

#endif // header
