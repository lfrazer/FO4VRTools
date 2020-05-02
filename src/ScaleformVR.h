#pragma once
#include "openvr.h"
#include "f4se/ScaleformAPI.h"
#include "f4se/ScaleformValue.h"

namespace ScaleformVR
{
	void DispatchControllerState(vr::ETrackedControllerRole controllerHand, vr::VRControllerState_t controllerState);
	bool RegisterFuncs(GFxMovieView * view, GFxValue * root);
}