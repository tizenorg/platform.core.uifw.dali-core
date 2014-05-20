#ifndef __DALI_TEXT_PROCESSOR_H__
#define __DALI_TEXT_PROCESSOR_H__

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

#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/text/font.h>

namespace Dali DALI_IMPORT_API
{

class StyledText;
class StyledCharacters;

/**
 * @brief
 */
class TextProcessor
{
public:
  struct FontInfo
  {
    std::size_t mNumberOfCharacters;    ///< Number of consecutive characters with the same font.
    Font::GlobalMetrics mGlobalMetrics; ///< Global metrics of the font (height, ascender, underline position and underline thickness).
    bool mIsEmoji;                      ///< Whether this font if an emoji font.
  };

public:
  static TextProcessor& Get();

  ~TextProcessor();

  /**
   * @brief Retrieves metrics for the given text.
   *
   * It also checks for each character if it's supported by the given font.
   * If it's not, it replaces the given font by a most suitable one.
   *
   * A void font family name means the default platform font family name.
   *
   * @param[in,out] styledText The text with style.
   * @param[out] metrics Glyph metrics for each character.
   * @param[out] fontInfoForCharacters
   */
  void ProcessMetrics( StyledText& styledText,
                       std::vector<Font::Metrics>& metrics,
                       std::vector<FontInfo>& fontInfoForCharacters );

  /**
   * @param[in] styledText The text with style.
   * @param[out] styleInfoForCharacters
   */
  void ProcessStyles( const StyledText& styledText,
                      Vector<StyledCharacters*>& styleInfoForCharacters );

private:
  TextProcessor();
};

} // namespace Dali

#endif // __DALI_TEXT_PROCESSOR_H__
