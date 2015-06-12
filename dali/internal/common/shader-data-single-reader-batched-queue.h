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
#include <dali/integration-api/shader-data.h>

namespace Dali
{

namespace Internal
{

/**
 * Class to allow pieces of data to be passed from one or more threads to
 * another and then retrieved as a batch by the destination thread.
 */

class ShaderDataSingleReaderBatchedQueue
{
public:
  typedef std::vector<Integration::ShaderDataPtr> Container;
  typedef typename Container::value_type ValueType;
  typedef typename Container::iterator Iterator;

  /**
   * @brief Default constructor,
   */
  ShaderDataSingleReaderBatchedQueue() : mCurrWriteBuffer( &mBuffer1 ) {}

  /**
   * @brief Destructor does nothing,
   */
  ~ShaderDataSingleReaderBatchedQueue() {}

  /**
   * @brief Pass a piece of data from multiple threads.
   * @param[in] v Piece of data to pass.
   */
  void Push( Integration::ShaderDataPtr& v );

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

  /// Not implemented as ShaderDataSingleReaderBatchedQueue is not copyable
  ShaderDataSingleReaderBatchedQueue( const ShaderDataSingleReaderBatchedQueue& );
  const ShaderDataSingleReaderBatchedQueue& operator= ( const ShaderDataSingleReaderBatchedQueue& );

  Mutex          mMutex;
  Container  mBuffer1;
  Container  mBuffer2;
  Container* mCurrWriteBuffer;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_MULTI_WRITER_SINGLE_READER_BUFFER_H__
