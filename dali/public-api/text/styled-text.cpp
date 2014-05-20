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

//Forward declarations.
class StyledCharacters;

namespace
{

const Text VOID_TEXT;

bool HasZeroCharacters( const StyledCharacters* styledCharacters )
{
  std::size_t firstCharacter = 0u;
  std::size_t numberOfCharacters = 0u;

  styledCharacters->GetRange( firstCharacter, numberOfCharacters );

  return ( 0u == firstCharacter ) && ( 0u == numberOfCharacters );
}

} // namespace

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
    for( Vector<StyledCharacters*>::Iterator it = mStyles.Begin(), endIt =  mStyles.End(); it != endIt; ++it )
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
    for( Vector<StyledCharacters*>::ConstIterator it = mStyles.Begin(), endIt =  mStyles.End(); it != endIt; ++it )
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
  void SetStyle( const TextStyle& style, std::size_t position, std::size_t numberOfCharacters, TextStyle::Mask mask )
  {
    // std::cout << "-->StyledText::Impl::SetStyle" << std::endl;
    // std::cout << "            position : " << position << std::endl;
    // std::cout << "  numberOfCharacters : " << numberOfCharacters << std::endl;

    mStyles.PushBack( new StyledCharacters( position, numberOfCharacters, style, mask ) );
    // std::cout << "<--StyledText::Impl::SetStyle" << std::endl;
  }

  /**
   *
   */
  void GetStyleRanges( Vector<std::size_t>& positions ) const
  {
    // std::cout << "-->GetStyleRanges" << std::endl;
    for( Vector<StyledCharacters*>::ConstIterator it = mStyles.Begin(), endIt =  mStyles.End(); it != endIt; ++it )
    {
      const StyledCharacters* styledCharacters( *it );

      std::size_t firstCharacter = 0u;
      std::size_t numberOfCharacters = 0u;

      styledCharacters->GetRange( firstCharacter, numberOfCharacters );

      // std::cout << "  add : " << firstCharacter << std::endl;
      positions.PushBack( firstCharacter );
      if( 1u < numberOfCharacters )
      {
        // std::cout << "  add : " << ( firstCharacter + numberOfCharacters - 1u ) << std::endl;
        positions.PushBack( firstCharacter + numberOfCharacters - 1u );
      }
    }

    // Remove duplicates.
    Sort<Vector<std::size_t>::Iterator>( positions.Begin(), positions.End() );
    Vector<std::size_t>::Iterator it = Unique<Vector<std::size_t>::Iterator>( positions.Begin(), positions.End() );
    positions.Resize( it - positions.Begin() );
    // std::cout << "<--GetStyleRanges" << std::endl;
  }

  void Insert( std::size_t position, const StyledText& styledText )
  {
    const Text& text = styledText.GetText();

    const std::size_t length = text.GetLength();

    // Insert the text.
    mText.Insert( position, text );

    // Update current styles.
    for( Vector<StyledCharacters*>::Iterator it = mStyles.Begin(), endIt =  mStyles.End(); it != endIt; ++it )
    {
      StyledCharacters* styledCharacters( *it );

      std::size_t index = 0u;
      std::size_t rangeLength = 0u;

      styledCharacters->GetRange( index, rangeLength );

      if( position <= index )
      {
	index += length;
      }
      else if( position < index + rangeLength )
      {
	rangeLength += length;
      }

      styledCharacters->UpdateRange( index, rangeLength );
    }

    // Add new styles.
    for( Vector<StyledCharacters*>::ConstIterator it = styledText.mImpl->mStyles.Begin(), endIt =  styledText.mImpl->mStyles.End(); it != endIt; ++it )
    {
      StyledCharacters* styledCharacters = new StyledCharacters( **it );

      std::size_t index = 0u;
      std::size_t rangeLength = 0u;

      styledCharacters->GetRange( index, rangeLength );

      styledCharacters->UpdateRange( index + position, rangeLength );

      mStyles.PushBack( styledCharacters );
    }
  }

  void Remove( std::size_t position, std::size_t numberOfCharacters )
  {
    // Remove the text.
    if( !mText.IsEmpty() )
    {
      mText.Remove( position, numberOfCharacters );
    }

    // Update current style ranges.
    const std::size_t removeRange = position + numberOfCharacters;

    for( Vector<StyledCharacters*>::Iterator it = mStyles.Begin(), endIt =  mStyles.End(); it != endIt; ++it )
    {
      StyledCharacters* styledCharacters( *it );

      std::size_t index = 0u;
      std::size_t rangeLength = 0u;

      styledCharacters->GetRange( index, rangeLength );

      const std::size_t styleRange = index + rangeLength;

      if( removeRange <= index )
      {
	// Range to be removed is on the left of the style range.
	index -= numberOfCharacters;
      }
      else if( position <= index )
      {
	if( removeRange >= styleRange )
	{
	  // The range to be removed contains completely the style range
	  // Mark the range to be deleted.
	  index = 0u;
	  rangeLength = 0u;
	}
	else
	{
	  // The range to be removed starts on the left of the style range but finishes in the middle of it.
	  rangeLength -= ( removeRange - index );
	  index = position;
	}
      }
      else if( position < styleRange )
      {
	if( removeRange <= styleRange )
	{
	  // The range to be removed is contained completely in the style range.
	  rangeLength -= numberOfCharacters;
	}
	else
	{
	  // The range to be removed starts inside the style range but finishes outside it.
	  rangeLength -= ( styleRange - position );
	}
      }

      styledCharacters->UpdateRange( index, rangeLength );
    }
    
    // Remove style ranges with no characters.
    const std::size_t stylesSize = mStyles.Count();

    Vector<StyledCharacters*>::Iterator endIt = mStyles.End();
    Vector<StyledCharacters*>::Iterator removeIt = RemoveIf<Vector<StyledCharacters*>::Iterator,bool(*)(const StyledCharacters*)>( mStyles.Begin(), endIt, HasZeroCharacters );

    for( Vector<StyledCharacters*>::Iterator it = removeIt; it != endIt; ++it )
    {
      delete *it;
    }

    mStyles.Resize( stylesSize - ( endIt - removeIt ) );
  }

  /**
   *
   */
  void CopySubStyles( std::size_t from, std::size_t to, StyledText* styledText ) const
  {
    // std::cout << "-->StyledText::Impl::CopySubStyles" << std::endl;
    for( Vector<StyledCharacters*>::ConstIterator it = mStyles.Begin(), endIt =  mStyles.End(); it != endIt; ++it )
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

        styledText->SetStyle( styledCharacters->GetTextStyle(), firstCharacter, 1u + lastCharacter - firstCharacter );
      }
    }
    // std::cout << "<--StyledText::Impl::CopySubStyles" << std::endl;
  }

  StyledText* GetSubText( std::size_t position, std::size_t numberOfCharacters ) const
  {
    const std::size_t to = position + numberOfCharacters - 1u;

    Text text;
    mText.GetSubText( position, to, text );

    StyledText* styledText = new StyledText( text );

    CopySubStyles( position, to, styledText );

    return styledText;
  }

  Text                      mText;
  Vector<StyledCharacters*> mStyles;
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
    mImpl->mStyles.Clear();

    mImpl->mStyles.Reserve( styledText.mImpl->mStyles.Count() );

    // Create new StyledCharacters objects pointing the same TextStyleItem in the TextStyleManager.
    for( Vector<StyledCharacters*>::ConstIterator it = styledText.mImpl->mStyles.Begin(), endIt = styledText.mImpl->mStyles.End(); it != endIt; ++it )
    {
      // The styled characters constructor registers the style into the style manager.
      mImpl->mStyles.PushBack( new StyledCharacters( *(*it) ) );
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
      if( 0u == mImpl->mStyles.Count() )
      {
        delete mImpl;
        mImpl = NULL;
      }
      else
      {
	mImpl->mText = text;
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

void StyledText::SetStyle( const TextStyle& style, std::size_t position, std::size_t numberOfCharacters, TextStyle::Mask mask )
{
  if( NULL == mImpl )
  {
    mImpl = new Impl();
  }

  mImpl->SetStyle( style, position, numberOfCharacters, mask );
}

void StyledText::GetStyleAt( std::size_t position, TextStyle& style, TextStyle::Mask mask ) const
{
  // Reset to default the output style.
  style.Reset();

  if( NULL == mImpl )
  {
    // If there is no implementation it returns the default style.
    return;
  }

  mImpl->GetStyleAt( position, style, mask );
}

void StyledText::GetStyleRanges( Vector<std::size_t>& positions ) const
{
  if( NULL == mImpl )
  {
    // nothing to do.
    return;
  }

  mImpl->GetStyleRanges( positions );
}

void StyledText::Insert( std::size_t position, const StyledText& styledText )
{
  if( NULL == styledText.mImpl )
  {
    // nothing to do.
    return;
  }

  if( NULL == mImpl )
  {
    *this = styledText;
  }
  else
  {
    mImpl->Insert( position, styledText );
  }
}

void StyledText::Remove( std::size_t position, std::size_t numberOfCharacters )
{
  if( ( NULL == mImpl ) ||
      ( 0u == numberOfCharacters) )
  {
    // nothing to do.
    return;
  }

  // Check first if all characters and styles are going to be removed.

  bool deleteImplementation = true;

  const std::size_t removeRange = position + numberOfCharacters;

  if( !mImpl->mText.IsEmpty() )
  {
    // Check if the whole text is deleted.
    if( ( 0u < position ) ||
	( removeRange < mImpl->mText.GetLength() ) )
    {
      // Do not delete the implementation as there is text left.
      deleteImplementation = false;
    }
  }

  if( deleteImplementation )
  {
    // Check if all style ranges are deleted.
    for( Vector<StyledCharacters*>::ConstIterator it = mImpl->mStyles.Begin(), endIt =  mImpl->mStyles.End();
	 deleteImplementation && ( it != endIt );
	 ++it )
    {
      const StyledCharacters* styledCharacters( *it );

      std::size_t index = 0u;
      std::size_t rangeLength = 0u;

      styledCharacters->GetRange( index, rangeLength );

      if( ( index < position ) ||
	  ( index + rangeLength > removeRange ) )
      {
	// Do not delete the implementation as there is some style ranges outside the remove range.
	deleteImplementation = false;
      }
    }
  }

  if( deleteImplementation )
  {
    delete mImpl;
    mImpl = NULL;
  }
  else
  {
    mImpl->Remove( position, numberOfCharacters );
  }
}

StyledText* StyledText::GetSubText( std::size_t position, std::size_t numberOfCharacters ) const
{
  if( ( NULL == mImpl ) ||
      ( 0u == numberOfCharacters) )
  {
    // nothing to do.
    return NULL;
  }

  // Check if the sub text range is within the text's range and / or the styles ranges.

  const bool isRangeWithinText = position < mImpl->mText.GetLength();

  bool isRangeWithinStyles = false;
  if( !isRangeWithinText )
  {
    const std::size_t subTextRange = position + numberOfCharacters;
    for( Vector<StyledCharacters*>::ConstIterator it = mImpl->mStyles.Begin(), endIt =  mImpl->mStyles.End();
	 !isRangeWithinStyles && ( it != endIt );
	 ++it )
    {
      const StyledCharacters* styledCharacters( *it );

      std::size_t index = 0u;
      std::size_t rangeLength = 0u;

      styledCharacters->GetRange( index, rangeLength );

      if( ( position < index + rangeLength ) &&
	  ( index < subTextRange ) )
      {
	isRangeWithinStyles = true;
      }
    }
  }

  if( isRangeWithinText || isRangeWithinStyles )
  {
    return mImpl->GetSubText( position, numberOfCharacters );
  }

  return NULL;
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
