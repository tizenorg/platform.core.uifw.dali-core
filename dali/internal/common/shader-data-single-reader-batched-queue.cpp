/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/shader-data-single-reader-batched-queue.h>

namespace Dali
{

namespace Internal
{


void ShaderDataSingleReaderBatchedQueue::Push( Integration::ShaderDataPtr& v )
{
  Mutex::ScopedLock lock( mMutex );
  mCurrWriteBuffer->push_back( v );
}

ShaderDataSingleReaderBatchedQueue::Container& ShaderDataSingleReaderBatchedQueue::GetReadBuffer()
{
  Mutex::ScopedLock lock( mMutex );
  Container* nextWriteBuffer = &mBuffer1;
  if( mCurrWriteBuffer == nextWriteBuffer )
  {
    nextWriteBuffer = &mBuffer2;
  }
  Container* nextReadBuffer = mCurrWriteBuffer;
  mCurrWriteBuffer = nextWriteBuffer;
  nextWriteBuffer->clear();
  return *nextReadBuffer;
}

} // namespace Internal

} // namespace Dali
