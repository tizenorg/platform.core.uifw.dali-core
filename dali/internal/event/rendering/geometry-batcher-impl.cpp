#include "geometry-batcher-impl.h"

#include <dali/public-api/actors/actor.h>
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{

namespace Internal
{

GeometryBatcher* GeometryBatcher::New( const Dali::Actor &batchParent )
{
  return new GeometryBatcher( batchParent );
}

GeometryBatcher::GeometryBatcher( const Dali::Actor& batchParent ) :
  mBatchParentActor( batchParent )
{

}

void GeometryBatcher::UpdateGeometry( const Dali::Actor& actor, Dali::Geometry& geometry )
{

  GeometryArray::iterator it;
  for( it = mGeometries.begin();
       it != mGeometries.end(); ++it )
  {
    if( it->first == actor )
    {
      // update actor
      it->second = geometry;
    }
  }

  if( it == mGeometries.end() )
  {
    mGeometries.push_back( ActorGeometryPair( actor, geometry ) );
  }
}


} // Internal

} // Dali

