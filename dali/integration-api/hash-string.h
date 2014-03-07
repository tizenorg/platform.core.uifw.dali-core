#ifndef __DALI_HASH_STRING_H__
#define __DALI_HASH_STRING_H__

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

// EXTERNAL INCLUDES
#include <stdio.h>
#include <map>
#include <string>

namespace Dali
{

/**
 * HashString
 *
 * Provides public hashing facility for strings
 * Provides an object that can collect hashing information
 */
class HashString
{
public:

  // Container descriptions and iterators to string to hash container
  typedef std::map< std::string, unsigned int > StringToHashMap;
  typedef StringToHashMap::iterator StringToHashMapIter;
  typedef StringToHashMap::const_iterator StringToHashMapConstIter;

  // Container descriptions and iterators to hash to string container
  typedef std::map< unsigned int, std::string > HashToStringMap;
  typedef HashToStringMap::iterator HashToStringMapIter;
  typedef HashToStringMap::const_iterator HashToStringMapConstIter;

  HashString() {};

  /**
   * @brief Calculate the hash of string reference
   *
   * @param string reference of a string
   * @return hash value
   */
  static unsigned int CalculateHash( const std::string& string );

  /**
   * @brief Calculate hash for a CString
   *
   * @param string pointer to a CString
   * @return hash value
   */
  static unsigned int CalculateHash( const char* string );

  /**
   * @brief Insert a string and corresponding hash into hash map and reverse lookup
   *
   * @param string reference to a string
   * @param hash previously obtained hash value
   */
  void Insert( const std::string& string, const unsigned int hash );

  /**
   * @brief Insert a string into hash map and return hash value
   *
   * @param string const reference to a string
   * @return hashed value
   */
  unsigned int Insert( const std::string& string );

  /**
   * @brief Insert a string into hash map and return hash value
   *
   * @param string eference to a string
   * @return hashed value
   */
  unsigned int Insert( std::string& string );

  /**
   * @brief Insert a string into hash map and return hash value
   *
   * @param string pointer to CString
   * @return hash value
   */
  unsigned int Insert( const char* string );

  /**
   * @brief Insert a string and corresponding hash into hash map and reverse lookup
   *
   * @param hash value
   * @param string reference to string
   */
  void Insert( const unsigned int hash, const std::string& string );

  /**
   * @brief Insert a string and corresponding hash into hash map and reverse lookup
   *
   * @param string pointer to CString
   * @param hash previously obtained hash value
   */
  void Insert( const char* string, const unsigned int hash );

  /**
   * @brief Insert a string and corresponding hash into hash map and reverse lookup
   *
   * @param hash value
   * @param string pointer to CString
   */
  void Insert( const unsigned int hash, const char* string );

  /**
   * @brief Find the hash for a string
   *
   * @param string reference to string
   * @return iterator to reverse lookup
   */
  StringToHashMapIter Find( std::string& string )
  {
    return mHashedStrings.find( string );
  }

  /**
   * @brief Find the hash for a CString
   *
   * @param string pointer to CString
   * @return interator to reverse lookup
   */
  StringToHashMapIter Find( const char* string )
  {
    std::string s( string );
    return Find( s );
  }

  /**
   * @brief Find the hashed string
   *
   * @param hash hash value
   * @return hashed string
   */
  std::string& Find( unsigned int hash )
  {
    HashToStringMapIter HIt = mHashLookUp.find( hash );
    return HIt->second;
  }

  /**
   * @brief Get the start of the reverse lookup map
   *
   * @return Iterator to the start
   */

  StringToHashMapIter StringsBegin()
  {
    return mHashedStrings.begin();
  }

  /**
   * @brief Get the start of the hash map
   *
   * @return Iterator to the start
   */
  HashToStringMapIter HashesBegin()
  {
    return mHashLookUp.begin();
  }

  /**
   * @brief Get the end of the reverse lookup map
   *
   * @return Iterator at the end
   */
  StringToHashMapIter StringsEnd()
  {
    return mHashedStrings.end();
  }

  /**
   * @brief Get the end of the hash map
   *
   * @return Iterator at the end
   */
  HashToStringMapIter HashesEnd()
  {
    return mHashLookUp.end();
  }

  /**
   * @brief Read hash value from reverse lookup at Iterator position
   * @details [long description]
   *
   * @param SIt Iterator to reverse lookup map
   * @return hash value
   */
  unsigned int ReadHash( StringToHashMapIter SIt ) const
  {
    return SIt->second;
  }

  /**
   * @brief Read hash value in reverse lookup for string
   *
   * @param string reference to string
   * @return hash value or 0 if not found
   */
  unsigned int ReadHash( std::string& string );

  /**
   * @brief Read hash value in reverse lookup for string
   *
   * @param string pointer to CString
   * @return hash value or 0 if not found
   */
  unsigned int ReadHash( const char* string );

  /**
   * @brief Read string from hash value
   *
   * @param hash hash value
   * @return string
   */
  std::string ReadString( unsigned int hash );

private:

  StringToHashMap mHashedStrings;              // Reverse lookup hash map
  HashToStringMap mHashLookUp;                 // Hash Map

};

} // Namespace Dali

#endif // __DALI_HASH_STRING_H__