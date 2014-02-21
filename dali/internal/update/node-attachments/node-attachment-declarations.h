#ifndef __DALI_INTERNAL_SCENE_GRAPH_NODE_ATTACHMENT_DECLARATIONS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_NODE_ATTACHMENT_DECLARATIONS_H__

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

// INTERNAL INCLUDES
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/common/owner-container.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class NodeAttachment;
typedef OwnerPointer<NodeAttachment> NodeAttachmentOwner;

typedef OwnerContainer< NodeAttachment* >           NodeAttachmentOwnerContainer;
typedef NodeAttachmentOwnerContainer::Iterator      NodeAttachmentIter;
typedef NodeAttachmentOwnerContainer::ConstIterator NodeAttachmentConstIter;

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_NODE_ATTACHMENT_DECLARATIONS_H__
