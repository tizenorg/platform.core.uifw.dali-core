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

// CLASS HEADER
#include<dali/internal/event/text/text-style-manager.h>

// INTERNAL INCLUDES
#include<dali/public-api/text/text-style.h>
#include<dali/internal/event/text/text-style-item.h>

// EXTERNAL INCLUDES

namespace Dali
{

namespace Internal
{

TextStyleManager& TextStyleManager::Get()
{
  static TextStyleManager textStyleManager;
  return textStyleManager;
}

TextStyleManager::~TextStyleManager()
{
  for( Vector<TextStyleItem*>::Iterator it = mTextStyles.Begin(), endIt = mTextStyles.End(); it != endIt; ++it )
  {
    delete *it;
  }
}

TextStyleItem* TextStyleManager::RegisterTextStyle( const Dali::TextStyle& style )
{
  // std::cout << "-->TextStyleManager::RegisterTextStyle" << std::endl;
  bool found = false;
  TextStyleItem* item = NULL;

  // first find if the style is already registered.
  for( Vector<TextStyleItem*>::Iterator it = mTextStyles.Begin(), endIt = mTextStyles.End(); !found && ( it != endIt ); ++it )
  {
    item = *it ;

    if( style == item->mStyle )
    {
      found = true;
      ++item->mNumberOfInstances;
    }
  }

  if( !found )
  {
    // Add the new style.
    item = new TextStyleItem();

    item->mStyle = style;
    item->mNumberOfInstances = 1u;
    item->mVectorPosition = mTextStyles.Count();

    mTextStyles.PushBack( item );
  }

  // std::size_t i = 0u;
  // for( Vector<TextStyleItem*>::Iterator it = mTextStyles.Begin(), endIt = mTextStyles.End(); it != endIt; ++it )
  // {
  //   std::cout << "  style : " << i << " number of instances : " << (*it)->mNumberOfInstances << std::endl;
  //   std::cout << (*it)->mStyle << std::endl;
  //   ++i;
  // }
  // std::cout << "<--TextStyleManager::RegisterTextStyle" << std::endl;
  return item;
}

void TextStyleManager::RegisterTextStyle( TextStyleItem* styleItem )
{
  // std::cout << "-->TextStyleManager::RegisterTextStyle" << std::endl;
  // Find  the item.
  Vector<TextStyleItem*>::Iterator it = mTextStyles.Begin() + styleItem->mVectorPosition;
  TextStyleItem* item( *it );

  // Increase the count reference
  ++item->mNumberOfInstances;
  // std::size_t i = 0u;
  // for( Vector<TextStyleItem*>::Iterator it = mTextStyles.Begin(), endIt = mTextStyles.End(); it != endIt; ++it )
  // {
  //   std::cout << "  style : " << i << " number of instances : " << (*it)->mNumberOfInstances << std::endl;
  //   std::cout << (*it)->mStyle << std::endl;
  //   ++i;
  // }
  // std::cout << "<--TextStyleManager::RegisterTextStyle" << std::endl;
}

void TextStyleManager::UnRegisterTextStyle( TextStyleItem* styleItem )
{
  // std::cout << "-->TextStyleManager::UnRegisterTextStyle" << std::endl;
  // Find  the item.
  Vector<TextStyleItem*>::Iterator it = mTextStyles.Begin() + styleItem->mVectorPosition;
  TextStyleItem* item( *it );

  // Decrease the count reference
  --item->mNumberOfInstances;

  if( 0u == item->mNumberOfInstances )
  {
    // Delete the item.
    // Vector::Remove() deletes an item by replacing it by the last one and erase then the last item.
    mTextStyles.Remove( it );

    // Regenerate vector positions.
    std::size_t index = 0u;
    for( Vector<TextStyleItem*>::Iterator it = mTextStyles.Begin(), endIt = mTextStyles.End(); it != endIt; ++it, ++index )
    {
      (*it)->mVectorPosition = index;
    }
  }
  // std::size_t i = 0u;
  // for( Vector<TextStyleItem*>::Iterator it = mTextStyles.Begin(), endIt = mTextStyles.End(); it != endIt; ++it )
  // {
  //   std::cout << "  style : " << i << " number of instances : " << (*it)->mNumberOfInstances << std::endl;
  //   std::cout << (*it)->mStyle << std::endl;
  //   ++i;
  // }
  // std::cout << "<--TextStyleManager::UnRegisterTextStyle" << std::endl;
}

TextStyleItem* TextStyleManager::GetDefaultStyle() const
{
  // The default style is always in the first position.
  return *mTextStyles.Begin();
}

TextStyleManager::TextStyleManager()
: mTextStyles()
{
  // Add the default text style.
  TextStyle style;
  RegisterTextStyle( style );
}

} // namespace Internal

} // namespace Dali
