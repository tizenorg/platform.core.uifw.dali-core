#ifndef _DALI_INTERNAL_PLATFORM_SCOPED_POINTER_H__
#define _DALI_INTERNAL_PLATFORM_SCOPED_POINTER_H__
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
#include <dali/integration-api/debug.h>

// EXTERNAL INCLUDES

namespace Dali
{
namespace Internal
{

/**
 * @brief Deletes the object pointed-to when it goes out of scope.
 *
 * A simple template class to call delete on an owned pointer when it goes
 * out of scope, whether that be by ordinary return or stack unwind for
 * exception throw.
 */
template<typename Owned>
class ScopedPointer
{
public:

  /**
   * @brief Construct a ScopedPointer guarding a new Owned*.
   */
  ScopedPointer( Owned * const owned ) :
    mOwned( owned )
  {
    DALI_ASSERT_DEBUG( owned != 0 && "Null, you give me null???" );
  }

   /**
    * @brief Destroy the ScopedPointer and clean up its Owned*.
    */
  ~ScopedPointer()
  {
    if( mOwned != 0 )
    {
      delete mOwned;
      mOwned = 0;
    }
  }

  /**
   * @return The Owned* guarded by this object.
   */
  Owned* Get() const
  {
    return mOwned;
  }

  /**
   * Give up ownership of the object guarded by this pointer.
   * @return The Owned* previously guarded by this object.
   */
  Owned* Release()
  {
    Owned* const owned = mOwned;
    mOwned = 0;
    return owned;
  }

  /**
   * Dereference this pointer.
   */
  Owned& operator*() const
  {
    return *mOwned;
  }

  /**
   * Allow member access through arrow notation.
   */
  Owned * operator->() const
  {
    DALI_ASSERT_DEBUG( mOwned != 0 && "Don't dereference a null pointer" );
    return mOwned;
  }

private:

  // Non-copyable:
  ScopedPointer( const ScopedPointer& rhs );
  ScopedPointer& operator = ( const ScopedPointer& rhs );

  Owned* mOwned;
};

} /* namespace Internal */
} /* namespace Dali */

#endif /* _DALI_INTERNAL_PLATFORM_SCOPED_POINTER_H__ */
