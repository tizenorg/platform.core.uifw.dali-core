#ifndef __DALI_RELAYOUT_CONTAINER_H__
#define __DALI_RELAYOUT_CONTAINER_H__

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

class RelayoutContainer;
typedef RelayoutContainer* RelayoutContainerPtr;


/**
 * @brief Interface to encapsulate information required for relayout
 */
class RelayoutContainer
{
public:

  /**
   * @brief Default constructor
   *
   */
  RelayoutContainer() {}

  /**
   * @brief Virtual destructor
   */
  virtual ~RelayoutContainer() {}

  /**
   * @brief Add relayout information to the container if it does'nt already exist
   *
   * @param actor The actor to relayout
   * @param size The size to relayout
   */
  virtual void Add( const Actor& actor, const Vector2& size ) = 0;

  /**
   * @brief Remove information from the container
   */
  virtual void PopBack() = 0;

  /**
   * @brief Retrieve relayout information for the given index
   *
   * @param[in] index The index of the information to retrieve
   * @param[out] actorOut The returned actor
   * @param[out] sizeOut The returned size
   */
  virtual void Get( size_t index, Actor& actorOut, Vector2& sizeOut ) const = 0;

  /**
   * @brief The count of information in the container
   */
  virtual size_t Size() const = 0;

  /**
   * @brief Set the capacity of the container
   *
   * @param capacity The new capacity for the container
   */
  virtual void Reserve( size_t capacity ) = 0;

  /**
   * @brief Reset the container, freeing all memory
   */
  virtual void Clear() = 0;

  /**
   * @brief Returns if the container contains the actor or not
   *
   * @param actor The actor to search for
   * @return Return if the actor was found or not
   */
  virtual bool Contains( const Actor& actor ) = 0;

};

} // namespace Dali

#endif // __DALI_RELAYOUT_CONTAINER_H__
