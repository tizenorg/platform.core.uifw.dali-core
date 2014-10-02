#ifndef DALI_FUNCTIONAL_H
#define DALI_FUNCTIONAL_H

#include "dali-common.h"

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
  //using std::tr1::shared_ptr;
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
