#ifndef __DALI_INTEGRATION_API_RESOURCE_TRACKING_H__
#define __DALI_INTEGRATION_API_RESOURCE_TRACKING_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

namespace Dali
{

namespace ResourceTrackMessage
{

enum Type
{
  NONE,
  LOAD_BITMAP,
  CREATE_BUFFER,
  DELETE_TEXTURE,
  DELETE_BUFFER,
  UPLOAD_TEXTURE
};

} // namespace ResourceTrackingMessage

} // namespace Dali

#endif // __DALI_INTEGRATION_API_RESOURCE_TRACKING_H__