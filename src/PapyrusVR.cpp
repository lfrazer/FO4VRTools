#include "PapyrusVR.h"
#include <ctime>
#include "hooks/openvr_hook.h"

#define ENABLE_PAPYRUS 1  // Papyrus events will be disabled until I figure out how to fix them for FO4

namespace PapyrusVR 
{

	// static BSFixedString crashes FO4 -- We don't actually know why this ever worked in SkyrimVR

	//Custom Pose Event
	const char* poseUpdateEventName = "OnPosesUpdate";
	RegistrationSetHolder<NullParameters>				g_posesUpdateEventRegs;

	//Custom Button Events
	const char* vrButtonEventName = "OnVRButtonEvent";
	RegistrationSetHolder<NullParameters>				g_vrButtonEventRegs;

	//Custom Overlap Events
	const char* vrOverlapEventName = "OnVROverlapEvent";
	RegistrationSetHolder<NullParameters>				g_vrOverlapEventRegs;

	//Custom Haptics Events
	const char* vrHapticEventName = "OnVRHapticEvent";
	RegistrationSetHolder<NullParameters>				g_vrHapticEventRegs;

	// IFunctionArguments don't exist in fallout..
#if ENABLE_PAPYRUS
	#pragma region Papyrus Events
		class EventQueueFunctor0 : public IFunctionArguments
		{
		public:
			EventQueueFunctor0(BSFixedString& a_eventName)
			{
				eventName = a_eventName;
			}

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
			VirtualMachine * registry = (*g_gameVM)->m_virtualMachine;
			PackValue(val, &arg, registry);
		}
		template <> void SetVMValue<SInt32>(VMValue * val, SInt32 arg) { val->SetInt(arg); }

		class EventFunctor3 : public IFunctionArguments
		{
		public:
			EventFunctor3(BSFixedString & a_eventName, SInt32 aParam1, SInt32 aPram2, SInt32 aParam3)
				: param1(aParam1), param2(aPram2), param3(aParam3)
			{
				eventName = a_eventName;
			}

			virtual bool Copy(Output * dst) 
			{ 
				dst->Resize(3);
				SetVMValue(dst->Get(0), param1);
				SetVMValue(dst->Get(1), param2);
				SetVMValue(dst->Get(2), param3);
				return true; 
			}

			void operator() (const EventRegistration<NullParameters> & reg)
			{
				VirtualMachine* registry = (*g_gameVM)->m_virtualMachine;
				//registry->QueueEvent(reg.handle, &eventName, this);

				// NOTE: Workaround for unfinished RE of QueueEvent in F4SE --> See the comment in PapyrusEventsDef_Base.inl  "This should eventually replaced by an actual call to the Event Queue (VM+0x158), this is a workaround for now"
				SendPapyrusEvent3<SInt32, SInt32, SInt32>(reg.handle, reg.scriptName, eventName, param1, param2, param3);
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
				//_MESSAGE("GetSteamVRDeviceRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Rotation);
			}

			void GetSteamVRDeviceQRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				//_MESSAGE("GetSteamVRDeviceQRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::QRotation);
			}

			void GetSteamVRDevicePosition(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				//_MESSAGE("GetSteamVRDevicePosition");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Position);

			}
		#pragma endregion

		#pragma region Skyrim Coordinates
			void GetFO4DeviceRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				//_MESSAGE("GetSkyrimDeviceRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Rotation, true);
			}

			void GetFO4DeviceQRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				//_MESSAGE("GetSkyrimDeviceQRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::QRotation, true);
			}

			void GetFO4DevicePosition(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues)
			{
				//_MESSAGE("GetSkyrimDevicePosition");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Position, true);
			}
		#pragma endregion

		#pragma region Events Management
			/*
			void FormRegisterForEvent(TESForm* object, RegistrationSetHolder<TESForm*>* regHolder)
			{
				GenericPapyrusRegisterForEvent(object, regHolder);
				if (object && object->formID)
					_MESSAGE("FormID: %x event registered", object->formID);
			}

			void FormUnregisterForEvent(TESForm* object, RegistrationSetHolder<TESForm*>* regHolder)
			{
				GenericPapyrusUnregisterForEvent(object, regHolder);
				if (object && object->formID)
					_MESSAGE("FormID: %x event unregistered", object->formID);
			}
			*/

			void RegisterForPoseUpdates(StaticFunctionTag* base, VMObject* thisObject)
			{
				if (!thisObject)
					return;

				_MESSAGE("RegisterForPoseUpdates");

				g_posesUpdateEventRegs.Register(thisObject->GetHandle(), thisObject->GetObjectType());
			}

			void UnregisterForPoseUpdates(StaticFunctionTag* base, VMObject* thisObject)
			{
				if (!thisObject)
					return;

				_MESSAGE("UnregisterForPoseUpdates");

				g_posesUpdateEventRegs.Unregister(thisObject->GetHandle(), thisObject->GetObjectType());
			}

			void RegisterForVRButtonEvents(StaticFunctionTag* base, VMObject* thisObject)
			{
				if (!thisObject)
					return;

				_MESSAGE("RegisterForVRButtonEvents");
				g_vrButtonEventRegs.Register(thisObject->GetHandle(), thisObject->GetObjectType());
			}

			void UnregisterForVRButtonEvents(StaticFunctionTag* base, VMObject* thisObject)
			{
				if (!thisObject)
					return;

				_MESSAGE("UnregisterForVRButtonEvents");
				g_vrButtonEventRegs.Unregister(thisObject->GetHandle(), thisObject->GetObjectType());
			}
			void RegisterForVROverlapEvents(StaticFunctionTag* base, VMObject* thisObject)
			{
				if (!thisObject)
					return;

				_MESSAGE("RegisterForVROverlapEvents");
				g_vrOverlapEventRegs.Register(thisObject->GetHandle(), thisObject->GetObjectType());
			}

			void UnregisterForVROverlapEvents(StaticFunctionTag* base, VMObject* thisObject)
			{
				if (!thisObject)
					return;

				_MESSAGE("UnregisterForVROverlapEvents");
				g_vrOverlapEventRegs.Unregister(thisObject->GetHandle(), thisObject->GetObjectType());
			}
			void RegisterForVRHapticEvents(StaticFunctionTag* base, VMObject* thisObject)
			{
				if (!thisObject)
					return;

				_MESSAGE("RegisterForVRHapticEvents");
				g_vrHapticEventRegs.Register(thisObject->GetHandle(), thisObject->GetObjectType());
			}
			void UnregisterForVRHapticEvents(StaticFunctionTag* base, VMObject* thisObject)
			{
				if (!thisObject)
					return;

				_MESSAGE("UnregisterForVRHapticEvents");
				g_vrHapticEventRegs.Unregister(thisObject->GetHandle(), thisObject->GetObjectType());
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
		//_MESSAGE("Dispatching eventType %d for button with ID: %d from device %d", eventType, buttonId, deviceId);
		//Notify Papyrus scripts
		if (g_vrButtonEventRegs.m_data.size() > 0)
			g_vrButtonEventRegs.ForEach(
				EventFunctor3( BSFixedString( vrButtonEventName), eventType, buttonId, deviceId)
				
			);
#endif
	}

	void OnVROverlapEventReceived(VROverlapEvent eventType, UInt32 objectHandle, VRDevice deviceId)
	{
#if ENABLE_PAPYRUS
		//_MESSAGE("Dispatching overlap %d for device with ID: %d from handle %d", eventType, deviceId, objectHandle);
		//Notify Papyrus scripts
		if (g_vrOverlapEventRegs.m_data.size() > 0)
			g_vrOverlapEventRegs.ForEach(
				EventFunctor3(BSFixedString(vrOverlapEventName), eventType, objectHandle, deviceId)
			);
#endif
	}

	void OnVRHapticEventReceived(UInt32 axisID, UInt32 pulseDuration, VRDevice device)
	{
#if ENABLE_PAPYRUS
		//_MESSAGE("Dispatching haptic event for device with ID: %d with axisID %d and duration %d", device, axisID, pulseDuration);
		//Notify Papyrus scripts
		if (g_vrHapticEventRegs.m_data.size() > 0)
			g_vrHapticEventRegs.ForEach(
				EventFunctor3(BSFixedString(vrHapticEventName), axisID, pulseDuration, device)
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
		vm->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, SInt32, VMArray<float>>("GetFO4DeviceRotation_Native", "PapyrusVR", PapyrusVR::GetFO4DeviceRotation, vm));
		vm->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, SInt32, VMArray<float>>("GetFO4DeviceQRotation_Native", "PapyrusVR", PapyrusVR::GetFO4DeviceQRotation, vm));
		vm->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, SInt32, VMArray<float>>("GetFO4DevicePosition_Native", "PapyrusVR", PapyrusVR::GetFO4DevicePosition, vm));

		vm->RegisterFunction(new NativeFunction4 <StaticFunctionTag, UInt32, float, VMArray<float>, VMArray<float>, SInt32>("RegisterLocalOverlapSphere", "PapyrusVR", PapyrusVR::RegisterLocalOverlapSphere, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, UInt32>("DestroyLocalOverlapObject", "PapyrusVR", PapyrusVR::DestroyLocalOverlapObject, vm));

		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, VMObject*>("RegisterForPoseUpdates", "PapyrusVR", PapyrusVR::RegisterForPoseUpdates, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, VMObject*>("UnregisterForPoseUpdates", "PapyrusVR", PapyrusVR::UnregisterForPoseUpdates, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, VMObject*>("RegisterForVRButtonEvents", "PapyrusVR", PapyrusVR::RegisterForVRButtonEvents, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, VMObject*>("UnregisterForVRButtonEvents", "PapyrusVR", PapyrusVR::UnregisterForVRButtonEvents, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, VMObject*>("RegisterForVROverlapEvents", "PapyrusVR", PapyrusVR::RegisterForVROverlapEvents, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, VMObject*>("UnregisterForVROverlapEvents", "PapyrusVR", PapyrusVR::UnregisterForVROverlapEvents, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, VMObject*>("RegisterForVRHapticEvents", "PapyrusVR", PapyrusVR::RegisterForVRHapticEvents, vm));
		vm->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, VMObject*>("UnregisterForVRHapticEvents", "PapyrusVR", PapyrusVR::UnregisterForVRHapticEvents, vm));

		vm->RegisterFunction(new NativeFunction0 <StaticFunctionTag, void>("TimeSinceLastCall", "PapyrusVR", PapyrusVR::TimeSinceLastCall, vm)); //Debug function

		_MESSAGE("Registering for VR Button Events");
		VRManager::GetInstance().RegisterVRButtonListener(PapyrusVR::OnVRButtonEventReceived);


		_MESSAGE("Registering for VR Overlap Events");
		VRManager::GetInstance().RegisterVROverlapListener(PapyrusVR::OnVROverlapEventReceived);

		_MESSAGE("Registering for VR Haptic Events");
		VRManager::GetInstance().RegisterVRHapticListener(PapyrusVR::OnVRHapticEventReceived);
		

		return true;
	}
}
