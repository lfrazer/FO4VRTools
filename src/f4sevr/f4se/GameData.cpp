#include "f4se/GameData.h"

// 856197F11173AF60E35EBF54A88E7BF43AFC3588+305
RelocPtr <DataHandler*> g_dataHandler(0x05930B50);

// 5ED90DCE1A1D1EDBCC888F3EA1234E23E307DD26+6
RelocPtr <bool> g_isGameDataReady(0x05AB9614);

// 71D5ABF10C2A12CB3C78D94DF3D2882337627F52+5F
RelocPtr <DefaultObjectMap*> g_defaultObjectMap(0x059357F0);

// 71D5ABF10C2A12CB3C78D94DF3D2882337627F52+6C
RelocPtr <BSReadWriteLock> g_defaultObjectMapLock(0x05936258);

// C449692A90CCF8E972B00FE2979EF8ADEED6925A+3D
RelocPtr <FavoritesManager*> g_favoritesManager(0x05AC0A50);

class LoadedModFinder
{
	const char * m_stringToFind;

public:
	LoadedModFinder(const char * str) : m_stringToFind(str) { }

	bool Accept(ModInfo* modInfo)
	{
		return _stricmp(modInfo->name, m_stringToFind) == 0;
	}
};

const ModInfo * DataHandler::LookupModByName(const char * modName)
{
	return modList.modInfoList.Find(LoadedModFinder(modName));
}

UInt8 DataHandler::GetModIndex(const char* modName)
{
	return modList.modInfoList.GetIndexOf(LoadedModFinder(modName));
}

const ModInfo* DataHandler::LookupLoadedModByName(const char* modName)
{
	for(UInt32 i = 0; i < modList.loadedModCount; i++) {
		ModInfo * modInfo = modList.loadedMods[i];
		if(_stricmp(modInfo->name, modName) == 0)
			return modInfo;
	}

	return nullptr;
}

const ModInfo* DataHandler::LookupLoadedLightModByName(const char* modName)
{
	// no light mods

	return nullptr;
}

UInt8 DataHandler::GetLoadedModIndex(const char* modName)
{
	const ModInfo * modInfo = LookupLoadedModByName(modName);
	if(modInfo) {
		return modInfo->modIndex;
	}

	return -1;
}

UInt16 DataHandler::GetLoadedLightModIndex(const char* modName)
{
	// no light mods

	return -1;
}

BGSDefaultObject * DefaultObjectMap::GetDefaultObject(BSFixedString name)
{
	BSReadLocker locker(g_defaultObjectMapLock);
	if(*g_defaultObjectMap) {
		auto entry = (*g_defaultObjectMap)->Find(&name.data);
		if(entry) {
			return entry->defaultObject;
		}
	}
	return nullptr;
}

