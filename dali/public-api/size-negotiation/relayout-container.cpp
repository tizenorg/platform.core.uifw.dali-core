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

// FILE HEADER
#include "relayout-container.h"

namespace Dali
{

RelayoutContainer::RelayoutContainer( MemoryPoolObjectAllocator< RelayoutInfo >& objectAllocator )
: mAllocator( objectAllocator )
{
}

bool RelayoutContainer::Contains( const Actor& actor )
{
  for( RelayoutInfoContainer::Iterator it = mRelayoutInfos.Begin(), itEnd = mRelayoutInfos.End(); it != itEnd; ++it )
  {
    RelayoutInfo* info = *it;

    if( info->actor == actor )
    {
      return true;
    }
  }

  return false;
}

void RelayoutContainer::Add( const Actor& actor, const Vector2& size )
{
  if( !Contains( actor ) )
  {
    RelayoutInfo* info = mAllocator.Allocate();
    info->actor = actor;
    info->size = size;

    mRelayoutInfos.PushBack( info );
  }
}

void RelayoutContainer::PopBack()
{
  if( mRelayoutInfos.Size() > 0 )
  {
    RelayoutInfoContainer::Iterator back = mRelayoutInfos.End();
    back--;
    RelayoutInfo* info = *back;
    mAllocator.Free( info );
    mRelayoutInfos.Erase( back );
  }
}

RelayoutContainer::RelayoutInfo& RelayoutContainer::Get( size_t index ) const
{
  DALI_ASSERT_DEBUG( index < Size() );

  return *mRelayoutInfos[ index ];
}

size_t RelayoutContainer::Size() const
{
  return mRelayoutInfos.Size();
}

void RelayoutContainer::Reserve( size_t capacity )
{
  mRelayoutInfos.Reserve( capacity );
}

void RelayoutContainer::Clear()
{
  for( size_t i = 0; i < Size(); ++i )
  {
    RelayoutInfo* info = &Get( i );
    mAllocator.Free( info );
  }

  mRelayoutInfos.Clear();
}

} // namespace Dali
