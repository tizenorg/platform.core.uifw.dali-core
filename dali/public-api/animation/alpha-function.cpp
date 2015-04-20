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
#include <dali/public-api/animation/alpha-function.h>

// INTERNAL INCLUDES

namespace Dali
{

AlphaFunction::AlphaFunction()
:mBezierControlPoints(Vector4::ZERO),
 mCustom(0),
 mPredefined(AlphaFunction::DEFAULT),
 mMode( AlphaFunction::PREDEFINED_FUNCTION)
{}

AlphaFunction::AlphaFunction( PredefinedFunction function)
:mBezierControlPoints(Vector4::ZERO),
 mCustom(0),
 mPredefined(function),
 mMode( AlphaFunction::PREDEFINED_FUNCTION)
{}

AlphaFunction::AlphaFunction( AlphaFunctionPrototype function)
:mBezierControlPoints(Vector4::ZERO),
 mCustom(function),
 mPredefined(AlphaFunction::DEFAULT),
 mMode( AlphaFunction::CUSTOM_FUNCTION)
{}

AlphaFunction::AlphaFunction( const Dali::Vector2& cp0, const Dali::Vector2& cp1 )
:mBezierControlPoints(Vector4(Clamp(cp0.x,0.0f,1.0f),cp0.y,Clamp(cp1.x,0.0f,1.0f),cp1.y)),
 mCustom(0),
 mPredefined(AlphaFunction::DEFAULT),
 mMode( AlphaFunction::BEZIER)
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

AlphaFunction::PredefinedFunction AlphaFunction::GetPredefinedFunction() const
{
  return mPredefined;
}

AlphaFunction::Mode AlphaFunction::GetMode() const
{
  return mMode;
}

} // namespace Dali
