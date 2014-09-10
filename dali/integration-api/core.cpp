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
#include <dali/integration-api/core.h>

// EXTERNAL INCLUDES
#include <iostream>
#include <stdarg.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/events/event.h>
#include <dali/integration-api/gl-sync-abstraction.h>
#include <dali/internal/common/core-impl.h>

namespace Dali
{

namespace Integration
{

namespace KeepUpdating
{

const unsigned int NOT_REQUESTED = 0x00; ///< Zero means that no further updates are required

// Bit-field values
const unsigned int STAGE_KEEP_RENDERING   = 0x01;  ///< Stage::KeepRendering() is being used
const unsigned int INCOMING_MESSAGES      = 0x02;  ///< Event-thread is sending messages to update-thread
const unsigned int ANIMATIONS_RUNNING     = 0x04;  ///< Animations are ongoing
const unsigned int DYNAMICS_CHANGED       = 0x08;  ///< A dynamics simulation is running
const unsigned int LOADING_RESOURCES      = 0x10;  ///< Resources are being loaded
const unsigned int NOTIFICATIONS_PENDING  = 0x20;  ///< Notifications are pending for the event-thread
const unsigned int MONITORING_PERFORMANCE = 0x40;  ///< The --enable-performance-monitor option is being used
const unsigned int RENDER_TASK_SYNC       = 0x80;  ///< The refresh once render task is waiting for render sync

} // namespace KeepUpdating

Core* Core::New(RenderController& renderController, PlatformAbstraction& platformAbstraction,
                GlAbstraction& glAbstraction, GlSyncAbstraction& glSyncAbstraction, GestureManager& gestureManager, ResourcePolicy::DataRetention policy )
{
  Core* instance = new Core;
  instance->mImpl = new Internal::Core( renderController, platformAbstraction, glAbstraction, glSyncAbstraction, gestureManager, policy );

  return instance;
}

Core::~Core()
{
  delete mImpl;
}

ContextNotifierInterface* Core::GetContextNotifier()
{
  return mImpl->GetContextNotifier();
}

// @todo Rename to ResetGlContext
void Core::ContextCreated()
{
  mImpl->ContextCreated();
}

// @todo Replace with StopRendering that prevents RenderManager from rendering
// until we get ResetGLContext again, change ContextCreated to reset gpu buffer cache,
// gl texture id's
void Core::ContextDestroyed()
{
  mImpl->ContextDestroyed();
}

void Core::RecoverFromContextLoss()
{
  mImpl->RecoverFromContextLoss();
}

void Core::SurfaceResized(unsigned int width, unsigned int height)
{
  mImpl->SurfaceResized(width, height);
}

void Core::SetDpi(unsigned int dpiHorizontal, unsigned int dpiVertical)
{
  mImpl->SetDpi(dpiHorizontal, dpiVertical);
}

void Core::Suspend()
{
  mImpl->Suspend();
}

void Core::Resume()
{
  mImpl->Resume();
}

void Core::QueueEvent(const Event& event)
{
  mImpl->QueueEvent(event);
}

void Core::ProcessEvents()
{
  mImpl->ProcessEvents();
}

void Core::UpdateTouchData(const TouchData& touch)
{
  mImpl->UpdateTouchData(touch);
}

unsigned int Core::GetMaximumUpdateCount() const
{
  return mImpl->GetMaximumUpdateCount();
}

void Core::Update( float elapsedSeconds, unsigned int lastVSyncTimeMilliseconds, unsigned int nextVSyncTimeMilliseconds, UpdateStatus& status )
{
  mImpl->Update( elapsedSeconds, lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds, status );
}

void Core::Render( RenderStatus& status )
{
  mImpl->Render( status );
}

SystemOverlay& Core::GetSystemOverlay()
{
  return mImpl->GetSystemOverlay();
}

void Core::SetViewMode( ViewMode viewMode )
{
  mImpl->SetViewMode( viewMode );
}

ViewMode Core::GetViewMode() const
{
  return mImpl->GetViewMode();
}

void Core::SetStereoBase( float stereoBase )
{
  mImpl->SetStereoBase( stereoBase );
}

float Core::GetStereoBase() const
{
  return mImpl->GetStereoBase();
}

Core::Core()
: mImpl( NULL )
{
}

} // namespace Integration

} // namespace Dali
