
#include <dali/internal/render/renderers/render-property-buffer.h>


namespace Dali
{
namespace Internal
{
namespace Render
{

void PropertyBuffer::Update( Context& context )
{
  if( !mGpuBuffer || mDataChanged )
  {
    if ( ! mGpuBuffer )
    {
       mGpuBuffer = new GpuBuffer( context, mGpuBufferTarget, mGpuBufferUsage );
    }

    // Update the GpuBuffer
    if ( mGpuBuffer )
    {
      DALI_ASSERT_DEBUG( mSize && "No data in the property buffer!" );

      const void *data = &(mData[0]);
      Vector<unsigned short> ushortData;

      // Index buffer needs to be unsigned short which is not supported by the property system
      if( mGpuBufferTarget == GpuBuffer::ELEMENT_ARRAY_BUFFER )
      {
        ushortData.Resize( mSize );
        const unsigned int* unsignedData = static_cast<const unsigned int*>(data);
        unsigned int numberOfElements = mSize / sizeof(unsigned int);
        for( unsigned int i = 0; i < numberOfElements; ++i )
        {
          ushortData[i] = unsignedData[i];
        }
        data = &(ushortData[0]);
      }

      mGpuBuffer->UpdateDataBuffer( mSize, data );
      mGpuBuffer->SetStride( mFormat->size );
    }

    mDataChanged = false;
  }
}

void PropertyBuffer::BindBuffer()
{
  mGpuBuffer->Bind();
}

unsigned int PropertyBuffer::EnableVertexAttributes( Context& context, Vector<GLint>& vAttributeLocation, unsigned int locationBase )
{
  unsigned int attributeCount = mFormat->components.size();

  GLsizei elementSize = mFormat->size;

  for( unsigned int i = 0; i < attributeCount; ++i )
  {
    GLint attributeLocation = vAttributeLocation[i+locationBase];
    if( attributeLocation != -1 )
    {
      context.EnableVertexAttributeArray( attributeLocation );

      GLint attributeSize = mFormat->components[i].size;
      size_t attributeOffset = mFormat->components[i].offset;
      Property::Type attributeType = mFormat->components[i].type;

      context.VertexAttribPointer( attributeLocation,
                                   attributeSize  / GetPropertyImplementationGlSize(attributeType),
                                   GetPropertyImplementationGlType(attributeType),
                                   GL_FALSE,  // Not normalized
                                   elementSize,
                                   (void*)attributeOffset );
    }
  }

  return attributeCount;
}

} //Render
} //Internal
} //Dali
