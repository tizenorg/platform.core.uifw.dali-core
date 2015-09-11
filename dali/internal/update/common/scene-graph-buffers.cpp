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

// CLASS HEADER
#include <dali/internal/update/common/scene-graph-buffers.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

BufferIndex SceneGraphBuffers::INITIAL_EVENT_BUFFER_INDEX  = 0u;
BufferIndex SceneGraphBuffers::INITIAL_UPDATE_BUFFER_INDEX = 1u;

SceneGraphBuffers::SceneGraphBuffers()
: mEventBufferIndex(INITIAL_EVENT_BUFFER_INDEX),
  mUpdateBufferIndex(INITIAL_UPDATE_BUFFER_INDEX)
{
	for ( unsigned int i = 0; i < MAX_ACCESSING; ++ i )
	{
		mMessageBuffers[ i ] = NULL;
	}
}

SceneGraphBuffers::~SceneGraphBuffers()
{
}

void SceneGraphBuffers::Swap()
{
  mUpdateBufferIndex = __sync_fetch_and_xor( &mEventBufferIndex, 1 );
}

#if defined ( __aarch64__ ) || defined ( __arm__ )
void SceneGraphBuffers::StartedAccess( Accessing accessing, MessageBuffer* buffer, unsigned int microseconds )
{
  mMessageBuffers[ accessing ] = buffer;
  mStartTimes[ accessing ] = microseconds;
}

void SceneGraphBuffers::EndedAccess( Accessing accessing, unsigned int microseconds )
{
  mMessageBuffers[ accessing ] = NULL;
  mLastTimes[ accessing ] = microseconds - mStartTimes[ accessing ];
}
#else

void SceneGraphBuffers::StartedAccess( Accessing accessing, MessageBuffer* buffer, unsigned int microseconds )
{
  MessageBuffer* messageBuffer = __sync_fetch_and_or( &mMessageBuffers[ accessing ], buffer );
  (void) messageBuffer;
  mStartTimes[ accessing ] = microseconds;
}

void SceneGraphBuffers::EndedAccess( Accessing accessing, unsigned int microseconds )
{
  MessageBuffer* messageBuffer = __sync_fetch_and_and( &mMessageBuffers[ accessing ], 0 );
  (void) messageBuffer;
  mLastTimes[ accessing ] = microseconds - mStartTimes[ accessing ];
}
#endif

void SceneGraphBuffers::SanityCheck()
{

#if defined ( __aarch64__ ) || defined ( __arm__ )
  MessageBuffer* updateBuffer = mMessageBuffers[ UPDATE ];
  MessageBuffer* renderBuffer = mMessageBuffers[ RENDER ];
#else
	MessageBuffer* updateBuffer = __sync_fetch_and_or( &mMessageBuffers[ UPDATE ], 0 );
	MessageBuffer* renderBuffer = __sync_fetch_and_or( &mMessageBuffers[ RENDER ], 0 );
#endif

	if ( ( updateBuffer && renderBuffer ) && ( updateBuffer == renderBuffer ) )
	{
    // If we get here then the two threads were trying to access the same buffer.
	  DALI_LOG_ERROR( "MessageBuffer conflict between Update and Render threads - Time in Update: %i ms, Time in Render: %i ms\n",
                    mLastTimes[ UPDATE ] / 1000u, mLastTimes[ RENDER ] / 1000u );

    DALI_ASSERT_DEBUG( false && "MessageBuffer conflict between Update and Render threads" );
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
