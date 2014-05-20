#ifndef __DALI_INTEGRATION_TEXT_H__
#define __DALI_INTEGRATION_TEXT_H__

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
#include <string>
#include <stdint.h>


// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Integration
{

typedef Vector<uint32_t> TextArray;

  //VCC remove
  /* static std::size_t gNumberOfFontPerCharacter = 0u; */

/**
 * Stores the font family name and the font style for a number of consecutive characters.
 */
struct DALI_IMPORT_API FontPerCharacter
{
  FontPerCharacter()
  : mNumberOfCharacters( 0u ),
    mFontFamilyName(),
    mFontStyle(),
    mFontPointSize( 0.f ),
    mIsFontPointSizeDefault( true ),
    mIsEmoji( false )
  {
    /* ++gNumberOfFontPerCharacter; */
    /* std::cout << "-----------------------------------------Integration::FontPerCharacter : " << gNumberOfFontPerCharacter << std::endl; */
  }

  ~FontPerCharacter()
  {
    /* --gNumberOfFontPerCharacter; */
    /* std::cout << "-----------------------------------------Integration::FontPerCharacter : " << gNumberOfFontPerCharacter << std::endl; */
  }

  std::size_t mNumberOfCharacters;       ///< Number of consecutive characters with the same font family name and font style.
  std::string mFontFamilyName;           ///< The font family name.
  std::string mFontStyle;                ///< The font style.
  float       mFontPointSize;            ///< The font point size. Used to retrieve the metrics.
  bool        mIsFontPointSizeDefault:1; ///< Whether the font point size is the default.
  bool        mIsEmoji:1;                ///< Whether characters are emojis.
};

/**
 * Stores references to a vector of characters and a vector of pointers to FontPerCharacter items which stores the font
 * family names and font styles of the characters.
 */
struct DALI_IMPORT_API Text
{
  /**
   * Constructor.
   *
   * Initializes the Text by storing a reference to a vector of unicode characters and a reference to
   * a vector of FontPerCharacter items which stores the font family names and the font styles.
   */
  Text( const TextArray& text,
        Vector<FontPerCharacter*>& fontPerCharacters )
  : mText( text ),
    mFontPerCharacters( fontPerCharacters )
  {}

  const TextArray&    mText;                     ///< Vector of characters encoded in unicode.
  Vector<FontPerCharacter*>& mFontPerCharacters; ///< Vector of pointers to FontPerCharacter items. It stores the font family names and the font styles.
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_TEXT_H__
