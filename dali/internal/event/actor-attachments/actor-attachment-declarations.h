#ifndef __DALI_INTERNAL_ACTOR_ATTACHMENT_DECLARATIONS_H__
#define __DALI_INTERNAL_ACTOR_ATTACHMENT_DECLARATIONS_H__

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
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

namespace Internal
{

class ActorAttachment;
class CameraAttachment;
class ImageAttachment;
class RendererAttachment;

typedef IntrusivePtr<ActorAttachment>     ActorAttachmentPtr;
typedef IntrusivePtr<CameraAttachment>    CameraAttachmentPtr;
typedef IntrusivePtr<ImageAttachment>     ImageAttachmentPtr;
typedef IntrusivePtr<RendererAttachment>  RendererAttachmentPtr;

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACTOR_ATTACHMENT_DECLARATIONS_H__
