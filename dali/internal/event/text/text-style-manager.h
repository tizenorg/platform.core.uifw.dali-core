#ifndef __DALI_TEXT_STYLE_MANAGER_H__
#define __DALI_TEXT_STYLE_MANAGER_H__

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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include<dali/public-api/common/dali-vector.h>
#include<dali/public-api/text/text-style.h>

namespace Dali
{

namespace Internal
{

// Forward declarations.
struct TextStyleItem;

/**
 * @brief
 */
class TextStyleManager
{
public:

  /**
   *
   */
  static TextStyleManager& Get();

  /**
   *
   */
  ~TextStyleManager();

  /**
   * Registers a new text style object.
   *
   * @param[in] style
   * @param[in] mask
   */
  TextStyleItem* RegisterTextStyle( const Dali::TextStyle& style, TextStyle::Mask mask );

  /**
   * Increases the reference counter of a previously registered text style object.
   *
   * @param[in] styleItem
   */
  void RegisterTextStyle( TextStyleItem* styleItem );

  /**
   * Unregister a previously registered text style object.
   *
   * @param[in] styleItem
   */
  void UnRegisterTextStyle( TextStyleItem* styleItem );

  /**
   * @return Retrieves the default style.
   */
  const TextStyle& GetDefaultStyle() const;

private:

  /**
   * @brief Default constructor.
   *
   * Initializes class members to default.
   */
  TextStyleManager();

  // private and left unimplemented.
  TextStyleManager( const TextStyleManager& );
  TextStyleManager& operator=( const TextStyleManager& );

private:
  Vector<TextStyleItem*> mTextStyles; ///< Stores TextStyle objects with their number of references.
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_TEXT_STYLE_MANAGER_H__
