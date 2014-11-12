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


namespace Dali
{
namespace Internal
{

const float BezierBasisA[] = {-1.0f,  3.0f, -3.0f, 1.0f,
                               3.0f, -6.0f,  3.0f, 0.0f,
                              -3.0f,  3.0f,  0.0f, 0.0f,
                               1.0f,  0.0f,  0.0f, 0.0f};

const Matrix Path::mBasis = Matrix( BezierBasisA );

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
  if( !mPoint.empty() )
  {
    mControlPoint.push_back(point);
    mControlPoint.push_back(point);
  }

  mPoint.push_back( point );
}

unsigned int Path::GetNumberOfSegments() const
{
  return (mPoint.size()>1)?mPoint.size()-1:0;
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

void Path::FindSegmentAndProgress( float t, unsigned int& segment, float& tLocal )
{
  unsigned int numSegs = GetNumberOfSegments();

  //Find segment and local progress
  segment = t * numSegs;
  float segLength = 1.0f / numSegs;
  float segStart  = (float)segment * segLength;
  tLocal = (t - segStart) * numSegs;
  if(segment >= numSegs)
  {
    segment = numSegs-1.0f;
    tLocal = 1.0f;
  }
}

void Path::Sample( float t, Vector3& position, Vector3& tangent )
{
  unsigned int numSegs = GetNumberOfSegments();
  if(numSegs > 0)
  {
    unsigned int segment;
    float tLocal;
    FindSegmentAndProgress( t, segment, tLocal );

    const Vector3& controlPoint0 = mControlPoint[2*segment];
    const Vector3& controlPoint1 = mControlPoint[2*segment+1];
    const Vector3& point0 = mPoint[segment];
    const Vector3& point1 = mPoint[segment+1];

    if( (tLocal < 0.0f) || (tLocal > 1.0f) )
    {
      position = Vector3::ZERO;
      tangent  = Vector3::ZERO;
    }
    else if(tLocal < Math::MACHINE_EPSILON_1)
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

      //Interpolate for X
      Vector4  cVect( point0.x, controlPoint0.x, controlPoint1.x,  point1.x);

      Vector4 A = mBasis * cVect;
      position.x = sVect.Dot4(A);
      tangent.x  = sVectDerivative.Dot(Vector3(A));

      //Interpolate for Y
      cVect.x  = point0.y;
      cVect.y  = controlPoint0.y;
      cVect.z  = controlPoint1.y;
      cVect.w  = point1.y;

      A = mBasis * cVect;
      position.y = sVect.Dot4(A);
      tangent.y  = sVectDerivative.Dot(Vector3(A));

      //Interpolate for Z
      cVect.x  = point0.z;
      cVect.y  = controlPoint0.z;
      cVect.z  = controlPoint1.z;
      cVect.w  = point1.z;

      A = mBasis * cVect;
      position.z = sVect.Dot4(A);
      tangent.z  = sVectDerivative.Dot(Vector3(A));

      tangent.Normalize();
    }

  }
}

Vector3 Path::SamplePosition( float t )
{
  Vector3 position;
  unsigned int numSegs = GetNumberOfSegments();
  if(numSegs > 0)
  {
    unsigned int segment;
    float tLocal;
    FindSegmentAndProgress( t, segment, tLocal );

    const Vector3& controlPoint0 = mControlPoint[2*segment];
    const Vector3& controlPoint1 = mControlPoint[2*segment+1];
    const Vector3& point0 = mPoint[segment];
    const Vector3& point1 = mPoint[segment+1];

    if( (tLocal < 0.0f) || (tLocal > 1.0f) )
    {
      position = Vector3::ZERO;
    }
    else if(tLocal < Math::MACHINE_EPSILON_1)
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

      //Interpolate for X
      Vector4  cVect( point0.x, controlPoint0.x, controlPoint1.x,  point1.x);
      position.x = sVect.Dot4(mBasis * cVect);

      //Interpolate for Y
      cVect.x  = point0.y;
      cVect.y  = controlPoint0.y;
      cVect.z  = controlPoint1.y;
      cVect.w  = point1.y;
      position.y = sVect.Dot4(mBasis * cVect);

      //Interpolate for Z
      cVect.x  = point0.z;
      cVect.y  = controlPoint0.z;
      cVect.z  = controlPoint1.z;
      cVect.w  = point1.z;
      position.z = sVect.Dot4(mBasis * cVect);
    }
  }

  return position;
}

Vector3 Path::SampleTangent( float t )
{
  Vector3 tangent;

  unsigned int numSegs = GetNumberOfSegments();
  if(numSegs > 0)
  {
    unsigned int segment;
    float tLocal;
    FindSegmentAndProgress( t, segment, tLocal );

    const Vector3& controlPoint0 = mControlPoint[2*segment];
    const Vector3& controlPoint1 = mControlPoint[2*segment+1];
    const Vector3& point0 = mPoint[segment];
    const Vector3& point1 = mPoint[segment+1];

    if( (tLocal < 0.0f) || (tLocal > 1.0f) )
    {
      tangent  = Vector3::ZERO;
    }
    else if(tLocal < Math::MACHINE_EPSILON_1)
    {
      tangent = ( controlPoint0 - point0 ) * 3.0f;
      tangent.Normalize();
    }
    else if( (1.0 - tLocal) < Math::MACHINE_EPSILON_1)
    {
      tangent = ( point1 - controlPoint1 ) * 3.0f;
      tangent.Normalize();
    }
    else
    {
      const Vector3 sVectDerivative(3.0f*tLocal*tLocal, 2.0f*tLocal, 1.0f );

      //Interpolate for X
      Vector4  cVect( point0.x, controlPoint0.x, controlPoint1.x,  point1.x);
      tangent.x  = sVectDerivative.Dot(Vector3(mBasis * cVect));

      //Interpolate for Y
      cVect.x  = point0.y;
      cVect.y  = controlPoint0.y;
      cVect.z  = controlPoint1.y;
      cVect.w  = point1.y;
      tangent.y  = sVectDerivative.Dot(Vector3(mBasis * cVect));

      //Interpolate for Z
      cVect.x  = point0.z;
      cVect.y  = controlPoint0.z;
      cVect.z  = controlPoint1.z;
      cVect.w  = point1.z;
      tangent.z  = sVectDerivative.Dot(Vector3(mBasis * cVect));

      tangent.Normalize();
    }

  }

  return tangent;
}

Vector3& Path::GetPoint( size_t index )
{
  if( index < mPoint.size() )
  {
    return mPoint[index];
  }

  return mPoint.back();
}

Vector3& Path::GetControlPoint( size_t index )
{
  if( index < mControlPoint.size() )
  {
    return mControlPoint[index];
  }

  return mControlPoint.back();
}


} // Internal
} // Dali


