#ifndef _CTS_PROPERTYNOTIFICATION_COMMON_H_
#define _CTS_PROPERTYNOTIFICATION_COMMON_H_

#include "../common/dali-common.h"

using namespace std;
using namespace Dali;

/** @addtogroup Cts-dali-core
* @brief Cts testsuites for module dali-core
* @ingroup cts
* @{
*/


/** @addtogroup Cts-property-notification
* @brief Cts testcases and testcase helper function, callbacks for suite actor
* @ingroup Cts-dali-core
* @{
*/

#define SUITE_NAME "PROPERTY_NOTIFICATION_CTS"


#define RENDER_FRAME_INTERVAL_P	16                             ///< Duration of each frame in ms. (at approx 60FPS)
#define DEFAULT_WAIT_PERIOD 100                            ///< Default waiting period for check.

extern bool gCallBackCalled;


/**Set the Variable gCallBackCalled true*/
void PropertyNotificationTestCallback(PropertyNotification& source);

/**Test for varrying the input parameter*/
bool PropertyNotificationSetGetNotifyMode(PropertyNotification &notification, PropertyNotification::NotifyMode notifyModeCheck );


/**========================================*/
/** Custom connection tracker class */
/**========================================*/
//Class which  inherit connectiontracker
class TestClass : public ConnectionTracker
{
   public:

    TestClass()
    {
    }

    ~TestClass()
    {
    }

    bool Initialize()
    {
	mGreaterThan = 100.0f;
	mActor = Actor::New();
	DALI_CHECK_FALSE(!mActor, "actor is not created" );

	Stage::GetCurrent().Add( mActor );
	mNotification = mActor.AddPropertyNotification( Actor::Property::POSITION_X, GreaterThanCondition(mGreaterThan));
	mNotification.NotifySignal().Connect( this, &TestClass::OnPropertyNotify );
	return true;
    }

    void RemovePropertyNotification()
    {
	mActor.RemovePropertyNotification( mNotification );
    }

    void RemovePropertyNotifications()
    {
	mActor.RemovePropertyNotifications();
    }

    void Terminate()
    {
	Stage::GetCurrent().Remove( mActor );
	mActor.Reset();
	mNotification.Reset();
    }

    void OnPropertyNotify( PropertyNotification& source )
    {
	gCallBackCalled = true;
    }

    Actor mActor;
    PropertyNotification mNotification;
    float mGreaterThan;
};

//Class for propertynotification
class PropertyNotificationConstWrapper
{
  public:

   PropertyNotificationConstWrapper(PropertyNotification propertyNotification):mPropertyNotification(propertyNotification)
   {
   }

  /**
   * Returns const reference to property notification.
   * @return const reference.
   */
  const PropertyCondition& GetCondition() const
  {
     return mPropertyNotification.GetCondition();
  }

  private:
   PropertyNotification mPropertyNotification;
};

/** @} */
/** @} */

#endif  //_CTS_PROPERTYNOTIFICATION_COMMON_H_
