#ifndef __DALI_BLENDING_H__
#define __DALI_BLENDING_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

namespace BlendMode
{
/**
 * @brief Blend mode.
 * @SINCE_1_0.0
 * @remarks This is an experimental feature and might not be supported in the next release.
 * We do recommend not to use it.
 */
enum Type
{
  OFF,  ///< Blending is disabled. @SINCE_1_0.0
  AUTO, ///< Blending is enabled if there is alpha channel. This is the default mode. @SINCE_1_0.0
  ON    ///< Blending is enabled. @SINCE_1_0.0
};

} //namespace BlendingMode

namespace BlendingMode = BlendMode; // @DEPRECATED_1.1.32. Use BlendMode instead.

namespace BlendFactor
{
/**
 * @brief Blend Factor.
 *
 * @SINCE_1_0.0
 * @remarks This is an experimental feature and might not be supported in the next release.
 * We do recommend not to use it.
 */
enum Type
{
  ZERO                     = 0,  ///< ZERO @SINCE_1_0.0
  ONE                      = 1,  ///< ONE @SINCE_1_0.0
  SRC_COLOR                = 0x0300,  ///< SRC_COLOR @SINCE_1_0.0
  ONE_MINUS_SRC_COLOR      = 0x0301,  ///< ONE_MINUS_SRC_COLOR @SINCE_1_0.0
  SRC_ALPHA                = 0x0302,  ///< SRC_ALPHA @SINCE_1_0.0
  ONE_MINUS_SRC_ALPHA      = 0x0303,  ///< ONE_MINUS_SRC_ALPHA @SINCE_1_0.0
  DST_ALPHA                = 0x0304,  ///< DST_ALPHA @SINCE_1_0.0
  ONE_MINUS_DST_ALPHA      = 0x0305,  ///< ONE_MINUS_DST_ALPHA @SINCE_1_0.0
  DST_COLOR                = 0x0306,  ///< DST_COLOR @SINCE_1_0.0
  ONE_MINUS_DST_COLOR      = 0x0307,  ///< ONE_MINUS_DST_COLOR @SINCE_1_0.0
  SRC_ALPHA_SATURATE       = 0x0308,  ///< SRC_ALPHA_SATURATE @SINCE_1_0.0
  CONSTANT_COLOR           = 0x8001,  ///< CONSTANT_COLOR @SINCE_1_0.0
  ONE_MINUS_CONSTANT_COLOR = 0x8002,  ///< ONE_MINUS_CONSTANT_COLOR @SINCE_1_0.0
  CONSTANT_ALPHA           = 0x8003,  ///< CONSTANT_ALPHA @SINCE_1_0.0
  ONE_MINUS_CONSTANT_ALPHA = 0x8004  ///< ONE_MINUS_CONSTANT_ALPHA @SINCE_1_0.0
};

} // namespace BlendFactor

namespace BlendingFactor = BlendFactor;

namespace BlendEquation
{
/**
 * @brief Blend Equation.
 *
 * @SINCE_1_0.0
 * @remarks This is an experimental feature and might not be supported in the next release.
 * We do recommend not to use it.
 */
enum Type
{
  ADD              = 0x8006,  ///< The source and destination colors are added to each other. @SINCE_1_0.0
  SUBTRACT         = 0x800A,  ///< Subtracts the destination from the source. @SINCE_1_0.0
  REVERSE_SUBTRACT = 0x800B  ///< Subtracts the source from the destination. @SINCE_1_0.0
};

} // namespace BlendEquation

namespace BlendingEquation = BlendEquation;

DALI_IMPORT_API extern const BlendFactor::Type   DEFAULT_BLEND_FACTOR_SRC_RGB;    ///< BlendFactor::SRC_ALPHA
DALI_IMPORT_API extern const BlendFactor::Type   DEFAULT_BLEND_FACTOR_DEST_RGB;   ///< BlendFactor::ONE_MINUS_SRC_ALPHA
DALI_IMPORT_API extern const BlendFactor::Type   DEFAULT_BLEND_FACTOR_SRC_ALPHA;  ///< BlendFactor::ONE
DALI_IMPORT_API extern const BlendFactor::Type   DEFAULT_BLEND_FACTOR_DEST_ALPHA; ///< BlendFactor::ONE_MINUS_SRC_ALPHA

DALI_IMPORT_API extern const BlendEquation::Type DEFAULT_BLEND_EQUATION_RGB;     ///< BlendingEquation::ADD
DALI_IMPORT_API extern const BlendEquation::Type DEFAULT_BLEND_EQUATION_ALPHA;   ///< BlendingEquation::ADD

// @DEPRECATED_1.1.32. 
DALI_IMPORT_API extern const BlendingFactor::Type   DEFAULT_BLENDING_SRC_FACTOR_RGB;    ///< @DEPRECATED_1.1.32. Use DEFAULT_BLEND_FACTOR_SRC_RGB instead.
DALI_IMPORT_API extern const BlendingFactor::Type   DEFAULT_BLENDING_DEST_FACTOR_RGB;   ///< @DEPRECATED_1.1.32. Use DEFAULT_BLEND_FACTOR_DEST_RGB instead.
DALI_IMPORT_API extern const BlendingFactor::Type   DEFAULT_BLENDING_SRC_FACTOR_ALPHA;  ///< @DEPRECATED_1.1.32. Use DEFAULT_BLEND_FACTOR_SRC_ALPHA values instead.
DALI_IMPORT_API extern const BlendingFactor::Type   DEFAULT_BLENDING_DEST_FACTOR_ALPHA; ///< @DEPRECATED_1.1.32. Use DEFAULT_BLEND_FACTOR_DEST_ALPHA values instead.

DALI_IMPORT_API extern const BlendingEquation::Type DEFAULT_BLENDING_EQUATION_RGB;     ///< @DEPRECATED_1.1.32. Use DEFAULT_BLEND_EQUATION_RGB instead.
DALI_IMPORT_API extern const BlendingEquation::Type DEFAULT_BLENDING_EQUATION_ALPHA;   ///< @DEPRECATED_1.1.32. Use DEFAULT_BLEND_EQUATION_ALPHA values instead.

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_BLENDING_H__
