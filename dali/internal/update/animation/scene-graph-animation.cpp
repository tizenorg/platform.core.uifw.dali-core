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

// CLASS HEADER
#include <dali/internal/update/animation/scene-graph-animation.h>

// EXTERNAL INCLUDES
#include <cmath> // fmod

// INTERNAL INCLUDES
#include <dali/internal/render/common/performance-monitor.h>

using namespace std;

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

float DefaultAlphaFunc(float progress)
{
  return progress; // linear
}

Animation::Animation(float durationSeconds, bool isLooping, PlayDirection direction, Dali::Animation::EndAction endAction, Dali::Animation::EndAction destroyAction)
: mDurationSeconds(durationSeconds),
  mLooping(isLooping),
  mPlayDirection(direction),
  mEndAction(endAction),
  mDestroyAction(destroyAction),
  mState(Stopped),
  mElapsedSeconds(0.0f),
  mPlayCount(0),
  mPlayRange( 0.0f, durationSeconds )
{
}

Animation::~Animation()
{
}

void Animation::SetDuration(float durationSeconds)
{
  DALI_ASSERT_DEBUG(durationSeconds > 0.0f);

  //Update mPlayRange
  mPlayRange = ( mPlayRange / mDurationSeconds ) * durationSeconds;
  mDurationSeconds = durationSeconds;
}

void Animation::SetLooping(bool looping)
{
  mLooping = looping;
}

void Animation::SetPlayDirection(PlayDirection direction)
{
  if( direction != mPlayDirection )
  {
    if( mElapsedSeconds > 0.0f )
    {
      //If the animation has done any progress, recalculate mElapsedSeconds.
      mElapsedSeconds = mPlayRange.y - mPlayRange.x - mElapsedSeconds;
    }

    mPlayDirection = direction;
  }
}

void Animation::SetEndAction(Dali::Animation::EndAction action)
{
  mEndAction = action;
}

void Animation::SetDestroyAction(Dali::Animation::EndAction action)
{
  mDestroyAction = action;
}

void Animation::SetPlayRange( const Vector2& range )
{
  mPlayRange = range * mDurationSeconds;
  mElapsedSeconds = mPlayRange.x;
}

void Animation::Play()
{
  mState = Playing;
}

void Animation::PlayFrom( float progress )
{
  //If the animation is already playing this has no effect
  if( mState != Playing )
  {
    mElapsedSeconds = progress * mDurationSeconds;
    mState = Playing;
  }
}

void Animation::Pause()
{
  if (mState == Playing)
  {
    mState = Paused;
  }
}

bool Animation::Stop(BufferIndex bufferIndex)
{
  bool animationFinished(false);

  if (mState == Playing || mState == Paused)
  {
    animationFinished = true; // The actor-thread should be notified of this

    if( mEndAction != Dali::Animation::Discard )
    {
      if( mEndAction == Dali::Animation::BakeFinal )
      {
        mElapsedSeconds = mDurationSeconds + Math::MACHINE_EPSILON_1; // Force animation to reach it's end
      }

      UpdateAnimators(bufferIndex, true/*bake the final result*/);
    }

    // The animation has now been played to completion
    ++mPlayCount;
  }

  mElapsedSeconds = mPlayRange.x;
  mState = Stopped;

  return animationFinished;
}

void Animation::OnDestroy(BufferIndex bufferIndex)
{
  if (mState == Playing || mState == Paused)
  {
    if (mDestroyAction != Dali::Animation::Discard)
    {
      UpdateAnimators(bufferIndex, true/*bake the final result*/ );
    }
  }

  mState = Destroyed;
}

void Animation::AddAnimator( AnimatorBase* animator, PropertyOwner* propertyOwner )
{
  animator->Attach( propertyOwner );

  mAnimators.PushBack( animator );
}

bool Animation::Update(BufferIndex bufferIndex, float elapsedSeconds)
{
  if (mState == Stopped || mState == Destroyed)
  {
    // Short circuit when animation isn't running
    return false;
  }

  // The animation must still be applied when Paused/Stopping
  if (mState == Playing)
  {
    mElapsedSeconds += elapsedSeconds;
  }

  if (mLooping)
  {
    if (mElapsedSeconds > mPlayRange.y )
    {
      mElapsedSeconds = fmod(mElapsedSeconds, mPlayRange.y) + mPlayRange.x;
    }
  }

  const bool animationFinished(mState == Playing && mElapsedSeconds > mPlayRange.y );

  UpdateAnimators(bufferIndex, animationFinished && (mEndAction != Dali::Animation::Discard) );

  if (animationFinished)
  {
    // The animation has now been played to completion
    ++mPlayCount;

    mElapsedSeconds = mPlayRange.x;
    mState = Stopped;
  }

  return animationFinished;
}

void Animation::UpdateAnimators(BufferIndex bufferIndex, bool bake )
{
  float elapsedSeconds = min( mElapsedSeconds, mPlayRange.y );
  for ( AnimatorIter iter = mAnimators.Begin(); iter != mAnimators.End(); )
  {
    // If an animator is not successfully applied, then it has been orphaned
    bool applied(true);

    AnimatorBase *animator = *iter;
    const float initialDelay(animator->GetInitialDelay());

    if( mPlayDirection == Dali::Animation::PlayDirectionForward )
    {
      if (elapsedSeconds >= initialDelay)
      {
        // Calculate a progress specific to each individual animator
        float progress(1.0f);
        const float animatorDuration = animator->GetDuration();
        if (animatorDuration > 0.0f) // animators can be "immediate"
        {
          progress = min(1.0f, (elapsedSeconds - initialDelay) / animatorDuration);
        }

        applied = animator->Update(bufferIndex, progress, bake);
      }
    }
    else  //Play backward
    {
      //Calculate elapsed time from the end
      float elapsedInverse( mPlayRange.y - mPlayRange.x - elapsedSeconds );

      const float animatorDuration = animator->GetDuration();
      if( elapsedInverse <= animatorDuration + initialDelay )
      {
        float progress(0.0f);
        if (animatorDuration > 0.0f ) // animators can be "immediate"
        {
          progress = max(0.0f, (elapsedInverse - initialDelay) / animatorDuration );
        }

        applied = animator->Update(bufferIndex, progress, bake);
      }
    }
    // Animators are automatically removed, when orphaned from animatable scene objects.
    if (!applied)
    {
      iter = mAnimators.Erase(iter);
    }
    else
    {
      ++iter;

      INCREASE_COUNTER(PerformanceMonitor::ANIMATORS_APPLIED);
    }
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
