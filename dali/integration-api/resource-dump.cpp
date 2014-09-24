#include "resource-dump.h"

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/event/resources/resource-ticket.h>
using Dali::Internal::GestureEventProcessor;
using Dali::Internal::ThreadLocalStorage;

namespace Dali
{

namespace Integration
{
Integration::Bitmap* GetBitmapResource(Integration::ResourceId res)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  Internal::ResourceClient* client = &tls.GetResourceClient();
  Internal::ResourceTicketPtr ptr = client->RequestResourceTicket(res);
  Integration::Bitmap* bmp = client->GetBitmap(ptr);
  return bmp;
}

} // namespace Integration

} // namespace Dali
