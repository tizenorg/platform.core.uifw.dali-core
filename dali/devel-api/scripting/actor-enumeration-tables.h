#ifndef __DALI_ACTOR_ENUMERATION_TABLES_H__
#define __DALI_ACTOR_ENUMERATION_TABLES_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/internal/event/common/property-helper.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/images/image-operations.h>

namespace Dali
{

namespace EnumerationTables
{
Dali::Scripting::StringEnum;

DALI_ENUM_TO_STRING_TABLE_BEGIN( ColorMode )
DALI_ENUM_TO_STRING( USE_OWN_COLOR )
DALI_ENUM_TO_STRING( USE_PARENT_COLOR )
DALI_ENUM_TO_STRING( USE_OWN_MULTIPLY_PARENT_COLOR )
DALI_ENUM_TO_STRING( USE_OWN_MULTIPLY_PARENT_ALPHA )
DALI_ENUM_TO_STRING_TABLE_END( ColorMode )

DALI_ENUM_TO_STRING_TABLE_BEGIN( PositionInheritanceMode )
DALI_ENUM_TO_STRING( INHERIT_PARENT_POSITION )
DALI_ENUM_TO_STRING( USE_PARENT_POSITION )
DALI_ENUM_TO_STRING( USE_PARENT_POSITION_PLUS_LOCAL_POSITION )
DALI_ENUM_TO_STRING( DONT_INHERIT_POSITION )
DALI_ENUM_TO_STRING_TABLE_END( PositionInheritanceMode )

namespace FittingMode
{
using namespace Dali::FittingMode;
DALI_ENUM_TO_STRING_TABLE_BEGIN( FittingMode )
DALI_ENUM_TO_STRING( SHRINK_TO_FIT )
DALI_ENUM_TO_STRING( SCALE_TO_FILL )
DALI_ENUM_TO_STRING( FIT_WIDTH )
DALI_ENUM_TO_STRING( FIT_HEIGHT )
DALI_ENUM_TO_STRING_TABLE_END( FittingMode )
} // FittingMode

namespace SamplingMode
{
using namespace Dali::SamplingMode;
DALI_ENUM_TO_STRING_TABLE_BEGIN( SamplingMode )
DALI_ENUM_TO_STRING( BOX )
DALI_ENUM_TO_STRING( NEAREST )
DALI_ENUM_TO_STRING( BOX_THEN_NEAREST )
DALI_ENUM_TO_STRING( BOX_THEN_LINEAR )
DALI_ENUM_TO_STRING( NO_FILTER )
DALI_ENUM_TO_STRING( DONT_CARE )
DALI_ENUM_TO_STRING_TABLE_END( SamplingMode )
} // SamplingMode

namespace ResizePolicy
{
using namespace Dali::ResizePolicy;
DALI_ENUM_TO_STRING_TABLE_BEGIN( Type )
DALI_ENUM_TO_STRING( FIXED )
DALI_ENUM_TO_STRING( USE_NATURAL_SIZE )
DALI_ENUM_TO_STRING( FILL_TO_PARENT )
DALI_ENUM_TO_STRING( SIZE_RELATIVE_TO_PARENT )
DALI_ENUM_TO_STRING( SIZE_FIXED_OFFSET_FROM_PARENT )
DALI_ENUM_TO_STRING( FIT_TO_CHILDREN )
DALI_ENUM_TO_STRING( DIMENSION_DEPENDENCY )
DALI_ENUM_TO_STRING( USE_ASSIGNED_SIZE )
DALI_ENUM_TO_STRING_TABLE_END( Type )
} // ResizePolicy

namespace SizeScalePolicy
{
using namespace Dali::SizeScalePolicy;
DALI_ENUM_TO_STRING_TABLE_BEGIN( Type )
DALI_ENUM_TO_STRING( USE_SIZE_SET )
DALI_ENUM_TO_STRING( FIT_WITH_ASPECT_RATIO )
DALI_ENUM_TO_STRING( FILL_WITH_ASPECT_RATIO )
DALI_ENUM_TO_STRING_TABLE_END( Type )
} // SizeScalePolicy

namespace DrawMode
{
using namespace Dali::DrawMode;
DALI_ENUM_TO_STRING_TABLE_BEGIN( Type )
DALI_ENUM_TO_STRING( NORMAL )
DALI_ENUM_TO_STRING( OVERLAY_2D )
DALI_ENUM_TO_STRING( STENCIL )
DALI_ENUM_TO_STRING_TABLE_END( Type )
} // DrawMode

namespace ClippingMode
{
using namespace Dali::ClippingMode;
DALI_ENUM_TO_STRING_TABLE_BEGIN( Type )
DALI_ENUM_TO_STRING( CLIPPING_DISABLED )
DALI_ENUM_TO_STRING( CLIPPING_ENABLED )
DALI_ENUM_TO_STRING( CLIP_AND_RENDER )
DALI_ENUM_TO_STRING_TABLE_END( Type )
} // ClippingMode


} // EnumerationTables

} // Dali

#endif // __DALI_ACTOR_ENUMERATION_TABLES_H__
