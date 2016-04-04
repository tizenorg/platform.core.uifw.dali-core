#include "CtsDali-property-notification-common.h"
/** @addtogroup Cts-dali-core
* @brief Cts for module dali-core
* @ingroup cts
* @{
*/

/** @addtogroup Cts-property-notification
* @brief Cts testcases and testcase helper function, callbacks for suite actor
* @ingroup Cts-dali-core
* @{
*/

/** @addtogroup Cts-property-notification-testcases
* @brief Cts testcases and testcase helper function, callbacks for suite actor
* @ingroup  Cts-property-notification
* @{
*/
int gRenderCountPropertyNotification;
TestClass* gObject;
Actor gActorPropertyNotification;
PropertyNotification::NotifyMode gNotifyMode;
PropertyNotification::NotifyMode gNotifyModeCheck;
PropertyNotification gPropertyNotification;
const float MINIMUM_X_POSITION= 0.0f, MINIMUM_Y_POSITION = 0.0f, MINIMUM_Z_POSITION = 0.0f, MAXIMUM_X_POSITION = 200.0f;


// validate - common functions

void VTPropertyNotificationTriggered()
{
  DALI_CHECK_FAIL( !gCallBackCalled , "Notification is not Triggered");
  Stage::GetCurrent().Remove(gActorPropertyNotification);
  DaliLog::PrintPass();
}

void VTPropertyNotificationNotTriggered()
{
  DALI_CHECK_FAIL( gCallBackCalled , "Notification is Triggered");
  Stage::GetCurrent().Remove(gActorPropertyNotification);
  DaliLog::PrintPass();
}

void VTNoValidationRequired()
{
  DaliLog::PrintPass();
}

// setup - PropertyNotificationSetGetNotifyModeONChanged001

void PropertyNotificationSetGetNotifyModeONChanged001()
{
  DaliLog::PrintMgs("PropertyNotificationSetGetNotifyModeONChanged entered\n");
  const float GREATER_VALUE = 100.0f;
  gCallBackCalled = false;
  gNotifyModeCheck = PropertyNotification::NotifyOnTrue;

  gActorPropertyNotification = Actor::New();
  DALI_CHECK_FAIL(!gActorPropertyNotification, "actor is not created " );

  Stage::GetCurrent().Add(gActorPropertyNotification);
  gPropertyNotification = gActorPropertyNotification.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(GREATER_VALUE));

  bool bRet = false;
  bRet = PropertyNotificationSetGetNotifyMode( gPropertyNotification, gNotifyModeCheck );
  DALI_CHECK_FAIL(!bRet, "Notification check is failed." );
}

void PropertyNotificationSetGetNotifyModeONChanged002()
{
  gActorPropertyNotification.SetPosition(Vector3(MAXIMUM_X_POSITION,MINIMUM_Y_POSITION,MINIMUM_Z_POSITION));
}
void PropertyNotificationSetGetNotifyModeONChanged003()
{
  gActorPropertyNotification.SetPosition(Vector3(MINIMUM_X_POSITION, MINIMUM_Y_POSITION, MINIMUM_Z_POSITION));
}

// setup - PropertyNotificationSetGetNotifyModeONTrue001

void PropertyNotificationSetGetNotifyModeONTrue001()
{
  float GREATER_VALUE = 100.0f;
  gCallBackCalled = false;
  gNotifyMode = PropertyNotification::NotifyOnFalse;
  gNotifyModeCheck = PropertyNotification::NotifyOnTrue;

  gActorPropertyNotification = Actor::New();
  DALI_CHECK_FAIL(!gActorPropertyNotification, "actor is not created" );

  Stage::GetCurrent().Add(gActorPropertyNotification);
  gPropertyNotification= gActorPropertyNotification.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(GREATER_VALUE));

  bool bRet = false;
  bRet = PropertyNotificationSetGetNotifyMode( gPropertyNotification, gNotifyModeCheck );
  DALI_CHECK_FAIL(!bRet, "Notification check is failed." );
}

void PropertyNotificationSetGetNotifyModeONTrue002()
{
  gActorPropertyNotification.SetPosition(Vector3(MINIMUM_X_POSITION, MINIMUM_Y_POSITION, MINIMUM_Z_POSITION));
}
void PropertyNotificationSetGetNotifyModeONTrue003()
{
  gActorPropertyNotification.SetPosition(Vector3(MAXIMUM_X_POSITION, MINIMUM_Y_POSITION, MINIMUM_Z_POSITION));
}

// setup - PropertyNotificationSetGetNotifyModeONFalse001

void PropertyNotificationSetGetNotifyModeONFalse001()
{
  float GREATER_VALUE = 100.0f;
  gCallBackCalled = false;
  gNotifyMode =  PropertyNotification::NotifyOnFalse;
  gNotifyModeCheck = PropertyNotification::NotifyOnTrue;

  gActorPropertyNotification= Actor::New();
  DALI_CHECK_FAIL(!gActorPropertyNotification, " actor is not created" );

  Stage::GetCurrent().Add(gActorPropertyNotification);
  gPropertyNotification = gActorPropertyNotification.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(GREATER_VALUE));

  bool bRet = false;
  bRet = PropertyNotificationSetGetNotifyMode( gPropertyNotification, gNotifyModeCheck );
  DALI_CHECK_FAIL(!bRet, "Notification check is failed." );
}
void PropertyNotificationSetGetNotifyModeONFalse002()
{
  gActorPropertyNotification.SetPosition(Vector3(MAXIMUM_X_POSITION,MINIMUM_Y_POSITION, MINIMUM_Z_POSITION));
}
void PropertyNotificationSetGetNotifyModeONFalse003()
{
  gActorPropertyNotification.SetPosition(Vector3(MINIMUM_X_POSITION, MINIMUM_Y_POSITION, MINIMUM_Z_POSITION));
}

// setup - PropertyNotificationSetGetNotifyModeDisabled001

void PropertyNotificationSetGetNotifyModeDisabled001()
{
  float GREATER_VALUE = 100.0f;
  gCallBackCalled = false;
  gNotifyMode = PropertyNotification::Disabled;
  gNotifyModeCheck = PropertyNotification::NotifyOnTrue;

  gActorPropertyNotification = Actor::New();
  DALI_CHECK_FAIL(!gActorPropertyNotification, "actor is not created " );

  Stage::GetCurrent().Add(gActorPropertyNotification);
  gPropertyNotification= gActorPropertyNotification.AddPropertyNotification(Actor::Property::POSITION_X, GreaterThanCondition(GREATER_VALUE));

  bool bRet = false;
  bRet = PropertyNotificationSetGetNotifyMode( gPropertyNotification, gNotifyModeCheck );
  DALI_CHECK_FAIL(!bRet, "Notification check is failed." );
}
void PropertyNotificationSetGetNotifyModeDisabled002()
{
  gActorPropertyNotification.SetPosition(Vector3(MAXIMUM_X_POSITION, MINIMUM_Y_POSITION, MINIMUM_Z_POSITION));
}
void PropertyNotificationSetGetNotifyModeDisabled003()
{
  gActorPropertyNotification.SetPosition(Vector3(MINIMUM_X_POSITION,MINIMUM_Y_POSITION, MINIMUM_Z_POSITION ));
}

// setup - PropertyNotificationGetCondition

void PropertyNotificationGetCondition()
{
  Actor actor;
  Stage stage;
  float GREATER_VALUE = 100.0f;
  PropertyNotification notification;
  PropertyCondition condition;
  PropertyCondition condition2;

  stage = Stage::GetCurrent();
  DALI_CHECK_FAIL(!stage, "stage is not created" );

  actor = Actor::New();
  DALI_CHECK_FAIL(!actor, " actor is not created" );

  stage.Add(actor);

  condition = GreaterThanCondition(GREATER_VALUE);
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
  float GREATER_VALUE = 100.0f;
  PropertyNotification notification;
  PropertyCondition condition;

  stage = Stage::GetCurrent();
  DALI_CHECK_FAIL(!stage, " stage is not created" );
  actor = Actor::New();
  DALI_CHECK_FAIL(!actor, " actor is not created" );
  stage.Add(actor);

  condition = GreaterThanCondition(GREATER_VALUE);
  notification = actor.AddPropertyNotification(Actor::Property::POSITION_X,condition);
  PropertyNotificationConstWrapper notificationConst(notification);

  const PropertyCondition& conditionReference1 = notificationConst.GetCondition();

  DALI_CHECK_FAIL(conditionReference1 != condition , "conditionReference1 != condition" );

  stage.Remove(actor);

  DaliLog::PrintPass();
}

void PropertyNotificationGetTarget()
{
  float GREATER_VALUE = 100.0f;
  Handle handleobject;
  Handle handleobject2;
  PropertyNotification notification;
  PropertyCondition condition;

  handleobject = Actor::New();
  DALI_CHECK_FAIL(!handleobject, "handleobject is not created " );

  condition = GreaterThanCondition(GREATER_VALUE);
  notification = handleobject.AddPropertyNotification(Actor::Property::POSITION_X,condition);

  handleobject2 = notification.GetTarget();


  DALI_CHECK_FAIL( !handleobject2 , "Not a valid handleobject" );

  DALI_CHECK_FAIL( handleobject2 != handleobject , "Getting Target failed" );

  DaliLog::PrintPass();
}

void PropertyNotificationGetTargetProperty()
{
  float GREATER_VALUE = 100.0f;
  Handle handleobject;
  Handle handleobject2;
  PropertyNotification notification;
  PropertyCondition condition;

  Property::Index indexSetValue;
  Property::Index indexGetValue;

  handleobject = Actor::New();
  DALI_CHECK_FAIL(!handleobject, "handleobject is not created" );
  condition = GreaterThanCondition(GREATER_VALUE);

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

  gObject = new TestClass();

  bInitialize = gObject->Initialize();
  DALI_CHECK_FAIL(!bInitialize, "Object has not been initialized" );

  /** @note need to render to pass this tc */
  gObject->mNotification.SetNotifyMode(notifyModeCheck);
  /** @note need to render after set property to pass this tc */
  gObject->mActor.SetPosition(vecSetVal);
  /** @note need to render after set property to pass this tc */
  gCallBackCalled = false;
  gObject->mActor.SetPosition(vecSetVal2);
}

// validate - VTPropertyNotificationGetNotifyResult001

void VTPropertyNotificationGetNotifyResult001()
{
  bool bResult= false;
  DALI_CHECK_FAIL( !gCallBackCalled , "Notification not Triggered" );

  bResult= gObject->mNotification.GetNotifyResult();
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
