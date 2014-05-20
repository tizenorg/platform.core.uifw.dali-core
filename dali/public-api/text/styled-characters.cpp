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

// HEADER CLASS
#include <dali/public-api/text/styled-characters.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/internal/event/text/text-style-item.h>
#include <dali/internal/event/text/text-style-manager.h>

namespace Dali DALI_IMPORT_API
{


  // VCC remove
  // std::size_t gStyledCharactersImpl = 0u;
  // std::size_t gStyledCharacters = 0u;

struct StyledCharacters::Impl
{
  Impl()
  {
    // ++gStyledCharactersImpl;
    // std::cout << "-----------------------------------------StyledCharacters::Impl : " << gStyledCharactersImpl << std::endl;
  }
  ~Impl()
  {
    // --gStyledCharactersImpl;
    // std::cout << "-----------------------------------------StyledCharacters::Impl : " << gStyledCharactersImpl << std::endl;
  }
  std::size_t mFirstCharacter;
  std::size_t mNumberOfCharacters;
  Internal::TextStyleItem* mStyleItem;
};

StyledCharacters::StyledCharacters()
: mImpl( NULL )
{
  // ++gStyledCharacters;
  // std::cout << "-----------------------------------------StyledCharacters : " << gStyledCharacters << std::endl;
}

StyledCharacters::StyledCharacters( const StyledCharacters& styledCharacters )
: mImpl( NULL )
{
  *this = styledCharacters;
  // ++gStyledCharacters;
  // std::cout << "-----------------------------------------StyledCharacters : " << gStyledCharacters << std::endl;
}

StyledCharacters::~StyledCharacters()
{
  if( NULL != mImpl )
  {
    // Unregister the style before destroying the implementation.
    Internal::TextStyleManager::Get().UnRegisterTextStyle( mImpl->mStyleItem );
  }

  // --gStyledCharacters;
  // std::cout << "-----------------------------------------StyledCharacters : " << gStyledCharacters << std::endl;
  delete mImpl;
}

StyledCharacters& StyledCharacters::operator=( const StyledCharacters& styledCharacters )
{
  if( this == &styledCharacters )
  {
    return *this;
  }

  if( NULL != styledCharacters.mImpl )
  {
    if( NULL == mImpl )
    {
      // Create a new implementation.
      mImpl = new Impl();
    }

    // Copy the range of characters.
    mImpl->mFirstCharacter = styledCharacters.mImpl->mFirstCharacter;
    mImpl->mNumberOfCharacters = styledCharacters.mImpl->mNumberOfCharacters;
    mImpl->mStyleItem = styledCharacters.mImpl->mStyleItem;

    // Increases the count reference of the style.
    Internal::TextStyleManager::Get().RegisterTextStyle( mImpl->mStyleItem );
  }
  else
  {
    // Decreases the count reference of the style before destroying the implementation.
    // It unregisters the style if the count reference arrives to zero.
    Internal::TextStyleManager::Get().UnRegisterTextStyle( mImpl->mStyleItem );

    // Destroys the implementation.
    delete mImpl;
    mImpl = NULL;
  }

  return *this;
}

StyledCharacters::StyledCharacters( std::size_t firstCharacter, std::size_t numberOfCharacters, const TextStyle& style, TextStyle::Mask mask )
: mImpl( new Impl() )
{
  mImpl->mFirstCharacter = firstCharacter;
  mImpl->mNumberOfCharacters = numberOfCharacters;

  // Register the style.
  mImpl->mStyleItem = Internal::TextStyleManager::Get().RegisterTextStyle( style, mask );
  // ++gStyledCharacters;
  // std::cout << "-----------------------------------------StyledCharacters : " << gStyledCharacters << std::endl;
}

std::size_t StyledCharacters::GetFirstCharacterIndex() const
{
  return ( NULL != mImpl ) ? mImpl->mFirstCharacter : 0u;
}

std::size_t StyledCharacters::GetNumberOfCharacters() const
{
  return ( NULL != mImpl ) ? mImpl->mNumberOfCharacters : 0u;
}

void StyledCharacters::GetRange( std::size_t& firstCharacter, std::size_t& numberOfCharacters ) const
{
  firstCharacter = 0u;
  numberOfCharacters = 0u;

  if( mImpl )
  {
    firstCharacter = mImpl->mFirstCharacter;
    numberOfCharacters = mImpl->mNumberOfCharacters;
  }
}

void StyledCharacters::UpdateRange( std::size_t firstCharacter, std::size_t numberOfCharacters )
{
  if( NULL == mImpl )
  {
    // nothing to do if there is no implementation.
    return;
  }

  mImpl->mFirstCharacter = firstCharacter;
  mImpl->mNumberOfCharacters = numberOfCharacters;
}

const TextStyle& StyledCharacters::GetTextStyle() const
{
  if( NULL == mImpl )
  {
    return Internal::TextStyleManager::Get().GetDefaultStyle();
  }

  return mImpl->mStyleItem->mStyle;
}

} //namespace Dali
