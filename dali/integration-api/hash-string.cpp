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
#include <dali/integration-api/hash-string.h>

namespace Dali
{

// PJWHash Variant
// Algorithm based upon: http://www.drdobbs.com/database/hashing-rehashed/184409859

unsigned int HashString::CalculateHash( const std::string& s )
{
    const unsigned int BitsInUnsignedInt = static_cast< unsigned int >( sizeof( unsigned int ) * 8 );
    const unsigned int ThreeQuarters = static_cast< unsigned int >( ( ( BitsInUnsignedInt  * 3 ) / 4 ) );
    const unsigned int OneEighth = static_cast< unsigned int >( ( BitsInUnsignedInt / 8 ) );
    const unsigned int HighBits = static_cast< unsigned int >( ( 0xFFFFFFFF ) ) << ( BitsInUnsignedInt - OneEighth );
    unsigned int hash = 0;
    unsigned int test = 0;

    for(std::size_t i = 0; i < s.length(); i++)
    {
        hash = (hash << OneEighth) + s[i];

        if( ( test = hash & HighBits )  != 0 )
        {
            hash = ( ( hash ^ ( test >> ThreeQuarters) ) & (~HighBits) );
        }
    }
    return hash;
}

unsigned int HashString::CalculateHash( const char* string )
{
    std::string s( string );
    return CalculateHash( s );
}

void HashString::Insert( const std::string& string, const unsigned int hash )
{
  mHashedStrings.insert( std::make_pair( string, hash ) );
  mHashLookUp.insert( std::make_pair( hash, string ) );
}

unsigned int HashString::Insert( const std::string& string )
{
  unsigned int hash = CalculateHash( string );
  Insert( hash, string );
  return hash;
}

unsigned int HashString::Insert( std::string& string )
{
  unsigned int hash = CalculateHash( string );
  Insert( hash, string );
  return hash;
}

unsigned int HashString::Insert( const char* string )
{
  std::string s( string );
  return Insert( s );
}

void HashString::Insert( const unsigned int hash, const std::string& string )
{
  mHashLookUp.insert( std::make_pair( hash, string ) );
  mHashedStrings.insert( std::make_pair( string, hash ) );
}

void HashString::Insert( const char* string, const unsigned int hash )
{
  std::string s( string );
  mHashedStrings.insert( std::make_pair( s, hash ) );
  mHashLookUp.insert( std::make_pair( hash, s ) );
}

void HashString::Insert( const unsigned int hash, const char* string )
{
  std::string s( string );
  mHashLookUp.insert( std::make_pair( hash, s ) );
  mHashedStrings.insert( std::make_pair( s, hash ) );
}

unsigned int HashString::ReadHash( std::string& string )
{
  StringToHashMapIter SIt = mHashedStrings.find( string );
  if ( SIt == mHashedStrings.end() )
  {
    return 0;
    }
  return SIt->second;
}

unsigned int HashString::ReadHash( const char* string )
{
  std::string s( string );
  return ReadHash( s );
}

std::string HashString::ReadString( unsigned int hash )
{
  std::string string;
  HashToStringMapIter HIt = mHashLookUp.find( hash );
  if ( HIt == mHashLookUp.end() )
  {
    return string;
  }
  return HIt->second;
}

} // Namespace Dali