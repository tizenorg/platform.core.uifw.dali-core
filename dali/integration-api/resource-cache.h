#ifndef __DALI_INTEGRATION_RESOURCE_CACHE_H__
#define __DALI_INTEGRATION_RESOURCE_CACHE_H__

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

// EXTERNAL INCLUDES
#include <string>
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/image-attributes.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/text/text-style.h>
#include <dali/integration-api/resource-declarations.h>

namespace Dali
{

namespace Integration
{

// Resource Types

/**
 * Extendable set of resource types
 */
enum ResourceTypeId
{
  ResourceBitmap,
  ResourceNativeImage,
  ResourceTargetImage,
  ResourceShader,
  ResourceModel,
  ResourceMesh,
  ResourceText
};

/**
 * The abstract base class for a resource type
 */
struct DALI_IMPORT_API ResourceType
{
  /**
   * Constructor.
   * @param[in] typeId resource type id
   */
  ResourceType(ResourceTypeId typeId)
  : id(typeId) {}

  /**
   * Destructor.
   */
  virtual ~ResourceType() {}

  /**
   * Create a copy of the resource type with the same attributes.
   * @return pointer to the new ResourceType.
   */
  virtual ResourceType* Clone() const = 0;

  const ResourceTypeId id;

private:

  // Undefined copy constructor.
  ResourceType(const ResourceType& typePath);

  // Undefined assignment operator.
  ResourceType& operator=(const ResourceType& rhs);
};

/**
 * BitmapResourceType describes a bitmap resource, which can be requested
 * from ResourceLoader::LoadResource() or AllocateBitmapImage.
 */
struct DALI_IMPORT_API BitmapResourceType : public ResourceType
{
  /**
   * Constructor.
   * @param[in] attribs parameters for image loading request
   */
  BitmapResourceType(const ImageAttributes& attribs)
  : ResourceType(ResourceBitmap),
    imageAttributes(attribs) {}

  /**
   * Destructor.
   */
  virtual ~BitmapResourceType() {}

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new BitmapResourceType(imageAttributes);
  }

  /**
   * Attributes are copied from the request.
   */
  ImageAttributes imageAttributes;

private:

  // Undefined copy constructor.
  BitmapResourceType(const BitmapResourceType& typePath);

  // Undefined assignment operator.
  BitmapResourceType& operator=(const BitmapResourceType& rhs);
};

/**
 * NativeImageResourceType describes a native image resource, which can be injected
 * through ResourceManager::AddNativeImage() or requested through ResourceLoader::LoadResource().
 * If the adaptor does not support NativeImages, it can fall back to Bitmap type.
 */
struct DALI_IMPORT_API NativeImageResourceType : public ResourceType
{
  /**
   * Constructor.
   */
  NativeImageResourceType()
  : ResourceType(ResourceNativeImage) {}

  /**
   * Constructor.
   * @param[in] attribs parameters for image loading request
   */
  NativeImageResourceType(const ImageAttributes& attribs)
  : ResourceType(ResourceNativeImage),
    imageAttributes(attribs) {}

  /**
   * Destructor.
   */
  virtual ~NativeImageResourceType() {}

 /**
  * @copydoc ResourceType::Clone
  */
  virtual ResourceType* Clone() const
  {
    return new NativeImageResourceType(imageAttributes);
  }

  /**
   * Attributes are copied from the request (if supplied).
   */
  ImageAttributes imageAttributes;

private:

  // Undefined copy constructor.
  NativeImageResourceType(const NativeImageResourceType& typePath);

  // Undefined assignment operator.
  NativeImageResourceType& operator=(const NativeImageResourceType& rhs);
};

/**
 * RenderTargetResourceType describes a bitmap resource, which can injected
 * through ResourceManager::AddTargetImage()
 */
struct DALI_IMPORT_API RenderTargetResourceType : public ResourceType
{
  /**
   * Constructor.
   */
  RenderTargetResourceType()
  : ResourceType(ResourceTargetImage) {}

  /**
   * Constructor.
   * @param[in] attribs parameters for image loading request
   */
  RenderTargetResourceType(const ImageAttributes& attribs)
  : ResourceType(ResourceTargetImage),
    imageAttributes(attribs) {}

  /**
   * Destructor.
   */
  virtual ~RenderTargetResourceType() {}

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new RenderTargetResourceType(imageAttributes);
  }

  /**
   * Attributes are copied from the request.
   */
  ImageAttributes imageAttributes;

private:

  // Undefined copy constructor.
  RenderTargetResourceType(const RenderTargetResourceType& typePath);

  // Undefined assignment operator.
  RenderTargetResourceType& operator=(const RenderTargetResourceType& rhs);
};

/**
 * ShaderResourceType describes a shader program resource, which can be requested
 * from PlatformAbstraction::LoadResource()
 */
struct DALI_IMPORT_API ShaderResourceType : public ResourceType
{
  /**
   * Constructor.
   */
  ShaderResourceType(size_t shaderHash, const std::string& vertexSource, const std::string& fragmentSource)
  : ResourceType(ResourceShader),
    hash(shaderHash),
    vertexShader(vertexSource),
    fragmentShader(fragmentSource)
  {
  }

  /**
   * Destructor.
   */
  virtual ~ShaderResourceType()
  {
  }

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new ShaderResourceType(hash, vertexShader, fragmentShader);
  }

public: // Attributes
  size_t            hash;              ///< Hash of the vertex/fragment sources
  const std::string vertexShader;      ///< source code for vertex program
  const std::string fragmentShader;    ///< source code for fragment program

private:

  // Undefined copy constructor.
  ShaderResourceType(const ShaderResourceType& typePath);

  // Undefined assignment operator.
  ShaderResourceType& operator=(const ShaderResourceType& rhs);
};

/**
 * TextResourceType describes a font resource, which can be requested.
 * from PlatformAbstraction::LoadResource()  No font atlas is created.
 */
struct DALI_IMPORT_API TextResourceType : public ResourceType
{
  /**
   *  Text quality enum
   */
  enum TextQuality
  {
    TextQualityLow,       ///< Request lower quality text
    TextQualityHigh       ///< Request higher quality text
  };

  /**
   * Structure for requesting character to be loaded from file with atlas position
   * for automatic texture upload
   */
  struct GlyphPosition
  {
    GlyphPosition(unsigned int chr, unsigned int xPos, unsigned int yPos)
    : character(chr),
      quality(0),
      loaded(0),
      xPosition(xPos),
      yPosition(yPos)
    {
    }
    uint32_t character:21;       ///< character code (UTF-32), max value of 0x10ffff (21 bits)
    uint32_t quality:1;          ///< Loaded quality 0 = low quality, 1 = high quality
    uint32_t loaded:1;           ///< true if Loaded

    unsigned int xPosition;      ///< X Position in atlas
    unsigned int yPosition;      ///< Y Position in atlas

    /**
     * Used by ResourceTypeCompare
     */
    friend bool operator==(const GlyphPosition& lhs, const GlyphPosition& rhs);
  };

  typedef std::vector< GlyphPosition > CharacterList;      ///< List of glyphs requested

  enum GlyphCacheMode
  {
    GLYPH_CACHE_READ,    ///< Doesn't cache glyphs.
    GLYPH_CACHE_WRITE,   ///< Caches glyphs.
  };

  /**
   * Text resource type constructor
   * @param [in] hash           The resourceHash for the FontAtlas and FontMetrics
   * @param [in] style          The font style
   * @param [in] characterList  The requested text as a vector or UTF-32 codes
   * @param [in] textureAtlasId The resource ID of the texture atlas
   * @param [in] quality        A boolean, set to true to request high quality glyph bitmaps.
   * @param [in] maxGlyphSize   The size of the largest glyph in the font.
   * @param [in] cache          Whether text glyph should be cached or not.
   */
  TextResourceType( const size_t hash,
                    const std::string& style,
                    const CharacterList& characterList,
                    ResourceId textureAtlasId,
                    TextQuality quality = TextQualityLow,
                    Vector2 maxGlyphSize = Vector2::ONE,
                    GlyphCacheMode cache = GLYPH_CACHE_READ )
  : ResourceType(ResourceText),
    mFontHash(hash),
    mStyle(style),
    mCharacterList(characterList),
    mTextureAtlasId(textureAtlasId),
    mQuality(quality),
    mMaxGlyphSize(maxGlyphSize),
    mCache( cache )
  {
  }

  /**
   * virtual destructor
   */
  virtual ~TextResourceType()
  {
  }

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new TextResourceType(mFontHash, mStyle, mCharacterList, mTextureAtlasId, mQuality, mMaxGlyphSize, mCache);
  }

  /**
   * Font resource hash.
   */
  const size_t mFontHash;

  /**
   * Font style.
   */
  const std::string mStyle;

  /**
   * Displayed text (UTF-32 codes)
   */

  CharacterList mCharacterList; ///< List of characters

  ResourceId mTextureAtlasId; ///< Resource ID of the texture atlas this request is for

  TextQuality mQuality;  ///< Text quality setting

  Vector2 mMaxGlyphSize;  ///< Max glyph size for font

  GlyphCacheMode mCache; ///< Whether text glyphs should be cached.

private:

  // Undefined copy constructor.
  TextResourceType(const TextResourceType& typePath);

  // Undefined copy constructor.
  TextResourceType& operator=(const TextResourceType& rhs);
};

/**
 * ModelResourceType describes a model resource, which can be requested
 * from PlatformAbstraction::LoadResource()
 */
struct DALI_IMPORT_API ModelResourceType : public ResourceType
{
  /**
   * Constructor.
   */
  ModelResourceType()
    : ResourceType(ResourceModel)
  {
  }

  /**
   * Destructor.
   */
  virtual ~ModelResourceType()
  {
  }

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new ModelResourceType();
  }

private:

  // Undefined copy constructor.
  ModelResourceType(const ModelResourceType& typePath);

  // Undefined assignment operator.
  ModelResourceType& operator=(const ModelResourceType& rhs);
};


/**
 * MeshResourceType describes a mesh program resource, which can be created
 * using ResourceManager::AllocateMesh.
 */
struct DALI_IMPORT_API MeshResourceType : public ResourceType
{
  /**
   * Constructor.
   */
  MeshResourceType()
  : ResourceType(ResourceMesh) {}

  /**
   * Destructor.
   */
  virtual ~MeshResourceType() {}

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new MeshResourceType();
  }

private:

  // Undefined copy constructor.
  MeshResourceType(const MeshResourceType& typePath);

  // Undefined assignment operator.
  MeshResourceType& operator=(const MeshResourceType& rhs);
};

// Resource Requests

/**
 * Used to return loaded resources for rendering etc.
 */
typedef IntrusivePtr<Dali::RefObject> ResourcePointer;

/**
 * Used to prioritize between loading operations.
 */
enum LoadResourcePriority
{
  LoadPriorityLowest,
  LoadPriorityLow,
  LoadPriorityNormal,
  LoadPriorityHigh,
  LoadPriorityHighest,
};

/**
 * Used to request a resource from the native filesystem.
 */
class DALI_IMPORT_API ResourceRequest
{
public:

  /**
   * Used to request a resource be accessed from the native filesystem.
   * @param[in] newId         A unique ID for this request.
   * @param[in] resourceType  The type of resource requested. The implementation of
   *                          PlatformAbstraction::LoadResource() is responsible for
   *                          converting the native file(s) to this type
   *                          e.g. decoding a jpeg to a bitmap.
   * @param[in] resourcePath  The path of the resource; typically a filename.
   * @param[in] loadPriority  The priority of the request.
   */
  ResourceRequest(ResourceId newId,
                  const ResourceType& resourceType,
                  const std::string& resourcePath,
                  LoadResourcePriority loadPriority = LoadPriorityNormal)
  : id(newId),
    type(NULL),
    path(resourcePath),
    priority(loadPriority)
  {
    type = resourceType.Clone();
  }

  /**
   * Used to save a resource in the native filesystem.
   * @param[in] newId         A unique ID for this request.
   * @param[in] resourceType  The type of resource to save.
   * @param[in] resourcePath  The location where the resource should be saved.
   * @param[in] resourcePtr   The resource itself.
   * @param[in] savePriority  The priority of the save request.
   */
  ResourceRequest(ResourceId newId,
                  const ResourceType& resourceType,
                  const std::string& resourcePath,
                  ResourcePointer resourcePtr,
                  LoadResourcePriority savePriority = LoadPriorityNormal)
  : id(newId),
    type(NULL),
    path(resourcePath),
    resource(resourcePtr),
    priority(savePriority)
  {
    type = resourceType.Clone();
  }

  /**
   * Copy constructor.
   * @param[in] request The resource request to copy.
   */
  ResourceRequest(const ResourceRequest& request)
  : id(request.id),
    type(NULL),
    path(request.path),
    resource(request.resource),
    priority(request.priority)
  {
    type = request.type->Clone();
  }

  /**
   * Assignment operator.
   * @param[in] rhs The resource request to copy.
   */
  ResourceRequest& operator=(const ResourceRequest& rhs)
  {
    if( this != &rhs )
    {
      id = rhs.id;
      type = rhs.type->Clone();
      path = rhs.path;
      resource = rhs.resource;
      priority = rhs.priority;
    }

    return *this;
  }

  /**
   * Non-virtual destructor; not intended as a base class
   */
  ~ResourceRequest()
  {
    delete type;
  }

  /**
   * Retrieve the resource ID
   * @return The ID
   */
  ResourceId GetId() const
  {
    return id;
  }

  /**
   * Retrieve the resource type
   * @return The type
   */
  ResourceType* GetType() const
  {
    return type;
  }

  /**
   * Retrieve the resource path
   * @return The path
   */
  const std::string& GetPath() const
  {
    return path;
  }

  /**
   * Retrieve the resource (for save requests)
   * @return The resource
   */
  ResourcePointer GetResource() const
  {
    return resource;
  }

  /**
   * Retrieve the load priority
   * @return The priority
   */
  LoadResourcePriority GetPriority() const
  {
    return priority;
  }

private:

  ResourceId           id;
  ResourceType*        type;
  std::string          path;
  ResourcePointer      resource; // For saving resources
  LoadResourcePriority priority;
};

/**
 * Used to determine why a resource file operation has failed.
 */
enum ResourceFailure
{
  FailureUnknown,
  FailureFileNotFound,
  FailureInvalidPath
};

/**
 * Used to return loaded resources for rendering etc.
 */
typedef IntrusivePtr<Dali::RefObject> ResourcePointer;

/**
 * Abstract interface for a resource cache.
 * This is used when pulling loaded resources from the PlatformAbstraction.
 */
class DALI_IMPORT_API ResourceCache
{
protected:

  /**
   * Virtual protected destructor, no deletion through this interface
   */
  virtual ~ResourceCache() {}

public:

  /**
   * Provide the results of a resource loading operation to the cache.
   * @param[in] id The unique ID of the load request.
   *               This should match an ID previously passed into PlatformAbstraction::LoadResource().
   *               LoadResponse() may be called multiple times with the same ID, when results are available
   *               at different stages e.g. a thumbnail image may be provided, before the full image is loaded.
   * @param[in] type The type of the resource.
   * @param[in] resource A pointer to a resource (Bitmap etc).
   * @param[in] status The current loading status. RESOURCE_LOADING and RESOURCE_PARTIALLY_LOADED indicate there are more responses to come, RESOURCE_COMPLETELY_LOADED indicates this is the last response for this id.
   */
  virtual void LoadResponse(ResourceId id, ResourceTypeId type, ResourcePointer resource, LoadStatus status) = 0;

  /**
   * Provide the results of a resource saving operation.
   * @param[in] id The unique ID of the resource.
   *               This should match an ID previously passed into PlatformAbstraction::LoadResource().
   * @param[in] type The type of the resource.
   */
  virtual void SaveComplete(ResourceId id, ResourceTypeId type) = 0;

  /**
   * Report that a resource loading operation has failed.
   * @param[in] id The unique ID of the load request.
   *               This should match an ID previously passed into PlatformAbstraction::LoadResource().
   * @param[in] failure An error code, used to determine why the load failed.
   */
  virtual void LoadFailed(ResourceId id, ResourceFailure failure) = 0;

  /**
   * Report that a resource saving operation has failed.
   * @param[in] id The unique ID of the save request.
   *               This should match an ID previously passed into PlatformAbstraction::LoadResource().
   * @param[in] failure An error code, used to determine why the save failed.
   */
  virtual void SaveFailed(ResourceId id, ResourceFailure failure) = 0;
};

inline bool operator==(const TextResourceType::GlyphPosition& lhs, const TextResourceType::GlyphPosition& rhs)
{
  return lhs.character == rhs.character && lhs.xPosition == rhs.xPosition && lhs.yPosition == rhs.yPosition && lhs.quality == rhs.quality;
}

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_RESOURCE_CACHE_H__
