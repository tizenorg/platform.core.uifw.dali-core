#ifndef __DALI_MUTEX_H__
#define __DALI_MUTEX_H__

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

/**
 * The top level DALi namespace
 */
namespace Dali
{

/**
 * Class to synchronize access to critical resources from multiple threads
 */
class DALI_IMPORT_API Mutex
{
public:

  /**
   * @brief Constructor, acquires the mutex from the underlying OS
   */
  Mutex();

  /**
   * @brief Destructor, non virtual as this is not meant as a base class
   */
  ~Mutex();

  /**
   * @brief Check if the mutex is locked
   * @return true if the mutex is locked
   */
  bool IsLocked();

public:

  /**
   * Helper class to do a scoped lock on a mutex implementing the RAII idiom.
   * Note! this class *does not* prevent a deadlock in the case when same thread is
   * locking the same mutex twice.
   */
  class ScopedLock
  {
  public:

    /**
     * Constructor
     * @param mutex to lock
     */
    ScopedLock( Mutex& mutex )
    : mMutex( mutex )
    {
      mMutex.Lock();
    }

    /**
     * Destructor, releases the lock
     */
    ~ScopedLock()
    {
      mMutex.Unlock();
    }

  private:
    Mutex& mMutex;
  };

protected: // Not intended for direct use, use ScopedLock instead

  /**
   * @brief Lock the mutex
   * Note! calling Lock more than once from the same thread leads to a deadlock.
   */
  void Lock();

  /**
   * @brief Unlock the mutex
   * Note! calling Unlock from a thread that did not lock the mutex leads to undefined behaviour.
   */
  void Unlock();

private:
  struct MutexImpl;
  MutexImpl* mImpl;

};

} // namespace Dali

#endif // __DALI_MUTEX_H__
