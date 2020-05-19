/*
	FO4VRTools - VR Input plugin for mods of Fallout4 VR
	Copyright (C) 2020 L Frazer
	https://github.com/lfrazer

	This file is part of FO4VRTools.

	FO4VRTools is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FO4VRTools is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with FO4VRTools.  If not, see <https://www.gnu.org/licenses/>.
*/



#include "openvr_hook.h"
#include "f4se_common/Utilities.h"
#include "f4se_common/SafeWrite.h"
#include "common/IDebugLog.h"

using namespace vr;

OpenVRHookMgr* OpenVRHookMgr::sInstance = nullptr;

OpenVRHookMgr::OpenVRHookMgr()
{
	mTimer.Init();
}

OpenVRHookMgr* OpenVRHookMgr::GetInstance()
{
	if (sInstance == nullptr)
	{
		sInstance = new OpenVRHookMgr();
	}

	return sInstance;
}

bool OpenVRHookMgr::IsInitialized()
{
	return mVRCompositor != nullptr && mVRSystem != nullptr;
}

void OpenVRHookMgr::RegisterControllerStateCB(GetControllerState_CB cbfunc)
{
	if (mGetControllerStateCallbacks.find(cbfunc) == mGetControllerStateCallbacks.end())
	{
		mGetControllerStateCallbacks.insert(cbfunc);
	}
}

void OpenVRHookMgr::RegisterGetPosesCB(WaitGetPoses_CB cbfunc)
{
	if (mWaitGetPosesCallbacks.find(cbfunc) == mWaitGetPosesCallbacks.end())
	{
		mWaitGetPosesCallbacks.insert(cbfunc);
	}
}

void OpenVRHookMgr::UnregisterControllerStateCB(GetControllerState_CB cbfunc)
{
	auto it = mGetControllerStateCallbacks.find(cbfunc);
	if (it != mGetControllerStateCallbacks.end())
	{
		mGetControllerStateCallbacks.erase(it);
	}
}
void OpenVRHookMgr::UnregisterGetPosesCB(WaitGetPoses_CB cbfunc)
{
	auto it = mWaitGetPosesCallbacks.find(cbfunc);
	if (it != mWaitGetPosesCallbacks.end())
	{
		mWaitGetPosesCallbacks.erase(it);
	}
}


void OpenVRHookMgr::StartHaptics(unsigned int trackedControllerId, float hapticTime, float hapticIntensity)
{
	const unsigned short kVRHapticConstant = 2000;  // max value is 3999 but ue4 suggest max 2000? - time in microseconds to pulse per frame, also described by Valve as "strength"

	if (trackedControllerId > 0 && trackedControllerId < 3)
	{
		if (hapticIntensity > 1.0f)
		{
			hapticIntensity = 1.0f;
		}
		if (hapticIntensity < 0.0f)
		{
			hapticIntensity = 0.0f;
		}

		mControllerHapticTime[trackedControllerId - 1].time = mTimer.GetLastTime() + (double)hapticTime;
		mControllerHapticTime[trackedControllerId - 1].intensity = (int)(kVRHapticConstant * hapticIntensity);
	}
}

void OpenVRHookMgr::UpdateHaptics()
{
	mTimer.TimerUpdate();

	if (mVRSystem)
	{
		for (int i = 0; i < 2; ++i)
		{
			if (mControllerHapticTime[i].time > mTimer.GetLastTime())
			{
				// haptic remaining times are indexed in array by (ETrackedControllerRole enum value - 1) - so add 1 to loop index i to get the correct value - see StartHaptics()
				auto controllerId = mVRSystem->GetTrackedDeviceIndexForControllerRole((vr::ETrackedControllerRole)(i + 1));
				mVRSystem->TriggerHapticPulse(controllerId, 0, mControllerHapticTime[i].intensity);
			}
		}
	}
}


static VR_GetGenericInterfaceFunc  VR_GetGenericInterface_RealFunc;

// We replaced the original VR_GetGenericInterface() function SkyrimVR calls with this one to intercept objects it returns
static void * VR_CALLTYPE Hook_VR_GetGenericInterface_Execute(const char *pchInterfaceVersion, vr::EVRInitError *peError)
{
	_MESSAGE("Intercepted call to VR_GetGenericInterface - interface version=%s", pchInterfaceVersion);

	//If we have a translation for our interface, return the value
	if (interfaceTranslationMap.count(pchInterfaceVersion) != 0)
	{
		IHookInterfaceFactory* translatedVersion = interfaceTranslationMap[pchInterfaceVersion];
		_MESSAGE("[OpenVR_Hook] Custom interface %s defined for original interface %s", translatedVersion->GetWrappedVersion().c_str(), pchInterfaceVersion);
		void* requestedInterface = VR_GetGenericInterface_RealFunc(translatedVersion->GetWrappedVersion().c_str(), peError);
		void* builtInterface = translatedVersion->Build(requestedInterface);
		if (builtInterface == nullptr)
		{
			_MESSAGE("[OpenVR_Hook] Hooked cannot cast interface %s to anything...", typeid(requestedInterface).name());
			return requestedInterface;
		}
		else
			return builtInterface;
	}
	else
	{
		_MESSAGE("[OpenVR_Hook] No custom interface defined for version: %s", pchInterfaceVersion);
		return VR_GetGenericInterface_RealFunc(pchInterfaceVersion, peError);
	}
}

// Apply DLL hooking
bool DoOpenVRHook()
{
	// Get address of the original VR_GetGenericInterface in the openvr DLL Skyrim uses
	uintptr_t thunkAddress = (uintptr_t)GetIATAddr((UInt8 *)GetModuleHandle(NULL), "openvr_api.dll", "VR_GetGenericInterface");

	if (thunkAddress == 0)
	{
		_MESSAGE("Failed to find address of VR_GetGenericInterface()\n");
		return false;
	}

	//Creates translation map to wrap older OpenVR versions to new ones
	interfaceTranslationMap.emplace(std::string(SkyrimVR_IVRSystem_Version), (IHookInterfaceFactory*) new HookVRSystemFactory());
	interfaceTranslationMap.emplace(std::string(SkyrimVR_IVRCompositor_Version), (IHookInterfaceFactory*)new HookVRCompositorFactory());

	// Save address of the original function
	VR_GetGenericInterface_RealFunc = (VR_GetGenericInterfaceFunc)*(uintptr_t *)thunkAddress;
	// overwrite it in the process memory with our new function to intercept data
	SafeWrite64(thunkAddress, (uintptr_t)Hook_VR_GetGenericInterface_Execute);

	_MESSAGE("Hooked VR_GetGenericInterface!");

	return true;
}


bool getControllerStateUpdateShutoff()
{
	return OpenVRHookMgr::GetInstance()->GetFakeVRSystem()->m_getControllerStateShutoff;
}

void setControllerStateUpdateShutoff(bool enable)
{
	OpenVRHookMgr::GetInstance()->GetFakeVRSystem()->m_getControllerStateShutoff = enable;
}
