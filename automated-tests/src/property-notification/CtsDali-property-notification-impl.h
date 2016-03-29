#include "CtsDali-property-notification-common.h"
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

/** @addtogroup Cts-property-notification-testcases
*	@brief Integrated testcases and testcase helper function, callbacks for suite actor
*	@ingroup  Cts-property-notification
*	@{ 
*/
int g_nRenderCount_Property_Notification;
TestClass* g_object;
Actor g_actor_pn;
PropertyNotification::NotifyMode g_notifyMode;	
PropertyNotification::NotifyMode g_notifyModeCheck;
PropertyNotification g_property_notification;
float g_nminXasixPN= 0.0f, g_nminYaxisPN = 0.0f, g_nminZaxisPN = 0.0f, g_nmaxXasixPN = 200.0f;


// validate - common functions

void VTPropertyNotificationTriggered()
{
  DALI_CHECK_FAIL( !gCallBackCalled , "Notification is not Triggered");
  Stage::GetCurrent().Remove(g_actor_pn);
  DaliLog::PrintPass();
}

void VTPropertyNotificationNotTriggered()
{
  DALI_CHECK_FAIL( gCallBackCalled , "Notification is Triggered");
  Stage::GetCurrent().Remove(g_actor_pn);
  DaliLog::PrintPass();
}

void VTNoValidationRequired()
{
  DaliLog::PrintPass();
}

// setup - PropertyNotificationSetGetNotifyModeONChanged001

void PropertyNotificationSetGetNotifyModeONChanged001()
{
  printf("PropertyNotificationSetGetNotifyModeONChanged entered\n");
	float fGreaterVal = 100.0f;
	gCallBackCalled = false;
	g_notifyModeCheck = PropertyNotification::NotifyOnTrue;
	
	g_actor_pn = Actor::New();
	DALI_CHECK_FAIL(!g_actor_pn, "actor is not created " );
	
	Stage::GetCurrent().Add(g_actor_pn);
	g_property_notification = g_actor_pn.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(fGreaterVal));
	
	bool bRet = false;
	bRet = PropertyNotificationSetGetNotifyMode( g_property_notification, g_notifyModeCheck );
	DALI_CHECK_FAIL(!bRet, "Notification check is failed." );
}

void PropertyNotificationSetGetNotifyModeONChanged002()
{
	 g_actor_pn.SetPosition(Vector3(g_nmaxXasixPN,g_nminYaxisPN,g_nminZaxisPN));
}
void PropertyNotificationSetGetNotifyModeONChanged003()
{
	 g_actor_pn.SetPosition(Vector3(g_nminXasixPN, g_nminYaxisPN, g_nminZaxisPN));
}

// setup - PropertyNotificationSetGetNotifyModeONTrue001

void PropertyNotificationSetGetNotifyModeONTrue001()
{
	float fGreaterVal = 100.0f;
	gCallBackCalled = false;
	g_notifyMode = PropertyNotification::NotifyOnFalse;
	g_notifyModeCheck = PropertyNotification::NotifyOnTrue;
	
	g_actor_pn = Actor::New();	
	DALI_CHECK_FAIL(!g_actor_pn, "actor is not created" );
	
	Stage::GetCurrent().Add(g_actor_pn);
	g_property_notification= g_actor_pn.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(fGreaterVal));
	
	bool bRet = false;
	bRet = PropertyNotificationSetGetNotifyMode( g_property_notification, g_notifyModeCheck );
	DALI_CHECK_FAIL(!bRet, "Notification check is failed." );	
}

void PropertyNotificationSetGetNotifyModeONTrue002()
{
	 g_actor_pn.SetPosition(Vector3(g_nminXasixPN, g_nminYaxisPN, g_nminZaxisPN));
}
void PropertyNotificationSetGetNotifyModeONTrue003()
{
	 g_actor_pn.SetPosition(Vector3(g_nmaxXasixPN, g_nminYaxisPN, g_nminZaxisPN));
}

// setup - PropertyNotificationSetGetNotifyModeONFalse001

void PropertyNotificationSetGetNotifyModeONFalse001()
{
	float fGreaterVal = 100.0f;
	gCallBackCalled = false;
	g_notifyMode =  PropertyNotification::NotifyOnFalse;
	g_notifyModeCheck = PropertyNotification::NotifyOnTrue;
	
	g_actor_pn= Actor::New();
	DALI_CHECK_FAIL(!g_actor_pn, " actor is not created" );
	
	Stage::GetCurrent().Add(g_actor_pn);
	g_property_notification = g_actor_pn.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(fGreaterVal));
	
	bool bRet = false;
	bRet = PropertyNotificationSetGetNotifyMode( g_property_notification, g_notifyModeCheck );
	DALI_CHECK_FAIL(!bRet, "Notification check is failed." );		
}
void PropertyNotificationSetGetNotifyModeONFalse002()
{
	 g_actor_pn.SetPosition(Vector3(g_nmaxXasixPN,g_nminYaxisPN, g_nminZaxisPN));
}
void PropertyNotificationSetGetNotifyModeONFalse003()
{
	 g_actor_pn.SetPosition(Vector3(g_nminXasixPN, g_nminYaxisPN, g_nminZaxisPN));
}

// setup - PropertyNotificationSetGetNotifyModeDisabled001

void PropertyNotificationSetGetNotifyModeDisabled001()
{
	float fGreaterVal = 100.0f;
	gCallBackCalled = false;
	g_notifyMode = PropertyNotification::Disabled;	
	g_notifyModeCheck = PropertyNotification::NotifyOnTrue;
	
	g_actor_pn = Actor::New();
	DALI_CHECK_FAIL(!g_actor_pn, "actor is not created " );
	
	Stage::GetCurrent().Add(g_actor_pn);
	g_property_notification= g_actor_pn.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(fGreaterVal));
	
	bool bRet = false;
	bRet = PropertyNotificationSetGetNotifyMode( g_property_notification, g_notifyModeCheck );
	DALI_CHECK_FAIL(!bRet, "Notification check is failed." );	
}
void PropertyNotificationSetGetNotifyModeDisabled002()
{
	 g_actor_pn.SetPosition(Vector3(g_nmaxXasixPN, g_nminYaxisPN, g_nminZaxisPN));	
}
void PropertyNotificationSetGetNotifyModeDisabled003()
{
	 g_actor_pn.SetPosition(Vector3(g_nminXasixPN,g_nminYaxisPN, g_nminZaxisPN ));
}

// setup - PropertyNotificationGetCondition

void PropertyNotificationGetCondition()
{
	Actor actor;
	Stage stage;
	float fGreaterVal = 100.0f;
	PropertyNotification notification;
	PropertyCondition condition;
	PropertyCondition condition2;
	
	stage = Stage::GetCurrent();
	DALI_CHECK_FAIL(!stage, "stage is not created" );
	
	actor = Actor::New();
	DALI_CHECK_FAIL(!actor, " actor is not created" );	
	
	stage.Add(actor);
	
	condition = GreaterThanCondition(fGreaterVal);
	notification = actor.AddPropertyNotification(Actor::Property::POSITION_X,condition);	
	
	condition2 = notification.GetCondition(); 
	
	DALI_CHECK_FAIL( condition2 != condition , " PropertyNotification Condition missmatched to the set condition" );
	
	stage.Remove(actor);
	
	DaliLog::PrintPass();
}

void PropertyNotificationGetConditionWithAddress()
{
	Actor actor;
	Stage stage;
	float fGreaterVal = 100.0f;
	PropertyNotification notification;
	PropertyCondition condition;
	
	stage = Stage::GetCurrent();
	DALI_CHECK_FAIL(!stage, " stage is not created" );
	actor = Actor::New();
	DALI_CHECK_FAIL(!actor, " actor is not created" );
	stage.Add(actor);
	
	condition = GreaterThanCondition(fGreaterVal);
	notification = actor.AddPropertyNotification(Actor::Property::POSITION_X,condition);
	PropertyNotificationConstWrapper notificationConst(notification);			
	
	const PropertyCondition& conditionReference1 = notificationConst.GetCondition();	
		
	DALI_CHECK_FAIL(conditionReference1 != condition , "conditionReference1 != condition" );
	
	stage.Remove(actor);
	
	DaliLog::PrintPass();
}

void PropertyNotificationGetTarget()
{
	float fGreaterVal = 100.0f;
	Handle handleobject;
	Handle handleobject2;
	PropertyNotification notification;
	PropertyCondition condition;
	
	handleobject = Actor::New();
	DALI_CHECK_FAIL(!handleobject, "handleobject is not created " );
	
	condition = GreaterThanCondition(fGreaterVal);
	notification = handleobject.AddPropertyNotification(Actor::Property::POSITION_X,condition);
	
	handleobject2 = notification.GetTarget(); 
	

	DALI_CHECK_FAIL( !handleobject2 , "Not a valid handleobject" );
	
	DALI_CHECK_FAIL( handleobject2 != handleobject , "Getting Target failed" );

	
	DaliLog::PrintPass();
}

void PropertyNotificationGetTargetProperty()
{
	float fGreaterVal = 100.0f;
	Handle handleobject;
	Handle handleobject2;
	PropertyNotification notification;
	PropertyCondition condition;
	
	Property::Index indexSetValue;
	Property::Index indexGetValue;

	handleobject = Actor::New();
	DALI_CHECK_FAIL(!handleobject, "handleobject is not created" );
	condition = GreaterThanCondition(fGreaterVal);	
	
	indexSetValue = Actor::Property::POSITION_X;		
	notification = handleobject.AddPropertyNotification(indexSetValue,condition);			
	indexGetValue = notification.GetTargetProperty();	
	DALI_CHECK_FAIL( indexGetValue != indexSetValue , "Getting Target failed for position X" );
	
	indexSetValue = Actor::Property::POSITION_Y;
	notification = handleobject.AddPropertyNotification(indexSetValue,condition);			
	indexGetValue = notification.GetTargetProperty();
	DALI_CHECK_FAIL( indexGetValue != indexSetValue , "Getting Target failed  for position Y" );
		
	DaliLog::PrintPass();
}

// setup - PropertyNotificationGetNotifyResult001

void PropertyNotificationGetNotifyResult001()
{
	PropertyNotification::NotifyMode notifyModeCheck = PropertyNotification::NotifyOnChanged;
	
	bool bInitialize = false;
	Vector3 vecSetVal(0.0f, 0.0f, 0.0f), vecSetVal2(200.0f, 0.0f, 0.0f);	

	g_object = new TestClass();
	
	bInitialize = g_object->Initialize();	
	DALI_CHECK_FAIL(!bInitialize, "Object has not been initialized" );
	
	/** @note need to render to pass this tc */
	g_object->m_Notification.SetNotifyMode(notifyModeCheck);
	/** @note need to render after set property to pass this tc */
	g_object->m_Actor.SetPosition(vecSetVal);
	/** @note need to render after set property to pass this tc */
	gCallBackCalled = false;
	g_object->m_Actor.SetPosition(vecSetVal2);	
}

// validate - VTPropertyNotificationGetNotifyResult001

void VTPropertyNotificationGetNotifyResult001()
{
	bool bResult= false;
	DALI_CHECK_FAIL( !gCallBackCalled , "Notification not Triggered" );
	
	bResult= g_object->m_Notification.GetNotifyResult();
	DALI_CHECK_FAIL( !bResult , "GetNotifyResult Invalid" );
	
	DaliLog::PrintPass();
}

void PropertyNotificationDownCast()
{
	float fCondition = 100.0f;
	/**Actor handle construction is required for constructing property notification**/
	Actor actor = Actor::New();
	DALI_CHECK_FAIL(!actor, "Actor handle is empty");
	
	/**PropertyCondition handle construction is required for constructing property notification**/
	PropertyCondition condition = GreaterThanCondition(fCondition);
	DALI_CHECK_FAIL(!condition, "PropertyCondition handle is empty");
	
	/**An property index is required for constructing property notification**/
	Property::Index index = Actor::Property::POSITION_X;
	
	PropertyNotification notification = actor.AddPropertyNotification(index, condition);
	DALI_CHECK_FAIL(!notification, "PropertyNotification instance is not created.");
	
	BaseHandle baseHandle = notification;
	PropertyNotification notificationDownCasted = PropertyNotification::DownCast( baseHandle );
	DALI_CHECK_FAIL(!notificationDownCasted, "PropertyNotification instance is empty after downcast.");
	
	Handle handleTarget = notificationDownCasted.GetTarget();
	Property::Index indexTarget = notificationDownCasted.GetTargetProperty();
	PropertyCondition conditionGet  = notificationDownCasted.GetCondition();
	
	DALI_CHECK_FAIL( (handleTarget != actor) || (indexTarget != index) || ( conditionGet != condition ), "PropertyNotification::DownCast is failed with expected target, property and condition" );
	
	DaliLog::PrintPass();
}

/** @} */
/** @} */
/** @} */
