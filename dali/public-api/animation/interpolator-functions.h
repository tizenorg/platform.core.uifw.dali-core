#ifndef __DALI_INTERPOLATOR_FUNCTIONS_H__
#define __DALI_INTERPOLATOR_FUNCTIONS_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/property.h>

namespace Dali
{

/**
 * @brief Any interpolator function.
 */
typedef Any AnyInterpolator;

/**
 * @brief Retrieve an interpolator function for a property.
 *
 * This can be cast to Dali::function<P (const P&, const P&, float)> where P corresponds to the property type.
 * @param[in] type The property type for which to get an interpolator
 * @return The interpolator function.
 */
AnyInterpolator DALI_IMPORT_API GetDefaultInterpolator(Property::Type type);

} // namespace Dali

#endif // __DALI_INTERPOLATOR_FUNCTIONS_H__
