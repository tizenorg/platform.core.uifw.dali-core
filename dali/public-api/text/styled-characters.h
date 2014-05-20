#ifndef __DALI_STYLED_CHARACTERS_H__
#define __DALI_STYLED_CHARACTERS_H__

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
#include<dali/public-api/text/text-style.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief Stores the style used within a range of consecutive characters.
 */
class StyledCharacters
{
public:
  StyledCharacters();
  StyledCharacters( const StyledCharacters& styledCharacters );
  ~StyledCharacters();

  StyledCharacters& operator=( const StyledCharacters& styledCharacters );

  StyledCharacters( std::size_t firstCharacter, std::size_t numberOfCharacters, const TextStyle& style );

  std::size_t GetFirstCharacterIndex() const;
  std::size_t GetNumberOfCharacters() const;
  void GetRange( std::size_t& firstCharacter, std::size_t& numberOfCharacters ) const;

  const TextStyle& GetTextStyle() const;

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace Dali

#endif // __DALI_STYLED_CHARACTERS_H__
