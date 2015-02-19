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

/**
 * The top level DALi namespace
 */
namespace Dali
{

/**
 * Class to synchronize access to critical resources from multiple threads
 */
class Mutex
{
public:

  /**
   * Constructor, acquires the mutex from the underlying OS
   */
  Mutex();

  /**
   * Destructor, non virtual as this is not meant as a base class
   */
  ~Mutex();

  /**
   * Lock the mutex
   */
  void Lock();

  /**
   * Unlock the mutex
   */
  void Unlock();

  /**
   * Helper class to do a scoped lock on a mutex implementing the RAII idiom.
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

private:
  struct MutexImpl;
  MutexImpl* mImpl;

};

} // namespace Dali

#endif // __DALI_MUTEX_H__
