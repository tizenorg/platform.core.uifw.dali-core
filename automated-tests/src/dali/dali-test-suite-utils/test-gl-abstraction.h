#ifndef __TEST_GL_ABSTRACTION_H__
#define __TEST_GL_ABSTRACTION_H__

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

// EXTERNAL INCLUDES
#include <sstream>
#include <string>
#include <map>

// INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/gl-abstraction.h>
#include "test-trace-call-stack.h"

namespace Dali
{

static const unsigned int MAX_ATTRIBUTE_CACHE_SIZE = 64;
static const char *mStdAttribs[MAX_ATTRIBUTE_CACHE_SIZE] =
{
    "aPosition",    // ATTRIB_POSITION
    "aNormal",      // ATTRIB_NORMAL
    "aTexCoord",    // ATTRIB_TEXCOORD
    "aColor",       // ATTRIB_COLOR
    "aBoneWeights", // ATTRIB_BONE_WEIGHTS
    "aBoneIndices"  // ATTRIB_BONE_INDICES
};

class DALI_IMPORT_API TestGlAbstraction: public Dali::Integration::GlAbstraction
{
public:
  TestGlAbstraction();
  ~TestGlAbstraction();
  void Initialize();

  void PreRender();
  void PostRender(unsigned int timeDelta);

  /* OpenGL ES 2.0 */

  inline void ActiveTexture( GLenum textureUnit )
  {
    mActiveTextureUnit = textureUnit - GL_TEXTURE0;
  }

  inline GLenum GetActiveTextureUnit() const
  {
    return mActiveTextureUnit + GL_TEXTURE0;
  }

  inline void AttachShader( GLuint program, GLuint shader )
  {
    std::stringstream out;
    out << program << ", " << shader;
    mShaderTrace.PushCall("AttachShader", out.str());
  }

  inline void BindAttribLocation( GLuint program, GLuint index, const char* name )
  {
  }

  inline void BindBuffer( GLenum target, GLuint buffer )
  {
  }

  inline void BindFramebuffer( GLenum target, GLuint framebuffer )
  {
  }

  inline void BindRenderbuffer( GLenum target, GLuint renderbuffer )
  {
  }

  /**
   * This method can be used by test cases, to query the texture IDs that have been bound by BindTexture.
   * @return A vector containing the IDs that were bound.
   */
  inline const std::vector<GLuint>& GetBoundTextures() const
  {
    return mBoundTextures;
  }

  /**
   * Query the texture IDs that have been bound with BindTexture, with a specific active texture unit.
   * @param[in] activeTextureUnit The specific active texture unit.
   * @return A vector containing the IDs that were bound.
   */
  inline const std::vector<GLuint>& GetBoundTextures( GLuint activeTextureUnit ) const
  {
    return mActiveTextures[ activeTextureUnit - GL_TEXTURE0 ].mBoundTextures;
  }

  /**
   * This method can be used by test cases, to clear the record of texture IDs that have been bound by BindTexture.
   */
  inline void ClearBoundTextures()
  {
    mBoundTextures.clear();

    for( unsigned int i=0; i<MIN_TEXTURE_UNIT_LIMIT; ++i )
    {
      mActiveTextures[ i ].mBoundTextures.clear();
    }
  }

  inline void BindTexture( GLenum target, GLuint texture )
  {
    // Record the bound textures for future checks
    if( texture )
    {
      mBoundTextures.push_back( texture );

      if( mActiveTextureUnit < MIN_TEXTURE_UNIT_LIMIT )
      {
        mActiveTextures[ mActiveTextureUnit ].mBoundTextures.push_back( texture );
      }
    }

    std::stringstream out;
    out << target << ", " << texture;
    mTextureTrace.PushCall("BindTexture", out.str());
  }

  inline void BlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
  {
    mLastBlendColor.r = red;
    mLastBlendColor.g = green;
    mLastBlendColor.b = blue;
    mLastBlendColor.a = alpha;
  }

  inline const Vector4& GetLastBlendColor() const
  {
    return mLastBlendColor;
  }

  inline void BlendEquation( GLenum mode )
  {
    mLastBlendEquationRgb   = mode;
    mLastBlendEquationAlpha = mode;
  }

  inline void BlendEquationSeparate( GLenum modeRgb, GLenum modeAlpha )
  {
    mLastBlendEquationRgb   = modeRgb;
    mLastBlendEquationAlpha = modeAlpha;
  }

  inline GLenum GetLastBlendEquationRgb() const
  {
    return mLastBlendEquationRgb;
  }

  inline GLenum GetLastBlendEquationAlpha() const
  {
    return mLastBlendEquationAlpha;
  }

  inline void BlendFunc(GLenum sfactor, GLenum dfactor)
  {
    mLastBlendFuncSrcRgb = sfactor;
    mLastBlendFuncDstRgb = dfactor;
    mLastBlendFuncSrcAlpha = sfactor;
    mLastBlendFuncDstAlpha = dfactor;
  }

  inline void BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
  {
    mLastBlendFuncSrcRgb = srcRGB;
    mLastBlendFuncDstRgb = dstRGB;
    mLastBlendFuncSrcAlpha = srcAlpha;
    mLastBlendFuncDstAlpha = dstAlpha;
  }

  inline GLenum GetLastBlendFuncSrcRgb() const
  {
    return mLastBlendFuncSrcRgb;
  }

  inline GLenum GetLastBlendFuncDstRgb() const
  {
    return mLastBlendFuncDstRgb;
  }

  inline GLenum GetLastBlendFuncSrcAlpha() const
  {
    return mLastBlendFuncSrcAlpha;
  }

  inline GLenum GetLastBlendFuncDstAlpha() const
  {
    return mLastBlendFuncDstAlpha;
  }

  inline void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
  {
  }

  inline void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
  {
  }

  inline GLenum CheckFramebufferStatus(GLenum target)
  {
    return mCheckFramebufferStatusResult;
  }

  inline void Clear(GLbitfield mask)
  {
  }

  inline void ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
  {
  }

  inline void ClearDepthf(GLclampf depth)
  {
  }

  inline void ClearStencil(GLint s)
  {
  }

  inline void ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
  {
  }

  inline void CompileShader(GLuint shader)
  {
    std::stringstream out;
    out << shader;
    mShaderTrace.PushCall("CompileShader", out.str());
  }

  inline void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
  {
  }

  inline void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
  {
  }

  inline void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
  {
  }

  inline void CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
  {
  }

  inline GLuint CreateProgram(void)
  {
    mShaderTrace.PushCall("CreateProgram", "");

    ++mLastProgramIdUsed;
    mUniforms[mLastProgramIdUsed] = UniformIDMap();
    return mLastProgramIdUsed;
  }

  inline GLuint CreateShader(GLenum type)
  {
    std::stringstream out;
    out << type;
    mShaderTrace.PushCall("CreateShader", out.str());

    return ++mLastShaderIdUsed;
  }

  inline void CullFace(GLenum mode)
  {
    std::stringstream out;
    out << mode;
    mCullFaceTrace.PushCall("CullFace", out.str());
  }

  inline void DeleteBuffers(GLsizei n, const GLuint* buffers)
  {
  }

  inline void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
  {
  }

  inline void DeleteProgram(GLuint program)
  {
    std::stringstream out;
    out << program;
    mShaderTrace.PushCall("DeleteProgram", out.str());
  }

  inline void DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
  {
  }

  inline void DeleteShader(GLuint shader)
  {
    std::stringstream out;
    out << shader;
    mShaderTrace.PushCall("DeleteShader", out.str());
  }

  inline void DeleteTextures(GLsizei n, const GLuint* textures)
  {
    std::stringstream out;
    out << n << ", " << textures << " = [" ;

    for(GLsizei i=0; i<n; i++)
    {
      out << textures[i] << ", " ;
      mDeletedTextureIds.push_back(textures[i]);
    }
    out << "]";
    mTextureTrace.PushCall("DeleteTextures", out.str());
  }

  inline bool CheckNoTexturesDeleted()
  {
    return mDeletedTextureIds.size() == 0;
  }

  inline bool CheckTextureDeleted( GLuint textureId )
  {
    bool found = false;

    for(std::vector<GLuint>::iterator iter=mDeletedTextureIds.begin(); iter != mDeletedTextureIds.end(); ++iter)
    {
      if(*iter == textureId)
      {
        found = true;
        break;
      }
    }
    return found;
  }

  inline void ClearDeletedTextures()
  {
    mDeletedTextureIds.clear();
  }

  inline void DepthFunc(GLenum func)
  {
  }

  inline void DepthMask(GLboolean flag)
  {
  }

  inline void DepthRangef(GLclampf zNear, GLclampf zFar)
  {
  }

  inline void DetachShader(GLuint program, GLuint shader)
  {
    std::stringstream out;
    out << program << ", " << shader;
    mShaderTrace.PushCall("DetachShader", out.str());
  }

  inline void Disable(GLenum cap)
  {
    std::stringstream out;
    out << cap;
    mCullFaceTrace.PushCall("Disable", out.str());
  }

  inline void DisableVertexAttribArray(GLuint index)
  {
    SetVertexAttribArray( index, false );
  }

  inline void DrawArrays(GLenum mode, GLint first, GLsizei count)
  {
    std::stringstream out;
    out << mode << ", " << first << ", " << count;
    mDrawTrace.PushCall("DrawArrays", out.str());
  }

  inline void DrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
  {
    std::stringstream out;
    out << mode << ", " << count << ", " << type << ", indices";
    mDrawTrace.PushCall("DrawElements", out.str());
  }

  inline void Enable(GLenum cap)
  {
    std::stringstream out;
    out << cap;
    mCullFaceTrace.PushCall("Enable", out.str());
  }

  inline void EnableVertexAttribArray(GLuint index)
  {
    SetVertexAttribArray( index, true);
  }

  inline void Finish(void)
  {
  }

  inline void Flush(void)
  {
  }

  inline void FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
  {
  }

  inline void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
  {
  }

  inline void FrontFace(GLenum mode)
  {
  }

  inline void GenBuffers(GLsizei n, GLuint* buffers)
  {
    // avoids an assert in GpuBuffers
    *buffers = 1u;
  }

  inline void GenerateMipmap(GLenum target)
  {
  }

  inline void GenFramebuffers(GLsizei n, GLuint* framebuffers)
  {
  }

  inline void GenRenderbuffers(GLsizei n, GLuint* renderbuffers)
  {
  }

  /**
   * This method can be used by test cases, to manipulate the texture IDs generated by GenTextures.
   * @param[in] ids A vector containing the next IDs to be generated
   */
  inline void SetNextTextureIds( const std::vector<GLuint>& ids )
  {
    mNextTextureIds = ids;
  }

  inline const std::vector<GLuint>& GetNextTextureIds()
  {
    return mNextTextureIds;
  }

  inline void GenTextures(GLsizei n, GLuint* textures)
  {
    for( int i=0; i<n; ++i )
    {
      if( !mNextTextureIds.empty() )
      {
        *(textures+i) = mNextTextureIds[0];
        mNextTextureIds.erase( mNextTextureIds.begin() );
      }
      else
      {
        *(textures+i) = ++mLastAutoTextureIdUsed;
      }
    }

    std::stringstream out;
    for(int i=0; i<n; i++)
    {
      out << textures[i];
      if(i<n-1)
      {
        out << ", ";
      }
    }
    mTextureTrace.PushCall("GenTexture", out.str());
  }

  inline void GetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
  {
  }

  inline void GetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
  {
  }

  inline void GetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
  {
  }

  inline int  GetAttribLocation(GLuint program, const char* name)
  {
    std::string attribName(name);

    for( unsigned int i = 0; i < ATTRIB_TYPE_LAST; ++i )
    {
      if( mStdAttribs[i] == attribName )
      {
        return i;
      }
    }

    // 0 is a valid location
    return 0;
  }

  inline void GetBooleanv(GLenum pname, GLboolean* params)
  {
  }

  inline void GetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
  {
  }

  inline GLenum GetError(void)
  {
    return mGetErrorResult;
  }

  inline void GetFloatv(GLenum pname, GLfloat* params)
  {
  }

  inline void GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
  {
  }

  inline void GetIntegerv(GLenum pname, GLint* params)
  {
    switch( pname )
    {
      case GL_MAX_TEXTURE_SIZE:
        *params = 2048;
        break;
      case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
        *params = 8;
        break;
      case GL_NUM_PROGRAM_BINARY_FORMATS_OES:
        *params = mNumBinaryFormats;
        break;
      case GL_PROGRAM_BINARY_FORMATS_OES:
        *params = mBinaryFormats;
        break;
    }
  }

  inline void GetProgramiv(GLuint program, GLenum pname, GLint* params)
  {
    switch( pname ) {
      case GL_LINK_STATUS:
        *params = mLinkStatus;
        break;
      case GL_PROGRAM_BINARY_LENGTH_OES:
        *params = mProgramBinaryLength;
        break;
    }
  }

  inline void GetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog)
  {
  }


  inline void GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
  {
  }

  inline void GetShaderiv(GLuint shader, GLenum pname, GLint* params)
  {
    switch( pname ) {
      case GL_COMPILE_STATUS:
        *params = mCompileStatus;
        break;
    }
  }

  inline void GetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog)
  {
  }

  inline void GetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
  {
  }

  inline const GLubyte* GetString(GLenum name)
  {
    return mGetStringResult;
  }

  inline void GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
  {
  }

  inline void GetTexParameteriv(GLenum target, GLenum pname, GLint* params)
  {
  }

  inline void GetUniformfv(GLuint program, GLint location, GLfloat* params)
  {
  }

  inline void GetUniformiv(GLuint program, GLint location, GLint* params)
  {
  }

  inline GLint GetUniformLocation(GLuint program, const char* name)
  {
    ProgramUniformMap::iterator it = mUniforms.find(program);
    if( it == mUniforms.end() )
    {
      // Not a valid program ID
      mGetErrorResult = GL_INVALID_OPERATION;
      return -1;
    }

    UniformIDMap& uniformIDs = it->second;
    UniformIDMap::iterator it2 = uniformIDs.find( name );
    if( it2 == uniformIDs.end() )
    {
      // Uniform not found, so add it...
      uniformIDs[name] = ++mLastUniformIdUsed;
      return mLastUniformIdUsed;
    }

    return it2->second;
  }

  inline void GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
  {
  }

  inline void GetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
  {
  }

  inline void GetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)
  {
  }

  inline void Hint(GLenum target, GLenum mode)
  {
  }

  inline GLboolean IsBuffer(GLuint buffer)
  {
    return mIsBufferResult;
  }

  inline GLboolean IsEnabled(GLenum cap)
  {
    return mIsEnabledResult;
  }

  inline GLboolean IsFramebuffer(GLuint framebuffer)
  {
    return mIsFramebufferResult;
  }

  inline GLboolean IsProgram(GLuint program)
  {
    return mIsProgramResult;
  }

  inline GLboolean IsRenderbuffer(GLuint renderbuffer)
  {
    return mIsRenderbufferResult;
  }

  inline GLboolean IsShader(GLuint shader)
  {
    return mIsShaderResult;
  }

  inline GLboolean IsTexture(GLuint texture)
  {
    return mIsTextureResult;
  }

  inline void LineWidth(GLfloat width)
  {
  }

  inline void LinkProgram(GLuint program)
  {
    std::stringstream out;
    out << program;
    mShaderTrace.PushCall("LinkProgram", out.str());
  }

  inline void PixelStorei(GLenum pname, GLint param)
  {
  }

  inline void PolygonOffset(GLfloat factor, GLfloat units)
  {
  }

  inline void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels)
  {
  }

  inline void ReleaseShaderCompiler(void)
  {
  }

  inline void RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
  {
  }

  inline void SampleCoverage(GLclampf value, GLboolean invert)
  {
  }

  inline void Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
  {
  }

  inline void ShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length)
  {
  }

  inline void ShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length)
  {
    std::string stringBuilder;
    for(int i = 0; i < count; ++i)
    {
      stringBuilder += string[i];
    }
    mShaderSources[shader] = stringBuilder;
    mLastShaderCompiled = shader;
  }

  inline void GetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source)
  {
    const std::string shaderSource = mShaderSources[shader];
    if( static_cast<int>(shaderSource.length()) < bufsize )
    {
      strcpy(source, shaderSource.c_str());
      *length = shaderSource.length();
    }
    else
    {
      *length = bufsize -1;
      strncpy(source, shaderSource.c_str(), *length);
      source[*length] = 0x0;
    }
  }

  inline std::string GetShaderSource(GLuint shader)
  {
    return mShaderSources[shader];
  }

  inline void StencilFunc(GLenum func, GLint ref, GLuint mask)
  {
  }

  inline void StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
  {
  }

  inline void StencilMask(GLuint mask)
  {
  }

  inline void StencilMaskSeparate(GLenum face, GLuint mask)
  {
  }

  inline void StencilOp(GLenum fail, GLenum zfail, GLenum zpass)
  {
  }

  inline void StencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
  {
  }

  inline void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)
  {
    std::stringstream out;
    out << width << ", " << height;
    mTextureTrace.PushCall("TexImage2D", out.str());
  }

  inline void TexParameterf(GLenum target, GLenum pname, GLfloat param)
  {
    std::stringstream out;
    out << target << ", " << pname << ", " << param;
    mTexParamaterTrace.PushCall("TexParameterf", out.str());
  }

  inline void TexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
  {
    std::stringstream out;
    out << target << ", " << pname << ", " << params[0];
    mTexParamaterTrace.PushCall("TexParameterfv", out.str());
  }

  inline void TexParameteri(GLenum target, GLenum pname, GLint param)
  {
    std::stringstream out;
    out << target << ", " << pname << ", " << param;
    mTexParamaterTrace.PushCall("TexParameteri", out.str());
  }

  inline void TexParameteriv(GLenum target, GLenum pname, const GLint* params)
  {
    std::stringstream out;
    out << target << ", " << pname << ", " << params[0];
    mTexParamaterTrace.PushCall("TexParameteriv", out.str());
  }

  inline void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
  {
    std::stringstream out;
    out << xoffset << ", " << yoffset << ", " << width << ", " << height;
    mTextureTrace.PushCall("TexSubImage2D", out.str());
  }

  inline void Uniform1f(GLint location, GLfloat x)
  {
    if( ! mProgramUniforms1f.SetUniformValue( mCurrentProgram, location, x ) )
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  inline void Uniform1fv(GLint location, GLsizei count, const GLfloat* v)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniforms1f.SetUniformValue( mCurrentProgram, location, v[i] ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void Uniform1i(GLint location, GLint x)
  {
    if( ! mProgramUniforms1i.SetUniformValue( mCurrentProgram, location, x ) )
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  inline void Uniform1iv(GLint location, GLsizei count, const GLint* v)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniforms1i.SetUniformValue( mCurrentProgram,
                                                 location,
                                                 v[i] ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void Uniform2f(GLint location, GLfloat x, GLfloat y)
  {
    if( ! mProgramUniforms2f.SetUniformValue( mCurrentProgram,
                                               location,
                                               Vector2( x, y ) ) )
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  inline void Uniform2fv(GLint location, GLsizei count, const GLfloat* v)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniforms2f.SetUniformValue( mCurrentProgram,
                                                 location,
                                                 Vector2( v[2*i], v[2*i+1] ) ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void Uniform2i(GLint location, GLint x, GLint y)
  {
  }

  inline void Uniform2iv(GLint location, GLsizei count, const GLint* v)
  {
  }

  inline void Uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
  {
    if( ! mProgramUniforms3f.SetUniformValue( mCurrentProgram,
                                               location,
                                               Vector3( x, y, z ) ) )
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  inline void Uniform3fv(GLint location, GLsizei count, const GLfloat* v)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniforms3f.SetUniformValue(
          mCurrentProgram,
          location,
          Vector3( v[3*i], v[3*i+1], v[3*i+2] ) ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void Uniform3i(GLint location, GLint x, GLint y, GLint z)
  {
  }

  inline void Uniform3iv(GLint location, GLsizei count, const GLint* v)
  {
  }

  inline void Uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
  {
    if( ! mProgramUniforms4f.SetUniformValue( mCurrentProgram,
                                              location,
                                              Vector4( x, y, z, w ) ) )
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  inline void Uniform4fv(GLint location, GLsizei count, const GLfloat* v)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniforms4f.SetUniformValue(
          mCurrentProgram,
          location,
          Vector4( v[4*i], v[4*i+1], v[4*i+2], v[4*i+3] ) ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void Uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
  {
  }

  inline void Uniform4iv(GLint location, GLsizei count, const GLint* v)
  {
  }

  inline void UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  inline void UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniformsMat3.SetUniformValue(
            mCurrentProgram,
            location,
            Matrix3( value[0], value[1], value[2], value[3], value[4], value[5], value[6], value[7], value[8] ) ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniformsMat4.SetUniformValue(
          mCurrentProgram,
          location,
          Matrix( value ) ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void UseProgram(GLuint program)
  {
    mCurrentProgram = program;
  }

  inline void ValidateProgram(GLuint program)
  {
  }

  inline void VertexAttrib1f(GLuint indx, GLfloat x)
  {
  }

  inline void VertexAttrib1fv(GLuint indx, const GLfloat* values)
  {
  }

  inline void VertexAttrib2f(GLuint indx, GLfloat x, GLfloat y)
  {
  }

  inline void VertexAttrib2fv(GLuint indx, const GLfloat* values)
  {
  }

  inline void VertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z)
  {
  }

  inline void VertexAttrib3fv(GLuint indx, const GLfloat* values)
  {
  }

  inline void VertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
  {
  }

  inline void VertexAttrib4fv(GLuint indx, const GLfloat* values)
  {
  }

  inline void VertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)
  {
  }

  inline void Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
  {
  }

  /* OpenGL ES 3.0 */

  inline void ReadBuffer(GLenum mode)
  {
  }

  inline void DrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices)
  {
  }

  inline void TexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
  {
  }

  inline void TexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels)
  {
  }

  inline void CopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
  {
  }

  inline void CompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data)
  {
  }

  inline void CompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data)
  {
  }

  inline void GenQueries(GLsizei n, GLuint* ids)
  {
  }

  inline void DeleteQueries(GLsizei n, const GLuint* ids)
  {
  }

  inline GLboolean IsQuery(GLuint id)
  {
    return false;
  }

  inline void BeginQuery(GLenum target, GLuint id)
  {
  }

  inline void EndQuery(GLenum target)
  {
  }

  inline void GetQueryiv(GLenum target, GLenum pname, GLint* params)
  {
  }

  inline void GetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params)
  {
  }

  inline GLboolean UnmapBuffer(GLenum target)
  {
    return false;
  }

  inline void GetBufferPointerv(GLenum target, GLenum pname, GLvoid** params)
  {
  }

  inline void DrawBuffers(GLsizei n, const GLenum* bufs)
  {
  }

  inline void UniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  inline void UniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  inline void UniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  inline void UniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  inline void UniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  inline void UniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  inline void BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
  {
  }

  inline void RenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
  {
  }

  inline void FramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
  {
  }

  inline GLvoid* MapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
  {
    return NULL;
  }

  inline void FlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length)
  {
  }

  inline void BindVertexArray(GLuint array)
  {
  }

  inline void DeleteVertexArrays(GLsizei n, const GLuint* arrays)
  {
  }

  inline void GenVertexArrays(GLsizei n, GLuint* arrays)
  {
  }

  inline GLboolean IsVertexArray(GLuint array)
  {
    return false;
  }

  inline void GetIntegeri_v(GLenum target, GLuint index, GLint* data)
  {
  }

  inline void BeginTransformFeedback(GLenum primitiveMode)
  {
  }

  inline void EndTransformFeedback(void)
  {
  }

  inline void BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
  {
  }

  inline void BindBufferBase(GLenum target, GLuint index, GLuint buffer)
  {
  }

  inline void TransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar* const* varyings, GLenum bufferMode)
  {
  }

  inline void GetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name)
  {
  }

  inline void VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
  {
  }

  inline void GetVertexAttribIiv(GLuint index, GLenum pname, GLint* params)
  {
  }

  inline void GetVertexAttribIuiv(GLuint index, GLenum pname, GLuint* params)
  {
  }

  inline void VertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w)
  {
  }

  inline void VertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
  {
  }

  inline void VertexAttribI4iv(GLuint index, const GLint* v)
  {
  }

  inline void VertexAttribI4uiv(GLuint index, const GLuint* v)
  {
  }

  inline void GetUniformuiv(GLuint program, GLint location, GLuint* params)
  {
  }

  inline GLint GetFragDataLocation(GLuint program, const GLchar *name)
  {
    return -1;
  }

  inline void Uniform1ui(GLint location, GLuint v0)
  {
  }

  inline void Uniform2ui(GLint location, GLuint v0, GLuint v1)
  {
  }

  inline void Uniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2)
  {
  }

  inline void Uniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
  {
  }

  inline void Uniform1uiv(GLint location, GLsizei count, const GLuint* value)
  {
  }

  inline void Uniform2uiv(GLint location, GLsizei count, const GLuint* value)
  {
  }

  inline void Uniform3uiv(GLint location, GLsizei count, const GLuint* value)
  {
  }

  inline void Uniform4uiv(GLint location, GLsizei count, const GLuint* value)
  {
  }

  inline void ClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value)
  {
  }

  inline void ClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value)
  {
  }

  inline void ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value)
  {
  }

  inline void ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
  {
  }

  inline const GLubyte* GetStringi(GLenum name, GLuint index)
  {
    return NULL;
  }

  inline void CopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
  {
  }

  inline void GetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar* const* uniformNames, GLuint* uniformIndices)
  {
  }

  inline void GetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params)
  {
  }

  inline GLuint GetUniformBlockIndex(GLuint program, const GLchar* uniformBlockName)
  {
    return GL_INVALID_INDEX;
  }

  inline void GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params)
  {
  }

  inline void GetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName)
  {
  }

  inline void UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
  {
  }

  inline void DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
  {
  }

  inline void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instanceCount)
  {
  }

  inline GLsync FenceSync(GLenum condition, GLbitfield flags)
  {
    return NULL;
  }

  inline GLboolean IsSync(GLsync sync)
  {
    return false;
  }

  inline void DeleteSync(GLsync sync)
  {
  }

  inline GLenum ClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
  {
    return 0;
  }

  inline void WaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
  {
  }

  inline void GetInteger64v(GLenum pname, GLint64* params)
  {
  }

  inline void GetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values)
  {
  }

  inline void GetInteger64i_v(GLenum target, GLuint index, GLint64* data)
  {
  }

  inline void GetBufferParameteri64v(GLenum target, GLenum pname, GLint64* params)
  {
  }

  inline void GenSamplers(GLsizei count, GLuint* samplers)
  {
  }

  inline void DeleteSamplers(GLsizei count, const GLuint* samplers)
  {
  }

  inline GLboolean IsSampler(GLuint sampler)
  {
    return false;
  }

  inline void BindSampler(GLuint unit, GLuint sampler)
  {
  }

  inline void SamplerParameteri(GLuint sampler, GLenum pname, GLint param)
  {
  }

  inline void SamplerParameteriv(GLuint sampler, GLenum pname, const GLint* param)
  {
  }

  inline void SamplerParameterf(GLuint sampler, GLenum pname, GLfloat param)
  {
  }

  inline void SamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat* param)
  {
  }

  inline void GetSamplerParameteriv(GLuint sampler, GLenum pname, GLint* params)
  {
  }

  inline void GetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat* params)
  {
  }

  inline void VertexAttribDivisor(GLuint index, GLuint divisor)
  {
  }

  inline void BindTransformFeedback(GLenum target, GLuint id)
  {
  }

  inline void DeleteTransformFeedbacks(GLsizei n, const GLuint* ids)
  {
  }

  inline void GenTransformFeedbacks(GLsizei n, GLuint* ids)
  {
  }

  inline GLboolean IsTransformFeedback(GLuint id)
  {
    return false;
  }

  inline void PauseTransformFeedback(void)
  {
  }

  inline void ResumeTransformFeedback(void)
  {
  }

  inline void GetProgramBinary(GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary)
  {
  }

  inline void ProgramBinary(GLuint program, GLenum binaryFormat, const GLvoid* binary, GLsizei length)
  {
  }

  inline void ProgramParameteri(GLuint program, GLenum pname, GLint value)
  {
  }

  inline void InvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments)
  {
  }

  inline void InvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height)
  {
  }

  inline void TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
  {
  }

  inline void TexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
  {
  }

  inline void GetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params)
  {
  }

public: // TEST FUNCTIONS
  inline void SetCompileStatus( GLuint value ) { mCompileStatus = value; }
  inline void SetLinkStatus( GLuint value ) { mLinkStatus = value; }
  inline void SetGetAttribLocationResult(  int result) { mGetAttribLocationResult = result; }
  inline void SetGetErrorResult(  GLenum result) { mGetErrorResult = result; }
  inline void SetGetStringResult(  GLubyte* result) { mGetStringResult = result; }
  inline void SetIsBufferResult(  GLboolean result) { mIsBufferResult = result; }
  inline void SetIsEnabledResult(  GLboolean result) { mIsEnabledResult = result; }
  inline void SetIsFramebufferResult(  GLboolean result) { mIsFramebufferResult = result; }
  inline void SetIsProgramResult(  GLboolean result) { mIsProgramResult = result; }
  inline void SetIsRenderbufferResult(  GLboolean result) { mIsRenderbufferResult = result; }
  inline void SetIsShaderResult(  GLboolean result) { mIsShaderResult = result; }
  inline void SetIsTextureResult(  GLboolean result) { mIsTextureResult = result; }
  inline void SetCheckFramebufferStatusResult(  GLenum result) { mCheckFramebufferStatusResult = result; }
  inline void SetNumBinaryFormats( GLint numFormats ) { mNumBinaryFormats = numFormats; }
  inline void SetBinaryFormats( GLint binaryFormats ) { mBinaryFormats = binaryFormats; }
  inline void SetProgramBinaryLength( GLint length ) { mProgramBinaryLength = length; }

  inline bool GetVertexAttribArrayState(GLuint index)
  {
    if( index >= MAX_ATTRIBUTE_CACHE_SIZE )
    {
      // out of range
      return false;
    }
    return mVertexAttribArrayState[ index ];
  }
  inline void ClearVertexAttribArrayChanged() {  mVertexAttribArrayChanged = false; }
  inline bool GetVertexAttribArrayChanged()  { return mVertexAttribArrayChanged; }

  //Methods for CullFace verification
  inline void EnableCullFaceCallTrace(bool enable) { mCullFaceTrace.Enable(enable); }
  inline void ResetCullFaceCallStack() { mCullFaceTrace.Reset(); }
  inline TraceCallStack& GetCullFaceTrace() { return mCullFaceTrace; }

  //Methods for Shader verification
  inline void EnableShaderCallTrace(bool enable) { mShaderTrace.Enable(enable); }
  inline void ResetShaderCallStack() { mShaderTrace.Reset(); }
  inline TraceCallStack& GetShaderTrace() { return mShaderTrace; }

  //Methods for Texture verification
  inline void EnableTextureCallTrace(bool enable) { mTextureTrace.Enable(enable); }
  inline void ResetTextureCallStack() { mTextureTrace.Reset(); }
  inline TraceCallStack& GetTextureTrace() { return mTextureTrace; }

  //Methods for Texture verification
  inline void EnableTexParameterCallTrace(bool enable) { mTexParamaterTrace.Enable(enable); }
  inline void ResetTexParameterCallStack() { mTexParamaterTrace.Reset(); }
  inline TraceCallStack& GetTexParameterTrace() { return mTexParamaterTrace; }

  //Methods for Draw verification
  inline void EnableDrawCallTrace(bool enable) { mDrawTrace.Enable(enable); }
  inline void ResetDrawCallStack() { mDrawTrace.Reset(); }
  inline TraceCallStack& GetDrawTrace() { return mDrawTrace; }

  template <typename T>
  inline bool CheckUniformValue( const char* name, const T& value ) const
  {
    for( ProgramUniformMap::const_iterator program_it = mUniforms.begin();
          program_it != mUniforms.end();
          ++program_it )
    {
      const UniformIDMap &uniformIDs = program_it->second;

      UniformIDMap::const_iterator uniform_it = uniformIDs.find( name );
      if( uniform_it != uniformIDs.end() )
      {
        // found one matching uniform name, lets check the value...
        GLuint programId = program_it->first;
        GLint uniformId = uniform_it->second;

        const ProgramUniformValue<T> &mProgramUniforms = GetProgramUniformsForType( value );
        if( mProgramUniforms.CheckUniformValue( programId, uniformId, value ) )
        {
          // the value matches
          return true;
        }
      }
    }

    fprintf(stderr, "Not found, printing possible values:" );
    for( ProgramUniformMap::const_iterator program_it = mUniforms.begin();
          program_it != mUniforms.end();
          ++program_it )
    {
      const UniformIDMap &uniformIDs = program_it->second;

      UniformIDMap::const_iterator uniform_it = uniformIDs.find( name );
      if( uniform_it != uniformIDs.end() )
      {
        // found one matching uniform name, lets check the value...
        GLuint programId = program_it->first;
        GLint uniformId = uniform_it->second;

        const ProgramUniformValue<T> &mProgramUniforms = GetProgramUniformsForType( value );
        T origValue;
        if ( mProgramUniforms.GetUniformValue(programId, uniformId, origValue) )
        {
          std::stringstream out;
          out << uniform_it->first << ": " << origValue;
          fprintf(stderr, "%s", out.str().c_str() );
        }
      }
    }
    return false;
  }

  template <typename T>
  inline bool GetUniformValue( GLuint programId, GLuint uniformId, T& outValue) const
  {
    const ProgramUniformValue<T> &mProgramUniforms = GetProgramUniformsForType( outValue );
    return mProgramUniforms.GetUniformValue( programId, uniformId, outValue );
  }

  inline bool GetUniformIds( const char* name, GLuint& programId, GLuint& uniformId ) const
  {
    for( ProgramUniformMap::const_iterator program_it = mUniforms.begin();
          program_it != mUniforms.end();
          ++program_it )
    {
      const UniformIDMap &uniformIDs = program_it->second;

      UniformIDMap::const_iterator uniform_it = uniformIDs.find( name );
      if( uniform_it != uniformIDs.end() )
      {
        programId = program_it->first;
        uniformId = uniform_it->second;
        return true;
      }
    }
    return false;
  }



  inline GLuint GetLastShaderCompiled() const
  {
    return mLastShaderCompiled;
  }

  inline GLuint GetLastProgramCreated() const
  {
    return mLastProgramIdUsed;
  }

  enum AttribType
  {
    ATTRIB_UNKNOWN = -1,
    ATTRIB_POSITION,
    ATTRIB_NORMAL,
    ATTRIB_TEXCOORD,
    ATTRIB_COLOR,
    ATTRIB_BONE_WEIGHTS,
    ATTRIB_BONE_INDICES,
    ATTRIB_TYPE_LAST
  };


private:
  GLuint     mCurrentProgram;
  GLuint     mCompileStatus;
  GLuint     mLinkStatus;
  GLint      mGetAttribLocationResult;
  GLenum     mGetErrorResult;
  GLubyte*   mGetStringResult;
  GLboolean  mIsBufferResult;
  GLboolean  mIsEnabledResult;
  GLboolean  mIsFramebufferResult;
  GLboolean  mIsProgramResult;
  GLboolean  mIsRenderbufferResult;
  GLboolean  mIsShaderResult;
  GLboolean  mIsTextureResult;
  GLenum     mActiveTextureUnit;
  GLenum     mCheckFramebufferStatusResult;
  GLint      mNumBinaryFormats;
  GLint      mBinaryFormats;
  GLint      mProgramBinaryLength;
  bool       mVertexAttribArrayState[MAX_ATTRIBUTE_CACHE_SIZE];
  bool       mVertexAttribArrayChanged;                            // whether the vertex attrib array has been changed
  typedef std::map< GLuint, std::string> ShaderSourceMap;
  ShaderSourceMap mShaderSources;
  GLuint     mLastShaderCompiled;

  Vector4 mLastBlendColor;
  GLenum  mLastBlendEquationRgb;
  GLenum  mLastBlendEquationAlpha;
  GLenum  mLastBlendFuncSrcRgb;
  GLenum  mLastBlendFuncDstRgb;
  GLenum  mLastBlendFuncSrcAlpha;
  GLenum  mLastBlendFuncDstAlpha;

  // Data for manipulating the IDs returned by GenTextures
  GLuint mLastAutoTextureIdUsed;
  std::vector<GLuint> mNextTextureIds;
  std::vector<GLuint> mDeletedTextureIds;
  std::vector<GLuint> mBoundTextures;

  struct ActiveTextureType
  {
    std::vector<GLuint> mBoundTextures;
  };

  ActiveTextureType mActiveTextures[ MIN_TEXTURE_UNIT_LIMIT ];

  TraceCallStack mCullFaceTrace;
  TraceCallStack mShaderTrace;
  TraceCallStack mTextureTrace;
  TraceCallStack mTexParamaterTrace;
  TraceCallStack mDrawTrace;

  // Shaders & Uniforms
  GLuint mLastShaderIdUsed;
  GLuint mLastProgramIdUsed;
  GLuint mLastUniformIdUsed;
  typedef std::map< std::string, GLint > UniformIDMap;
  typedef std::map< GLuint, UniformIDMap > ProgramUniformMap;
  ProgramUniformMap mUniforms;

  template <typename T>
  struct ProgramUniformValue : public std::map< GLuint, std::map< GLint, T > >
  {
  public:
    typedef std::map< GLint, T > UniformValueMap;
    typedef std::map< GLuint, UniformValueMap > Map;

    bool SetUniformValue( GLuint program, GLuint uniform, const T& value )
    {
      if( program == 0 )
      {
        return false;
      }

      typename Map::iterator it = Map::find( program );
      if( it == Map::end() )
      {
        // if its the first uniform for this program add it
        std::pair< typename Map::iterator, bool > result =
            Map::insert( typename Map::value_type( program, UniformValueMap() ) );
        it = result.first;
      }

      UniformValueMap& uniforms = it->second;
      uniforms[uniform] = value;

      return true;
    }

    bool CheckUniformValue( GLuint program, GLuint uniform, const T& value ) const
    {
      T uniformValue;
      if ( GetUniformValue( program, uniform, uniformValue ) )
      {
        return value == uniformValue;
      }

      return false;
    }

    bool GetUniformValue( GLuint program, GLuint uniform, T& value ) const
    {
      if( program == 0 )
      {
        return false;
      }

      typename Map::const_iterator it = Map::find( program );
      if( it == Map::end() )
      {
        // Uniform values always initialised as 0
        value = GetZero();
        return true;
      }

      const UniformValueMap& uniforms = it->second;
      typename UniformValueMap::const_iterator it2 = uniforms.find( uniform );
      if( it2 == uniforms.end() )
      {
        // Uniform values always initialised as 0
        value = GetZero();
        return true;
      }
      value = it2->second;

      return true;
    }

    T GetZero() const;
  };
  ProgramUniformValue<int> mProgramUniforms1i;
  ProgramUniformValue<float> mProgramUniforms1f;
  ProgramUniformValue<Vector2> mProgramUniforms2f;
  ProgramUniformValue<Vector3> mProgramUniforms3f;
  ProgramUniformValue<Vector4> mProgramUniforms4f;
  ProgramUniformValue<Matrix> mProgramUniformsMat4;
  ProgramUniformValue<Matrix3> mProgramUniformsMat3;

  inline const ProgramUniformValue<int>& GetProgramUniformsForType( const int ) const
  {
    return mProgramUniforms1i;
  }
  inline const ProgramUniformValue<float>& GetProgramUniformsForType( const float ) const
  {
    return mProgramUniforms1f;
  }
  inline const ProgramUniformValue<Vector2>& GetProgramUniformsForType( const Vector2& ) const
  {
    return mProgramUniforms2f;
  }
  inline const ProgramUniformValue<Vector3>& GetProgramUniformsForType( const Vector3& ) const
  {
    return mProgramUniforms3f;
  }
  inline const ProgramUniformValue<Vector4>& GetProgramUniformsForType( const Vector4& ) const
  {
    return mProgramUniforms4f;
  }
  inline const ProgramUniformValue<Matrix>& GetProgramUniformsForType( const Matrix& ) const
  {
    return mProgramUniformsMat4;
  }
  inline const ProgramUniformValue<Matrix3>& GetProgramUniformsForType( const Matrix3& ) const
  {
    return mProgramUniformsMat3;
  }
  inline void SetVertexAttribArray(GLuint index, bool state)
  {
    if( index >= MAX_ATTRIBUTE_CACHE_SIZE )
    {
      // out of range
      return;
    }
    mVertexAttribArrayState[ index ] = state;
    mVertexAttribArrayChanged = true;
  }
};

template <>
inline int TestGlAbstraction::ProgramUniformValue<int>::GetZero() const
{
  return 0;
}

template <>
inline float TestGlAbstraction::ProgramUniformValue<float>::GetZero() const
{
  return 0.0f;
}

template <>
inline Vector2 TestGlAbstraction::ProgramUniformValue<Vector2>::GetZero() const
{
  return Vector2::ZERO;
}

template <>
inline Vector3 TestGlAbstraction::ProgramUniformValue<Vector3>::GetZero() const
{
  return Vector3::ZERO;
}

template <>
inline Vector4 TestGlAbstraction::ProgramUniformValue<Vector4>::GetZero() const
{
  return Vector4::ZERO;
}

template <>
inline Matrix TestGlAbstraction::ProgramUniformValue<Matrix>::GetZero() const
{
  return Matrix();
}

template <>
inline Matrix3 TestGlAbstraction::ProgramUniformValue<Matrix3>::GetZero() const
{
  return Matrix3( Matrix() );
}

} // namespace Dali

bool BlendEnabled(const Dali::TraceCallStack& callStack);
bool BlendDisabled(const Dali::TraceCallStack& callStack);




#endif // __TEST_GL_ES_H__
