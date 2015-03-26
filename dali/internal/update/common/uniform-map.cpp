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
 */

#include "uniform-map.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

UniformMap::UniformMap()
{
}

UniformMap::~UniformMap()
{
  // Nothing to do - let the owner container delete the maps
}

void UniformMap::AddObserver( Observer& observer )
{
  bool foundObserver = false;
  for( ObserversIter iter = mObservers.Begin(); iter != mObservers.End(); ++iter )
  {
    if( *iter == &observer )
    {
      foundObserver = true;
      break;
    }
  }
  if( !foundObserver )
  {
    mObservers.PushBack( &observer );
  }
}

void UniformMap::RemoveObserver( Observer& observer )
{
  for( ObserversIter iter = mObservers.Begin(); iter != mObservers.End(); ++iter )
  {
    if( *iter == &observer )
    {
      mObservers.Erase(iter);
      return;
    }
  }
}

void UniformMap::MappingChanged()
{
  for( ObserversIter iter = mObservers.Begin(); iter != mObservers.End(); ++iter )
  {
    Observer* observer = (*iter);
    observer->UniformMappingsChanged( *this );
  }
}

//
// The uniform map can never grow beyond the limits of GL - so really, the
// map size is likely to be small; if retaining an unsorted map proves to be
// slow, then it should be changed to perform an insertion sort.
void UniformMap::Add( UniformPropertyMapping* newMap )
{
  UniformPropertyMapping::Hash nameHash = CalculateHash( newMap->uniformName );

  bool found = false;

  for( UniformMapIter iter = mUniformMaps.Begin() ;
       iter != mUniformMaps.End() ;
       ++iter )
  {
    UniformPropertyMapping* map = *iter;
    if( map->uniformNameHash == nameHash )
    {
      if( map->uniformName == newMap->uniformName )
      {
        found = true;
        // Mapping already exists - update it.
        map->propertyPtr = newMap->propertyPtr;
        break;
      }
    }
  }

  if( found == false )
  {
    // Take ownership of the new map
    mUniformMaps.PushBack(newMap);
  }

  MappingChanged();
}

void UniformMap::Remove( const std::string& uniformName )
{
  UniformPropertyMapping::Hash nameHash = CalculateHash( uniformName );

  bool found=false;

  for( UniformMapIter iter = mUniformMaps.Begin() ;
       iter != mUniformMaps.End() ;
       ++iter )
  {
    UniformPropertyMapping* map = *iter;
    if( map->uniformNameHash == nameHash )
    {
      if( map->uniformName == uniformName )
      {
        mUniformMaps.Erase( iter );
        found = true;
        break;
      }
    }
  }

  if( found )
  {
    MappingChanged();
  }
}

const PropertyInputImpl* UniformMap::Find( const std::string& uniformName )
{
  UniformPropertyMapping::Hash nameHash = CalculateHash( uniformName );

  for( UniformMapIter iter = mUniformMaps.Begin() ;
       iter != mUniformMaps.End() ;
       ++iter )
  {
    UniformPropertyMapping* map = *iter;
    if( map->uniformNameHash == nameHash )
    {
      if( map->uniformName == uniformName )
      {
        return map->propertyPtr;
      }
    }
  }
  return NULL;
}

unsigned int UniformMap::Count() const
{
  return mUniformMaps.Count();
}

const UniformPropertyMapping& UniformMap::operator[]( unsigned int index ) const
{
  return *mUniformMaps[index];
}

void UniformMap::Reset()
{
  mUniformMaps.Clear(); // Destroys all owned objects
  // Don't perform MappingsChanged - this interface is only called when regenerating
  // an overall mapping, and observers should not be notified at this time.
}

void UniformMap::AddMappings( const UniformMap& uniformMap )
{
  // Iterate thru uniformMap.
  //   Any maps that aren't in this should be added at the end of the iteration
  Dali::Vector< UniformPropertyMapping* > newUniformMappings;

  for( UniformMapContainer::ConstIterator newIter = uniformMap.mUniformMaps.Begin(); newIter != uniformMap.mUniformMaps.End(); ++newIter )
  {
    UniformPropertyMapping::Hash nameHash = (*newIter)->uniformNameHash;
    bool found = false;

    for( UniformMapIter iter = mUniformMaps.Begin() ;
         iter != mUniformMaps.End() ;
         ++iter )
    {
      UniformPropertyMapping* map = *iter;
      if( map->uniformNameHash == nameHash )
      {
        if( map->uniformName == (*newIter)->uniformName )
        {
          found = true;
          break;
        }
      }
    }
    if( !found )
    {
      // it's a new mapping. Add raw ptr to temporary list
      newUniformMappings.PushBack( *newIter );
    }
  }

  mUniformMaps.Reserve( mUniformMaps.Count() + newUniformMappings.Count() );

  for( UniformMapContainer::Iterator iter = newUniformMappings.Begin(),
         end = newUniformMappings.End() ;
       iter != end ;
       ++iter )
  {
    UniformPropertyMapping* map = new UniformPropertyMapping((*iter)->uniformName, (*iter)->propertyPtr);

    mUniformMaps.PushBack( map );
  }
}


} // SceneGraph
} // Internal
} // Dali
