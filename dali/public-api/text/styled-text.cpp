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
#include<dali/public-api/text/styled-text.h>

// INTERNAL INCLUDES
#include<dali/public-api/text/styled-characters.h>
#include<dali/public-api/text/text.h>

// EXTERNAL INCLUDES

namespace Dali
{

namespace
{
const Text VOID_TEXT;
} // namespace

class StyledCharacters;

  //VCC remove
  // std::size_t gStyledTextImpl = 0u;
  // std::size_t gStyledText = 0u;


struct StyledText::Impl
{
  // Impl()
  // {
  //   ++gStyledTextImpl;
  //   std::cout << "----------------------------------StyledText::Impl : " << gStyledTextImpl << std::endl;
  // }

  // ~Impl()
  // {
  //   --gStyledTextImpl;
  //   std::cout << "----------------------------------StyledText::Impl : " << gStyledTextImpl << std::endl;
  // }


  /**
   * Traverses and deletes all items of the StyledCharacters vector.
   */
  void DeleteStyles()
  {
    for( std::vector<StyledCharacters*>::iterator it = mStyles.begin(), endIt =  mStyles.end(); it != endIt; ++it )
    {
      delete *it;
    }
  }

  /**
   * Retrieves the style of the given character.
   *
   * It copies non default values to the output style. It gives more priority to the last style set.
   *
   * @param[in]  characterIndex
   * @param[out] style The output style.
   */
  void GetStyleAt( std::size_t characterIndex, TextStyle& style, TextStyle::Mask mask )
  {
    for( std::vector<StyledCharacters*>::const_iterator it = mStyles.begin(), endIt =  mStyles.end(); it != endIt; ++it )
    {
      const StyledCharacters* styledCharacters( *it );

      std::size_t firstCharacter = 0u;
      std::size_t numberOfCharacters = 0u;

      styledCharacters->GetRange( firstCharacter, numberOfCharacters );

      if( ( firstCharacter <= characterIndex ) && ( characterIndex < firstCharacter + numberOfCharacters ) )
      {
        style.Merge( styledCharacters->GetTextStyle(), mask );
      }
    }
  }

  /**
   *
   *
   * @param[in] style
   * @param[in] begin
   * @param[in] end
   * @param[in] mask
   */
  void SetStyle( const TextStyle& style, std::size_t begin, std::size_t end, TextStyle::Mask mask )
  {
    // std::cout << "-->StyledText::Impl::SetStyle" << std::endl;
    // std::cout << "    begin : " << begin << std::endl;
    // std::cout << "  numChar : " << ( ( end - begin ) + 1u ) << std::endl;
    // All styles are going to be merged when a style is retrieved from a character,
    // so to merge only the parameters defined in the mask, is needed to store a copy
    // of those parameters.

    TextStyle toBeMerged;
    toBeMerged.Copy( style, mask );

    mStyles.push_back( new StyledCharacters( begin, 1u + end - begin, toBeMerged ) );
    // std::cout << "<--StyledText::Impl::SetStyle" << std::endl;
  }

  /**
   *
   */
  void GetStyleRanges( std::vector<std::size_t>& positions ) const
  {
    // std::cout << "-->GetStyleRanges" << std::endl;
    for( std::vector<StyledCharacters*>::const_iterator it = mStyles.begin(), endIt =  mStyles.end(); it != endIt; ++it )
    {
      const StyledCharacters* styledCharacters( *it );

      std::size_t firstCharacter = 0u;
      std::size_t numberOfCharacters = 0u;

      styledCharacters->GetRange( firstCharacter, numberOfCharacters );

      // std::cout << "  add : " << firstCharacter << std::endl;
      positions.push_back( firstCharacter );
      if( 1u < numberOfCharacters )
      {
        // std::cout << "  add : " << ( firstCharacter + numberOfCharacters - 1u ) << std::endl;
        positions.push_back( firstCharacter + numberOfCharacters - 1u );
      }
    }

    // Remove duplicates.
    std::sort( positions.begin(), positions.end() );
    std::vector<std::size_t>::iterator it = std::unique( positions.begin(), positions.end() );
    positions.resize( it - positions.begin() );
    // std::cout << "<--GetStyleRanges" << std::endl;
  }

  /**
   *
   */
  void CopySubStyles( std::size_t from, std::size_t to, StyledText& styledText ) const
  {
    // std::cout << "-->StyledText::Impl::CopySubStyles" << std::endl;
    for( std::vector<StyledCharacters*>::const_iterator it = mStyles.begin(), endIt =  mStyles.end(); it != endIt; ++it )
    {
      const StyledCharacters* styledCharacters( *it );

      std::size_t firstCharacter = 0u;
      std::size_t numberOfCharacters = 0u;

      styledCharacters->GetRange( firstCharacter, numberOfCharacters );

      std::size_t lastCharacter = firstCharacter + numberOfCharacters - 1u;

      if( ( from <= lastCharacter ) && ( to >= firstCharacter ) )
      {
        if( firstCharacter <= from )
        {
          firstCharacter = 0u;
        }
        else
        {
          firstCharacter -= from;
        }

        if( lastCharacter > to )
        {
          lastCharacter = to - from;
        }
        else
        {
          lastCharacter -= from;
        }

        styledText.SetStyle( styledCharacters->GetTextStyle(), firstCharacter, lastCharacter );
      }
    }
    // std::cout << "<--StyledText::Impl::CopySubStyles" << std::endl;
  }

  /**
   *
   */
  void Split( const Vector<std::size_t>& positions, std::vector<StyledText>& styledTexts ) const
  {
    // std::cout << "-->StyledText::Split" << std::endl;
    // Reserve space for split styled texts.
    styledTexts.reserve( positions.Size() + 1u );

    std::size_t from = 0u;
    for( Vector<std::size_t>::ConstIterator it = positions.Begin(), endIt = positions.End(); it != endIt; ++it )
    {
      std::size_t to = *it;

      Text text;
      mText.GetSubText( from, to, text );

      StyledText styledText;
      styledText.SetText( text );
      CopySubStyles( from, to, styledText );

      styledTexts.push_back( styledText );

      from = to + 1u;
    }

    // The last piece of text.
    std::size_t to = mText.GetLength() - 1u;
    if( from <= to )
    {
      Text text;
      mText.GetSubText( from, to, text );

      StyledText styledText;
      styledText.SetText( text );
      CopySubStyles( from, to, styledText );

      styledTexts.push_back( styledText );
    }
    // std::cout << "<--StyledText::Split" << std::endl;
  }

  Text                           mText;
  std::vector<StyledCharacters*> mStyles;
};

StyledText::StyledText()
: mImpl( NULL )
{
  // ++gStyledText;
  // std::cout << "----------------------------------StyledText" << gStyledText << std::endl;
}

StyledText::~StyledText()
{
  Clear();
  // --gStyledText;
  // std::cout << "----------------------------------StyledText" << gStyledText << std::endl;
}

StyledText::StyledText( const StyledText& styledText )
: mImpl( NULL )
{
  // ++gStyledText;
  // std::cout << "----------------------------------StyledText" << gStyledText << std::endl;
  *this = styledText;
}

StyledText& StyledText::operator=( const StyledText& styledText )
{
  if( this == &styledText )
  {
    return *this;
  }

  if( styledText.mImpl )
  {
    if( NULL == mImpl )
    {
      mImpl = new Impl();
    }

    mImpl->mText = styledText.mImpl->mText;

    mImpl->DeleteStyles();
    mImpl->mStyles.clear();

    mImpl->mStyles.reserve( styledText.mImpl->mStyles.size() );

    // Create new StyledCharacters objects pointing the same TextStyleItem in the TextStyleManager.
    for( std::vector<StyledCharacters*>::const_iterator it = styledText.mImpl->mStyles.begin(), endIt = styledText.mImpl->mStyles.end(); it != endIt; ++it )
    {
      // The styled characters constructor registers the style into the style manager.
      mImpl->mStyles.push_back( new StyledCharacters( *(*it) ) );
    }
  }
  else
  {
    Clear();
  }

  return *this;
}

StyledText::StyledText( const Text& text )
: mImpl( NULL )
{
  // ++gStyledText;
  // std::cout << "----------------------------------StyledText" << gStyledText << std::endl;
  if( !text.IsEmpty() )
  {
    mImpl = new Impl();

    mImpl->mText = text;
  }
}

void StyledText::Clear()
{
  // Delete styled characters. The styled characters destructor unregisters the style from the style manager.

  if( NULL != mImpl )
  {
    mImpl->DeleteStyles();
  }

  delete mImpl;
  mImpl = NULL;
}

void StyledText::SetText( const Text& text )
{
  if( text.IsEmpty() )
  {
    // If text is empty and there are no styles, delete the implementation.
    if( NULL != mImpl )
    {
      if( mImpl->mStyles.empty() )
      {
        delete mImpl;
        mImpl = NULL;
      }
    }

    return;
  }

  if( NULL == mImpl )
  {
    mImpl = new Impl();
  }

  mImpl->mText = text;
}

const Text& StyledText::GetText() const
{
  if( NULL == mImpl )
  {
    // Return a void text if there is no implementation.
    return VOID_TEXT;
  }

  return mImpl->mText;
}

void StyledText::SetStyle( const TextStyle& style, std::size_t begin, std::size_t end, TextStyle::Mask mask )
{
  if( NULL == mImpl )
  {
    mImpl = new Impl();
  }

  mImpl->SetStyle( style, begin, end, mask );
}

void StyledText::GetStyleAt( std::size_t position, TextStyle& style, TextStyle::Mask mask ) const
{
  // Reset to default the output style.
  style.Reset();

  if( ( NULL == mImpl ) || ( position >= mImpl->mText.GetLength() ) )
  {
    // If there is no text or the position exceeds the boundaries of the text, it returns the default style.
    return;
  }

  mImpl->GetStyleAt( position, style, mask );
}

void StyledText::GetStyleRanges( std::vector<std::size_t>& positions ) const
{
  if( NULL == mImpl )
  {
    // nothing to do.
    return;
  }

  mImpl->GetStyleRanges( positions );
}

void StyledText::Split( const Vector<std::size_t>& positions, std::vector<StyledText>& styledTexts ) const
{
  if( NULL == mImpl )
  {
    // nothing to do.
    return;
  }

  if( 0u == positions.Count() )
  {
    // nothing to do if there is no positions to split.
    return;
  }

  mImpl->Split( positions, styledTexts );
}

std::ostream& operator<<( std::ostream& o, const StyledText& styledText )
{
  o << "{" << std::endl;

  std::cout << "  text : [" << styledText.GetText().GetText() << "]" << std::endl;
  std::cout << "  styles : " << std::endl;
  for( std::size_t index = 0u, length = styledText.GetText().GetLength(); index < length; ++index )
  {
    TextStyle style;
    styledText.GetStyleAt( index, style );

    std::cout << "  style for character : " << index << std::endl;
    std::cout << style << std::endl;
  }

  o << "}" << std::endl;

  return o;
}

} // namespace Dali
