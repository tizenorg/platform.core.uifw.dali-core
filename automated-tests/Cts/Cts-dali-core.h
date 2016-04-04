#ifndef __CTS_DALI_CORE_H__
#define __CTS_DALI_CORE_H__

#include "testcase.h"

extern void Cts_propertynotification_startup(void);
extern void Cts_propertynotification_cleanup(void);

extern int CtsDaliPropertyNotificationSetGetNotifyModeOnChanged(void);
extern int CtsDaliPropertyNotificationSetGetNotifyModeOnTrue(void);
extern int CtsDaliPropertyNotificationSetGetNotifyModeOnFalse(void);
extern int CtsDaliPropertyNotificationSetGetNotifyModeDisabled(void);
extern int CtsDaliPropertyNotificationGetCondition(void);
extern int CtsDaliPropertyNotificationGetConditionWithAddress(void);
extern int CtsDaliPropertyNotificationGetTarget(void);
extern int CtsDaliPropertyNotificationGetTargetProperty(void);
extern int CtsDaliPropertyNotificationGetNotifyResult(void);
extern int CtsDaliPropertyNotificationDownCast(void);

testcase tc_array[] = {
    {"CtsDaliPropertyNotificationSetGetNotifyModeOnChanged", CtsDaliPropertyNotificationSetGetNotifyModeOnChanged, Cts_propertynotification_startup, Cts_propertynotification_cleanup},
    {"CtsDaliPropertyNotificationSetGetNotifyModeOnTrue", CtsDaliPropertyNotificationSetGetNotifyModeOnTrue, Cts_propertynotification_startup, Cts_propertynotification_cleanup},
    {"CtsDaliPropertyNotificationSetGetNotifyModeOnFalse", CtsDaliPropertyNotificationSetGetNotifyModeOnFalse, Cts_propertynotification_startup, Cts_propertynotification_cleanup},
    {"CtsDaliPropertyNotificationSetGetNotifyModeDisabled", CtsDaliPropertyNotificationSetGetNotifyModeDisabled, Cts_propertynotification_startup, Cts_propertynotification_cleanup},
    {"CtsDaliPropertyNotificationGetCondition", CtsDaliPropertyNotificationGetCondition, Cts_propertynotification_startup, Cts_propertynotification_cleanup},
    {"CtsDaliPropertyNotificationGetConditionWithAddress", CtsDaliPropertyNotificationGetConditionWithAddress, Cts_propertynotification_startup, Cts_propertynotification_cleanup},
    {"CtsDaliPropertyNotificationGetTarget", CtsDaliPropertyNotificationGetTarget, Cts_propertynotification_startup, Cts_propertynotification_cleanup},
    {"CtsDaliPropertyNotificationGetTargetProperty", CtsDaliPropertyNotificationGetTargetProperty, Cts_propertynotification_startup, Cts_propertynotification_cleanup},
    {"CtsDaliPropertyNotificationGetNotifyResult", CtsDaliPropertyNotificationGetNotifyResult, Cts_propertynotification_startup, Cts_propertynotification_cleanup},
    {"CtsDaliPropertyNotificationDownCast", CtsDaliPropertyNotificationDownCast, Cts_propertynotification_startup, Cts_propertynotification_cleanup},
    {NULL, NULL}
};

#endif // __CTS_DALI_CORE_H__
