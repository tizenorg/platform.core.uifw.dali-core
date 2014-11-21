#ifndef __TEST_FILE_ABSTRACTION_H__
#define __TEST_FILE_ABSTRACTION_H__

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
#include <sstream>
#include <string>
#include <map>
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/file-abstraction.h>
#include "test-trace-call-stack.h"

namespace Dali
{

class DALI_IMPORT_API TestFileAbstraction : public Dali::Integration::File
{
protected:
  std::string	mName;
  char 		buffer[8];
  int64_t	mPosition;
  int64_t	mSize;
  
public:
  TestFileAbstraction(const std::string& name);
  ~TestFileAbstraction();
  
  virtual bool      IsOpen() const;
  virtual int       Seek(int64_t offset, int whence);
  virtual int       Read(void* buffer, int count, int size);

  virtual int64_t   Available() const;
  virtual int64_t   Size() const;

  virtual const std::string& GetName() const;
};
  
} // namespace Dali




#endif // __TEST_GL_ES_H__
