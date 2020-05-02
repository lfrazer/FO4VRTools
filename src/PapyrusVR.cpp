#include "PapyrusVR.h"
#include <ctime>
#include "hooks/openvr_hook.h"

#define ENABLE_PAPYRUS 0  // Papyrus events will be disabled until I figure out how to fix them for FO4

namespace PapyrusVR 
{


	//Custom Pose Event
	BSFixedString poseUpdateEventName("OnPosesUpdate");
	RegistrationSetHolder<TESForm*>				g_posesUpdateEventRegs;

	//Custom Button Events
	BSFixedString vrButtonEventName("OnVRButtonEvent");
	RegistrationSetHolder<TESForm*>				g_vrButtonEventRegs;

	//Custom Overlap Events
	BSFixedString vrOverlapEventName("OnVROverlapEvent");
	RegistrationSetHolder<TESForm*>				g_vrOverlapEventRegs;

	//Custom Haptics Events
	BSFixedString vrHapticEventName("OnVRHapticEvent");
	RegistrationSetHolder<TESForm*>				g_vrHapticEventRegs;


	// IFunctionArguments don't exist in fallout..
#if ENABLE_PAPYRUS
	#pragma region Papyrus Events
		class EventQueueFunctor0 : public IFunctionArguments
		{
		public:
			EventQueueFunctor0(BSFixedString & a_eventName)
				: eventName(a_eventName.data) {}

			virtual bool Copy(Output * dst) { return true; }

			void operator() (const EventRegistration<TESForm*> & reg)
			{
				VirtualMachine* registry = (*g_gameVM)->m_virtualMachine;
				registry->QueueEvent(reg.handle, &eventName, this);
			}

		private:
			BSFixedString	eventName;
		};

		template <typename T> void SetVMValue(VMValue * val, T arg)
		{
			VMClassRegistry * registry = (*g_skyrimVM)->GetClassRegistry();
			PackValue(val, &arg, registry);
		}
		template <> void SetVMValue<SInt32>(VMValue * val, SInt32 arg) { val->SetInt(arg); }

		class EventFunctor3 : public IFunctionArguments
		{
		public:
			EventFunctor3(BSFixedString & a_eventName, SInt32 aParam1, SInt32 aPram2, SInt32 aParam3)
				: eventName(a_eventName.data), param1(aParam1), param2(aPram2), param3(aParam3){}

			virtual bool Copy(Output * dst) 
			{ 
				dst->Resize(3);
				SetVMValue(dst->Get(0), param1);
				SetVMValue(dst->Get(1), param2);
				SetVMValue(dst->Get(2), param3);
				return true; 
			}

			void operator() (const EventRegistration<TESForm*> & reg)
			{
				VirtualMachine* registry = (*g_gameVM)->m_virtualMachine;
				registry->QueueEvent(reg.handle, &eventName, this);
			}

		private:
			SInt32				param1;
			SInt32				param2;
			SInt32				param3;
			BSFixedString		eventName;
		};
	#pragma endregion
#endif

	#pragma region Papyrus Native Functions
		#pragma region SteamVR Coordinates
			void GetSteamVRDeviceRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSteamVRDeviceRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Rotation);
			}

			void GetSteamVRDeviceQRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSteamVRDeviceQRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::QRotation);
			}

			void GetSteamVRDevicePosition(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSteamVRDevicePosition");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Position);

			}
		#pragma endregion

		#pragma region Skyrim Coordinates
			void GetSkyrimDeviceRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSkyrimDeviceRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Rotation, true);
			}

			void GetSkyrimDeviceQRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSkyrimDeviceQRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::QRotation, true);
			}

			void GetSkyrimDevicePosition(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSkyrimDevicePosition");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Position, true);
			}
		#pragma endregion

		#pragma region Events Management
			void FormRegisterForEvent(TESForm* object, RegistrationSetHolder<TESForm*>* regHolder)
			{
				GenericPapyrusRegisterForEvent(object, regHolder);
				if (object && object->formID)
					_MESSAGE("%d registered", object->formID);
			}

			void FormUnregisterForEvent(TESForm* object, RegistrationSetHolder<TESForm*>* regHolder)
			{
				GenericPapyrusUnregisterForEvent(object, regHolder);
				if (object && object->formID)
					_MESSAGE("%d unregistered", object->formID);
			}

			void RegisterForPoseUpdates(StaticFunctionTag *base,	TESForm* thisForm)
			{
				_MESSAGE("RegisterForPoseUpdates");
				FormRegisterForEvent(thisForm, &g_posesUpdateEventRegs);
			}

			void UnregisterForPoseUpdates(StaticFunctionTag *base,	TESForm* thisForm)
			{
				_MESSAGE("UnregisterForPoseUpdates");
				FormUnregisterForEvent(thisForm, &g_posesUpdateEventRegs);
			}

			void RegisterForVRButtonEvents(StaticFunctionTag *base, TESForm * thisForm)
			{
				_MESSAGE("RegisterForVRButtonEvents");
				FormRegisterForEvent(thisForm, &g_vrButtonEventRegs);
			}

			void UnregisterForVRButtonEvents(StaticFunctionTag *base, TESForm * thisForm)
			{
				_MESSAGE("UnregisterForVRButtonEvents");
				FormUnregisterForEvent(thisForm, &g_vrButtonEventRegs);
			}
			void RegisterForVROverlapEvents(StaticFunctionTag *base, TESForm * thisForm)
			{
				_MESSAGE("RegisterForVROverlapEvents");
				FormRegisterForEvent(thisForm, &g_vrOverlapEventRegs);
			}

			void UnregisterForVROverlapEvents(StaticFunctionTag *base, TESForm * thisForm)
			{
				_MESSAGE("UnregisterForVROverlapEvents");
				FormUnregisterForEvent(thisForm, &g_vrOverlapEventRegs);
			}
			void RegisterForVRHapticEvents(StaticFunctionTag *base, TESForm * thisForm)
			{
				_MESSAGE("RegisterForVRHapticEvents");
				FormRegisterForEvent(thisForm, &g_vrHapticEventRegs);
			}
			void UnregisterForVRHapticEvents(StaticFunctionTag *base, TESForm * thisForm)
			{
				_MESSAGE("UnregisterForVRHapticEvents");
				FormUnregisterForEvent(thisForm, &g_vrHapticEventRegs);
			}
		#pragma endregion

		#pragma region Overlap Objects
			UInt32 RegisterLocalOverlapSphere(StaticFunctionTag *base, float radius, VMArray<float> vmPosition, VMArray<float> vmRotation, SInt32 deviceEnum)
			{
				_MESSAGE("RegisterLocalOverlapSphere");
				if (vmPosition.Length() != 3)
					return 0;
				if (vmRotation.Length() != 4)
					return 0;

				Vector3 position;
				Quaternion rotation;
				OpenVRUtils::CopyVMArrayToVector3(&vmPosition, &position);
				_MESSAGE("Got position X:%f Y:%f Z:%f", position.x, position.y, position.z);
				OpenVRUtils::CopyVMArrayToQuaternion(&vmRotation, &rotation);
				_MESSAGE("Got rotation X:%f Y:%f Z:%f W:%f", rotation.x, rotation.y, rotation.z, rotation.w);

				Matrix34* transform = new Matrix34;
				*transform = OpenVRUtils::CreateTransformMatrix(&position, &rotation);

				_MESSAGE("Created Transform Matrix");
				_MESSAGE("|\t%f\t%f\t%f\t%f\t|", transform->m[0][0], transform->m[0][1], transform->m[0][2], transform->m[0][3]);
				_MESSAGE("|\t%f\t%f\t%f\t%f\t|", transform->m[1][0], transform->m[1][1], transform->m[1][2], transform->m[1][3]);
				_MESSAGE("|\t%f\t%f\t%f\t%f\t|", transform->m[2][0], transform->m[2][1], transform->m[2][2], transform->m[2][3]);
				return VRManager::GetInstance().CreateLocalOverlapSphere(radius, transform, (VRDevice)deviceEnum);
			}

			void DestroyLocalOverlapObject(StaticFunctionTag *base, UInt32 objectHandle)
			{
				VRManager::GetInstance().DestroyLocalOverlapObject(objectHandle);
			}
		#pragma endregion

		//Used for debugging
		std::clock_t start = clock();
		void TimeSinceLastCall(StaticFunctionTag* base)
		{
			clock_t end = clock();
			double elapsed_seconds = double(end - start) / CLOCKS_PER_SEC;
			_MESSAGE("90 events fired after %f seconds", elapsed_seconds);
			start = end;
		}

	#pragma endregion

	#pragma region Utility Methods
		void CopyPoseToVMArray(UInt32 deviceType, VMArray<float>* resultArray, PoseParam parameter, bool skyrimWorldSpace)
		{
			TrackedDevicePose* requestedPose = VRManager::GetInstance().GetPoseByDeviceEnum((VRDevice)deviceType);

			//Pose exists
			if (requestedPose)
			{
				Matrix34 matrix = requestedPose->mDeviceToAbsoluteTracking;

				if (parameter == PoseParam::Position)
				{
					//Position
					Vector3 devicePosition = OpenVRUtils::GetPosition(&(requestedPose->mDeviceToAbsoluteTracking));

					//Really dumb way to do it, just for testing
					if (skyrimWorldSpace)
					{
						NiAVObject* playerNode = (*g_player)->firstPersonSkeleton;  //->GetNiNode(); (No GetNiNode in F4SE?)
						devicePosition.x += playerNode->m_worldTransform.pos.x;
						devicePosition.y += playerNode->m_worldTransform.pos.y;
						devicePosition.z += playerNode->m_worldTransform.pos.z;
					}

					OpenVRUtils::CopyVector3ToVMArray(&devicePosition, resultArray);
				}
				else
				{
					Quaternion quatRotation = OpenVRUtils::GetRotation(&(requestedPose->mDeviceToAbsoluteTracking));

					if (parameter == PoseParam::Rotation)
					{
						//Euler
						Vector3 deviceRotation = OpenVRUtils::QuatToEuler(&quatRotation);
						OpenVRUtils::CopyVector3ToVMArray(&deviceRotation, resultArray);
					}
					else
					{
						//Quaternion
						OpenVRUtils::CopyQuaternionToVMArray(&quatRotation, resultArray);
					}
				}
			}
		}
	#pragma endregion

	void OnVRButtonEventReceived(VREventType eventType, EVRButtonId buttonId, VRDevice deviceId)
	{
#if ENABLE_PAPYRUS
		_MESSAGE("Dispatching eventType %d for button with ID: %d from device %d", eventType, buttonId, deviceId);
		//Notify Papyrus scripts
		if (g_vrButtonEventRegs.m_data.size() > 0)
			g_vrButtonEventRegs.ForEach(
				EventFunctor3(vrButtonEventName, eventType, buttonId, deviceId)
			);
#endif
	}

	void OnVROverlapEventReceived(VROverlapEvent eventType, UInt32 objectHandle, VRDevice deviceId)
	{
#if ENABLE_PAPYRUS
		_MESSAGE("Dispatching overlap %d for device with ID: %d from handle %d", eventType, deviceId, objectHandle);
		//Notify Papyrus scripts
		if (g_vrOverlapEventRegs.m_data.size() > 0)
			g_vrOverlapEventRegs.ForEach(
				EventFunctor3(vrOverlapEventName, eventType, objectHandle, deviceId)
			);
#endif
	}

	void OnVRHapticEventReceived(UInt32 axisID, UInt32 pulseDuration, VRDevice device)
	{
#if ENABLE_PAPYRUS
		_MESSAGE("Dispatching haptic event for device with ID: %d with axisID %d and duration %d", device, axisID, pulseDuration);
		//Notify Papyrus scripts
		if (g_vrHapticEventRegs.m_data.size() > 0)
			g_vrHapticEventRegs.ForEach(
				EventFunctor3(vrHapticEventName, axisID, pulseDuration, device)
			);
#endif
	}

	//Entry Point
	bool RegisterFuncs(VirtualMachine* vm) 
	{
		_MESSAGE("Registering native functions...");
		vm->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, SInt32, VMArray<float>>("GetSteamVRDeviceRotation_Native", "PapyrusVR", PapyrusVR::GetSteamVRDeviceRotation, vm));
		vm->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, SInt32, VMArray<float>>("GetSteamVRDeviceQRotation_Native", "PapyrusVR", PapyrusVR::GetSteamVRDeviceQRotation, vm));
		vm->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, SInt32, VMArray<float>>("GetSteamVRDevicePosition_Native", "PapyrusVR", PapyrusVR::GetSteamVRDevicePosition, vm));
		vm->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, SInt32, VMArray<float>>("GetSkyrimDeviceRotation_Native", "PapyrusVR", PapyrusVR::GetSkyrimDeviceRotation, vm));
		vm->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, SInt32, VMArray<float>>("GetSkyrimDeviceQRotation_Native", "PapyrusVR", PapyrusVR::GetSkyrimDeviceQRotation, vm));
		vm->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, SInt32, VMArray<float>>("GetSkyrimDevicePosition_Native", "PapyrusVR", PapyrusVR::GetSkyrimDevicePosition, vm));

		vm->RegisterFunction(new NativeFunction4 <StaticFunctionTag, UInt32, float, VMArray<float>, VMArray<float>, SInt32>("RegisterLocalOverlapSphere", "PapyrusVR", PapyrusVR::RegisterLocalOverlapSphere, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, UInt32>("DestroyLocalOverlapObject", "PapyrusVR", PapyrusVR::DestroyLocalOverlapObject, vm));

		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("RegisterForPoseUpdates", "PapyrusVR", PapyrusVR::RegisterForPoseUpdates, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("UnregisterForPoseUpdates", "PapyrusVR", PapyrusVR::UnregisterForPoseUpdates, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("RegisterForVRButtonEvents", "PapyrusVR", PapyrusVR::RegisterForVRButtonEvents, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("UnregisterForVRButtonEvents", "PapyrusVR", PapyrusVR::UnregisterForVRButtonEvents, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("RegisterForVROverlapEvents", "PapyrusVR", PapyrusVR::RegisterForVROverlapEvents, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("UnregisterForVROverlapEvents", "PapyrusVR", PapyrusVR::UnregisterForVROverlapEvents, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("RegisterForVRHapticEvents", "PapyrusVR", PapyrusVR::RegisterForVRHapticEvents, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("UnregisterForVRHapticEvents", "PapyrusVR", PapyrusVR::UnregisterForVRHapticEvents, vm));

		vm->RegisterFunction(new NativeFunction0 <StaticFunctionTag, void>("TimeSinceLastCall", "PapyrusVR", PapyrusVR::TimeSinceLastCall, vm)); //Debug function

		/*_MESSAGE("Creating trampoline");
		if (!l_LocalBranchTrampoline.Create(1024 * 64))
		{
			_MESSAGE("Can't create local branch trampoline!");
			return false;
		}*/

		_MESSAGE("Registering for VR Button Events");
		VRManager::GetInstance().RegisterVRButtonListener(PapyrusVR::OnVRButtonEventReceived);


		_MESSAGE("Registering for VR Overlap Events");
		VRManager::GetInstance().RegisterVROverlapListener(PapyrusVR::OnVROverlapEventReceived);

		_MESSAGE("Registering for VR Haptic Events");
		VRManager::GetInstance().RegisterVRHapticListener(PapyrusVR::OnVRHapticEventReceived);
		
		/*_MESSAGE("Hooking into OpenVR calls");
		l_LocalBranchTrampoline.Write5Call(OpenVR_Call, GetFnAddr(&PapyrusVR::OnVRUpdate));*/

		return true;
	}
}
