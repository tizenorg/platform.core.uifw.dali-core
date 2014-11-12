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

}//Unnamed namespace

namespace Dali
{
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

void Path::AddPoint(const Vector3& point )
{
  if( mPoint.Size() > 0 )
  {
    //If there is at least one point, each new interpolation point will generate two new control points.
    mControlPoint.Resize( mControlPoint.Size()+2 );
  }

  mPoint.PushBack( point );
}

unsigned int Path::GetNumberOfSegments() const
{
  return (mPoint.Size()>1)?mPoint.Size()-1:0;
}

void Path::GenerateControlPoints( float curvature )
{
  unsigned int numSegments = GetNumberOfSegments();
  DALI_ASSERT_ALWAYS( numSegments > 0 && "Need at least 1 segment to generate control points" ); // need at least 1 segment

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

bool Path::FindSegmentAndProgress( float t, unsigned int& segment, float& tLocal ) const
{
  //Find segment and local progress
  unsigned int numSegs = GetNumberOfSegments();
  if( numSegs == 0 )
  {
    return false;
  }

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

  return true;
}

void Path::Sample( float t, Vector3& position, Vector3& tangent ) const
{
  unsigned int segment;
  float tLocal;
  if( FindSegmentAndProgress( t, segment, tLocal ) )
  {
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
}

Vector3 Path::SamplePosition( float t ) const
{
  Vector3 position(0.0f,0.0f,0.0f);
  unsigned int segment;
  float tLocal;
  if( FindSegmentAndProgress( t, segment, tLocal ) )
  {
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
  }

  return position;
}

Vector3 Path::SampleTangent( float t ) const
{
  Vector3 tangent(0.0f,0.0f,0.0f);

  unsigned int segment;
  float tLocal;
  if( FindSegmentAndProgress( t, segment, tLocal ) )
  {
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
  }

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


