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

// CLASS HEADER
#include <dali/internal/event/events/tap-gesture-processor.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/math/vector2.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali/integration-api/gesture-manager.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/common/stage-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

/**
 * Creates a TapGesture and asks the specified detector to emit its detected signal.
 * @param[in]  actor             The actor on which a tap has occurred.
 * @param[in]  gestureDetectors  A reference to gesture detectors that should emit the signal.
 * @param[in]  tapEvent          The tapEvent received from the adaptor.
 * @param[in]  localPoint        Relative to the actor attached to the detector.
 */
void EmitTapSignal(
    Dali::Actor actor,
    TapGestureDetectorContainer& gestureDetectors,
    const Integration::TapGestureEvent& tapEvent,
    Vector2 localPoint)
{
  TapGesture tap;
  tap.time = tapEvent.time;
  tap.numberOfTaps = tapEvent.numberOfTaps;
  tap.numberOfTouches = tapEvent.numberOfTouches;
  tap.screenPoint = tapEvent.point;
  tap.localPoint = localPoint;

  for ( TapGestureDetectorContainer::iterator iter = gestureDetectors.begin(), endIter = gestureDetectors.end(); iter != endIter; ++iter )
  {
    (*iter)->EmitTapGestureSignal( actor, tap );
  }
}

} // unnamed namespace

struct TapGestureProcessor::TapEventFunctor : public GestureProcessor::Functor
{
  /**
   * Constructor
   * @param[in]  tapEvent   The current gesture event.
   */
  TapEventFunctor( const Integration::TapGestureEvent& tapEvent )
  : tapEvent(tapEvent)
  {
  }

  /**
   * Check if the detector meets the current gesture event parameters.
   */
   virtual bool operator() ( GestureDetector* detector, Actor* )
  {
    TapGestureDetector* tapDetector ( static_cast< TapGestureDetector* >( detector ) );

    return tapDetector->GetTapsRequired() == tapEvent.numberOfTaps &&
           tapDetector->GetTouchesRequired() == tapEvent.numberOfTouches;
  }

  /**
   * Gestured actor and gesture detectors that meet the gesture's parameters found, emit and save required information.
   */
  virtual void operator() ( Dali::Actor actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates )
  {
    TapGestureDetectorContainer derivedContainer;
    DownCastContainer<TapGestureDetector>( gestureDetectors, derivedContainer );
    EmitTapSignal( actor, derivedContainer, tapEvent, actorCoordinates );
  }

  const Integration::TapGestureEvent& tapEvent;
};

TapGestureProcessor::TapGestureProcessor( Stage& stage, Integration::GestureManager& gestureManager)
: mStage( stage ),
  mGestureManager( gestureManager ),
  mGestureDetectors(),
  mMinTapsRequired( 1 ),
  mMaxTapsRequired( 1 ),
  mMinTouchesRequired( 1 ),
  mMaxTouchesRequired( 1 )
{
}

TapGestureProcessor::~TapGestureProcessor()
{
}

void TapGestureProcessor::Process( const Integration::TapGestureEvent& tapEvent )
{
  switch ( tapEvent.state )
  {
    case Gesture::Possible:
    {
      ResetActor();

      HitTestAlgorithm::Results hitTestResults;
      if( HitTest( mStage, tapEvent.point, hitTestResults ) )
      {
        // Only sets the actor if we have a hit.
        SetActor( hitTestResults.actor );
      }
      break;
    }

    case Gesture::Started:
    {
      if ( mCurrentGesturedActor )
      {
        HitTestAlgorithm::Results hitTestResults;
        HitTestAlgorithm::HitTest( mStage, tapEvent.point, hitTestResults );

        if ( hitTestResults.actor && ( mCurrentGesturedActor == &GetImplementation( hitTestResults.actor ) ) )
        {
          TapEventFunctor functor( tapEvent );
          GestureDetectorContainer gestureDetectors;
          UpCastContainer<TapGestureDetector>( mGestureDetectors, gestureDetectors );
          ProcessAndEmit( hitTestResults, gestureDetectors, functor );
        }

        ResetActor();
      }
      break;
    }

    case Gesture::Cancelled:
      ResetActor();
      break;

    case Gesture::Continuing:
      DALI_ASSERT_ALWAYS( false && "Incorrect state received from Integration layer: Continuing\n" );
      break;

    case Gesture::Finished:
      DALI_ASSERT_ALWAYS( false && "Incorrect state received from Integration layer: Finished\n" );
      break;

    case Gesture::Clear:
      DALI_ASSERT_ALWAYS( false && "Incorrect state received from Integration layer: Clear\n" );
      break;
  }
}

void TapGestureProcessor::AddGestureDetector( TapGestureDetector* gestureDetector )
{
  bool firstRegistration(mGestureDetectors.empty());

  mGestureDetectors.push_back(gestureDetector);

  unsigned int tapsRequired = gestureDetector->GetTapsRequired();
  unsigned int touchesRequired = gestureDetector->GetTouchesRequired();

  if (firstRegistration)
  {
    // If this is the first tap gesture detector that has been added, then our minimum and maximum
    // requirements are the same as each other.

    mMinTapsRequired = mMaxTapsRequired = tapsRequired;
    mMinTouchesRequired = mMaxTouchesRequired = touchesRequired;

    Integration::TapGestureRequest request;
    request.minTaps = mMinTapsRequired;
    request.maxTaps = mMaxTapsRequired;
    request.minTouches = mMinTouchesRequired;
    request.maxTouches = mMaxTouchesRequired;
    mGestureManager.Register(request);
  }
  else
  {
    // If we have already registered for tap gesture detection before then we need to check our
    // minimum and maximums and see if our gesture detection requirements have changed, if they
    // have, then we should ask the adaptor to update its detection policy.

    unsigned int minTaps = mMinTapsRequired < tapsRequired ? mMinTapsRequired : tapsRequired;
    unsigned int maxTaps = mMaxTapsRequired > tapsRequired ? mMaxTapsRequired : tapsRequired;
    unsigned int minTouches = mMinTouchesRequired < touchesRequired ? mMinTouchesRequired : touchesRequired;
    unsigned int maxTouches = mMaxTouchesRequired > touchesRequired ? mMaxTouchesRequired : touchesRequired;

    if ( (minTaps != mMinTapsRequired)||(maxTaps != mMaxTapsRequired) ||
         (minTouches != mMinTouchesRequired)||(maxTouches != mMaxTouchesRequired) )
    {
      Integration::TapGestureRequest request;
      request.minTaps = mMinTapsRequired = minTaps;
      request.maxTaps = mMaxTapsRequired = maxTaps;
      request.minTouches = mMinTouchesRequired = minTouches;
      request.maxTouches = mMaxTouchesRequired = maxTouches;
      mGestureManager.Update(request);
    }
  }
}

void TapGestureProcessor::RemoveGestureDetector( TapGestureDetector* gestureDetector )
{
  // Find detector ...
  TapGestureDetectorContainer::iterator endIter = std::remove( mGestureDetectors.begin(), mGestureDetectors.end(), gestureDetector );
  DALI_ASSERT_DEBUG( endIter != mGestureDetectors.end() );

  // ... and remove it
  mGestureDetectors.erase( endIter, mGestureDetectors.end() );

  if ( mGestureDetectors.empty() )
  {
    Integration::GestureRequest request(Gesture::Tap);
    mGestureManager.Unregister(request);

    ResetActor();
  }
  else
  {
    UpdateDetection();
  }
}

void TapGestureProcessor::GestureDetectorUpdated( TapGestureDetector* gestureDetector )
{
  DALI_ASSERT_DEBUG(find(mGestureDetectors.begin(), mGestureDetectors.end(), gestureDetector) != mGestureDetectors.end());

  UpdateDetection();
}

void TapGestureProcessor::UpdateDetection()
{
  DALI_ASSERT_DEBUG(!mGestureDetectors.empty());

  unsigned int minTaps = UINT_MAX;
  unsigned int maxTaps = 0;
  unsigned int minTouches = UINT_MAX;
  unsigned int maxTouches = 0;

  for ( TapGestureDetectorContainer::iterator iter = mGestureDetectors.begin(), endIter = mGestureDetectors.end(); iter != endIter; ++iter )
  {
    TapGestureDetector* detector(*iter);

    unsigned int tapsRequired = detector->GetTapsRequired();
    unsigned int touchesRequired = detector->GetTouchesRequired();

    minTaps = tapsRequired < minTaps ? tapsRequired : minTaps;
    maxTaps = tapsRequired > maxTaps ? tapsRequired : maxTaps;
    minTouches = touchesRequired < minTouches ? touchesRequired : minTouches;
    maxTouches = touchesRequired > maxTouches ? touchesRequired : maxTouches;
  }

  if ( (minTaps != mMinTapsRequired)||(maxTaps != mMaxTapsRequired) ||
       (minTouches != mMinTouchesRequired)||(maxTouches != mMaxTouchesRequired) )
  {
    Integration::TapGestureRequest request;
    request.minTaps = mMinTapsRequired = minTaps;
    request.maxTaps = mMaxTapsRequired = maxTaps;
    request.minTouches = mMinTouchesRequired = minTouches;
    request.maxTouches = mMaxTouchesRequired = maxTouches;
    mGestureManager.Update(request);
  }
}

} // namespace Internal

} // namespace Dali
