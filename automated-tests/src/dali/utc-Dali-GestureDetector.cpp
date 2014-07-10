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

#include <iostream>

#include <stdlib.h>
#include <algorithm>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace std;

void utc_dali_gesture_detector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gesture_detector_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliGestureDetectorConstructorNegative(void)
{
  TestApplication application;

  GestureDetector detector;

  Actor actor = Actor::New();

  try
  {
    detector.Attach(actor);
    tet_result(TET_FAIL);
  }
  catch (DaliException& exception)
  {
    if ( exception.mCondition.find("detector") != string::npos )
    {
      tet_result(TET_PASS);
    }
  }
  END_TEST;
}

int UtcDaliGestureDetectorConstructorPositive(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();

  try
  {
    detector.Attach(actor);
    tet_result(TET_PASS);
  }
  catch (DaliException& exception)
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::GestureDetector::DownCast()");

  GestureDetector detector = PanGestureDetector::New();

  BaseHandle object(detector);

  GestureDetector detector2 = GestureDetector::DownCast(object);
  DALI_TEST_CHECK(detector2);

  GestureDetector detector3 = DownCast< GestureDetector >(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle unInitializedObject;
  GestureDetector detector4 = GestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  GestureDetector detector5 = DownCast< GestureDetector >(unInitializedObject);
  DALI_TEST_CHECK(!detector5);
  END_TEST;
}

int UtcDaliGestureDetectorAttachPositive(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();

  detector.Attach(actor);

  vector<Actor> actors = detector.GetAttachedActors();

  if (find(actors.begin(), actors.end(), actor) != actors.end())
  {
    tet_result(TET_PASS);
  }
  else
  {
    tet_result(TET_FAIL);
  }

  // Scoped gesture detector. GestureDetectors connect to a destroy signal of the actor. If the
  // actor is still alive when the gesture detector is destroyed, then it should disconnect from
  // this signal.  If it does not, then when this function ends, there will be a segmentation fault
  // thus, a TET case failure.
  {
    GestureDetector detector2 = PanGestureDetector::New();
    detector2.Attach(actor);
  }
  END_TEST;
}

int UtcDaliGestureDetectorAttachNegative(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  // Do not initialise actor
  Actor actor;

  try
  {
    detector.Attach(actor);
    tet_result(TET_FAIL);
  }
  catch (DaliException& exception)
  {
    if ( exception.mCondition.find("actor") != string::npos )
    {
      tet_result(TET_PASS);
    }
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachPositive(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();
  detector.Attach(actor);
  vector<Actor> actors = detector.GetAttachedActors();

  if (find(actors.begin(), actors.end(), actor) != actors.end())
  {
    tet_result(TET_PASS);
  }
  else
  {
    tet_result(TET_FAIL);
  }

  // Detach and retrive attached actors again, the vector should be empty.
  detector.Detach(actor);

  actors = detector.GetAttachedActors();
  if (actors.empty())
  {
    tet_result(TET_PASS);
  }
  else
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachNegative01(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  // Do not initialise actor
  Actor actor;

  try
  {
    detector.Detach(actor);
    tet_result(TET_FAIL);
  }
  catch (DaliException& exception)
  {
    if ( exception.mCondition.find("actor") != string::npos )
    {
      tet_result(TET_PASS);
    }
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachNegative02(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();
  detector.Attach(actor);

  // Detach an actor that hasn't been attached.  This should not cause an exception, it's a no-op.
  try
  {
    Actor actor2 = Actor::New();
    detector.Detach(actor2);
    tet_result(TET_PASS);
  }
  catch (DaliException& exception)
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachNegative03(void)
{
  TestApplication application;
  TestGestureManager& gestureManager = application.GetGestureManager();

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  Actor actor = Actor::New();
  detector.Attach(actor);

  // Detach an actor twice - no exception should happen.
  try
  {
    detector.Detach(actor);
    DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

    gestureManager.Initialize(); // Reset values
    detector.Detach(actor);
    DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
  }
  catch (DaliException& exception)
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorDetachAll(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  const unsigned int actorsToAdd = 5;
  vector<Actor> myActors;

  for (unsigned int i = 0; i < actorsToAdd; ++i)
  {
    Actor actor = Actor::New();
    myActors.push_back(actor);
    detector.Attach(actor);
  }

  vector<Actor> attachedActors = detector.GetAttachedActors();
  DALI_TEST_EQUALS(actorsToAdd, attachedActors.size(), TEST_LOCATION);

  // Detach and retrieve attached actors again, the vector should be empty.
  detector.DetachAll();

  attachedActors = detector.GetAttachedActors();
  DALI_TEST_EQUALS(true, attachedActors.empty(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGestureDetectorDetachAllNegative(void)
{
  TestApplication application;
  TestGestureManager& gestureManager = application.GetGestureManager();

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  const unsigned int actorsToAdd = 5;
  vector<Actor> myActors;

  for (unsigned int i = 0; i < actorsToAdd; ++i)
  {
    Actor actor = Actor::New();
    myActors.push_back(actor);
    detector.Attach(actor);
  }

  vector<Actor> attachedActors = detector.GetAttachedActors();
  DALI_TEST_EQUALS(actorsToAdd, attachedActors.size(), TEST_LOCATION);

  // Detach and retrieve attached actors again, the vector should be empty.
  detector.DetachAll();

  attachedActors = detector.GetAttachedActors();
  DALI_TEST_EQUALS(true, attachedActors.empty(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

  // Call DetachAll again, there should not be any exception
  try
  {
    gestureManager.Initialize(); // Reset values
    detector.DetachAll();
    DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
  }
  catch (DaliException& exception)
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliGestureDetectorGetAttachedActors(void)
{
  TestApplication application;

  // Use pan gesture as GestureDetector cannot be created.
  GestureDetector detector = PanGestureDetector::New();

  // Initially there should not be any actors.
  DALI_TEST_EQUALS(0u, detector.GetAttachedActors().size(), TEST_LOCATION);

  // Attach one actor
  Actor actor1 = Actor::New();
  detector.Attach(actor1);
  DALI_TEST_EQUALS(1u, detector.GetAttachedActors().size(), TEST_LOCATION);

  // Attach another actor
  Actor actor2 = Actor::New();
  detector.Attach(actor2);
  DALI_TEST_EQUALS(2u, detector.GetAttachedActors().size(), TEST_LOCATION);

  // Attach another five actors
  vector<Actor> myActors;
  for (unsigned int i = 0; i < 5; ++i)
  {
    Actor actor = Actor::New();
    myActors.push_back(actor);
    detector.Attach(actor);
  }
  DALI_TEST_EQUALS(7u, detector.GetAttachedActors().size(), TEST_LOCATION);

  // Detach actor2
  detector.Detach(actor2);
  DALI_TEST_EQUALS(6u, detector.GetAttachedActors().size(), TEST_LOCATION);

  // Attach actor1 again, count should not increase.
  detector.Attach(actor1);
  DALI_TEST_EQUALS(6u, detector.GetAttachedActors().size(), TEST_LOCATION);

  // Detach actor2 again, count should not decrease.
  detector.Detach(actor2);
  DALI_TEST_EQUALS(6u, detector.GetAttachedActors().size(), TEST_LOCATION);

  // Detach actor1.
  detector.Detach(actor1);
  DALI_TEST_EQUALS(5u, detector.GetAttachedActors().size(), TEST_LOCATION);

  // Create scoped actor, actor should be automatically removed from the detector when it goes out
  // of scope.
  {
    Actor scopedActor = Actor::New();
    detector.Attach(scopedActor);
    DALI_TEST_EQUALS(6u, detector.GetAttachedActors().size(), TEST_LOCATION);
  }
  DALI_TEST_EQUALS(5u, detector.GetAttachedActors().size(), TEST_LOCATION);

  // Detach all so nothing remains.
  detector.DetachAll();
  DALI_TEST_EQUALS(0u, detector.GetAttachedActors().size(), TEST_LOCATION);
  END_TEST;
}
