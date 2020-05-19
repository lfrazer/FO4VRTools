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
#include "f4se/PluginAPI.h"		// super
#include "f4se_common/f4se_version.h"	// What version of SKSE is running?
#include "common/IDebugLog.h"
#include <shlobj.h>				// CSIDL_MYCODUMENTS

#include "PapyrusVR.h"
#include "ScaleformVR.h"
#include "VRManager.h"
#include "api/VRManagerAPI.h"
#include "api/VRHookAPI.h"
#include "hooks/openvr_hook.h"
#include "api/utils/OpenVRUtils.h"

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static F4SEPapyrusInterface         * g_papyrus = NULL;
static F4SEScaleformInterface       * g_scaleform = NULL;
static F4SEMessagingInterface		* g_messagingInterface = NULL;

//API
static PapyrusVRAPI apiMessage;

// Func prototype
void OnF4SEMessageReceived(F4SEMessagingInterface::Message* message);


#pragma region API

//Returns the VRManager singleton instance
PapyrusVR::VRManagerAPI* GetVRManager()
{
	return &PapyrusVR::VRManager::GetInstance();
}


OpenVRHookManagerAPI* GetVRHookManagerAPI()
{
	return (OpenVRHookManagerAPI*)OpenVRHookMgr::GetInstance();
}

#pragma endregion


extern "C" 
{
	
	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info)
	{

		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4VR\\F4SE\\FO4VRTools.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "FO4VRTools";
		info->version = kPluginVersionNum;


		if (f4se->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if (f4se->runtimeVersion < CURRENT_RELEASE_RUNTIME)
		{
			_MESSAGE("unsupported runtime version %08X", f4se->runtimeVersion);

			return false;
		}

		_MESSAGE("FO4VRTools v%x query passed", info->version);

		// ### do not do anything else in this callback
		// ### only fill out PluginInfo and return true/false

		// supported runtime version
		return true;
	}

	//TODO: Create proper namespaces for everything
	bool F4SEPlugin_Load(const F4SEInterface* f4se)
	{	
		// Moving init to Load func
		
		// store plugin handle so we can identify ourselves later
		g_pluginHandle = f4se->GetPluginHandle();

		//Registers for messages
		g_messagingInterface = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
		if (g_messagingInterface && g_pluginHandle)
		{
			_MESSAGE("Registering for plugin loaded message!");
			g_messagingInterface->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessageReceived);
		}

		// Called by F4SE to load this plugin
		_MESSAGE("FO4VRTools loaded");

		g_papyrus = (F4SEPapyrusInterface *)f4se->QueryInterface(kInterface_Papyrus);
		g_scaleform = (F4SEScaleformInterface *)f4se->QueryInterface(kInterface_Scaleform);

		//Updates pointer
		_MESSAGE("Current register plugin function at memory address: %p", PapyrusVR::RegisterForPoseUpdates);

		bool btest = true;

		//Debug
		//__debugbreak();

		// Register PapyrusVR events & functions
		_MESSAGE("Registering Papyrus native functions...");
		btest = g_papyrus->Register(PapyrusVR::RegisterFuncs);
		if (btest)
		{
			_MESSAGE("Papyrus Functions Register Succeeded");
		}

		//Scaleform (not supported in FO4)
		/*
		btest &= g_scaleform->Register("vrinput", ScaleformVR::RegisterFuncs);
		if (btest) {
			_MESSAGE("Scaleform Functions Register Succeeded");
		}
		*/

		//OpenVR Hook
		_MESSAGE("Hooking into OpenVR calls");
		if (!DoOpenVRHook())
		{
			_MESSAGE("Failed to hook to OpenVR...");
			btest = false;
		}

		return btest;
	}

	//Returns the OpenVRHookMgr singleton instance (lets give the user full control here)
	OpenVRHookMgr* GetVRHookManager()
	{
		return OpenVRHookMgr::GetInstance();
	}

	unsigned int GetPluginVersion()
	{
		return kPluginVersionNum;
	}

};



#pragma region Messaging Interface

// Listens for F4SE events
void OnF4SEMessageReceived(F4SEMessagingInterface::Message* message)
{
	if (message)
	{
		_MESSAGE("Received F4SE message %d", message->type);
		if (message->type == F4SEMessagingInterface::kMessage_PostPostLoad)
		{
			if (g_messagingInterface && g_pluginHandle)
			{
				_MESSAGE("Game Loaded, Dispatching Init messages to all listeners");
				apiMessage.GetVRManager = GetVRManager;
				apiMessage.GetOpenVRHook = GetVRHookManagerAPI;
				//apiMessage.RegisterPoseUpdateListener = GetVRManager()->RegisterVRUpdateListener;

				//Sends pointers to API functions/classes
				g_messagingInterface->Dispatch(g_pluginHandle, kPapyrusVR_Message_Init, &apiMessage, sizeof(apiMessage), NULL);
			}
		}

		if (message->type == F4SEMessagingInterface::kMessage_GameLoaded)
		{
			//Register manifest file
			PapyrusVR::VRManager::GetInstance().RegisterInputActions();
		}

		if (message->type == F4SEMessagingInterface::kMessage_PostLoadGame)
		{
			//Get player nodes, etc.
			PapyrusVR::OpenVRUtils::SetupConversion();
		}
	}
}


#pragma endregion

