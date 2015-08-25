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
#include <dali/public-api/animation/alpha-function.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace // Unnamed namespace
{

/**
 * Helper function to evaluate a cubic bezier curve assuming first point is at 0.0 and last point is at 1.0
 * @param[in] p0 First control point of the bezier curve
 * @param[in] p1 Second control point of the bezier curve
 * @param[in] t A floating point value between 0.0 and 1.0
 * @return Value of the curve at progress t
 */
inline float EvaluateCubicBezier( float p0, float p1, float t )
{
  float tSquare = t * t;
  return 3.f * ( 1.f - t ) * ( 1.f - t ) * t * p0 + 3.f * ( 1.f - t ) * tSquare * p1 + tSquare * t;
}

} // Unnamed namespace

AlphaFunction::AlphaFunction()
:mBezierControlPoints(Vector4::ZERO),
 mCustom(0),
 mBuiltin(DEFAULT),
 mMode(BUILTIN_FUNCTION)
{}

AlphaFunction::AlphaFunction( BuiltinFunction function)
:mBezierControlPoints(Vector4::ZERO),
 mCustom(0),
 mBuiltin(function),
 mMode(BUILTIN_FUNCTION)
{}

AlphaFunction::AlphaFunction( AlphaFunctionPrototype function)
:mBezierControlPoints(Vector4::ZERO),
 mCustom(function),
 mBuiltin(DEFAULT),
 mMode(CUSTOM_FUNCTION)
{}

AlphaFunction::AlphaFunction( const Vector2& controlPoint0, const Vector2& controlPoint1 )
:mBezierControlPoints(Vector4(Clamp(controlPoint0.x,0.0f,1.0f),controlPoint0.y,
                              Clamp(controlPoint1.x,0.0f,1.0f),controlPoint1.y)),
 mCustom(0),
 mBuiltin(DEFAULT),
 mMode(BEZIER)
{
}

Vector4 AlphaFunction::GetBezierControlPoints() const
{
  return mBezierControlPoints;
}

AlphaFunctionPrototype AlphaFunction::GetCustomFunction() const
{
  return mCustom;
}

AlphaFunction::BuiltinFunction AlphaFunction::GetBuiltinFunction() const
{
  return mBuiltin;
}

AlphaFunction::Mode AlphaFunction::GetMode() const
{
  return mMode;
}

float AlphaFunction::Apply( float progress ) const
{
  float result = progress;

  switch( mMode )
  {
    case AlphaFunction::BUILTIN_FUNCTION:
    {
      switch(mBuiltin)
      {
        case AlphaFunction::DEFAULT:
        case AlphaFunction::LINEAR:
        {
          break;
        }
        case AlphaFunction::REVERSE:
        {
          result = 1.f - progress;
          break;
        }
        case AlphaFunction::EASE_IN_SQUARE:
        {
          result = progress * progress;
          break;
        }
        case AlphaFunction::EASE_OUT_SQUARE:
        {
          result = 1.f - ( 1.f - progress ) * ( 1.f - progress );
          break;
        }
        case AlphaFunction::EASE_IN:
        {
          result = progress * progress * progress;
          break;
        }
        case AlphaFunction::EASE_OUT:
        {
          result = ( progress-1.f ) * ( progress-1.f ) * ( progress-1.f ) + 1.f;
          break;
        }
        case AlphaFunction::EASE_IN_OUT:
        {
          result = progress * progress * ( 3.f - 2.f * progress );
          break;
        }
        case AlphaFunction::EASE_IN_SINE:
        {
          result = -1.f * cosf( progress * Math::PI_2 ) + 1.f;
          break;
        }
        case AlphaFunction::EASE_OUT_SINE:
        {
          result = sinf( progress * Math::PI_2 );
          break;
        }
        case AlphaFunction::EASE_IN_OUT_SINE:
        {
          result = -0.5f * ( cosf( Math::PI * progress ) - 1.f );
          break;
        }
        case AlphaFunction::BOUNCE:
        {
          result = sinf( progress * Math::PI );
          break;
        }
        case AlphaFunction::SIN:
        {
          result = 0.5f - cosf( progress * 2.f * Math::PI ) * 0.5f;
          break;
        }
        case AlphaFunction::EASE_OUT_BACK:
        {
          const float sqrt2 = 1.70158f;
          progress -= 1.f;
          result = 1.f + progress * progress * ( ( sqrt2 + 1.f ) * progress + sqrt2 );
          break;
        }
      }
      break;
    }
    case CUSTOM_FUNCTION:
    {
      if( mCustom )
      {
        result = mCustom( progress );
      }
      break;
    }
    case BEZIER:
    {
      //If progress is very close to 0 or very close to 1 we don't need to evaluate the curve as the result will
      //be almost 0 or almost 1 respectively
      if( ( progress > Math::MACHINE_EPSILON_1 ) && ( ( 1.f - progress ) > Math::MACHINE_EPSILON_1 ) )
      {
        static const float tolerance = 0.001f;  //10 iteration max

        //Perform a binary search on the curve
        float lowerBound( 0.f );
        float upperBound( 1.f );
        float currentT( 0.5f );
        float currentX = EvaluateCubicBezier( mBezierControlPoints.x, mBezierControlPoints.z, currentT );
        while( fabs( progress - currentX ) > tolerance )
        {
          if( progress > currentX )
          {
            lowerBound = currentT;
          }
          else
          {
            upperBound = currentT;
          }
          currentT = ( upperBound + lowerBound ) * 0.5f;
          currentX = EvaluateCubicBezier( mBezierControlPoints.x, mBezierControlPoints.z, currentT );
        }
        result = EvaluateCubicBezier( mBezierControlPoints.y, mBezierControlPoints.w, currentT );
      }
      break;
    }
  }

  return result;
}

} // namespace Dali
