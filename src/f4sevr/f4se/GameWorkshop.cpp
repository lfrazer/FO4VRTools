#include "f4se/GameWorkshop.h"

RelocAddr <_LinkPower> LinkPower_Internal(0x00206730); // Power related natives
RelocAddr <_LinkPower2> LinkPower2_Internal(0x00211420); // Usually paired with LinkPower
RelocAddr <_GetObjectAtConnectPoint> GetObjectAtConnectPoint(0x0020EC70); // Acquires objects that are touching attach points
RelocAddr <_LinkPower3> LinkPower3_Internal(0x002061A0); // Wire related calls
RelocAddr <_LinkPower4> LinkPower4_Internal(0x00213F20);
RelocAddr <_SetWireEndpoints> SetWireEndpoints_Internal(0x00210760);
RelocAddr <_FinalizeWireLink> FinalizeWireLink(0x00210460);
RelocAddr <_ScrapReference> ScrapReference(0x00217DF0);
