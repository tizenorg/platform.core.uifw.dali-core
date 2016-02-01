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
#include <dali/public-api/object/property-value.h>

// EXTERNAL INCLUDES
#include <ostream>

// INTERNAL INCLUDES
#include <dali/public-api/math/angle-axis.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-types.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace
{
/**
 * Helper to check if the property value can be read as int/bool
 */
inline bool IsIntegerType( Property::Type type )
{
  return ( Property::BOOLEAN == type )||( Property::INTEGER == type );
}

// true if character represent a digit
inline bool IsDigit(char c)
{
  return (c >= '0' && c <= '9');
}

// convert string to integer
bool StringToInteger(const char *first, const char *last, int& out)
{
  int sign = 1;
  if (first != last)
  {
    if (*first == '-')
    {
      sign = -1;
      ++first;
    }
    else if (*first == '+')
    {
      ++first;
    }
  }

  if( 0 == (*first - '0') && (first+1 != last))
  {
    return false;
  }

  int result = 0;
  for (; first != last && IsDigit(*first); ++first)
  {
    result = 10 * result + (*first - '0');
  }
  out = result * sign;

  if(first != last)
  {
    return false;
  }
  else
  {
    return true;
  }
}

// convert hexadecimal string to unsigned integer
bool HexStringToUnsignedInteger(const char *first, const char *last, unsigned int& out)
{
  unsigned int result = 0;
  for (; first != last; ++first)
  {
    int digit;
    if (IsDigit(*first))
    {
      digit = *first - '0';
    }
    else if (*first >= 'a' && *first <= 'f')
    {
      digit = *first - 'a' + 10;
    }
    else if (*first >= 'A' && *first <= 'F')
    {
      digit = *first - 'A' + 10;
    }
    else
    {
      break;
    }
    result = 16 * result + digit;
  }
  out = result;

  if(first != last)
  {
    return false;
  }
  else
  {
    return true;
  }
}

// convert string to floating point
bool StringToFloat(const char* first, const char* last, float& out)
{
  // sign
  float sign = 1;
  if (first != last)
  {
    if (*first == '-')
    {
      sign = -1;
      ++first;
    }
    else if (*first == '+')
    {
      ++first;
    }
  }

  // integer part
  float result = 0;
  for (; first != last && IsDigit(*first); ++first)
  {
    result = 10 * result + (*first - '0');
  }

  // fraction part
  if (first != last && *first == '.')
  {
    ++first;

    float inv_base = 0.1f;
    for (; first != last && IsDigit(*first); ++first)
    {
      result += (*first - '0') * inv_base;
      inv_base *= 0.1f;
    }
  }

  // result w\o exponent
  result *= sign;

  // exponent
  bool exponent_negative = false;
  int exponent = 0;
  if (first != last && (*first == 'e' || *first == 'E'))
  {
    ++first;

    if (*first == '-')
    {
      exponent_negative = true;
      ++first;
    }
    else if (*first == '+')
    {
      ++first;
    }

    if(first == last || !IsDigit(*first))
    {
      return false;
    }

    for (; first != last && IsDigit(*first); ++first)
    {
      exponent = 10 * exponent + (*first - '0');
    }
  }

  if (exponent)
  {
    float power_of_ten = 10;
    for (; exponent > 1; exponent--)
    {
      power_of_ten *= 10;
    }

    if (exponent_negative)
    {
      result /= power_of_ten;
    }
    else
    {
      result *= power_of_ten;
    }
  }

  out = result;

  if(first != last)
  {
    return false;
  }
  else
  {
    return true;
  }
}

/**
 * Converts a HTML style 'color' hex string ("#FF0000" for bright red) to a Vector4.
 * The Vector4 alpha component will be set to 1.0f
 * @param hexString The HTML style hex string
 * @return a Vector4 containing the new color value
 */
Vector4 HexStringToVector4( const char* s )
{
  unsigned int value(0u);
  std::istringstream( s ) >> std::hex >> value;
  return Vector4( ((value >> 16 ) & 0xff ) / 255.0f,
                  ((value >> 8 ) & 0xff ) / 255.0f,
                  (value & 0xff ) / 255.0f,
                  1.0f );
}

template<> struct Component {}
template<> struct Component<Vector2> { static const int Size = 2}
template<> struct Component<Vector3> { static const int Size = 3}
template<> struct Component<Vector4> { static const int Size = 4}
template<> struct Component<Matrix3x3> { static const int Size = 9}
template<> struct Component<Matrix> { static const int Size = 16}
template<> struct Component<Rect<int> > { static const int Size = 4}

template<typename T>
inline bool CopyNumbers(Property::Array& array, T& vector)
{
  if( array.Size() >= Component<T>::Size  )
  {
    for(int i = 0; i < Component<T>::Size; ++i)
    {
      Value item& = array.GetElementAt(i);

      float value = 0.f;

      if( item.Get<float>(value) )
      {
        vector[i] = value;
      }
      else
      {
        return false;
      }
    }
  }
  return true;
}

inline bool GetMapComponent(Property::Map& map, const std::string& field, float& out)
{
  bool found = false
  Value* value = map.Find(field);
  if(value)
  {
    if( Get<float>(out) )
    {
      found = true;
    }
  }
  return found;
}

} // anonymous namespace

struct Property::Value::Impl
{
  Impl()
  : type( Property::NONE ),
    integerValue( 0 )
  { }

  Impl( bool booleanValue )
  : type( Property::BOOLEAN ),
    integerValue( booleanValue )
  { }

  Impl( float floatValue )
  : type( Property::FLOAT ),
    floatValue( floatValue )
  { }

  Impl( int integerValue )
  : type( Property::INTEGER ),
    integerValue( integerValue )
  { }

  Impl( const Vector2& vectorValue )
  : type( Property::VECTOR2 ),
    vector2Value( new Vector2( vectorValue ) )
  { }

  Impl( const Vector3& vectorValue )
  : type( Property::VECTOR3 ),
    vector3Value( new Vector3( vectorValue ) )
  { }

  Impl( const Vector4& vectorValue )
  : type( Property::VECTOR4 ),
    vector4Value( new Vector4( vectorValue ) )
  { }

  Impl( const Matrix3& matrixValue )
  : type( Property::MATRIX3 ),
    matrix3Value( new Matrix3( matrixValue ) )
  {
  }

  Impl( const Matrix& matrixValue )
  : type( Property::MATRIX ),
    matrixValue( new Matrix( matrixValue ) )
  {
  }

  Impl( const AngleAxis& angleAxisValue )
  : type( Property::ROTATION ),
    quaternionValue( new Quaternion( angleAxisValue.angle, angleAxisValue.axis ) )
  {
  }

  Impl( const Quaternion& quaternionValue )
  : type( Property::ROTATION ),
    quaternionValue( new Quaternion( quaternionValue ) )
  {
  }

  Impl(const std::string& stringValue)
  : type( Property::STRING ),
    stringValue( new std::string( stringValue ) )
  {
  }

  Impl( const Rect<int>& rectValue )
  : type( Property::RECTANGLE ),
    rectValue( new Rect<int>( rectValue ) )
  {
  }

  Impl( const Property::Array& arrayValue )
  : type( Property::ARRAY ),
    arrayValue( new Property::Array( arrayValue ) )
  {
  }

  Impl( const Property::Map& mapValue )
  : type( Property::MAP ),
    mapValue( new Property::Map( mapValue ) )
  {
  }

  /**
   * Destructor, takes care of releasing the dynamically allocated types
   */
  ~Impl()
  {
    switch( type )
    {
      case Property::NONE :             // FALLTHROUGH
      case Property::BOOLEAN :          // FALLTHROUGH
      case Property::FLOAT :            // FALLTHROUGH
      case Property::INTEGER :
      {
        break; // nothing to do
      }
      case Property::VECTOR2 :
      {
        delete vector2Value;
        break;
      }
      case Property::VECTOR3:
      {
        delete vector3Value;
        break;
      }
      case Property::VECTOR4:
      {
        delete vector4Value;
        break;
      }
      case Property::MATRIX3:
      {
        delete matrix3Value;
        break;
      }
      case Property::MATRIX:
      {
        delete matrixValue;
        break;
      }
      case Property::RECTANGLE:
      {
        delete rectValue;
        break;
      }
      case Property::ROTATION:
      {
        delete quaternionValue;
        break;
      }
      case Property::STRING:
      {
        delete stringValue;
        break;
      }
      case Property::ARRAY:
      {
        delete arrayValue;
        break;
      }
      case Property::MAP:
      {
        delete mapValue;
        break;
      }
    }
  }

public: // Data

  Type type;
  union
  {
    int integerValue;
    float floatValue;
    // must use pointers for any class value pre c++ 11
    Vector2* vector2Value;
    Vector3* vector3Value;
    Vector4* vector4Value;
    Matrix3* matrix3Value;
    Matrix* matrixValue;
    Quaternion* quaternionValue;
    std::string* stringValue;
    Rect<int>* rectValue;
    Property::Array* arrayValue;
    Property::Map* mapValue;
  };
};

Property::Value::Value()
: mImpl( NULL )
{
}

Property::Value::Value( bool booleanValue )
: mImpl( new Impl( booleanValue ) )
{
}

Property::Value::Value( float floatValue )
: mImpl( new Impl( floatValue ) )
{
}

Property::Value::Value( int integerValue )
: mImpl( new Impl( integerValue ) )
{
}

Property::Value::Value( const Vector2& vectorValue )
: mImpl( new Impl( vectorValue ) )
{
}

Property::Value::Value( const Vector3& vectorValue )
: mImpl( new Impl( vectorValue ) )
{
}

Property::Value::Value( const Vector4& vectorValue )
: mImpl( new Impl( vectorValue ) )
{
}

Property::Value::Value( const Matrix3& matrixValue )
: mImpl( new Impl( matrixValue ) )
{
}

Property::Value::Value( const Matrix& matrixValue )
: mImpl( new Impl( matrixValue ) )
{
}

Property::Value::Value( const Rect<int>& rectValue )
: mImpl( new Impl( rectValue ) )
{
}

Property::Value::Value( const AngleAxis& angleAxisValue )
: mImpl( new Impl( angleAxisValue ) )
{
}

Property::Value::Value( const Quaternion& quaternionValue )
: mImpl( new Impl( quaternionValue ) )
{
}

Property::Value::Value( const std::string& stringValue )
: mImpl( new Impl( stringValue ) )
{
}

Property::Value::Value( const char* stringValue )
: mImpl( NULL )
{
  if( stringValue ) // string constructor is undefined with NULL pointer
  {
    mImpl = new Impl( std::string(stringValue) );
  }
  else
  {
    mImpl = new Impl( std::string() );
  }
}

Property::Value::Value( Property::Array& arrayValue )
: mImpl( new Impl( arrayValue ) )
{
}

Property::Value::Value( Property::Map& mapValue )
: mImpl( new Impl( mapValue ) )
{
}

Property::Value::Value( Type type )
{
  switch (type)
  {
    case Property::BOOLEAN:
    {
      mImpl = new Impl( false );
      break;
    }
    case Property::FLOAT:
    {
      mImpl = new Impl( 0.f );
      break;
    }
    case Property::INTEGER:
    {
      mImpl = new Impl( 0 );
      break;
    }
    case Property::VECTOR2:
    {
      mImpl = new Impl( Vector2::ZERO );
      break;
    }
    case Property::VECTOR3:
    {
      mImpl = new Impl( Vector3::ZERO );
      break;
    }
    case Property::VECTOR4:
    {
      mImpl = new Impl( Vector4::ZERO );
      break;
    }
    case Property::RECTANGLE:
    {
      mImpl = new Impl( Rect<int>(0,0,0,0) );
      break;
    }
    case Property::ROTATION:
    {
      mImpl = new Impl( Quaternion() );
      break;
    }
    case Property::STRING:
    {
      mImpl = new Impl( std::string() );
      break;
    }
    case Property::MATRIX:
    {
      mImpl = new Impl( Matrix() );
      break;
    }
    case Property::MATRIX3:
    {
      mImpl = new Impl( Matrix3() );
      break;
    }
    case Property::ARRAY:
    {
      mImpl = new Impl( Property::Array() );
      break;
    }
    case Property::MAP:
    {
      mImpl = new Impl( Property::Map() );
      break;
    }
    case Property::NONE:
    {
      mImpl = new Impl();
      break;
    }
  }
}

Property::Value::Value( const Property::Value& value )
: mImpl( NULL )
{
  // reuse assignment operator
  operator=( value );
}

Property::Value& Property::Value::operator=( const Property::Value& value )
{
  if ( this == &value )
  {
    // skip self assignment
    return *this;
  }
  // if we are assigned an empty value, just drop impl
  if( !value.mImpl )
  {
    delete mImpl;
    mImpl = NULL;
    return *this;
  }
  // first check if the type is the same, no need to change impl, just assign
  if( mImpl && ( mImpl->type == value.mImpl->type ) )
  {
    switch( mImpl->type )
    {
      case Property::BOOLEAN:
      {
        mImpl->integerValue = value.mImpl->integerValue;
        break;
      }
      case Property::FLOAT:
      {
        mImpl->floatValue = value.mImpl->floatValue;
        break;
      }
      case Property::INTEGER:
      {
        mImpl->integerValue = value.mImpl->integerValue;
        break;
      }
      case Property::VECTOR2:
      {
        *mImpl->vector2Value = *value.mImpl->vector2Value; // type cannot change in mImpl so vector is allocated
        break;
      }
      case Property::VECTOR3:
      {
        *mImpl->vector3Value = *value.mImpl->vector3Value; // type cannot change in mImpl so vector is allocated
        break;
      }
      case Property::VECTOR4:
      {
        *mImpl->vector4Value = *value.mImpl->vector4Value; // type cannot change in mImpl so vector is allocated
        break;
      }
      case Property::RECTANGLE:
      {
        *mImpl->rectValue = *value.mImpl->rectValue; // type cannot change in mImpl so rect is allocated
        break;
      }
      case Property::ROTATION:
      {
        *mImpl->quaternionValue = *value.mImpl->quaternionValue; // type cannot change in mImpl so quaternion is allocated
        break;
      }
      case Property::STRING:
      {
        *mImpl->stringValue = *value.mImpl->stringValue; // type cannot change in mImpl so string is allocated
        break;
      }
      case Property::MATRIX:
      {
        *mImpl->matrixValue = *value.mImpl->matrixValue; // type cannot change in mImpl so matrix is allocated
        break;
      }
      case Property::MATRIX3:
      {
        *mImpl->matrix3Value = *value.mImpl->matrix3Value; // type cannot change in mImpl so matrix is allocated
        break;
      }
      case Property::ARRAY:
      {
        *mImpl->arrayValue = *value.mImpl->arrayValue; // type cannot change in mImpl so array is allocated
        break;
      }
      case Property::MAP:
      {
        *mImpl->mapValue = *value.mImpl->mapValue; // type cannot change in mImpl so map is allocated
        break;
      }
      case Property::NONE:
      { // mImpl will be NULL, there's no way to get to this case
      }
    }
  }
  else
  {
    // different type, release old impl and create new
    Impl* newImpl( NULL );
    switch ( value.mImpl->type )
    {
      case Property::BOOLEAN:
      {
        newImpl = new Impl( bool( value.mImpl->integerValue ) );
        break;
      }
      case Property::FLOAT:
      {
        newImpl = new Impl( value.mImpl->floatValue );
        break;
      }
      case Property::INTEGER:
      {
        newImpl = new Impl( value.mImpl->integerValue );
        break;
      }
      case Property::VECTOR2:
      {
        newImpl = new Impl( *value.mImpl->vector2Value ); // type cannot change in mImpl so vector is allocated
        break;
      }
      case Property::VECTOR3:
      {
        newImpl = new Impl( *value.mImpl->vector3Value ); // type cannot change in mImpl so vector is allocated
        break;
      }
      case Property::VECTOR4:
      {
        newImpl = new Impl( *value.mImpl->vector4Value ); // type cannot change in mImpl so vector is allocated
        break;
      }
      case Property::RECTANGLE:
      {
        newImpl = new Impl( *value.mImpl->rectValue ); // type cannot change in mImpl so rect is allocated
        break;
      }
      case Property::ROTATION:
      {
        newImpl = new Impl( *value.mImpl->quaternionValue ); // type cannot change in mImpl so quaternion is allocated
        break;
      }
      case Property::MATRIX3:
      {
        newImpl = new Impl( *value.mImpl->matrix3Value ); // type cannot change in mImpl so matrix is allocated
        break;
      }
      case Property::MATRIX:
      {
        newImpl = new Impl( *value.mImpl->matrixValue ); // type cannot change in mImpl so matrix is allocated
        break;
      }
      case Property::STRING:
      {
        newImpl = new Impl( *value.mImpl->stringValue ); // type cannot change in mImpl so string is allocated
        break;
      }
      case Property::ARRAY:
      {
        newImpl = new Impl( *value.mImpl->arrayValue ); // type cannot change in mImpl so array is allocated
        break;
      }
      case Property::MAP:
      {
        newImpl = new Impl( *value.mImpl->mapValue ); // type cannot change in mImpl so map is allocated
        break;
      }
      case Property::NONE:
      { // NULL value will be used for "empty" value
      }
    }
    delete mImpl;
    mImpl = newImpl;
  }

  return *this;
}

Property::Value::~Value()
{
  delete mImpl;
}

Property::Type Property::Value::GetType() const
{
  Property::Type type( Property::NONE );
  if( mImpl )
  {
    type = mImpl->type;
  }
  return type;
}

bool Property::Value::Get( bool& booleanValue ) const
{
  bool converted = false;
  if( mImpl )
  {
    if( IsIntegerType( mImpl->type ) )
    {
      booleanValueValue = mImpl->integerValue ? true : false;
      converted = true;
    }
    else if( Property::FLOAT == mImpl->type )
    {
      booleanValue = (mImpl->floatValue == f ? true : false);
      converted = true;
    }
    else if( Property::ARRAY == mImpl->type)
    {
      if( mImpl->arrayValue )
      {
        booleanValue = !mImpl->arrayValue->Empty();
        converted = true;
      }
    }
    else if( Property::STRING == mImpl->type )
    {
      std::s = std::toupper( Get<std::string>() );
      if( s == "TRUE" )
      {
        booleanValue = true;
        converted = true;
      }
      else if( s == "FALSE" )
      {
        booleanValue = false;
        converted = true;
      }
      else if( s == "1" )
      {
        booleanValue = true;
        converted = true;
      }
      else if( s == "0" )
      {
        booleanValue = false;
        converted = true;
      }
      else
      {
        int i = 0;
        if( Get<int>(i) )
        {
          booleanValue = i ? true : false;
          converted = true;
        }
      }
    }
  }

  return converted;
}

bool Property::Value::Get( float& floatValue ) const
{
  bool converted = false;
  if( mImpl )
  {
    if( Property::FLOAT == mImpl->type )
    {
      floatValue = mImpl->floatValue;
      converted = true;
    }
    else if( Property::INTEGER == mImpl->type )
    {
      floatValue = static_cast<float>(mImpl->integerValue);
      converted = true;
    }
    else if( Property::STRING == mImpl->type )
    {
      const char* first = *mImpl->stringValue.c_str();
      const char* last = first + mImpl->stringValue.size() - 1;
      converted = StringToFloat(first, last, floatValue);
    }
  }
  return converted;
}

bool Property::Value::Get( int& integerValue ) const
{
  bool converted = false;
  if( mImpl )
  {
    if( IsIntegerType( mImpl->type ) )
    {
      integerValue = mImpl->integerValue;
      converted = true;
    }
    else if( mImpl->type == FLOAT )
    {
      integerValue = static_cast< int >( mImpl->floatValue );
      converted = true;
    }
    else if( Property::STRING == mImpl->type )
    {
      const char* first = *mImpl->stringValue.c_str();
      const char* last = first + mImpl->stringValue.size() - 1;
      converted = StringToInteger(first, last, integerValue);
      // @todo hex strings?
    }
  }
  return converted;
}

bool Property::Value::Get( Vector2& vectorValue ) const
{
  bool converted = false;
  if( mImpl )
  {
    if( mImpl->type == VECTOR2 ) // type cannot change in mImpl so vector is allocated
    {
      vectorValue = *(mImpl->vector2Value);
      converted = true;
    }
    else if( mImpl->type == ARRAY )
    {
      if(mImpl->arrayValue)
      {
        converted = CopyNumbers( *mimpl->arrayValue, vectorValue );
      }
    }
    else if( mImpl->type == MAP )
    {
      if(mImpl->mapValue)
      {
        if( GetMapComponent(*mImpl->mapValue, "x", vectorValue.x) &&
            GetMapComponent(*mImpl->mapValue, "y", vectorValue.y) )
        {
          converted = true;
        }
      }
    }
  }
  return converted;
}

bool Property::Value::Get( Vector3& vectorValue ) const
{
  bool converted = false;
  if( mImpl )
  {
    if( mImpl->type == VECTOR3 ) // type cannot change in mImpl so vector is allocated
    {
      vectorValue = *(mImpl->vector3Value);
      converted = true;
    }
    else if( mImpl->type == ARRAY )
    {
      if(mImpl->arrayValue)
      {
        converted = CopyNumbers( *mimpl->arrayValue, vectorValue );
      }
    }
    else if( mImpl->type == MAP )
    {
      if(mImpl->mapValue)
      {
        if( GetMapComponent(*mImpl->mapValue, "x", vectorValue.x) &&
            GetMapComponent(*mImpl->mapValue, "y", vectorValue.y) &&
            GetMapComponent(*mImpl->mapValue, "z", vectorValue.z) )
        {
          converted = true;
        }

        if( !converted )
        {
          if( GetMapComponent(*mImpl->mapValue, "r", vectorValue.x) &&
              GetMapComponent(*mImpl->mapValue, "g", vectorValue.y) &&
              GetMapComponent(*mImpl->mapValue, "b", vectorValue.z) )
          {
            converted = true;
          }
        }
      }
    }
    else if( mImpl->type == STRING )
    {
      Property::Array array;
      if( Get(array) )
      {
        converted = CopyNumbers( *mimpl->arrayValue, vectorValue );
      }
    }
  }

  return converted;
}

bool Property::Value::Get( Vector4& vectorValue ) const
{
  bool converted = false;
  if( mImpl )
  {
    if( mImpl && (mImpl->type == VECTOR4) ) // type cannot change in mImpl so vector is allocated
    {
      vectorValue = *(mImpl->vector4Value);
      converted = true;
    }
    else if( mImpl->type == ARRAY )
    {
      if(mImpl->arrayValue)
      {
        converted = CopyNumbers( *mimpl->arrayValue, vectorValue );
      }
    }
    else if( mImpl->type == MAP )
    {
      if(mImpl->mapValue)
      {
        if( GetMapComponent(*mImpl->mapValue, "x", vectorValue.x) &&
            GetMapComponent(*mImpl->mapValue, "y", vectorValue.y) &&
            GetMapComponent(*mImpl->mapValue, "z", vectorValue.z) &&
            GetMapComponent(*mImpl->mapValue, "w", vectorValue.w) )
        {
          converted = true;
        }

        if( !converted )
        {
          if( GetMapComponent(*mImpl->mapValue, "r", vectorValue.x) &&
              GetMapComponent(*mImpl->mapValue, "g", vectorValue.y) &&
              GetMapComponent(*mImpl->mapValue, "b", vectorValue.z) &&
              GetMapComponent(*mImpl->mapValue, "a", vectorValue.w) )
          {
            converted = true;
          }
        }
      }
    }
    else if( mImpl->type == STRING )
    {
      if( mImpl->stringValue )
      {
        if( (*mImpl->stringValue)[0] == '#' && 7 == (*mImpl->stringValue).size() )
        {
          value = HexStringToVector4( &(*mImpl->stringValue)[1] );
          converted = true;
        }
        else if( Dali::ColorController::Get() )
        {
          Vector4 color;
          converted = Dali::ColorController::Get().RetrieveColor( *mImpl->stringValue, color );
          value = color;
        }
        if(!converted)
        {
          Property::Array array;
          if( Get(array) )
          {
            converted = CopyNumbers( *mimpl->arrayValue, vectorValue );
          }
        }
      }
    }
  }

  return converted;
}

bool Property::Value::Get( Matrix3& matrixValue ) const
{
  bool converted = false;
  if( mImpl )
  {
    if( mImpl->type == MATRIX3 ) // type cannot change in mImpl so matrix is allocated
    {
      matrixValue = *(mImpl->matrix3Value);
      converted = true;
    }
    else if( mImpl->type == ARRAY )
    {
      if(mImpl->arrayValue)
      {
        converted = CopyNumbers( *mimpl->matrix3Value, matrixValue );
      }
    }
    else if( mImpl->type == MAP )
    {
      if(mImpl->mapValue)
      {
        if( GetMapComponent(*mImpl->mapValue, "e1", *mImpl->matrix3Value->AsFloat(0) ) &&
            GetMapComponent(*mImpl->mapValue, "e2", *mImpl->matrix3Value->AsFloat(1) ) &&
            GetMapComponent(*mImpl->mapValue, "e3", *mImpl->matrix3Value->AsFloat(2) ) &&
            GetMapComponent(*mImpl->mapValue, "e4", *mImpl->matrix3Value->AsFloat(3) ) &&
            GetMapComponent(*mImpl->mapValue, "e5", *mImpl->matrix3Value->AsFloat(4) ) &&
            GetMapComponent(*mImpl->mapValue, "e6", *mImpl->matrix3Value->AsFloat(5) ) &&
            GetMapComponent(*mImpl->mapValue, "e7", *mImpl->matrix3Value->AsFloat(6) ) &&
            GetMapComponent(*mImpl->mapValue, "e8", *mImpl->matrix3Value->AsFloat(7) ) &&
            GetMapComponent(*mImpl->mapValue, "e9", *mImpl->matrix3Value->AsFloat(8) ) )
        {
          converted = true;
        }

        if( !converted )
        {
          if( GetMapComponent(*mImpl->mapValue, "s00", *mImpl->matrix3Value->AsFloat(0) ) &&
              GetMapComponent(*mImpl->mapValue, "s01", *mImpl->matrix3Value->AsFloat(1) ) &&
              GetMapComponent(*mImpl->mapValue, "s02", *mImpl->matrix3Value->AsFloat(2) ) &&
              GetMapComponent(*mImpl->mapValue, "s10", *mImpl->matrix3Value->AsFloat(3) ) &&
              GetMapComponent(*mImpl->mapValue, "s11", *mImpl->matrix3Value->AsFloat(4) ) &&
              GetMapComponent(*mImpl->mapValue, "s12", *mImpl->matrix3Value->AsFloat(5) ) &&
              GetMapComponent(*mImpl->mapValue, "s20", *mImpl->matrix3Value->AsFloat(6) ) &&
              GetMapComponent(*mImpl->mapValue, "s21", *mImpl->matrix3Value->AsFloat(7) ) &&
              GetMapComponent(*mImpl->mapValue, "s22", *mImpl->matrix3Value->AsFloat(8) ) )
          {
            converted = true;
          }
        }
      }
    }
  }
  return converted;
}

bool Property::Value::Get( Matrix& matrixValue ) const
{
  bool converted = false;
  if( mImpl )
  {
    if( mImpl->type == MATRIX ) // type cannot change in mImpl so matrix is allocated
    {
      matrixValue = *(mImpl->matrixValue);
      converted = true;
    }
    else if( mImpl->type == ARRAY )
    {
      if(mImpl->arrayValue)
      {
        converted = CopyNumbers( *mimpl->matrixValue, matrixValue );
      }
    }
    else if( mImpl->type == MAP )
    {
      if(mImpl->mapValue)
      {
        if( GetMapComponent(*mImpl->mapValue, "e1",  *mImpl->matrixValue->AsFloat(0) ) &&
            GetMapComponent(*mImpl->mapValue, "e2",  *mImpl->matrixValue->AsFloat(1) ) &&
            GetMapComponent(*mImpl->mapValue, "e3",  *mImpl->matrixValue->AsFloat(2) ) &&
            GetMapComponent(*mImpl->mapValue, "e4",  *mImpl->matrixValue->AsFloat(3) ) &&
            GetMapComponent(*mImpl->mapValue, "e5",  *mImpl->matrixValue->AsFloat(4) ) &&
            GetMapComponent(*mImpl->mapValue, "e6",  *mImpl->matrixValue->AsFloat(5) ) &&
            GetMapComponent(*mImpl->mapValue, "e7",  *mImpl->matrixValue->AsFloat(6) ) &&
            GetMapComponent(*mImpl->mapValue, "e8",  *mImpl->matrixValue->AsFloat(7) ) &&
            GetMapComponent(*mImpl->mapValue, "e9",  *mImpl->matrixValue->AsFloat(8) ) &&
            GetMapComponent(*mImpl->mapValue, "e10", *mImpl->matrixValue->AsFloat(9) ) &&
            GetMapComponent(*mImpl->mapValue, "e11", *mImpl->matrixValue->AsFloat(10) ) &&
            GetMapComponent(*mImpl->mapValue, "e12", *mImpl->matrixValue->AsFloat(11) ) &&
            GetMapComponent(*mImpl->mapValue, "e13", *mImpl->matrixValue->AsFloat(12) ) &&
            GetMapComponent(*mImpl->mapValue, "e14", *mImpl->matrixValue->AsFloat(13) ) &&
            GetMapComponent(*mImpl->mapValue, "e15", *mImpl->matrixValue->AsFloat(14) ) &&
            GetMapComponent(*mImpl->mapValue, "e16", *mImpl->matrixValue->AsFloat(15) ) )
        {
          converted = true;
        }

        if( !converted )
        {
          if( GetMapComponent(*mImpl->mapValue, "s00", *mImpl->matrixValue->AsFloat(0) ) &&
              GetMapComponent(*mImpl->mapValue, "s01", *mImpl->matrixValue->AsFloat(1) ) &&
              GetMapComponent(*mImpl->mapValue, "s02", *mImpl->matrixValue->AsFloat(2) ) &&
              GetMapComponent(*mImpl->mapValue, "s03", *mImpl->matrixValue->AsFloat(3) ) &&
              GetMapComponent(*mImpl->mapValue, "s10", *mImpl->matrixValue->AsFloat(4) ) &&
              GetMapComponent(*mImpl->mapValue, "s11", *mImpl->matrixValue->AsFloat(5) ) &&
              GetMapComponent(*mImpl->mapValue, "s12", *mImpl->matrixValue->AsFloat(6) ) &&
              GetMapComponent(*mImpl->mapValue, "s13", *mImpl->matrixValue->AsFloat(7) ) &&
              GetMapComponent(*mImpl->mapValue, "s20", *mImpl->matrixValue->AsFloat(8) ) &&
              GetMapComponent(*mImpl->mapValue, "s21", *mImpl->matrixValue->AsFloat(9) ) &&
              GetMapComponent(*mImpl->mapValue, "s22", *mImpl->matrixValue->AsFloat(10) ) &&
              GetMapComponent(*mImpl->mapValue, "s23", *mImpl->matrixValue->AsFloat(11) ) &&
              GetMapComponent(*mImpl->mapValue, "s30", *mImpl->matrixValue->AsFloat(12) ) &&
              GetMapComponent(*mImpl->mapValue, "s31", *mImpl->matrixValue->AsFloat(13) ) &&
              GetMapComponent(*mImpl->mapValue, "s32", *mImpl->matrixValue->AsFloat(14) ) &&
              GetMapComponent(*mImpl->mapValue, "s33", *mImpl->matrixValue->AsFloat(15) ) )
          {
            converted = true;
          }
        }
      }
    }
  }
  return converted;
}

bool Property::Value::Get( Rect<int>& rectValue ) const
{
  bool converted = false;
  if( mImpl )
  {
    if( mImpl->type == RECTANGLE ) // type cannot change in mImpl so rect is allocated
    {
      rectValue = *(mImpl->rectValue);
      converted = true;
    }
    else if( mImpl->type == ARRAY )
    {
      if(mImpl->arrayValue)
      {
        converted = CopyNumbers( *mimpl->rectValue, rectValue );
      }
    }
    else if( mImpl->type == MAP )
    {
      if(mImpl->mapValue)
      {
        if( GetMapComponent(*mImpl->mapValue, "x",  *mImpl->rectValue.x ) &&
            GetMapComponent(*mImpl->mapValue, "y",  *mImpl->rectValue.y ) &&
            GetMapComponent(*mImpl->mapValue, "width",  *mImpl->rectValue.width ) &&
            GetMapComponent(*mImpl->mapValue, "height",  *mImpl->rectValue.height) )
        {
          converted = true;
        }
      }
    }
  }
  return converted;
}

bool Property::Value::Get( AngleAxis& angleAxisValue ) const
{
  bool converted = false;
  if( mImpl )
    if( mImpl->type == ROTATION ) // type cannot change in mImpl so quaternion is allocated
    {
      mImpl->quaternionValue->ToAxisAngle( angleAxisValue.axis, angleAxisValue.angle );
      converted = true;
    }
    else if( mImpl->type == ARRAY )
    {
      if(mImpl->arrayValue)
      {
        converted = CopyNumbers( *mimpl->rectValue, rectValue );
      }
    }
    else if( mImpl->type == MAP )
    {
      if(mImpl->mapValue)
      {
        if( GetMapComponent(*mImpl->mapValue, "x",  *mImpl->rectValue.x ) &&
            GetMapComponent(*mImpl->mapValue, "y",  *mImpl->rectValue.y ) &&
            GetMapComponent(*mImpl->mapValue, "width",  *mImpl->rectValue.width ) &&
            GetMapComponent(*mImpl->mapValue, "height",  *mImpl->rectValue.height) )
        {
          converted = true;
        }
      }
    }

  return converted;
}

bool Property::Value::Get( Quaternion& quaternionValue ) const
{
  bool converted = false;
  if( mImpl && (mImpl->type == ROTATION) ) // type cannot change in mImpl so quaternion is allocated
  {
    quaternionValue = *(mImpl->quaternionValue);
    converted = true;
  }
  return converted;
}

bool Property::Value::Get( std::string& stringValue ) const
{
  bool converted = false;
  if( mImpl && (mImpl->type == STRING) ) // type cannot change in mImpl so string is allocated
  {
    stringValue.assign( *(mImpl->stringValue) );
    converted = true;
  }
  return converted;
}

bool Property::Value::Get( Property::Array& arrayValue ) const
{
  bool converted = false;
  if( mImpl && (mImpl->type == ARRAY) ) // type cannot change in mImpl so array is allocated
  {
    arrayValue = *(mImpl->arrayValue);
    converted = true;
  }
  return converted;
}

bool Property::Value::Get( Property::Map& mapValue ) const
{
  bool converted = false;
  if( mImpl && (mImpl->type == MAP) ) // type cannot change in mImpl so map is allocated
  {
    mapValue = *(mImpl->mapValue);
    converted = true;
  }
  return converted;
}

Property::Array* Property::Value::GetArray() const
{
  Property::Array* array = NULL;
  if( mImpl && (mImpl->type == ARRAY) ) // type cannot change in mImpl so array is allocated
  {
    array = mImpl->arrayValue;
  }
  return array;
}

Property::Map* Property::Value::GetMap() const
{
  Property::Map* map = NULL;
  if( mImpl && (mImpl->type == MAP) ) // type cannot change in mImpl so map is allocated
  {
    map = mImpl->mapValue;
  }
  return map;
}

std::ostream& operator<<( std::ostream& stream, const Property::Value& value )
{
  if( value.mImpl )
  {
    const Property::Value::Impl& impl( *value.mImpl );

    switch( impl.type )
    {
      case Dali::Property::BOOLEAN:
      {
        stream << impl.integerValue;
        break;
      }
      case Dali::Property::FLOAT:
      {
        stream << impl.floatValue;
        break;
      }
      case Dali::Property::INTEGER:
      {
         stream << impl.integerValue;
         break;
      }
      case Dali::Property::VECTOR2:
      {
        stream << *impl.vector2Value;
        break;
      }
      case Dali::Property::VECTOR3:
      {
        stream << *impl.vector3Value;
        break;
      }
      case Dali::Property::VECTOR4:
      {
        stream << *impl.vector4Value;
        break;
      }
      case Dali::Property::MATRIX3:
      {
        stream << *impl.matrix3Value;
        break;
      }
      case Dali::Property::MATRIX:
      {
        stream << *impl.matrixValue;
        break;
      }
      case Dali::Property::RECTANGLE:
      {
        stream << *impl.rectValue;
        break;
      }
      case Dali::Property::ROTATION:
      {
        stream << *impl.quaternionValue;
        break;
      }
      case Dali::Property::STRING:
      {
        stream << *impl.stringValue;
        break;
      }
      case Dali::Property::ARRAY:
      {
        stream << "Array containing" << impl.arrayValue->Count() << " elements"; // TODO add ostream<< operator in array
        break;
      }
      case Dali::Property::MAP:
      {
        stream << "Map containing " << impl.mapValue->Count() << " elements"; // TODO add ostream<< operator in map
        break;
      }
      case Dali::Property::NONE:
      {
        stream << "undefined type";
        break;
      }
    }
  }
  else
  {
    stream << "empty type";
  }
  return stream;
}


} // namespace Dali
