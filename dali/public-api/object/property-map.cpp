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
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace
{
typedef std::vector< StringValuePair > Container;
}; // unnamed namespace

struct Property::Map::Impl
{
  Container mContainer;
};

Property::Map::Map()
: mImpl( new Impl )
{
}

Property::Map::Map( const Property::Map& other )
: mImpl( new Impl )
{
  mImpl->mContainer = other.mImpl->mContainer;
}

Property::Map::~Map()
{
  delete mImpl;
}

Property::Map::SizeType Property::Map::Count() const
{
  return mImpl->mContainer.size();
}

bool Property::Map::Empty() const
{
  return mImpl->mContainer.empty();
}

void Property::Map::Insert( const char* key, const Value& value )
{
  mImpl->mContainer.push_back( std::make_pair( key, value ) );
}

void Property::Map::Insert( const std::string& key, const Value& value )
{
  mImpl->mContainer.push_back( std::make_pair( key, value ) );
}

Property::Value& Property::Map::GetValue( SizeType position ) const
{
  DALI_ASSERT_ALWAYS( position < Count() && "position out-of-bounds" );

  return mImpl->mContainer[ position ].second;
}

const std::string& Property::Map::GetKey( SizeType position ) const
{
  DALI_ASSERT_ALWAYS( position < Count() && "position out-of-bounds" );

  return mImpl->mContainer[ position ].first;
}

StringValuePair& Property::Map::GetPair( SizeType position ) const
{
  DALI_ASSERT_ALWAYS( position < Count() && "position out-of-bounds" );

  return mImpl->mContainer[ position ];
}

Property::Value* Property::Map::Find( const char* key ) const
{
  for ( Container::iterator iter = mImpl->mContainer.begin(), endIter = mImpl->mContainer.end(); iter != endIter; ++iter )
  {
    if ( iter->first == key )
    {
      return &iter->second;
    }
  }
  return NULL; // Not found
}

Property::Value* Property::Map::Find( const std::string& key ) const
{
  return Find( key.c_str() );

}

Property::Value* Property::Map::Find( const std::string& key, Property::Type type ) const
{
  for ( Container::iterator iter = mImpl->mContainer.begin(), endIter = mImpl->mContainer.end(); iter != endIter; ++iter )
  {
    // test type first to shortcut eval (possibly reducing string compares)
    if( (iter->second.GetType() == type) && (iter->first == key) )
    {
      return &iter->second;
    }
  }
  return NULL; // Not found
}

void Property::Map::Clear()
{
  mImpl->mContainer.clear();
}

void Property::Map::Merge( const Property::Map& from )
{
  // Ensure we're not attempting to merge with ourself
  if ( this != &from )
  {
    if ( Count() )
    {
      for ( unsigned int i = 0, count = from.Count(); i < count; ++i )
      {
        StringValuePair& pair( from.GetPair( i ) );
        (*this)[ pair.first ] = pair.second;
      }
    }
    else
    {
      // If we're empty, then just copy
      *this = from;
    }
  }
}

const Property::Value& Property::Map::operator[]( const std::string& key ) const
{
  for ( Container::const_iterator iter = mImpl->mContainer.begin(), endIter = mImpl->mContainer.end(); iter != endIter; ++iter )
  {
    if ( iter->first == key )
    {
      return iter->second;
    }
  }

  DALI_ASSERT_ALWAYS( ! "Invalid Key" );
}

Property::Value& Property::Map::operator[]( const std::string& key )
{
  for ( Container::iterator iter = mImpl->mContainer.begin(), endIter = mImpl->mContainer.end(); iter != endIter; ++iter )
  {
    if ( iter->first == key )
    {
      return iter->second;
    }
  }

  // Create and return reference to new value
  mImpl->mContainer.push_back( std::make_pair( key, Property::Value() ) );
  return (mImpl->mContainer.end() - 1)->second;
}

Property::Map& Property::Map::operator=( const Property::Map& other )
{
  if( this != &other )
  {
    delete mImpl;
    mImpl = new Impl;
    mImpl->mContainer = other.mImpl->mContainer;
  }
  return *this;
}

std::ostream& operator<<( std::ostream& stream, const Property::Map& map )
{
  stream << "Map(" << map.Count() << ") = {";
  for( unsigned int i=0; i<map.Count(); ++i )
  {
    if( i>0 )
    {
      stream << ", ";
    }
    stream << map.GetKey(i) << ":" << map.GetValue( i );
  }
  stream << "}";

  return stream;
}

} // namespace Dali
