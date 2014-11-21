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
#include <dali/internal/event/animation/path-impl.h>

namespace
{
const float BezierBasisCoeff[] = {  -1.0f,  3.0f, -3.0f, 1.0f,
                                     3.0f, -6.0f,  3.0f, 0.0f,
                                    -3.0f,  3.0f,  0.0f, 0.0f,
                                     1.0f,  0.0f,  0.0f, 0.0f  };

const Dali::Matrix BezierBasis = Dali::Matrix( BezierBasisCoeff );

struct PropertyDetails
{
  std::string name;           ///< The name of the property.
  Dali::Property::Type type;  ///< The property type.
  bool writable:1;            ///< Whether the property is writable
  bool animatable:1;          ///< Whether the property is animatable.
  bool constraintInput:1;     ///< Whether the property can be used as an input to a constraint.
};

const PropertyDetails DEFAULT_PROPERTY_DETAILS[] =  {{"points", Dali::Property::ARRAY, true, false, false },
                                               {"control-points", Dali::Property::ARRAY, true, false, false },
                                              };

const int DEFAULT_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_DETAILS ) / sizeof( PropertyDetails );

}//Unnamed namespace

namespace Dali
{
const Property::Index Path::POINTS              = 0;
const Property::Index Path::CONTROL_POINTS      = 1;

namespace Internal
{

Path* Path::New()
{
  return new Path();
}

Path::Path()
{
}

Path::~Path()
{
}

unsigned int Path::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void Path::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const std::string& Path::GetDefaultPropertyName(Property::Index index) const
{
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }
  else
  {
    // index out of range
    static const std::string INVALID_PROPERTY_NAME;
    return INVALID_PROPERTY_NAME;
  }
}

Property::Index Path::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  for( int i(0); i<DEFAULT_PROPERTY_COUNT; ++i )
  {
    if ( name == DEFAULT_PROPERTY_DETAILS[0].name )
    {
      index = i;
      break;
    }
  }
  return index;
}

Property::Type Path::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }
  else
  {
    // index out of range
    return Property::NONE;
  }
}

Property::Value Path::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;
  switch ( index )
  {
    case Dali::Path::POINTS:
    {
      size_t pointCount( mPoint.Size() );
      for( size_t i(0); i!=pointCount; ++i )
      {
        value.AppendItem( mPoint[i] );
      }
      break;
    }
    case Dali::Path::CONTROL_POINTS:
    {
      size_t controlpointCount( mControlPoint.Size() );
      for( size_t i(0); i!=controlpointCount; ++i )
      {
        value.AppendItem( mControlPoint[i] );
      }
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false && "Path::Property is out of bounds");
      break;
    }
  }

  return value;
}

void Path::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  switch ( index )
  {
    case Dali::Path::POINTS:
    {
      Property::Array propertyArray;
      propertyValue.Get(propertyArray);

      size_t propertyArrayCount = propertyArray.size();
      mPoint.Resize( propertyArrayCount );
      for( size_t i(0); i!=propertyArrayCount; ++i )
      {
        propertyArray[i].Get( mPoint[i]);
      }
      break;
    }
    case Dali::Path::CONTROL_POINTS:
    {
      Property::Array propertyArray;
      propertyValue.Get(propertyArray);

      size_t propertyArrayCount = propertyArray.size();
      mControlPoint.Resize( propertyArrayCount );
      for( size_t i(0); i!=propertyArrayCount; ++i )
      {
        propertyArray[i].Get( mControlPoint[i]);
      }
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false && "Path::Property is out of bounds");
      break;
    }
  }
}

bool Path::IsDefaultPropertyWritable(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].writable;
  }
  else
  {
    return false;
  }
}

bool Path::IsDefaultPropertyAnimatable(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].animatable;
  }
  else
  {
    return false;
  }
}

bool Path::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].constraintInput;
  }
  else
  {
    return false;
  }
}

void Path::AddPoint(const Vector3& point )
{
  mPoint.PushBack( point );
}

void Path::AddControlPoint(const Vector3& point )
{
  mControlPoint.PushBack( point );
}

unsigned int Path::GetNumberOfSegments() const
{
  return (mPoint.Size()>1)?mPoint.Size()-1:0;
}

void Path::GenerateControlPoints( float curvature )
{
  unsigned int numSegments = GetNumberOfSegments();
  DALI_ASSERT_ALWAYS( numSegments > 0 && "Need at least 1 segment to generate control points" ); // need at least 1 segment

  mControlPoint.Resize( numSegments * 2);

  for( unsigned int i(0); i<numSegments; ++i )
  {
    Vector3 p1 = mPoint[i];
    Vector3 p2 = mPoint[i+1];

    Vector3 p0;
    if( i == 0 )
    {
      p0 = p1 - (p2 - p1)/8.0f;
    }
    else
    {
      p0 = mPoint[i-1];
    }

    Vector3 p3;
    if( i == numSegments - 1)
    {
      p3 = p2 - (p1 - p2)/8.0f;
    }
    else
    {
      p3 = mPoint[i+2];
    }

    Vector3 p0p1 = p1 - p0;
    Vector3 p1p2 = p2 - p1;
    Vector3 p2p3 = p3 - p2;

    float length = p1p2.Length();

    Vector3 tangentOut = ( p0p1*length + p1p2*p0p1.Length() ) * 0.5f;
    tangentOut.Normalize();

    Vector3 tangentIn = ( p1p2*p2p3.Length() + p2p3*length ) * 0.5f;
    tangentIn.Normalize();

    length *= curvature;
    mControlPoint[2*i] =   p1 + tangentOut*length;
    mControlPoint[2*i+1] = p2 - tangentIn*length;
  }
}

void Path::FindSegmentAndProgress( float t, unsigned int& segment, float& tLocal ) const
{
  //Find segment and local progress
  unsigned int numSegs = GetNumberOfSegments();

  if( t <= 0.0f )
  {
    segment = 0;
    tLocal = 0.0f;
  }
  else if( t >= 1.0f )
  {
    segment = numSegs-1;
    tLocal = 1.0f;
  }
  else
  {
    segment = t * numSegs;
    float segLength = 1.0f / numSegs;
    float segStart  = (float)segment * segLength;
    tLocal = (t - segStart) * numSegs;
  }
}

void Path::Sample( float t, Vector3& position, Vector3& tangent ) const
{
  DALI_ASSERT_ALWAYS(mPoint.Size() > 1 && mControlPoint.Size() == (mPoint.Size()-1)*2 && "Spline not fully initialized" );

  unsigned int segment;
  float tLocal;
  FindSegmentAndProgress( t, segment, tLocal );

  //Get points and control points in the segment
  const Vector3& controlPoint0 = mControlPoint[2*segment];
  const Vector3& controlPoint1 = mControlPoint[2*segment+1];
  const Vector3& point0 = mPoint[segment];
  const Vector3& point1 = mPoint[segment+1];

  if(tLocal < Math::MACHINE_EPSILON_1)
  {
    position = point0;
    tangent = ( controlPoint0 - point0 ) * 3.0f;
    tangent.Normalize();
  }
  else if( (1.0 - tLocal) < Math::MACHINE_EPSILON_1)
  {
    position = point1;
    tangent = ( point1 - controlPoint1 ) * 3.0f;
    tangent.Normalize();
  }
  else
  {
    const Vector4 sVect(tLocal*tLocal*tLocal, tLocal*tLocal, tLocal, 1.0f );
    const Vector3 sVectDerivative(3.0f*tLocal*tLocal, 2.0f*tLocal, 1.0f );

    //X
    Vector4  cVect( point0.x, controlPoint0.x, controlPoint1.x,  point1.x);

    Vector4 A = BezierBasis * cVect;
    position.x = sVect.Dot4(A);
    tangent.x  = sVectDerivative.Dot(Vector3(A));

    //Y
    cVect.x  = point0.y;
    cVect.y  = controlPoint0.y;
    cVect.z  = controlPoint1.y;
    cVect.w  = point1.y;

    A = BezierBasis * cVect;
    position.y = sVect.Dot4(A);
    tangent.y  = sVectDerivative.Dot(Vector3(A));

    //Z
    cVect.x  = point0.z;
    cVect.y  = controlPoint0.z;
    cVect.z  = controlPoint1.z;
    cVect.w  = point1.z;

    A = BezierBasis * cVect;
    position.z = sVect.Dot4(A);
    tangent.z  = sVectDerivative.Dot(Vector3(A));

    tangent.Normalize();
  }
}

Vector3 Path::SamplePosition( float t ) const
{
  DALI_ASSERT_ALWAYS(mPoint.Size() > 1 && mControlPoint.Size() == (mPoint.Size()-1)*2 && "Spline not fully initialized" );

  Vector3 position(0.0f,0.0f,0.0f);
  unsigned int segment;
  float tLocal;
  FindSegmentAndProgress( t, segment, tLocal );

  const Vector3& controlPoint0 = mControlPoint[2*segment];
  const Vector3& controlPoint1 = mControlPoint[2*segment+1];
  const Vector3& point0 = mPoint[segment];
  const Vector3& point1 = mPoint[segment+1];

  if(tLocal < Math::MACHINE_EPSILON_1)
  {
    position = point0;
  }
  else if( (1.0 - tLocal) < Math::MACHINE_EPSILON_1)
  {
    position = point1;
  }
  else
  {
    const Vector4 sVect(tLocal*tLocal*tLocal, tLocal*tLocal, tLocal, 1.0f );

    //X
    Vector4  cVect( point0.x, controlPoint0.x, controlPoint1.x,  point1.x);
    position.x = sVect.Dot4(BezierBasis * cVect);

    //Y
    cVect.x  = point0.y;
    cVect.y  = controlPoint0.y;
    cVect.z  = controlPoint1.y;
    cVect.w  = point1.y;
    position.y = sVect.Dot4(BezierBasis * cVect);

    //Z
    cVect.x  = point0.z;
    cVect.y  = controlPoint0.z;
    cVect.z  = controlPoint1.z;
    cVect.w  = point1.z;
    position.z = sVect.Dot4(BezierBasis * cVect);
  }

  return position;
}

Vector3 Path::SampleTangent( float t ) const
{
  DALI_ASSERT_ALWAYS(mPoint.Size() > 1 && mControlPoint.Size() == (mPoint.Size()-1)*2 && "Spline not fully initialized" );

  Vector3 tangent(0.0f,0.0f,0.0f);
  unsigned int segment;
  float tLocal;
  FindSegmentAndProgress( t, segment, tLocal );

  const Vector3& controlPoint0 = mControlPoint[2*segment];
  const Vector3& controlPoint1 = mControlPoint[2*segment+1];
  const Vector3& point0 = mPoint[segment];
  const Vector3& point1 = mPoint[segment+1];

  if(tLocal < Math::MACHINE_EPSILON_1)
  {
    tangent = ( controlPoint0 - point0 ) * 3.0f;
  }
  else if( (1.0f - tLocal) < Math::MACHINE_EPSILON_1)
  {
    tangent = ( point1 - controlPoint1 ) * 3.0f;
  }
  else
  {
    const Vector3 sVectDerivative(3.0f*tLocal*tLocal, 2.0f*tLocal, 1.0f );

    //X
    Vector4  cVect( point0.x, controlPoint0.x, controlPoint1.x,  point1.x);
    tangent.x  = sVectDerivative.Dot(Vector3(BezierBasis * cVect));

    //Y
    cVect.x  = point0.y;
    cVect.y  = controlPoint0.y;
    cVect.z  = controlPoint1.y;
    cVect.w  = point1.y;
    tangent.y  = sVectDerivative.Dot(Vector3(BezierBasis * cVect));

    //Z
    cVect.x  = point0.z;
    cVect.y  = controlPoint0.z;
    cVect.z  = controlPoint1.z;
    cVect.w  = point1.z;
    tangent.z  = sVectDerivative.Dot(Vector3(BezierBasis * cVect));
  }

  tangent.Normalize();
  return tangent;
}

Vector3& Path::GetPoint( size_t index )
{
  DALI_ASSERT_ALWAYS( index < mPoint.Size() && "Path: Point index out of bounds" );

  return mPoint[index];
}

Vector3& Path::GetControlPoint( size_t index )
{
  DALI_ASSERT_ALWAYS( index < mControlPoint.Size() && "Path: Control Point index out of bounds" );

  return mControlPoint[index];
}


} // Internal
} // Dali


