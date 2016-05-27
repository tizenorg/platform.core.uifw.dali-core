
// CLASS HEADER
#include <dali/internal/render/renderers/render-texture.h>

//INTERNAL INCLUDES
#include <dali/devel-api/rendering/texture.h>  // Dali::Texture

// EXTERNAL INCLUDES
#include <math.h>   //floor, log2

namespace Dali
{

namespace Internal
{

namespace Render
{


namespace
{
/**
 * @brief Convert a FilterMode to it's corresponding GL type.
 *
 * @param[in] filterMode The FilterMode type.
 * @param[in] defaultfilterMode The filter mode to use if filterMode is DEFAULT.
 * @param[in] defaultSystemFilterMode The filter mode to use if filterMode is NONE.
 * @return Return the equivalent GL type.
 */

// These match the GL specification
const GLint GL_MINIFY_DEFAULT  = GL_NEAREST_MIPMAP_LINEAR;
const GLint GL_MAGNIFY_DEFAULT = GL_LINEAR;
const GLint GL_WRAP_DEFAULT  = GL_CLAMP_TO_EDGE;

// These are the Dali defaults
const GLint DALI_MINIFY_DEFAULT  = GL_LINEAR;
const GLint DALI_MAGNIFY_DEFAULT = GL_LINEAR;

GLint FilterModeToGL( FilterMode::Type filterMode, GLint daliDefault, GLint glDefault )
{
  switch( filterMode )
  {
    case FilterMode::NEAREST:
    {
      return GL_NEAREST;
    }
    case FilterMode::LINEAR:
    {
      return GL_LINEAR;
    }
    case FilterMode::NONE:
    {
      return glDefault;
    }
    case FilterMode::NEAREST_MIPMAP_NEAREST:
    {
      return GL_NEAREST_MIPMAP_NEAREST;
    }
    case FilterMode::LINEAR_MIPMAP_NEAREST:
    {
      return GL_LINEAR_MIPMAP_NEAREST;
    }
    case FilterMode::NEAREST_MIPMAP_LINEAR:
    {
      return GL_NEAREST_MIPMAP_LINEAR;
    }
    case FilterMode::LINEAR_MIPMAP_LINEAR:
    {
      return GL_LINEAR_MIPMAP_LINEAR;
    }
    case FilterMode::DEFAULT:
    default:
    {
      return daliDefault;
    }
  }
}

GLint WrapModeToGL( WrapMode::Type wrapMode, GLint defaultWrapMode )
{
  switch( wrapMode )
  {
    case WrapMode::CLAMP_TO_EDGE:
    {
      return GL_CLAMP_TO_EDGE;
    }
    case WrapMode::REPEAT:
    {
      return GL_REPEAT;
    }
    case WrapMode::MIRRORED_REPEAT:
    {
      return GL_MIRRORED_REPEAT;
    }
    case WrapMode::DEFAULT:
    default:
    {
      return defaultWrapMode;
    }
  }
}

void PixelFormatToGl( Pixel::Format pixelformat, unsigned& pixelDataType, unsigned& internalFormat )
{
  // Compressed textures have no pixelDataType, so init to an invalid value:
  pixelDataType  = -1;

  switch( pixelformat )
  {
    case Pixel::A8:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_ALPHA;
      break;
    }

    case Pixel::L8:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_LUMINANCE;
      break;
    }

    case Pixel::LA88:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_LUMINANCE_ALPHA;
      break;
    }

    case Pixel::RGB565:
    {
      pixelDataType = GL_UNSIGNED_SHORT_5_6_5;
      internalFormat= GL_RGB;
      break;
    }

    case Pixel::BGR565:
    {
      DALI_LOG_ERROR("Pixel format BGR565 is not supported by GLES.\n");
      pixelDataType  = GL_UNSIGNED_SHORT_5_6_5;
#ifdef _ARCH_ARM_
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    case Pixel::RGBA4444:
    {
      pixelDataType = GL_UNSIGNED_SHORT_4_4_4_4;
      internalFormat= GL_RGBA;
      break;
    }

    case Pixel::BGRA4444:
    {
      DALI_LOG_ERROR("Pixel format BGRA4444 is not supported by GLES.\n");
      pixelDataType  = GL_UNSIGNED_SHORT_4_4_4_4;
#ifdef _ARCH_ARM_
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    case Pixel::RGBA5551:
    {
      pixelDataType = GL_UNSIGNED_SHORT_5_5_5_1;
      internalFormat= GL_RGBA;
      break;
    }

    case Pixel::BGRA5551:
    {
      DALI_LOG_ERROR("Pixel format BGRA5551 is not supported by GLES.\n");
      pixelDataType  = GL_UNSIGNED_SHORT_5_5_5_1;
#ifdef _ARCH_ARM_
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    case Pixel::RGB888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_RGB;
      break;
    }

    case Pixel::RGB8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_RGBA;     // alpha is reserved but not used
      break;
    }

    case Pixel::BGR8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
#ifdef GL_BGRA_EXT
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
    break;
    }

    case Pixel::RGBA8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_RGBA;
      break;
    }

    case Pixel::BGRA8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
#ifdef GL_BGRA_EXT
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    // GLES 2 extension compressed formats:
    case Pixel::COMPRESSED_RGB8_ETC1:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using non-standard GLES 2.0 extension compressed pixel format COMPRESSED_RGB8_ETC1.\n" );
      internalFormat = 0x8D64; ///! < Hardcoded so we can test before we move to GLES 3.0 or greater.
      break;
    }
    case Pixel::COMPRESSED_RGB_PVRTC_4BPPV1:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using non-standard GLES 2.0 extension compressed pixel format COMPRESSED_RGB_PVRTC_4BPPV1.\n" );
      internalFormat = 0x8C00; ///! < Hardcoded so we can test before we move to GLES 3.0 or greater.
      break;
    }

    // GLES 3.0 standard compressed formats:
    case Pixel::COMPRESSED_R11_EAC:
    {
      DALI_LOG_INFO(Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_R11_EAC.\n");
      internalFormat = GL_COMPRESSED_R11_EAC;
      break;
    }
    case Pixel::COMPRESSED_SIGNED_R11_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SIGNED_R11_EAC.\n" );
      internalFormat = GL_COMPRESSED_SIGNED_R11_EAC;
      break;
    }
    case Pixel::COMPRESSED_RG11_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RG11_EAC.\n" );
      internalFormat = GL_COMPRESSED_RG11_EAC;
      break;
    }
    case Pixel::COMPRESSED_SIGNED_RG11_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SIGNED_RG11_EAC.\n" );
      internalFormat = GL_COMPRESSED_SIGNED_RG11_EAC;
      break;
    }
    case Pixel::COMPRESSED_RGB8_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RGB8_ETC2.\n" );
      internalFormat = GL_COMPRESSED_RGB8_ETC2;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SRGB8_ETC2.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ETC2;
      break;
    }
    case Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2.\n" );
      internalFormat = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
      break;
    }
    case Pixel::COMPRESSED_RGBA8_ETC2_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RGBA8_ETC2_EAC.\n" );
      internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ETC2_EAC.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
      break;
    }

    // GLES 3.1 extension compressed formats:
    case Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_4x4_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_5x4_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_5x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_6x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_6x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_8x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_8x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_8x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_8x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_8x8_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_10x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_10x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_10x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_10x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_10x8_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_10x8_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_10x10_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_12x10_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_12x10_KHR;
      break;
    }
    case Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_RGBA_ASTC_12x12_KHR.\n" );
      internalFormat = GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
      break;
    }
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.1 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR.\n" );
      internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
      break;
    }

    case Pixel::INVALID:
    {
      DALI_LOG_ERROR( "Invalid pixel format for bitmap\n" );
      internalFormat = 0;
      break;
    }
  }
}



bool PixelFormatHasAlpha( Pixel::Format pixelformat )
{
  switch( pixelformat )
  {
    case Pixel::A8:
    case Pixel::LA88:
    case Pixel::RGBA4444:
    case Pixel::BGRA4444:
    case Pixel::RGBA5551:
    case Pixel::BGRA5551:
    case Pixel::RGB8888:
    case Pixel::BGR8888:
    case Pixel::RGBA8888:
    case Pixel::BGRA8888:
    case Pixel::COMPRESSED_RGBA8_ETC2_EAC:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    case Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    {
      return true;
    }

    default:
    {
      return false;
    }
  }
}
} //Unnamed namespace



NewTexture::NewTexture( Dali::TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height, bool mipmaps )
:mId( 0 ),
 mType( type ),
 mInternalFormat(GL_RGB),
 mPixelDataType(GL_UNSIGNED_BYTE),
 mWidth( width ),
 mHeight( height ),
 mSampler(),
 mMipmapCount( mipmaps ? floor( log2(width > height ? width : height) ) + 1 : 0u ),
 mHasAlpha( PixelFormatHasAlpha( format ) )
{
  PixelFormatToGl( format, mPixelDataType, mInternalFormat );
}

NewTexture::~NewTexture()
{}

void NewTexture::Destroy( Context& context )
{
  if( mId )
  {
    context.DeleteTextures( 1, &mId );
  }
}

void NewTexture::Initialize(Context& context)
{
  //Create and reserve memory for the texture in the GPU
  context.GenTextures( 1, &mId );

  if( mType == TextureType::TEXTURE_2D )
  {
    context.Bind2dTexture( mId );
    context.PixelStorei(GL_UNPACK_ALIGNMENT, 1);
    context.TexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mInternalFormat, mPixelDataType, 0 );

    //Apply default sampling parameters
    context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DALI_MINIFY_DEFAULT );
    context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DALI_MAGNIFY_DEFAULT );
    context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_DEFAULT );
    context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_DEFAULT );

    //Generate mipmaps if needed
    if( mMipmapCount > 0u )
    {
      context.GenerateMipmap( GL_TEXTURE_2D );
    }
  }
  else if( mType == TextureType::TEXTURE_CUBE )
  {
    context.BindCubeMapTexture( mId );
    context.PixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for( unsigned int i(0); i<6; ++i )
    {
      context.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mInternalFormat, mWidth, mHeight, 0, mInternalFormat, mPixelDataType, 0 );
    }

    //Apply default sampling parameters
    context.TexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, DALI_MINIFY_DEFAULT );
    context.TexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, DALI_MAGNIFY_DEFAULT );
    context.TexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_WRAP_DEFAULT );
    context.TexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_WRAP_DEFAULT );
    context.TexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_WRAP_DEFAULT );

    //Generate mipmaps if needed
    if( mMipmapCount > 0u )
    {
      context.GenerateMipmap( GL_TEXTURE_CUBE_MAP );
    }
  }
}

void NewTexture::Upload( Context& context, Vector<unsigned char>& buffer, const Dali::TextureUploadParams& params  )
{
  if( mType == TextureType::TEXTURE_2D )
  {
    context.Bind2dTexture( mId );
    context.TexSubImage2D( GL_TEXTURE_2D, params.mipmap, params.x0, params.y0, params.width, params.height, mInternalFormat, mPixelDataType, &buffer[0] );

    //FERRAN : mipmaps are regenerated when uploading layer 0...We probably should add a function to generate mipmaps
    if( params.mipmap == 0 && mMipmapCount > 0u )
    {
      context.GenerateMipmap( GL_TEXTURE_2D );
    }
  }
  else if( mType == TextureType::TEXTURE_CUBE )
  {
    context.BindCubeMapTexture( mId );
    context.TexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + params.layer, params.mipmap, params.x0, params.y0, params.width, params.height, mInternalFormat, mPixelDataType, &buffer[0] );
  }
}

void NewTexture::Bind( Context& context, unsigned int textureUnit, Render::Sampler* sampler )
{
  context.ActiveTexture( static_cast<TextureUnit>(textureUnit) );

  if( mType == TextureType::TEXTURE_2D )
  {
    context.Bind2dTexture( mId );
  }
  else if( mType == TextureType::TEXTURE_CUBE )
  {
    context.BindCubeMapTexture( mId );
  }

  ApplySampler( context, sampler );
}

void NewTexture::ApplySampler( Context& context, Render::Sampler* sampler )
{
  Render::Sampler oldSampler = mSampler;
  if( sampler )
  {
    mSampler = *sampler;
  }

  if( mSampler.mBitfield != oldSampler.mBitfield )
  {
    if( mSampler.mMinificationFilter != oldSampler.mMinificationFilter )
    {
      GLint glFilterMode = FilterModeToGL( mSampler.mMinificationFilter, DALI_MINIFY_DEFAULT, GL_MINIFY_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilterMode );
    }

    if( mSampler.mMagnificationFilter != oldSampler.mMagnificationFilter )
    {
      GLint glFilterMode = FilterModeToGL( mSampler.mMagnificationFilter, DALI_MAGNIFY_DEFAULT, GL_MAGNIFY_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilterMode );
    }

    if( mSampler.mSWrapMode != oldSampler.mSWrapMode )
    {
      GLint glWrapMode = WrapModeToGL( mSampler.mSWrapMode, GL_WRAP_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapMode );
    }

    if( mSampler.mTWrapMode != oldSampler.mTWrapMode )
    {
      GLint glWrapMode = WrapModeToGL( mSampler.mTWrapMode, GL_WRAP_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapMode );
    }

    if( mType == TextureType::TEXTURE_CUBE && mSampler.mRWrapMode != oldSampler.mRWrapMode )
    {
      GLint glWrapMode = WrapModeToGL( mSampler.mRWrapMode, GL_WRAP_DEFAULT );
      context.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_U, glWrapMode );
    }
  }
}

bool NewTexture::HasAlphaChannel()
{
  return mHasAlpha;
}

} //Render

} //Internal

} //Dali
