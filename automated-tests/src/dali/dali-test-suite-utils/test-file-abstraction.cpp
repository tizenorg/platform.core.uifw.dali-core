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

#include "test-file-abstraction.h"
#include <stdio.h>
#include <algorithm>

namespace Dali
{
  TestFileAbstraction::TestFileAbstraction(const std::string& name) : mName(name), mSize(8)
  {
  }
  
  TestFileAbstraction::~TestFileAbstraction()
  {
  }
  
  const std::string& 
  TestFileAbstraction::GetName() const
  {
    return mName;
  }

  bool 
  TestFileAbstraction::IsOpen() const
  {
    return true;
  }
  
  int 
  TestFileAbstraction::Seek(int64_t offset, int whence)
  {
    int64_t currentPos = mPosition;
    switch(whence)
    {
      case SEEK_SET:
	currentPos = mPosition + offset;
	break;
	
      case SEEK_CUR:
	currentPos = mPosition + offset;
	break;
	
      case SEEK_END:
	currentPos = mSize + offset - 1;
	break;
	
      default:
	return -1;
	break;
    }    
    
    bool successCondition =  ((currentPos >= 0) && (currentPos < mSize));
    return successCondition ? 0 : 1;
  }
  
  int
  TestFileAbstraction::Read(void* destination_buffer, int count, int size)
  {
    const int64_t copiable_length = std::min((int64_t)size, Available());
    if (copiable_length > 0)
    {
      memcpy(destination_buffer, buffer + mPosition, copiable_length);
      mPosition += copiable_length;
    }
    return copiable_length;
  }

  int64_t 
  TestFileAbstraction::Available() const
  {
    int64_t available = mSize - mPosition;
    return available;
  }
  
  int64_t
  TestFileAbstraction::Size() const
  {
    return mSize;
  }
} // Namespace dali

