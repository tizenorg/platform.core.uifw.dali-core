#ifndef __DALI_THREAD_H__
#define __DALI_THREAD_H__

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
#include <dali/public-api/common/dali-common.h>

// EXTERNAL INCLUDES
#include <cstddef>

/**
 * The top level DALi namespace
 */
namespace Dali
{

/*
 * Abstract class of pthread wrapper
 */
class DALI_IMPORT_API Thread
{
public:

  /**
   * Type of the thread states.
   */
  enum State
  {
    STATE_NONE,
    STATE_STARTED,
    STATE_JOINED
  };

public:

  /**
   * @brief Constructor
   */
  Thread();

  /**
   * @brief Destructor, virtual as this is used as base class
   */
  virtual ~Thread();

  /**
   * @brief Creates a new thread and make it executable.
   */
  void Start();

  /**
   * @brief Wait for thread termination.
   */
  void Join();

  /**
   * @brief Query the current state of the thread.
   *
   * @return The state of the thread.
   */
  State GetCurrentState() const;

protected:

  /**
   * The routine that the thread will execute once it is created.
   */
  virtual void Run() = 0;

private:

  /**
   * Helper for the thread calling the entry function.
   * @param[in] This A pointer to the current RenderThread object
   */
  static inline void* InternalThreadEntryFunc( void* This )
  {
    ( static_cast<Thread*>( This ) )->Run();
    return NULL;
  }

  // Undefined
  Thread( const Thread& );
  // Undefined
  const Thread& operator=( const Thread& );

private:

  struct ThreadImpl;
  ThreadImpl* mImpl;
};

}

#endif /* __DALI_THREAD_H__ */
