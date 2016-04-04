#ifndef TEST_UTILITIES_H
#define TEST_UTILITIES_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 */

namespace TestUtilities
{

typedef void (*functionPointer)();


struct TestCasePair
{
  functionPointer setup;
  functionPointer validate;
};

/**
 * @brief
 *
 * TestApp iterates through an array of pairs,
 * Each pair is a setup and validate function.
 *
 * The 'setup' function does something like set an actor's position then the 'validate' function confirms that the position is correct.
 * returning FAIL if not correct and the test has failed.
 *
 * Step 1:
 *
 * Create a setup test function and a matching validate function
 *
 * Step 2:
 *
 * Add the test functions to the Array of TestCasePairs, see below.
 *
 *  TestUtilities::TestCasePair testPairs[] = {
 *    {&TestFunctionSetup001, &VTestFunctionValidate001},
 *    {&TestFunctionSetup002, &VTestFunctionValidate002},
 *    {&TestFunctionSetupAndValidate003, &VTNoValidationRequired}
 *  };
 *
 *  VTestFunctionValidate001 ensures the code in TestFunctionSetup001 behaved correctly after a tick
 *  VTestFunctionValidate002 ensures the code in TestFunctionSetup002 behaved correctly after a tick
 *  TestFunctionSetupAndValidate003 setup and validated without waiting for a tick.
 *
 *  Step 3:
 *
 *  Calculate number of test steps - Use boiler plate code below (copy into your test code)
 *
 *  const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
 *
 *
 *  Step 4:
 *
 *  Set step interval ( in future may be able to set for each pair)
 *  This gives the engine under test time to render, load resources or signal before the validation is done.
 *
 *  const unsigned int STEP_INTERVAL = 500;
 *
 *  Step 5
 *
 *  Create and then start the Test app
 *
 *  TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
 *  application.MainLoop();
 *
 *  Step 6
 *
 *  Check result
 *
 */


struct TestApp : public ConnectionTracker
{
  TestApp( Application& app, TestCasePair* testPairsStruct, unsigned int numberOfTestSteps, unsigned int interval )
  : mApplication( app ),
    mTestCasePairs( testPairsStruct ),
    mNumberOfTestSteps( numberOfTestSteps ),
    mInterval( interval ),
    mTestStepCounter( 0 )
  {
    mApplication.InitSignal().Connect( this, &TestApp::OnInit );
  }

  void OnInit(Application& app)
  {
    // Execute first test from array of pairs
    mTestCasePairs[mTestStepCounter].setup();

    printf("TestApp Starting timer\n");
    mTimer = Timer::New( mInterval );
    mTimer.TickSignal().Connect( this, &TestApp::Tick );
    mTimer.Start();
  }

  bool Tick()
  {
    if(test_return_value==TC_FAIL)
    {
      mTimer.Stop();
      mApplication.Quit();
      return false;
    }

    // Execute first validation from of pairs
    mTestCasePairs[mTestStepCounter].validate();

    if(test_return_value==TC_FAIL)
    {
      mTimer.Stop();
      mApplication.Quit();
      return false;
    }

    mTestStepCounter++;
    if ( mTestStepCounter < mNumberOfTestSteps )
    {
      mTestCasePairs[mTestStepCounter].setup();
    }
    else
    {
      mTimer.Stop();
      mApplication.Quit();
      return false;
    }

    return true;
  }


  // Data
  Application& mApplication;
  TestCasePair* mTestCasePairs;
  unsigned int mNumberOfTestSteps;
  unsigned int mInterval;
  Timer mTimer;
  unsigned mTestStepCounter;

};
} // namespace TEST_UTILITIES_H

#endif
