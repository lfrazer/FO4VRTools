# FO4VRTools
F4SE_VR Plugin for handling VR input - based on the original SkyrimVRTools https://github.com/lfrazer/SkyrimVRTools-dev

### Example PapyrusVR script
```Papyrus
Scriptname PapyrusQuest extends Quest

int testSphereHandle = 1 

Event Oninit()    
    Debug.Notification("The mod has been installed")
    Actor player = Game.GetPlayer()
    float[] pose = PapyrusVR.GetSteamVRDevicePosition(0)
    Debug.Notification("Pose X: " + pose[0])
    Debug.Notification("Pose Y: " + pose[1])
    Debug.Notification("Pose Z: " + pose[2])
    Debug.Notification("Registering for VR Button events!")
    PapyrusVR.RegisterForVRButtonEvents(Self)
    Debug.Notification("Testing collision spheres!")
    PapyrusVR.RegisterForVROverlapEvents(Self)
    ;25cm sphere about 25cm on top of the headset
    float[] position = PapyrusVR.Vector3(0.0, 0.25, 0.0)
    float[] rotation = PapyrusVR.Quaternion_Zero()
    testSphereHandle = PapyrusVR.RegisterLocalOverlapSphere(0.125, position, rotation, 0)
    if testSphereHandle != 0
        Debug.Notification("Registered sphere " + testSphereHandle + " correctly!")
    endIf
    Debug.Notification("The mod has finished initializing")
    PapyrusVR.StartHaptics(1, 2.0, 0.7)
    Utility.Wait(2.0)
    PapyrusVR.StartHaptics(2, 2.0, 0.7)
EndEvent

Function OnVRButtonEvent(int buttonEvent, int buttonId, int deviceId)
    Debug.Notification("Got event from device" + deviceId + " with id " + buttonEvent + " for button " + buttonId)
EndFunction

Function OnVROverlapEvent(int overlapEventType, int objectHandle, int deviceId)
   if(overlapEventType == 1 && objectHandle == testSphereHandle)
       Debug.Notification("Entered Test Sphere Handle")
   elseif(overlapEventType == 2 && objectHandle == testSphereHandle)
       Debug.Notification("Exited Test Sphere Handle")
   endIf
EndFunction
```
