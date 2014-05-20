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

#include <dali/public-api/text/text-processor.h>

// INTERNAL INCLUDES

#include <dali/public-api/text/styled-characters.h>
#include <dali/public-api/text/styled-text.h>
#include <dali/internal/common/text-array.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/emoji-factory.h>
#include <dali/internal/event/text/character-impl.h>
#include <dali/internal/event/text/text-impl.h>
#include <dali/internal/event/text/font-impl.h>
#include <dali/integration-api/platform-abstraction.h>

namespace Dali
{

namespace
{

bool AreAllWhiteSpaces( const Internal::TextArray& text, std::size_t from, std::size_t numberOfCharacters )
{
  for( Internal::TextArray::ConstIterator it = text.Begin() + from, endIt = text.Begin() + ( from + numberOfCharacters ); it != endIt; ++it )
  {
    if( !Internal::Character::IsWhiteSpace( *it ) )
    {
      return false;
    }
  }

  return true;
}

void MergeWhiteSpaceStyles( Integration::Text& textToProcess )
{
  // Merge white space character's styles with previous style (if possible). If the text starts
  // with a white space it merges with the next non white space.

  Vector<Integration::FontPerCharacter*> fontPerCharacters = textToProcess.mFontPerCharacters;
  textToProcess.mFontPerCharacters.Clear();

  std::size_t characterIndex = 0u;
  bool previousUnmergedWhiteSpaces = false;
  bool previousMergedWhiteSpaces = false;
  Integration::FontPerCharacter* previousFontPerCharacter = NULL;

  for( Vector<Integration::FontPerCharacter*>::ConstIterator it = fontPerCharacters.Begin(), endIt = fontPerCharacters.End(); it != endIt; ++it )
  {
    Integration::FontPerCharacter* fontPerCharacter = *it;

    // Check if all characters of current group are white spaces.
    const bool isWhiteSpace = AreAllWhiteSpaces( textToProcess.mText, characterIndex, fontPerCharacter->mNumberOfCharacters );

    // point to the first character of the next group.
    characterIndex += fontPerCharacter->mNumberOfCharacters;

    if( isWhiteSpace )
    {
      if( ( NULL != previousFontPerCharacter ) &&
          ( fabs( previousFontPerCharacter->mFontPointSize - fontPerCharacter->mFontPointSize ) < Math::MACHINE_EPSILON_1000 ) )
      {
        // Current group of characters are white spaces and have the same size than the previous group of characters. Merge them.
        previousFontPerCharacter->mNumberOfCharacters += fontPerCharacter->mNumberOfCharacters;
        previousMergedWhiteSpaces = true;

        // this group is not needed.
        delete fontPerCharacter;
      }
      else
      {
        // Current group of characters are white spaces but with a different size or there isn't a previous group of characters.
        // Push it to the list and try to merge with the next group of characters in the next iteration.
        textToProcess.mFontPerCharacters.PushBack( fontPerCharacter );
        previousFontPerCharacter = fontPerCharacter;
        previousUnmergedWhiteSpaces = true;
      }
    }
    else
    {
      if( previousUnmergedWhiteSpaces &&
          ( fabs( previousFontPerCharacter->mFontPointSize - fontPerCharacter->mFontPointSize ) < Math::MACHINE_EPSILON_1000 ) )
      {
        // Current group of characters are not all white spaces but the previous one it was,
        // it was not merged with other group and they have the same size. Merge them.
        previousFontPerCharacter->mFontFamilyName = fontPerCharacter->mFontFamilyName;
        previousFontPerCharacter->mFontStyle = fontPerCharacter->mFontStyle;
        previousFontPerCharacter->mNumberOfCharacters += fontPerCharacter->mNumberOfCharacters;
      }
      else if( previousMergedWhiteSpaces &&
               ( previousFontPerCharacter->mFontFamilyName == fontPerCharacter->mFontFamilyName ) &&
               ( previousFontPerCharacter->mFontStyle == fontPerCharacter->mFontStyle ) &&
               ( fabs( previousFontPerCharacter->mFontPointSize - fontPerCharacter->mFontPointSize ) < Math::MACHINE_EPSILON_1000 ) )
      {
        // Current group of characters are not all white spaces but the previous one it was,
        // it was merged with other group of characters with the same font and same size than current one. Merge them.
        previousFontPerCharacter->mNumberOfCharacters += fontPerCharacter->mNumberOfCharacters;
        previousMergedWhiteSpaces = false;

        // this group is not needed.
        delete fontPerCharacter;
      }
      else
      {
        // Can't merge this group of characters with the previous one.
        textToProcess.mFontPerCharacters.PushBack( fontPerCharacter );
        previousFontPerCharacter = fontPerCharacter;
      }
      previousUnmergedWhiteSpaces = false;
    }
  }
}

} // namespace

TextProcessor& TextProcessor::Get()
{
  static TextProcessor textProcessor;
  return textProcessor;
}

TextProcessor::~TextProcessor()
{
  // nothing to do.
}

void TextProcessor::ProcessMetrics( StyledText& styledText,
                                    std::vector<Font::Metrics>& metrics,
                                    std::vector<TextProcessor::FontInfo>& fontInfoForCharacters )
{
  // std::cout << "-->TextProcessor::ProcessMetrics" << std::endl;
  const Internal::TextArray& textArray = styledText.GetText().GetImplementation().GetTextArray();

  if( 0u == textArray.Count() )
  {
    // std::cout << "<--TextProcessor::ProcessMetrics. Empty text array" << std::endl;
    return;
  }

  // Retrieve the text, font family names and font styles from the text array and insert them into the vectors.
  Vector<Integration::FontPerCharacter*> fontPerCharacters;

  Integration::FontPerCharacter* fontPerCharacter = new Integration::FontPerCharacter();

  const TextStyle::Mask mask = static_cast<TextStyle::Mask>( TextStyle::FONT | TextStyle::STYLE | TextStyle::SIZE );

  std::size_t index = 0u;
  for( Internal::TextArray::ConstIterator it = textArray.Begin(), endIt = textArray.End(); it != endIt; ++it, ++index )
  {
    TextStyle style;
    styledText.GetStyleAt( index, style, mask );

    const std::string& fontFamilyName = style.GetFontName();
    const std::string& fontStyle = style.GetFontStyle();
    const float fontSize = style.GetFontPointSize();
    const bool isDefaultSize = style.IsFontSizeDefault();
    const bool isEmoji = Internal::ThreadLocalStorage::Get().GetEmojiFactory().IsEmoji( *it );

    if( ( isDefaultSize != fontPerCharacter->mIsFontPointSizeDefault ) ||
        ( isEmoji != fontPerCharacter->mIsEmoji ) ||
        ( !isEmoji && ( fontFamilyName != fontPerCharacter->mFontFamilyName ) ) || // If it's emoji, don't care which font name or style is defined, it's going to be replaced.
        ( !isEmoji &&( fontStyle != fontPerCharacter->mFontStyle ) ) ||
        ( fabsf( fontSize - fontPerCharacter->mFontPointSize ) > Math::MACHINE_EPSILON_1000 ) )
    {
      if( 0u < fontPerCharacter->mNumberOfCharacters )
      {
        fontPerCharacters.PushBack( fontPerCharacter );
        fontPerCharacter = new Integration::FontPerCharacter();
      }

      fontPerCharacter->mFontFamilyName = fontFamilyName;
      fontPerCharacter->mFontStyle = fontStyle;
      fontPerCharacter->mFontPointSize = fontSize;
      fontPerCharacter->mIsFontPointSizeDefault = isDefaultSize;
      fontPerCharacter->mIsEmoji = isEmoji;
    }

    ++fontPerCharacter->mNumberOfCharacters;
  }

  if( 0u < fontPerCharacter->mNumberOfCharacters )
  {
    fontPerCharacters.PushBack( fontPerCharacter );
  }
  else
  {
    delete fontPerCharacter;
  }

  Integration::Text textToProcess( textArray,
                                   fontPerCharacters );

  // Check for each character if the given font and style are supported. If not, it choses one which supports the character.
  Internal::ThreadLocalStorage::Get().GetPlatformAbstraction().ProcessText( textToProcess );

  // Merge the styles of white spaces is needed to avoid the creation of extra text-actor.
  // i.e The default system platform doesn't suport korean characters but supports latin characters,
  //     the text to process contains 2 korean characters with a white space in between and no styles are defined.
  //     The result to process the text is a font to support the first korean character, the default
  //     font which supports the white space and the font which supports the korean character again.
  //     This would create three text-actor. If the white space is merged, one text-actor will be created for the three characters.
  MergeWhiteSpaceStyles( textToProcess );


  // for( std::vector<Integration::FontPerCharacter*>::const_iterator it = fontPerCharacters.begin(), endIt = fontPerCharacters.end(); it != endIt; ++it )
  // {
  //   Integration::FontPerCharacter* fontPerCharacter = *it;

  //   std::cout << "   font name : [" << fontPerCharacter->mFontFamilyName << "]" << std::endl;
  //   std::cout << "  font style : [" << fontPerCharacter->mFontStyle << "]" << std::endl;
  //   std::cout << "   font size : [" << fontPerCharacter->mFontPointSize << "]" << std::endl;
  //   std::cout << "    num char : [" << fontPerCharacter->mNumberOfCharacters << "]" << std::endl;
  // }


  // Update the styled text with the new font family names and font styles. Fill metrics.

  // Reserve space and fill if with default containers for the metrics and the global metrics.
  metrics.resize( textArray.Count() );
  fontInfoForCharacters.resize( fontPerCharacters.Count() );

  const Font defaultFont = Font::New();
  std::size_t from = 0u;

  Vector<Integration::FontPerCharacter*>::Iterator fontIt, fontEndIt;
  std::vector<FontInfo>::iterator infoIt, infoEndIt;
  for( fontIt = fontPerCharacters.Begin(), fontEndIt = fontPerCharacters.End(), infoIt = fontInfoForCharacters.begin(), infoEndIt = fontInfoForCharacters.end();
       ( fontIt != fontEndIt ) && ( infoIt != infoEndIt );
       ++fontIt, ++infoIt )
  {
    Integration::FontPerCharacter* fontPerCharacter( *fontIt );
    FontInfo& fontInfo( *infoIt );

    const bool isDefaultFontName = fontPerCharacter->mFontFamilyName.empty();
    const bool isdefaultFontStyle = fontPerCharacter->mFontStyle.empty();

    Font font;

    if( !isDefaultFontName ||
        !isdefaultFontStyle ||
        !fontPerCharacter->mIsFontPointSizeDefault )
    {
      TextStyle style;
      if( !isDefaultFontName )
      {
        style.SetFontName( fontPerCharacter->mFontFamilyName );
      }
      if( !isdefaultFontStyle )
      {
        style.SetFontStyle( fontPerCharacter->mFontStyle );
      }
      if( !fontPerCharacter->mIsFontPointSizeDefault )
      {
        style.SetFontPointSize( PointSize( fontPerCharacter->mFontPointSize ) );
      }

      styledText.SetStyle( style, from, fontPerCharacter->mNumberOfCharacters, mask );

      font = Font::New( FontParameters( fontPerCharacter->mFontFamilyName,
                                        fontPerCharacter->mFontStyle,
                                        PointSize( fontPerCharacter->mFontPointSize ) ) );
    }
    else
    {
      font = defaultFont;
    }

    const Internal::Font& fontImpl = GetImplementation( font );
    fontImpl.GetMetrics( textArray,
                         from,
                         fontPerCharacter->mNumberOfCharacters,
                         metrics );

    fontInfo.mNumberOfCharacters = fontPerCharacter->mNumberOfCharacters;
    fontImpl.GetGlobalMetrics( fontInfo.mGlobalMetrics.GetImplementation() );
    fontInfo.mIsEmoji = fontPerCharacter->mIsEmoji;

    // std::cout << "  Font info global metrics" << std::endl;
    // std::cout << "  line height : " << fontInfo.mGlobalMetrics.GetLineHeight() << std::endl;

    from += fontPerCharacter->mNumberOfCharacters;

    delete fontPerCharacter;
  }

  // { // VCC To be deleted

  //   std::cout << "============================================= After text processing.=========================" << std::endl;

  //   std::cout << styledText << std::endl;
  // }

  DALI_ASSERT_DEBUG( textArray.Count() == metrics.size() );
  DALI_ASSERT_DEBUG( fontPerCharacters.Count() == fontInfoForCharacters.size() );
  // std::cout << "<--TextProcessor::ProcessMetrics" << std::endl;
}

void TextProcessor::ProcessStyles( const StyledText& styledText,
                                   Vector<StyledCharacters*>& styleInfoForCharacters )
{
  // std::cout << "-->TextProcessor::ProcessStyles" << std::endl;
  const Text& text = styledText.GetText();

  if( text.IsEmpty() )
  {
  // std::cout << "<--TextProcessor::ProcessStyles. Empty text." << std::endl;
    return;
  }

  Vector<std::size_t> styleChangePositions;

  styledText.GetStyleRanges( styleChangePositions );

  // if( styleChangePositions.empty() )
  // {
  //   std::cout << "  No styles!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  // }

  // for( std::vector<std::size_t>::const_iterator it = styleChangePositions.begin(), endIt = styleChangePositions.end(); it != endIt; ++it )
  // {
  //   std::cout << *it << " ";
  // }
  // std::cout << std::endl;

  const std::size_t length = text.GetLength();

  const TextStyle DEFAULT_STYLE;

  TextStyle previousStyle;
  std::size_t previousPosition = 0u;
  for( Vector<std::size_t>::ConstIterator it = styleChangePositions.Begin(), endIt = styleChangePositions.End(); it != endIt; ++it )
  {
    const std::size_t currentPosition = *it;
    // std::cout << "  ----------------------------------- position : " << currentPosition << std::endl;

    // Retrieve the style for the current position.
    TextStyle currentStyle;
    styledText.GetStyleAt( currentPosition, currentStyle );

    if( previousStyle != currentStyle )
    {
      // Current style is different than previous one.
      // Add previous style if is not a default style.
      if( DEFAULT_STYLE != previousStyle )
      {
        const std::size_t numberOfCharacters = currentPosition - previousPosition;
        // std::cout << "  (1) Add position : " << previousPosition << ", num chars : " << numberOfCharacters << std::endl;
        // std::cout << previousStyle << std::endl << std::endl;
        StyledCharacters* styledCharacters = new StyledCharacters( previousPosition, numberOfCharacters, previousStyle );
            styleInfoForCharacters.PushBack( styledCharacters );
      }
      previousPosition = currentPosition;
      previousStyle = currentStyle;
    }
    else
    {
      // Current style is equal than the previous one ...
    }


    // Retrieve the style for the next position as well.
    const std::size_t nextPosition = currentPosition + 1u;
    Vector<std::size_t>::ConstIterator nextIt = it + 1u;

    // Next position must be in the text boundaries.
    bool checkNextPosition = nextPosition < length;

    // Next position must not be already in one of the style ranges.
    if( checkNextPosition && ( nextIt != endIt ) )
    {
      checkNextPosition = nextPosition != *nextIt;
    }

    if( checkNextPosition )
    {
      TextStyle nextStyle;
      styledText.GetStyleAt( nextPosition, nextStyle );

      if( previousStyle != nextStyle )
      {
        // Add the previous style!

        const std::size_t numberOfCharacters = nextPosition - previousPosition;
        // std::cout << "  (2) Add position : " << previousPosition << ", num chars : " << numberOfCharacters << std::endl;
        // std::cout << previousStyle << std::endl << std::endl;
        StyledCharacters* styledCharacters = new StyledCharacters( previousPosition, numberOfCharacters, previousStyle );
        styleInfoForCharacters.PushBack( styledCharacters );

        previousStyle = nextStyle;
        previousPosition = nextPosition;
      }
    }

    if( nextPosition == length )
    {
      // Process the last style.
      if( DEFAULT_STYLE != currentStyle )
      {
        std::size_t numberOfCharacters = 0u;
        if( previousStyle == currentStyle )
        {
          numberOfCharacters = 1u + currentPosition - previousPosition;
        }
        else
        {
          numberOfCharacters = 1u;
          previousPosition = currentPosition;
        }
        // std::cout << "  (3) Add position : " << previousPosition << ", num chars : " << numberOfCharacters << std::endl;
        // std::cout << currentStyle << std::endl << std::endl;
        StyledCharacters* styledCharacters = new StyledCharacters( previousPosition, numberOfCharacters, currentStyle );
        styleInfoForCharacters.PushBack( styledCharacters );
      }
    }
  }

  // for( std::vector<StyledCharacters*>::const_iterator it = styleInfoForCharacters.begin(), endIt = styleInfoForCharacters.end(); it != endIt; ++it )
  // {
  //   const StyledCharacters* styledCharacters( *it );

  //   std::cout << "  first : " << styledCharacters->GetFirstCharacterIndex() << ", num chars : " << styledCharacters->GetNumberOfCharacters() << std::endl;
  // }

  // std::cout << "<--TextProcessor::ProcessStyles" << std::endl;
}

TextProcessor::TextProcessor()
{
  // nothing to do.
}

} // namespace Dali
