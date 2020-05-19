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

#include "HookVRSystem.h"
#include "../VRManager.h"
#include "../ScaleformVR.h"
#include "openvr_hook.h"

using namespace vr;
using namespace PapyrusVR;

#pragma region Factory Methods
void* HookVRSystemFactory::Build(void* realInterface)
{
	if (realInterface != nullptr)
	{
		_MESSAGE("[OpenVR_Hook] Hooked IVRSystem");
		VRManager::GetInstance().InitVRSystem((vr::IVRSystem*)realInterface);
		OpenVRHookMgr::GetInstance()->SetVRSystem((vr::IVRSystem*)realInterface);
		OpenVRHookMgr::GetInstance()->SetFakeVRSystem(new HookVRSystem((vr::IVRSystem*)realInterface));
		return (void*)(OpenVRHookMgr::GetInstance()->GetFakeVRSystem());
	}
	return nullptr;
}

std::string HookVRSystemFactory::GetWrappedVersion()
{
	return std::string(PapyrusVR_IVRSystem_Version);
}
#pragma endregion

#pragma region Hooked Methods

HookVRSystem::HookVRSystem(vr::IVRSystem* wrappedSystem) : vr_system(wrappedSystem) { }

bool HookVRSystem::GetControllerState(vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize)
{
	//TODO: Create VRInputManager with callbacks to unify input handling for every sub-module
	//return vr_system->GetControllerState(unControllerDeviceIndex, pControllerState, unControllerStateSize);
	
	const int controllerCount = 2;
	static vr::VRControllerState_t lastControllerState[controllerCount];
	static bool shutoffStateUpdate = false;

	vr::ETrackedControllerRole hand = (unControllerDeviceIndex == GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole::TrackedControllerRole_LeftHand)) ?
		vr::ETrackedControllerRole::TrackedControllerRole_LeftHand :
		vr::ETrackedControllerRole::TrackedControllerRole_RightHand;

	// TODO: Better way of doing it?
	// We're only watching for the controller mapped to right and left hand.
	// If the game requested access to anything else, just let it through.
	if (hand != vr::ETrackedControllerRole::TrackedControllerRole_LeftHand && hand != vr::ETrackedControllerRole::TrackedControllerRole_RightHand)
	{

		bool passthroughResult = vr_system->GetControllerState(unControllerDeviceIndex, pControllerState, sizeof(vr::VRControllerState_t));

		// TODO: support for VR trackers.. (non-VR hand based controllers)
		/*
		if(passthroughResult)
		{
			// call all the registered callbacks
			for (auto it = OpenVRHookMgr::GetInstance()->mGetControllerStateCallbacks.begin(); it != OpenVRHookMgr::GetInstance()->mGetControllerStateCallbacks.end(); ++it)
			{
				vr::VRControllerState_t modifiedControllerState = ;
				GetControllerState_CB cbfunc = *it;
				cbfunc(unControllerDeviceIndex, pControllerState, unControllerStateSize);
			}

		}
		*/
	}

	vr::VRControllerState_t* lastState = &lastControllerState[hand - 1];
	vr::VRControllerState_t curState;
	vr::VRControllerState_t finalState;

	// Grab the state of the controller
	const bool result = vr_system->GetControllerState(unControllerDeviceIndex, &curState, sizeof(vr::VRControllerState_t));
	finalState = curState;

	// check if device is tracking
	const bool isTracking = vr_system->IsTrackedDeviceConnected(unControllerDeviceIndex);

	// Dispatch the controller state to scaleform there are handlers registered
	//ScaleformVR::DispatchControllerState(hand, curState);

	if(result && isTracking)
	{
		// call all the registered callbacks
		for (auto it = OpenVRHookMgr::GetInstance()->mGetControllerStateCallbacks.begin(); it != OpenVRHookMgr::GetInstance()->mGetControllerStateCallbacks.end(); ++it)
		{
			vr::VRControllerState_t modifiedControllerState = curState;
			GetControllerState_CB cbfunc = *it;
			cbfunc(unControllerDeviceIndex, &curState, unControllerStateSize, &modifiedControllerState);  // last argument is ptr to VRControllerState that the mod authors can modify and use to block inputs

			// lfrazer: bitmask with modified state provided by mod developers, this will allow the mod developers to control what button presses the game actually gets
			finalState.ulButtonPressed &= modifiedControllerState.ulButtonPressed;
			finalState.ulButtonTouched &= modifiedControllerState.ulButtonTouched;

			// copy over modified axes from joystick (only allow reduction of values though)
			for (int i = 0; i < k_unControllerStateAxisCount; ++i)
			{
				// only block inputs, so take values only if lower than existing
				if (fabs(modifiedControllerState.rAxis[i].x) < fabs(finalState.rAxis[i].x))
				{
					finalState.rAxis[i].x = modifiedControllerState.rAxis[i].x;
				}

				if (fabs(modifiedControllerState.rAxis[i].y) < fabs(finalState.rAxis[i].y))
				{
					finalState.rAxis[i].y = modifiedControllerState.rAxis[i].y;
				}
			}
		}
	}

	// Give the game access to the controller depending on the shutoff flag
	if (m_getControllerStateShutoff == false && isTracking && result) 
	{
		memcpy(pControllerState, &finalState, sizeof(vr::VRControllerState_t));
	}

	// curState = lastState
	// lfrazer: We will always use finalState now incase mod authors want to do something special with inputs
	memcpy(lastState, &finalState, sizeof(vr::VRControllerState_t));

	// Finally, update haptics here too
	OpenVRHookMgr::GetInstance()->UpdateHaptics();

	return result;
}

void HookVRSystem::TriggerHapticPulse(vr::TrackedDeviceIndex_t unControllerDeviceIndex, uint32_t unAxisId, unsigned short usDurationMicroSec)
{
	vr_system->TriggerHapticPulse(unControllerDeviceIndex, unAxisId, usDurationMicroSec);

	//Usefull for haptic suits mods
	VRManager::GetInstance().ProcessHapticEvents(unControllerDeviceIndex, unAxisId, usDurationMicroSec);
}

#pragma endregion