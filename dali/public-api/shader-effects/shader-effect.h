#ifndef __DALI_SHADER_EFFECT_H__
#define __DALI_SHADER_EFFECT_H__

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
#include <dali/public-api/object/handle.h> // Dali::Handle

/**
 * @brief DALI_COMPOSE_SHADER macro provides a convenient way to write shader source code.
 *
 * We normally use double quotation marks to write a string such as "Hello World".
 * However many symbols are needed to add multiple lines of string.
 * We don't need to write quotation marks using this macro at every line.
 *
 * [An example of double quotation marks usage]
 * <pre>
 * const string FRAGMENT_SHADER_SOURCE = \
 * "  void main()\n"
 * "  {\n"
 * "    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n"
 * "  }\n";
 * </pre><br/>
 * [An example of DALI_COMPOSE_SHADER usage]
 * <pre>
 * const string VERTEX_SHADER_SOURCE = DALI_COMPOSE_SHADER (
 *   void main()
 *   {
 *     gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);
 *     vTexCoord = aTexCoord;
 *   }
 * );
 * </pre>
 */
#define DALI_COMPOSE_SHADER(STR) #STR

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class ShaderEffect;
}

/**
 * @brief ShaderEffects allows custom vertex and color transformations in the GPU
 */
class DALI_IMPORT_API ShaderEffect : public Handle
{
public:
  static const Property::Index PROGRAM;            ///< name "program",        type MAP; {"vertex-prefix":"","fragment-prefix":"","vertex":"","fragment":""}
  static const Property::Index GEOMETRY_HINTS;     ///< name "geometry-hints", type INT (bitfield) values from enum GeometryHints

  /**
   * @brief Hints for rendering/subdividing geometry.
   */
  enum GeometryHints
  {
    HINT_NONE                   = 0x00, ///< no hints
    HINT_SELF_DEPTH_TESTING     = 0x01, ///< Expects depth testing enabled
    HINT_TRANSPARENCY           = 0x02, ///< Might generate transparent alpha from opaque
    HINT_MODIFIES_GEOMETRY      = 0x04, ///< Might change position of vertices
  };

  /**
   * @brief Create ShaderEffect.
   *
   * @param vertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param fragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A handle to a shader effect
   */
  static ShaderEffect New( const std::string& vertexShader,
                           const std::string& fragmentShader,
                           GeometryHints hints = GeometryHints(HINT_NONE) );

  /**
   * @brief Create ShaderEffect.
   * @param vertexShaderPrefix code for the effect. It will be inserted before the default uniforms (ideal for \#defines)
   * @param vertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param fragmentShaderPrefix code for the effect. It will be inserted before the default uniforms (ideal for \#defines)
   * @param fragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A handle to a shader effect
   */
  static ShaderEffect NewWithPrefix(const std::string& vertexShaderPrefix,
                                    const std::string& vertexShader,
                                    const std::string& fragmentShaderPrefix,
                                    const std::string& fragmentShader,
                                    GeometryHints hints = GeometryHints(HINT_NONE) );

  /**
   * @brief Downcast an Object handle to ShaderEffect.
   *
   * If handle points to a ShaderEffect the downcast produces valid
   * handle. If not the returned handle is empty.
   *
   * @param[in] handle to An object
   * @return handle to a ShaderEffect object or an uninitialized handle
   */
  static ShaderEffect DownCast( BaseHandle handle );

  /**
   * @brief Default constructor, creates an empty handle
   */
  ShaderEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ShaderEffect();

  /**
   * @brief Copy constructor
   *
   * @param object A reference to a ShaderEffect object
   */
  ShaderEffect(const ShaderEffect& object);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  ShaderEffect& operator=(const ShaderEffect& rhs);

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali New() methods.
   * @param [in] effect A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL ShaderEffect(Internal::ShaderEffect* effect);
};

} // namespace Dali

#endif // __DALI_SHADER_EFFECT_H__
