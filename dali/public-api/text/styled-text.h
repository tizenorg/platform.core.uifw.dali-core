#ifndef __DALI_STYLED_TEXT_H__
#define __DALI_STYLED_TEXT_H__

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

/**
 * @addtogroup CAPI_DALI_TEXT_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <iostream> // VCC remove!!!!!!!!!!!!!!!!!!!!!

// INTERNAL INCLUDES
#include <dali/public-api/text/text-style.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief
 */
class StyledText
{
public:

  /**
   * @brief
   */
  StyledText();

  /**
   * @brief
   */
  ~StyledText();

  /**
   * @brief
   */
  StyledText( const StyledText& styledText );

  /**
   * @brief
   */
  StyledText& operator=( const StyledText& styledText );

  /**
   * @brief
   */
  StyledText( const Text& text );

  /**
   * @brief
   */
  void Clear();

  /**
   * @brief
   */
  void SetText( const Text& text );

  /**
   * @brief
   */
  const Text& GetText() const;

  /**
   * @brief
   */
  void SetStyle( const TextStyle& style, std::size_t begin, std::size_t end, TextStyle::Mask mask = TextStyle::ALL );

  /**
   * @brief
   */
  void GetStyleAt( std::size_t position, TextStyle& style, TextStyle::Mask mask = TextStyle::ALL ) const;

  /**
   * @brief
   */
  void GetStyleRanges( Vector<std::size_t>& positions ) const;

  /**
   * @brief
   */
  void Insert( std::size_t position, const StyledText& styledText );

  /**
   * @brief
   */
  void Remove( std::size_t position, std::size_t numberOfCharacters );

  /**
   * @brief
   */
  StyledText* GetSubText( std::size_t position, std::size_t numberOfCharacters ) const;

  /**
   * @brief
   */
  void Split( const Vector<std::size_t>& positions, Vector<StyledText*>& styledTexts ) const;

private:
  struct Impl;
  Impl* mImpl;
};

//VCC Remove
DALI_IMPORT_API std::ostream& operator<< (std::ostream& o, const StyledText& styledText );

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_STYLED_TEXT_H__
