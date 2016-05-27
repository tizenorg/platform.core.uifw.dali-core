#ifndef DALI_INTERNAL_NEW_TEXTURE_H
#define DALI_INTERNAL_NEW_TEXTURE_H

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
#include <dali/public-api/common/dali-common.h> // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/images/pixel.h>
#include <dali/devel-api/rendering/texture.h> // Dali::Internal::Render::Texture
#include <dali/internal/event/common/event-thread-services.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
class NewTexture;
}

class NewTexture;
typedef IntrusivePtr<NewTexture> NewTexturePtr;

class NewTexture : public BaseObject
{
public:

  /**
   * @brief Create a new Texture.
   *
   * @param[in] type The type of the texture
   * @param[in] format The format of the pixel data
   * @param[in] width The width of the texture
   * @param[in] height The height of the texture
   * @return A smart-pointer to the newly allocated Texture.
   */
  static NewTexturePtr New(TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height);

  /**
   * @brief Get the texture render object
   *
   * @return the texture render object
   */
  Render::NewTexture* GetRenderObject() const;

  /**
   * @copydoc Dali::Texture::Upload()
   */
  void Upload( Vector<unsigned char>& buffer, const TextureUploadParams& params );

  /**
   * @copydoc Dali::Texture::GenerateMipmaps()
   */
  void GenerateMipmaps();

  /**
   * @copydoc Dali::Texture::GetWidth()
   */
  unsigned int GetWidth() const;

  /**
   * @copydoc Dali::Texture::GetHeight()
   */
  unsigned int GetHeight() const;


private: // implementation

  /**
   * Constructor
   */
  NewTexture(TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height );

  /**
   * Second stage initialization of the Texture
   */
  void Initialize();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~NewTexture();

private: // unimplemented methods
  NewTexture( const NewTexture& );
  NewTexture& operator=( const NewTexture& );

private: // data

  Internal::EventThreadServices& mEventThreadServices;    ///<Used to send messages to the render thread via update thread
  Internal::Render::NewTexture* mRenderObject;            ///<The Render::Texture associated to this texture

  Dali::TextureType::Type mType;      ///< Texture type (cached)
  Pixel::Format mFormat;              ///< Pixel format
  unsigned int mWidth;                ///< Width of the texture
  unsigned int mHeight;               ///< Height of the texture
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::NewTexture& GetImplementation(Dali::Texture& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Texture handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::NewTexture&>(object);
}

inline const Internal::NewTexture& GetImplementation(const Dali::Texture& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Texture handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::NewTexture&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_TEXTURE_H
