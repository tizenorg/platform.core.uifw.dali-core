#ifndef __DALI_TEST_GESTURE_MANAGER_H__
#define __DALI_TEST_GESTURE_MANAGER_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/integration-api/gesture-manager.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

/**
 * Concrete implementation of the gesture manager class.
 */
class DALI_IMPORT_API TestGestureManager : public Dali::Integration::GestureManager
{

public:

  /**
   * Constructor
   */
  TestGestureManager()
  {
    Initialize();
  }

  /**
   * Destructor
   */
  virtual ~TestGestureManager()
  {
  }

  /**
   * @copydoc Dali::Integration::GestureManager::Register(Gesture::Type)
   */
  virtual void Register(const Integration::GestureRequest& request)
  {
    mFunctionsCalled.Register = true;
  }

  /**
   * @copydoc Dali::Integration::GestureManager::Unregister(Gesture::Type)
   */
  virtual void Unregister(const Integration::GestureRequest& request)
  {
    mFunctionsCalled.Unregister = true;
  }

  /**
   * @copydoc Dali::Integration::GestureManager::Update(Gesture::Type)
   */
  virtual void Update(const Integration::GestureRequest& request)
  {
    mFunctionsCalled.Update = true;
  }

public: // TEST FUNCTIONS

  // Enumeration of Gesture Manager methods
  enum TestFuncEnum
  {
    RegisterType,
    UnregisterType,
    UpdateType,
  };

  /** Call this every test */
  void Initialize()
  {
    mFunctionsCalled.Reset();
  }

  bool WasCalled(TestFuncEnum func)
  {
    switch(func)
    {
      case RegisterType:             return mFunctionsCalled.Register;
      case UnregisterType:           return mFunctionsCalled.Unregister;
      case UpdateType:               return mFunctionsCalled.Update;
    }
    return false;
  }

  void ResetCallStatistics(TestFuncEnum func)
  {
    switch(func)
    {
      case RegisterType:             mFunctionsCalled.Register = false; break;
      case UnregisterType:           mFunctionsCalled.Unregister = false; break;
      case UpdateType:               mFunctionsCalled.Update = false; break;
    }
  }

private:

  struct TestFunctions
  {
    TestFunctions()
    : Register(false),
      Unregister(false),
      Update(false)
    {
    }

    void Reset()
    {
      Register = false;
      Unregister = false;
      Update = false;
    }

    bool Register;
    bool Unregister;
    bool Update;
  };

  TestFunctions mFunctionsCalled;
};

} // Dali

#endif // __DALI_TEST_GESTURE_MANAGER_H__
