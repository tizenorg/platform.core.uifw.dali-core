#ifndef __DALI_TEXT_STYLE_ITEM_H__
#define __DALI_TEXT_STYLE_ITEM_H__

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
#include<dali/public-api/text/text-style.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Internal
{

/**
 * @brief Stores a text style, the number of references to it and the position of the text style manager's vector where it's stored.
 */
struct TextStyleItem
{
  TextStyle      mStyle;             ///< Stores a text style.
  std::size_t    mNumberOfInstances; ///< Number of references to that text style.
  unsigned short mVectorPosition;    ///< Position within the text style manager's vector.
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_TEXT_STYLE_ITEM_H__
