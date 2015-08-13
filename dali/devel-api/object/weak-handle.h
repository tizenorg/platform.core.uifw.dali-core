#ifndef __DALI_WEAK_HANDLE_H__
#define __DALI_WEAK_HANDLE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>

namespace Dali
{

/**
 * @brief WeakHandle stores a weak pointer to an internal Dali object. The handle to the object
 * can be accessed if the object exists, and such access is not reference counted. When the object
 * is deleted, the weak pointer will be set to NULL, and any further attmpt to access to a deleted
 * object will return an empty handle.
 *
 */
class DALI_IMPORT_API WeakHandle
{

public:

  /**
   * @brief Default constructor which provides an uninitialized Dali::WeakHandle.
   */
  WeakHandle();

  /**
   * @brief This constructor creates a weak handle of the Dali object.
   *
   * @param [in] handle A reference to the handle of the Dali object.
   */
  WeakHandle( Handle& handle );

  /**
   * @brief Destructor to free resources.
   */
  ~WeakHandle();

  /**
   * @brief Copy constructor.
   *
   * @param [in] handle A reference to the copied WeakHandle
   */
  WeakHandle(const WeakHandle& handle);

  /**
   * @brief Assignment operator.
   *
   * It makes this WeakHandle point to the same internal Dali object as the copied WeakHandle
   * @param [in] rhs  A reference to the copied WeakHandle
   * @return A reference to this WeakHandle
   */
  WeakHandle& operator=( const WeakHandle& rhs );

  /**
   * @brief Equality operator overload.
   *
   * @param [in] rhs A reference to the compared WeakHandle.
   * @return true if the handle points to the same Dali resource, or if both are uninitialized.
   */
  bool operator==(const WeakHandle& rhs) const;

  /**
   * @brief Inequality operator overload.
   *
   * @param [in] rhs A reference to the compared WeakHandle.
   * @return true if the handle points to the different Dali resources.
   */
  bool operator!=(const WeakHandle& rhs) const;

  /**
   * @brief Gets the handle to the Dali object.
   *
   * @return The handle of the Dali object pointed by this WeakHandle or an ampty handle if the Dali object doesn't exist.
   */
  Handle GetHandle() const;

  /**
   * @brief Template to allow access to the handle to the Dali object in the given type
   * without any need to downcast it.
   *
   * @tparam   HandleType           The handle class
   * @tparam   InternalObjectType   The internal object class
   *
   * @return Handle to a class HandleType or an uninitialized handle.
   */
  template< class HandleType, class InternalObjectType >
  DALI_INTERNAL HandleType GetObject()
  {
    Handle handle( GetHandle() );
    return handle ? HandleType( reinterpret_cast< InternalObjectType* >( handle.GetObjectPtr() ) ) : HandleType();
  }

private:

  struct Impl;
  Impl* mImpl;
};

} // namespace Dali

#endif // __DALI_WEAK_HANDLE_H__
