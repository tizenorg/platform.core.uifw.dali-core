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
#include <dali/public-api/actors/blending.h>

namespace Dali
{

const BlendFactor::Type   DEFAULT_BLEND_FACTOR_SRC_RGB(    BlendFactor::SRC_ALPHA );
const BlendFactor::Type   DEFAULT_BLEND_FACTOR_DEST_RGB(   BlendFactor::ONE_MINUS_SRC_ALPHA );
const BlendFactor::Type   DEFAULT_BLEND_FACTOR_SRC_ALPHA(  BlendFactor::ONE );
const BlendFactor::Type   DEFAULT_BLEND_FACTOR_DEST_ALPHA( BlendFactor::ONE_MINUS_SRC_ALPHA );

const BlendEquation::Type DEFAULT_BLEND_EQUATION_RGB(   BlendEquation::ADD );
const BlendEquation::Type DEFAULT_BLEND_EQUATION_ALPHA( BlendEquation::ADD );

// @DEPRECATED_1.1.32.
const BlendFactor::Type   DEFAULT_BLENDING_SRC_FACTOR_RGB(    BlendFactor::SRC_ALPHA );
const BlendFactor::Type   DEFAULT_BLENDING_DEST_FACTOR_RGB(   BlendFactor::ONE_MINUS_SRC_ALPHA );
const BlendFactor::Type   DEFAULT_BLENDING_SRC_FACTOR_ALPHA(  BlendFactor::ONE );
const BlendFactor::Type   DEFAULT_BLENDING_DEST_FACTOR_ALPHA( BlendFactor::ONE_MINUS_SRC_ALPHA );

const BlendEquation::Type DEFAULT_BLENDING_EQUATION_RGB(   BlendEquation::ADD );
const BlendEquation::Type DEFAULT_BLENDING_EQUATION_ALPHA( BlendEquation::ADD );

} // namespace Dali
