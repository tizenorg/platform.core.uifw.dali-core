#include "CtsDali-property-notification-impl.h"
#include "../test-utilities.h"

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

extern int gArgc;
extern char ** gArgv;
extern int test_return_value;


void Cts_propertynotification_startup(void)
{
  test_return_value = 0;
  gRenderCountPropertyNotification=TC_PASS;
}

void Cts_propertynotification_cleanup(void)
{

}


/** @addtogroup Cts-property-notification-testcases
*    @brief Cts testcases and testcase helper function, callbacks for suite actor
*    @ingroup  Cts-property-notification
*    @{
*/

//& purpose:    Sets NotifyOnChanged as Notification mode  and Get mode and also check whether signal has been notified.
//& type:    auto

/**
 * @testcase            CtsDaliPropertyNotificationSetGetNotifyModeONChanged
 * @type                Positive
 * @since_tizen            2.4
 * @description            Sets NotifyOnChanged as Notification mode  and Get mode and also check whether signal has been notified.
 * @scenario            Get the current stage and add actor to it. \n
 *                         A condition to actor X position is applied which will be greater than 100 \n
 *                         Set a property notification to the actor according to the condition. \n
 *                         Take NotifyOnChanged as notify mode which will be applied to property notification. \n
 *                         Sets the Notification mode to the property notification. \n
 *                         A Function Wait is Connected to this signal to be notified when the notification has occurred. \n
 *                         Now Get the mode of the propertynotification and check it whether it matched to the set mode or not. \n
 *                        Now Set the actor X axis position to the maximum which is 100 here. \n
 *                         Wait for something and again set the actor X axis position. \n
 *                         Check whether the Signal is emited after the Notification has occurred.
 * @apicovered            AddPropertyNotification,SetNotifyMode(),GetNotifyMode(),NotifySignal()
 * @passcase            When Set the Notification mode and get notify mode matches and signal is notified successfully.
 * @failcase            When Set the Notification mode and get notify mode does not matched or signal does not notified.
 * @precondition        NA
 * @postcondition        NA
 */
int CtsDaliPropertyNotificationSetGetNotifyModeOnChanged(void)
{
    DaliLog::PrintExecStarted(SUITE_NAME, __FUNCTION__);
    Application application = Application::New( &gArgc, &gArgv );

    // Pass in an array of TestCasePair structs
    TestUtilities::TestCasePair testPairs[] = {
        {&PropertyNotificationSetGetNotifyModeONChanged001, &VTPropertyNotificationNotTriggered},
        {&PropertyNotificationSetGetNotifyModeONChanged002, &VTPropertyNotificationNotTriggered},
        {&PropertyNotificationSetGetNotifyModeONChanged003, &VTPropertyNotificationTriggered}
    };

    const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
    const unsigned int STEP_INTERVAL = 500;
    TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
    application.MainLoop();

    return test_return_value;
}

//& purpose:    Sets NotifyOnTrue as Notification mode  and Get mode and also check whether signal has been notified.
//& type:    auto

/**
 * @testcase            CtsDaliPropertyNotificationSetGetNotifyModeONTrue
 * @type                Positive
 * @since_tizen            2.4
 * @description            Sets NotifyOnTrue as Notification mode  and Get mode and also check whether signal has been notified.
 * @scenario            Get the current stage and add actor to it. \n
 *                         A condition to actor X position is applied which will be greater than 100 \n
 *                         Set a property notification to the actor according to the condition. \n
 *                         Take NotifyOnTrue as notify mode which will be applied to property notification. \n
 *                         Sets the Notification mode to the property notification. \n
 *                         A Function Wait is Connected to this signal to be notified when the notification has occurred. \n
 *                         Now Get the mode of the propertynotification and check it whether it matched to the set mode or not. \n
 *                        Now Set the actor X axis position to the minimum which is 0 here. \n
 *                         Wait for sometime and again set the actor X axis position which will be greater than 100. \n
 *                         Check whether the Signal is emitted after the Notification has occurred.
 * @apicovered            AddPropertyNotification,SetNotifyMode(),GetNotifyMode(),NotifySignal()
 * @passcase            When Set the Notification mode and get notify mode matches and signal is notified successfully.
 * @failcase            When Set the Notification mode and get notify mode does not matched or signal does not notified.
 * @precondition        NA
 * @postcondition        NA
 */
int CtsDaliPropertyNotificationSetGetNotifyModeOnTrue(void)
{
  DaliLog::PrintExecStarted(SUITE_NAME, __FUNCTION__);
  Application application = Application::New( &gArgc, &gArgv );

  TestUtilities::TestCasePair testPairs[] = {
      {&PropertyNotificationSetGetNotifyModeONTrue001, &VTPropertyNotificationNotTriggered},
      {&PropertyNotificationSetGetNotifyModeONTrue002, &VTPropertyNotificationNotTriggered},
      {&PropertyNotificationSetGetNotifyModeONTrue003, &VTPropertyNotificationTriggered}
  };


  const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
  const unsigned int STEP_INTERVAL = 500;
  TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
  application.MainLoop();

  return test_return_value;
}

//& purpose:    Sets NotifyOnFalse as Notification mode  and Get mode and also check whether signal has been notified.
//& type:        auto

/**
 * @testcase            CtsDaliPropertyNotificationSetGetNotifyModeONFalse
 * @type                Positive
 * @since_tizen            2.4
 * @description            Sets NotifyOnFalse as Notification mode  and Get mode and also check whether signal has been notified.
 * @scenario            Get the current stage and add actor to it. \n
 *                         A condition to actor X position is applied which will be greater than 100 \n
 *                         Set a property notification to the actor according to the condition. \n
 *                         Take NotifyOnFalse as notify mode which will be applied to property notification. \n
 *                         Sets the Notification mode to the property notification. \n
 *                         A Function Wait is Connected to this signal to be notified when the notification has occurred. \n
 *                         Now Get the mode of the propertynotification and check it whether it matched to the set mode or not. \n
 *                        Now Set the actor X axis position to the maximum which is greater than 100 here. \n
 *                         Wait for sometime and again set the actor X axis position which will be less than 100. \n
 *                         Check whether the Signal is emitted after the Notification has occurred. \n
 * @apicovered            AddPropertyNotification,SetNotifyMode(),GetNotifyMode(),NotifySignal()
 * @passcase            When Set the Notification mode and get notify mode matches and signal is notified successfully.
 * @failcase            When Set the Notification mode and get notify mode does not matched or signal does not notified.
 * @precondition        NA
 * @postcondition        NA
 */
int CtsDaliPropertyNotificationSetGetNotifyModeOnFalse(void)
{
  DaliLog::PrintExecStarted(SUITE_NAME, __FUNCTION__);
  Application application = Application::New( &gArgc, &gArgv );

  // Pass in an array of TestCasePair structs
  TestUtilities::TestCasePair testPairs[] = {
      {&PropertyNotificationSetGetNotifyModeONFalse001, &VTPropertyNotificationNotTriggered},
      {&PropertyNotificationSetGetNotifyModeONFalse002, &VTPropertyNotificationNotTriggered},
      {&PropertyNotificationSetGetNotifyModeONFalse003, &VTPropertyNotificationTriggered}
  };

  const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
  const unsigned int STEP_INTERVAL = 500;
  TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
  application.MainLoop();

  return test_return_value;
}


//& purpose:    Sets Disabled as Notification mode  and Get mode and also check whether signal has been notified.
//& type:    auto

/**
 * @testcase            CtsDaliPropertyNotificationSetGetNotifyModeDisabled
 * @type                Positive
 * @since_tizen            2.4
 * @description            Sets Disabled as Notification mode  and Get mode and also check whether signal has been notified.
 * @scenario            Get the current stage and add actor to it. \n
 *                         A condition to actor X position is applied which will be greater than 100 \n
 *                         Set a property notification to the actor according to the condition. \n
 *                         Take Disabled as notify mode which will be applied to property notification. \n
 *                         Sets the Notification mode to the property notification. \n
 *                         A Function Wait is Connected to this signal to be notified when the notification has occurred. \n
 *                         Now Get the mode of the propertynotification and check it whether it matched to the set mode or not. \n
 *                        Now Set the actor X axis position to the maximum which is greater than 100 here. \n
 *                         Wait for sometime and again set the actor X axis position which will be less than 100. \n
 *                         Check whether the Signal is emitted after the Notification has occurred.
 * @apicovered            AddPropertyNotification,SetNotifyMode(),GetNotifyMode(),NotifySignal()
 * @passcase            When Set the Notification mode and get notify mode matches and signal is notified successfully.
 * @failcase            When Set the Notification mode and get notify mode does not matched or signal does not notified.
 * @precondition        NA
 * @postcondition        NA
 */
int CtsDaliPropertyNotificationSetGetNotifyModeDisabled(void)
{
  DaliLog::PrintExecStarted(SUITE_NAME, __FUNCTION__);
  Application application = Application::New( &gArgc, &gArgv );

  // Pass in an array of TestCasePair structs
  TestUtilities::TestCasePair testPairs[] = {
      {&PropertyNotificationSetGetNotifyModeDisabled001, &VTPropertyNotificationNotTriggered},
      {&PropertyNotificationSetGetNotifyModeDisabled002, &VTPropertyNotificationNotTriggered},
      {&PropertyNotificationSetGetNotifyModeDisabled003, &VTPropertyNotificationTriggered}
  };

  const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
  const unsigned int STEP_INTERVAL = 500;
  TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
  application.MainLoop();

  return test_return_value;
}

//& purpose:    Get the condition of this notification.
//& type:    auto

/**
 * @testcase            CtsDaliPropertyNotificationGetCondition
 * @type                Positive
 * @since_tizen            2.4
 * @description            Get the condition of this notification.
 * @scenario            Get the current stage and add actor to it. \n
 *                         A condition to actor X position is applied which will be greater than 100 \n
 *                         Set a property notification to the actor according to the condition. \n
 *                         Get the condition of this notification.  \n
 *                         Check the set condition. \n
 * @apicovered            AddPropertyNotification,GetCondition()
 * @passcase            Set and Get value of condition matched and Get the condition of this notification successfully.
 * @failcase            Set and Get value of condition matched and Get the condition of this notification not successfully.
 * @precondition        NA
 * @postcondition        NA
 */
int CtsDaliPropertyNotificationGetCondition(void)
{
  DaliLog::PrintExecStarted(SUITE_NAME, __FUNCTION__);
  Application application = Application::New( &gArgc, &gArgv );

  // Pass in an array of TestCasePair structs
  TestUtilities::TestCasePair testPairs[] = {
      {&PropertyNotificationGetCondition, &VTNoValidationRequired}
  };

  const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
  const unsigned int STEP_INTERVAL = 500;
  TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
  application.MainLoop();

  return test_return_value;
}

//& purpose:    Get the condition of this notification.
//& type:    auto

/**
 * @testcase            CtsDaliPropertyNotificationGetConditionWithAddress
 * @type                Positive
 * @since_tizen            2.4
 * @description            Get the condition of this notification.
 * @scenario            Get the current stage and add actor to it. \n
 *                         A condition to actor X position is applied which will be greater than 100 \n
 *                         Set a property notification to the actor according to the condition. \n
 *                         Construct a notification.  \n
 *                         Get the condition by reference of this notification.    \n
 *                         Check the set condition.
 * @apicovered            AddPropertyNotification,PropertyCondition & GetCondition()
 * @passcase            Get the condition of this notification successfully.
 * @failcase            Get the condition of this notification not successfully.
 * @precondition        NA
 * @postcondition        NA
 */
int CtsDaliPropertyNotificationGetConditionWithAddress(void)
{
  DaliLog::PrintExecStarted(SUITE_NAME, __FUNCTION__);
  Application application = Application::New( &gArgc, &gArgv );

  // Pass in an array of TestCasePair structs
  TestUtilities::TestCasePair testPairs[] = {
      {&PropertyNotificationGetConditionWithAddress, &VTNoValidationRequired}
  };

  const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
  const unsigned int STEP_INTERVAL = 500;
  TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
  application.MainLoop();

  return test_return_value;
}

//& purpose:    Get the target handle that this notification is observing.
//& type:    auto

/**
 * @testcase            CtsDaliPropertyNotificationGetTarget
 * @type                Positive
 * @since_tizen            2.4
 * @description            Get the target handle that this notification is observing.
 * @scenario            Initialize a handle object. \n
 *                         Set a property notification to the handle object according to the condition. \n
 *                         Get the target handle that this notification is observing.   \n
 *                         Check whether the target handle object is the same Handle object. \n
 * @apicovered            AddPropertyNotification,GetTarget()
 * @passcase            Get the target handle that this notification is observing successfully.
 * @failcase            Get the target handle that this notification is observing not successfully.
 * @precondition        NA
 * @postcondition        NA
 */
int CtsDaliPropertyNotificationGetTarget(void)
{
  DaliLog::PrintExecStarted(SUITE_NAME, __FUNCTION__);
  Application application = Application::New( &gArgc, &gArgv );

  // Pass in an array of TestCasePair structs
  TestUtilities::TestCasePair testPairs[] = {
      {&PropertyNotificationGetTarget, &VTNoValidationRequired}
  };

  const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
  const unsigned int STEP_INTERVAL = 500;
  TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
  application.MainLoop();

  return test_return_value;
}

/**
 * @testcase            CtsDaliPropertyNotificationGetTargetProperty
 * @type                Positive
 * @since_tizen            2.4
 * @description            Get the target handle that this notification is observing.
 * @scenario            Initialize a handle object. \n
 *                         Set a property notification to the handle object according to the condition and an index value. \n
 *                         Get the target handle's property index that this notification is observing.  \n
 *                         Check whether the target handle object's index value is the same index.
 * @apicovered            AddPropertyNotification,GetTargetProperty()
 * @passcase            Get the target handle's property index that this notification is observing successfully.
 * @failcase            Get the target handle's property index that this notification is observing not successfully.
 * @precondition        NA
 * @postcondition        NA
 */
int CtsDaliPropertyNotificationGetTargetProperty(void)
{
  DaliLog::PrintExecStarted(SUITE_NAME, __FUNCTION__);
  Application application = Application::New( &gArgc, &gArgv );

  // Pass in an array of TestCasePair structs
  TestUtilities::TestCasePair testPairs[] = {
      {&PropertyNotificationGetTargetProperty, &VTNoValidationRequired}
  };

  const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
  const unsigned int STEP_INTERVAL = 500;
  TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
  application.MainLoop();

  return test_return_value;
}

//& purpose:    Gets the result of the last condition check that caused a signal emit.
//& type:    auto

/**
 * @testcase            CtsDaliPropertyNotificationGetNotifyResult
 * @type                Positive
 * @since_tizen            2.4
 * @description            Gets the result of the last condition check that caused a signal emit.
 * @scenario            Initialize a object of testclass which inherits connection tracker. \n
 *                         Get the current stage and add actor to it. \n
 *                         A condition to actor X position is applied which will be greater than 100 \n
 *                         Set a property notification to the actor according to the condition. \n
 *                         Take NotifyOnChanged as notify mode which will be applied to property notification. \n
 *                         Sets the Notification mode to the property notification. \n
 *                         A Function Wait is Connected to this signal to be notified when the notification has occurred. \n
 *                        Now Set the actor X axis position to the maximum which is 100 here. \n
 *                         Wait for sometime and again set the actor X axis position. \n
 *                         Check whether the Signal is emitted after the Notification has occurred. \n
 *                         If notification occurred than get the result of the last condition check that caused a signal emit. \n
 *                         Check whether the result is true or false.
 * @apicovered            AddPropertyNotification,GetNotifyResult()
 * @passcase            Gets the result of the last condition check that caused a signal emit successfully.
 * @failcase            Gets the result of the last condition check that caused a signal emit  not successfully.
 * @precondition        NA
 * @postcondition        NA
 */
int CtsDaliPropertyNotificationGetNotifyResult(void)
{
  DaliLog::PrintExecStarted(SUITE_NAME, __FUNCTION__);
  Application application = Application::New( &gArgc, &gArgv );

  // Pass in an array of TestCasePair structs
  TestUtilities::TestCasePair testPairs[] = {
      {&PropertyNotificationGetNotifyResult001, &VTNoValidationRequired}
  };

  const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
  const unsigned int STEP_INTERVAL = 500;
  TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
  application.MainLoop();

  return test_return_value;
}


//& purpose:    DownCast a property notification handle
//& type:    auto

/**
 * @testcase            CtsDaliPropertyNotificationDownCast
 * @type                Positive
 * @since_tizen            2.4
 * @description            DownCast a property notification handle
 * @scenario            Construct an Actor and PropertyCondition for property notification \n
 *                         Construct a PropertyNotification instance using actor and condition targeting actor's POSITION_X;
 *                         DownCast the PropertyNotification intance \n
 *                         Check PropertyNotification intance is downcasted successfully with target, property and condition \n
 * @apicovered            PropertyNotification::DownCast, PropertyNotification::GetTarget(), PropertyNotification::GetTargetProperty(), PropertyNotification::GetCondition()
 * @passcase            If downcasts a property notification handle successfully with target, property and condition
 * @failcase            If fails to downcast a property notification handle with target, property or condition
 * @precondition        NA
 * @postcondition        NA
 */
int CtsDaliPropertyNotificationDownCast(void)
{
  DaliLog::PrintExecStarted(SUITE_NAME, __FUNCTION__);
  Application application = Application::New( &gArgc, &gArgv );

  // Pass in an array of TestCasePair structs
  TestUtilities::TestCasePair testPairs[] = {
      {&PropertyNotificationDownCast, &VTPropertyNotificationGetNotifyResult001}
  };

  const unsigned int NUMBER_OF_TEST_STEPS =  sizeof( testPairs ) / sizeof( TestUtilities::TestCasePair );
  const unsigned int STEP_INTERVAL = 500;
  TestUtilities::TestApp testApp( application, testPairs, NUMBER_OF_TEST_STEPS, STEP_INTERVAL );
  application.MainLoop();

  return test_return_value;
}

/** @} */
/** @} */
/** @} */
