#pragma once
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

#include <windows.h>
#include <inttypes.h>
#include <unordered_set>
#include "../api/VRHookAPI.h"
#include "IHookInterfaceFactory.h"
#include "HookVRSystem.h"
#include "HookVRCompositor.h"
#include "openvr.h"
#include "timer.h"

const unsigned int kPluginVersionNum = 0x1;

typedef void * (*VR_GetGenericInterfaceFunc)(const char *pchInterfaceVersion, vr::EVRInitError *peError);
static std::map<std::string, IHookInterfaceFactory*> interfaceTranslationMap;

bool DoOpenVRHook();

//TODO: Rename all namespaces to something else other than PapyrusVR
//TODO: Create VRInputMgr
//TODO: Move to VRInputMgr
bool getControllerStateUpdateShutoff();
void setControllerStateUpdateShutoff(bool enable);


/* OpenVR hook manager
*  OpenVRHookMgr offers direct hooks into RAW OpenVR calls to other modules
*/
class OpenVRHookMgr : OpenVRHookManagerAPI
{
friend class HookVRSystem;
friend class HookVRCompositor;

public:

	OpenVRHookMgr();

	static OpenVRHookMgr* GetInstance();

	void SetVRSystem(vr::IVRSystem* vrSystem)
	{
		mVRSystem = vrSystem;
	}

	void SetFakeVRSystem(HookVRSystem* vrSystem)
	{
		mFakeVRSystem = vrSystem;
	}

	void SetVRCompositor(vr::IVRCompositor* vrCompositor)
	{
		mVRCompositor = vrCompositor;
	}

	void SetFakeVRCompositor(HookVRCompositor* vrCompositor)
	{
		mFakeVRCompositor = vrCompositor;
	}

	HookVRSystem* GetFakeVRSystem() const
	{
		return mFakeVRSystem;
	}

	HookVRCompositor* GetFakeVRCompositor() const
	{
		return mFakeVRCompositor;
	}

	vr::IVRSystem* GetVRSystem() const override
	{
		return mVRSystem;
	}

	vr::IVRCompositor* GetVRCompositor() const override
	{
		return mVRCompositor;
	}

	virtual unsigned int GetVersion() override
	{
		return kPluginVersionNum;
	}

	bool IsInitialized() override;
	void RegisterControllerStateCB(GetControllerState_CB cbfunc) override;
	void RegisterGetPosesCB(WaitGetPoses_CB cbfunc) override;
	void UnregisterControllerStateCB(GetControllerState_CB cbfunc) override;
	void UnregisterGetPosesCB(WaitGetPoses_CB cbfunc) override;

	void StartHaptics(unsigned int trackedControllerId, float hapticTime, float hapticIntensity) override;

	void UpdateHaptics();
	
private:
	vr::IVRSystem* mVRSystem = nullptr;
	vr::IVRCompositor* mVRCompositor = nullptr;
	HookVRSystem* mFakeVRSystem = nullptr;
	HookVRCompositor* mFakeVRCompositor = nullptr;

	std::unordered_set<GetControllerState_CB> mGetControllerStateCallbacks;
	std::unordered_set<WaitGetPoses_CB>	mWaitGetPosesCallbacks;

	struct HapticTimeSettings
	{
		double time = 0.0;
		int intensity = 2000;
	};

	CTimer mTimer;
	HapticTimeSettings	mControllerHapticTime[2];

	static OpenVRHookMgr* sInstance;
};


