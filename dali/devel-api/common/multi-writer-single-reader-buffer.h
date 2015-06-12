#ifndef __DALI_MULTI_WRITER_SINGLE_READER_BUFFER_H__
#define __DALI_MULTI_WRITER_SINGLE_READER_BUFFER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/devel-api/common/mutex.h>

namespace Dali
{

/**
 * Class to allow pieces of data to be passed from one or more threads to
 * another and then retrieved as a batch by the destination thread.
 */
template <typename T>
class DALI_IMPORT_API MultiWriterSingleReaderBuffer
{
public:
  typedef std::vector<T> Container;
  typedef typename Container::value_type ValueType;
  typedef typename Container::iterator Iterator;

  /**
   * @brief Default constructor,
   */
  MultiWriterSingleReaderBuffer() : mCurrWriteBuffer( &mBuffer1 ) {}

  /**
   * @brief Destructor does nothing,
   */
  ~MultiWriterSingleReaderBuffer() {}

  /**
   * @brief Pass a piece of data from multiple threads.
   * @param[in] v Piece of data to pass.
   */
  void Push( ValueType& v );

  /**
   * @brief Retrieve a batch of data on destination thread.
   * Once called, the entire batch of work returned must be processed before this
   * function is called again or the container previously returned may be
   * overwritten and the refilled in a non thread-safe way.
   * If there is no data available, the returned container will be empty.
   * @note This should only be called from a single thread.
   * @return A container of data items to be processed.
   */
  Container& GetReadBuffer();

private:

  /// Not implemented as MultiWriterSingleReaderBuffer is not copyable
  MultiWriterSingleReaderBuffer( const MultiWriterSingleReaderBuffer& );
  const MultiWriterSingleReaderBuffer& operator= ( const MultiWriterSingleReaderBuffer& );

  Mutex          mMutex;
  Container  mBuffer1;
  Container  mBuffer2;
  Container* mCurrWriteBuffer;
};


template <typename T>
inline void MultiWriterSingleReaderBuffer<T>::Push( MultiWriterSingleReaderBuffer::ValueType& v )
{
  Mutex::ScopedLock lock( mMutex );
  mCurrWriteBuffer->push_back( v );
}

template <typename T>
inline typename MultiWriterSingleReaderBuffer<T>::Container& MultiWriterSingleReaderBuffer<T>::GetReadBuffer()
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

} // namespace Dali

#endif // __DALI_MULTI_WRITER_SINGLE_READER_BUFFER_H__
