
#ifndef FREE_LIST_H_
#define FREE_LIST_H_

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

//INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Internal
{

struct FreeList
{
  FreeList()
  :mData(),
   mFirstFreeIndex(0)
  {}

  ~FreeList()
  {}

  //Add a new item to the list
  unsigned int Add( unsigned int value )
  {
    if( mData.Empty() || mFirstFreeIndex == mData.Size() )
    {
      //Make room for another item
      size_t size = mData.Size();
      mData.PushBack( size+1 );
      mFirstFreeIndex = size;
    }

    //Update first free index
    unsigned int index = mFirstFreeIndex;
    mFirstFreeIndex = mData[mFirstFreeIndex];

    mData[index] = value;
    return index;
  }

  //Remove an item from the list
  void Remove( unsigned int index )
  {
    mData[index] = mFirstFreeIndex;
    mFirstFreeIndex = index;
  }

  unsigned int& operator[]( const unsigned int index )
  {
    return mData[index];
  }

  const unsigned int& operator[]( const unsigned int index ) const
  {
    return mData[index];
  }

  Dali::Vector<unsigned int> mData;
  unsigned int mFirstFreeIndex;
};

}
}

#endif /* FREE_LIST_H_ */
