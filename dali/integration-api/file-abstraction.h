#ifndef __DALI_FILESYSTEM_H__
#define __DALI_FILESYSTEM_H__

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

#include <stddef.h>        // for size_t
#include <stdint.h>
#include <vector>
#include <string>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Integration
{

  // Its an instantiable class, so it can have a state.
  // Use case : android, resource-connector.
  class File;

  class FileSystemBase
  {
  public:
    virtual size_t  GetFilesize(const std::string& filename) = 0;
    virtual bool    Exists(const std::string& filename) = 0;

    // return = errorcode, 0 ok, else ... whatever.
    virtual File*   OpenFile(const std::string& name) = 0;
    virtual File*   MapToMemory(const uint8_t* data, size_t size) = 0;

    virtual ~FileSystemBase()
    {
    }
  };

  class File
  {
  public:
    virtual bool      IsOpen() const = 0;
    virtual int       Seek(int64_t offset, int whence) = 0;
    virtual int       Read(void* buffer, int count, int size) = 0;

    virtual int64_t   Available() const = 0;
    virtual int64_t   Size() const = 0;

    virtual const std::string& GetName() const = 0;

    virtual ~File()
    {      
    }
  };


  class FileDataHelper
  {
    Dali::Integration::File* mHandle;

  public:

    FileDataHelper(Dali::Integration::File* handle) : mHandle(handle)
    {
    }

    Dali::Integration::File* Get()
    {
      return mHandle;
    }

    operator int()
    {
      return mHandle != NULL;
    }

    operator File*()
    {
      return mHandle;
    }

    File* operator-> ()
    {
      return mHandle;
    }

    ~FileDataHelper()
    {
      delete mHandle;
      mHandle = NULL;
    }
  };  

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_LOADER_FONT_H__
