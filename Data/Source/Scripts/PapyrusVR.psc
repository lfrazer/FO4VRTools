Scriptname PapyrusVR Native Hidden
;       PAPYRUS VR
;       MIT License
;
;		Common Data Structures:
;		|		Name		|	ReturnType	|	Indexes Order
;		+-------------------+---------------+--------------------
;		|	Vector3			|	float[3]	|	(x,y,z)
;		+-------------------+---------------+--------------------
;		|	Quaternion		|	float[4]	|	(x,y,z,w)
;		+-------------------+---------------+--------------------
;		|					|				|	( 0, 1, 2,	3 
;		|	Matrix34		|	float[12]	|	  4, 5, 6,	7 
;		|					|				|	  8, 9, 10, 11 )
;		------------------------------------+--------------------

;DeviceTypes
int property EDeviceTypeNone                = -1 autoReadOnly
int property EDeviceTypeHMD                 = 0 autoReadOnly
int property EDeviceTypeRightController     = 1 autoReadOnly
int property EDeviceTypeLeftController      = 2 autoReadOnly

;EventTypes
int property Touched                        = 0 autoReadOnly
int property Untouched                      = 1 autoReadOnly
int property Pressed                        = 2 autoReadOnly
int property Released                       = 3 autoReadOnly

;OverlapEventTypes
int property Entered                        = 1 autoReadOnly
int property Exited                         = 2 autoReadOnly

;SteamVR Buttons
int property k_EButton_System			    = 0 autoReadOnly
int property k_EButton_ApplicationMenu	    = 1 autoReadOnly
int property k_EButton_Grip				    = 2 autoReadOnly
int property k_EButton_DPad_Left			= 3 autoReadOnly
int property k_EButton_DPad_Up			    = 4 autoReadOnly
int property k_EButton_DPad_Right		    = 5 autoReadOnly
int property k_EButton_DPad_Down			= 6 autoReadOnly
int property k_EButton_A					= 7 autoReadOnly
int property k_EButton_ProximitySensor      = 31 autoReadOnly
int property k_EButton_Axis0				= 32 autoReadOnly
int property k_EButton_Axis1				= 33 autoReadOnly
int property k_EButton_Axis2				= 34 autoReadOnly
int property k_EButton_Axis3				= 35 autoReadOnly
int property k_EButton_Axis4				= 36 autoReadOnly
int property k_EButton_SteamVR_Touchpad	    = 32 autoReadOnly
int property k_EButton_SteamVR_Trigger	    = 33 autoReadOnly
int property k_EButton_Dashboard_Back	    = 2 autoReadOnly
int property k_EButton_Max				    = 64 autoReadOnly

;Debug Functions
Function TimeSinceLastCall() global native

;SteamVR Coordinates
Function GetSteamVRDeviceRotation_Native(int DeviceType, float[] returnValues) global native
Function GetSteamVRDeviceQRotation_Native(int DeviceType, float[] returnValues) global native
Function GetSteamVRDevicePosition_Native(int DeviceType, float[] returnValues) global native

;Fallout 4 Coordinates - Note: Currently unstable, use SteamVR Coordinates
Function GetFO4DeviceRotation_Native(int DeviceType, float[] returnValues) global native
Function GetFO4DeviceQRotation_Native(int DeviceType, float[] returnValues) global native
Function GetFO4DevicePosition_Native(int DeviceType, float[] returnValues) global native

;Overlap Objects
Function DestroyLocalOverlapObject(int objectHandle) global native

;RegisterLocalOverlapSphere
;returns ObjectHandle                       Meters     Vector3 Meters       Quaternion       EDeviceType
int Function RegisterLocalOverlapSphere(float radius, float[] position, float[] rotation, int DeviceType) global native

;Haptic Function
Function StartHaptics(int DeviceType, float hapticsTime, float hapticIntensity) native global

;Events Registration
Function RegisterForPoseUpdates(ScriptObject akObject) native global
Function UnregisterForPoseUpdates(ScriptObject akObject) native global
Function RegisterForVRButtonEvents(ScriptObject akObject) native global
Function UnregisterForVRButtonEvents(ScriptObject akObject) native global
Function RegisterForVROverlapEvents(ScriptObject akObject) native global
Function UnregisterForVROverlapEvents(ScriptObject akObject) native global

;Papyrus Native Functions Wrappers
float[] Function GetSteamVRDeviceRotation(int DeviceType) global
    float[] returnValues = new float[3]
    GetSteamVRDeviceRotation_Native(DeviceType, returnValues)
    return returnValues
EndFunction

float[] Function GetSteamVRDeviceQRotation(int DeviceType) global
    float[] returnValues = new float[4]
    GetSteamVRDeviceQRotation_Native(DeviceType, returnValues)
    return returnValues
EndFunction

float[] Function GetSteamVRDevicePosition(int DeviceType) global
    float[] returnValues = new float[3]
    GetSteamVRDevicePosition_Native(DeviceType, returnValues)
    return returnValues
EndFunction

float[] Function GetFO4DeviceRotation(int DeviceType) global
    float[] returnValues = new float[3]
    GetFO4DeviceRotation_Native(DeviceType, returnValues)
    return returnValues
EndFunction

float[] Function GetFO4DeviceQRotation(int DeviceType) global
    float[] returnValues = new float[4]
    GetFO4DeviceQRotation_Native(DeviceType, returnValues)
    return returnValues
EndFunction

float[] Function GetFO4DevicePosition(int DeviceType) global
    float[] returnValues = new float[3]
    GetFO4DevicePosition_Native(DeviceType, returnValues)
    return returnValues
EndFunction

;Utilities
float[] Function Vector3 (float X, float Y, float Z) global
    float[] vector = new float[3]
    vector[0] = X
    vector[1] = Y
    vector[2] = Z
    return vector
EndFunction

float[] Function Vector3_Zero() global
    return Vector3(0,0,0)
EndFunction

float[] Function Quaternion (float X, float Y, float Z, float W) global
    float[] vector = new float[4]
    vector[0] = X
    vector[1] = Y
    vector[2] = Z
    vector[3] = W
    return vector
EndFunction

float[] Function Quaternion_Zero() global
    return Quaternion(0,0,0,0)
EndFunction

;Events - No longer used in F4SE. See example script.
;Event OnVRButtonEvent(int eventType, int buttonId, int deviceId)
;EndEvent

;Event OnVROverlapEvent(int overlapEventType, int objectHandle, int deviceId)
;EndEvent



