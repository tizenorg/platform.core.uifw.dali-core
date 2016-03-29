#ifndef _CTS_PROPERTYNOTIFICATION_COMMON_H_
#define _CTS_PROPERTYNOTIFICATION_COMMON_H_

#include "dali-common.h"

using namespace std;
using namespace Dali;

/** @addtogroup Cts-dali-core
*	@brief Integrated testsuites for module dali-core
*	@ingroup itc
*	@{ 
*/


/** @addtogroup Cts-property-notification
*	@brief Integrated testcases and testcase helper function, callbacks for suite actor
*	@ingroup Cts-dali-core
*	@{ 
*/

#define SUITE_NAME             "PROPERTY_NOTIFICATION_CTS"


#define RENDER_FRAME_INTERVAL_P	    	16                             ///< Duration of each frame in ms. (at approx 60FPS)
#define DEFAULT_WAIT_PERIOD				100                            ///< Default waiting period for check.

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
		m_fGreatherthan = 100.0f;
		m_Actor = Actor::New();
		DALI_CHECK_FALSE(!m_Actor, "actor is not created" );
      
		Stage::GetCurrent().Add( m_Actor );
		m_Notification = m_Actor.AddPropertyNotification( Actor::Property::POSITION_X, GreaterThanCondition(m_fGreatherthan));
		m_Notification.NotifySignal().Connect( this, &TestClass::OnPropertyNotify );
		return true;
    }
 
    void RemovePropertyNotification()
    {
		m_Actor.RemovePropertyNotification( m_Notification );
    }
 
    void RemovePropertyNotifications()
    {
		m_Actor.RemovePropertyNotifications();
    }
 
    void Terminate()
    {
		Stage::GetCurrent().Remove( m_Actor );
		m_Actor.Reset();
		m_Notification.Reset();
    }
 
    void OnPropertyNotify( PropertyNotification& source )
    {
		gCallBackCalled = true;
    }
 
    Actor m_Actor;
    PropertyNotification m_Notification;
    float m_fGreatherthan;
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

#endif  //_ITS_PROPERTYNOTIFICATION_COMMON_H_
