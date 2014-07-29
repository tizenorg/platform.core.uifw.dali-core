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
#include <dali/public-api/signals/callback.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief Function template prototype.
 */
template< typename _Signature >
class Function
{
};

/**
 * @brief A template for Functions with no parameters or return value.
 */
template <>
class Function< void () > : public CallbackBase
{
  /**
   * @brief Create a Function from a function object.
   *
   * @param[in] func The functor to copy.
   */
  template <class T>
  Function( const T& func )
  : CallbackBase( reinterpret_cast< void* >( new T( func ) ), // copy the object
                  NULL, // uses operator() instead of member function
                  reinterpret_cast< CallbackBase::Dispatcher >( &FunctorDispatcher0<T>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<T>::Delete ),
                  reinterpret_cast< CallbackBase::ObjectCopier >( &OwnedObjectCopier<T>::Copy ) )
  {
  }

  // TODO

};

} // namespace Dali

#endif // __DALI_FUNCTION_H__
