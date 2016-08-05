/* =========================================

		FCNPC - Fully Controllable NPC
			----------------------

	- File: Hooks.cpp
	- Author(s): OrMisicL

  =========================================*/

#include "Main.hpp"

extern CServer      *pServer;
extern logprintf_t  logprintf;
extern void         *pAMXFunctions;

BYTE bytePushCount;
char szPreviousFuncName[32];
bool bHookIsExec;
bool bFindPublicIsBlocked;
bool bIsPublicFound;

// damage
struct t_OnPlayerDamage
{
	WORD wPlayerId;
	WORD wSecondPlayerId;
	float fHealthLoss;
	BYTE byteWeaponId;
	int iBodypart;
};

bool bTakeDamage;
bool bGiveDamage;

t_OnPlayerDamage pDamage;

// weapon shoot
bool bWeaponShot;

struct t_OnPlayerWeaponShot
{
	WORD wPlayerId;
	BYTE byteWeaponId;
	int iHitType;
	int iHitId;
	CVector vecHit;
};

t_OnPlayerWeaponShot pWeaponShot;

// stream in/out
struct t_OnPlayerStream
{
	WORD wPlayerId;
	WORD wForPlayerId;
};

t_OnPlayerStream pStream;

bool bStreamIn;
bool bStreamOut;

// subhook
subhook_t hookFindPublic;
subhook_t hookPush;
subhook_t hookExec;

// amx_FindPublic function definition
typedef int(*amx_FindPublic_t)(AMX *amx, const char *funcname, int *index);
amx_FindPublic_t pfn_amx_FindPublic = NULL;

// amx_Push function definition
typedef int(*amx_Push_t)(AMX *amx, cell value);
amx_Push_t pfn_amx_Push = NULL;

// amx_Exec function definition
typedef int(*amx_Exec_t)(AMX *amx, long *retval, int index);
amx_Exec_t pfn_amx_Exec = NULL;

int amx_FindPublic_Hook(AMX *amx, const char *funcname, int *index)
{
	pfn_amx_FindPublic = (amx_FindPublic_t)(subhook_get_trampoline(hookFindPublic));
	bytePushCount = 0;

	if (bHookIsExec) {
		if (!strcmp(funcname, szPreviousFuncName)) {
			if (bFindPublicIsBlocked) {
				return 1;
			} else {
				return pfn_amx_FindPublic(amx, funcname, index);
			}
		} else {
			bFindPublicIsBlocked = false;
			bHookIsExec = false;
			bIsPublicFound = false;
			szPreviousFuncName[0] = '\0';
		}
	}

	if (!bHookIsExec && !bIsPublicFound) {
		if (!strcmp(funcname, "OnPlayerGiveDamage")) {
			bIsPublicFound = true;
			bGiveDamage = true;
		} else if (!strcmp(funcname, "OnPlayerTakeDamage")) {
			bIsPublicFound = true;
			bTakeDamage = true;
		} else if (!strcmp(funcname, "OnPlayerWeaponShot")) {
			bIsPublicFound = true;
			bWeaponShot = true;
		} else if (!strcmp(funcname, "OnPlayerStreamIn")) {
			bIsPublicFound = true;
			bStreamIn = true;
		} else if (!strcmp(funcname, "OnPlayerStreamOut")) {
			bIsPublicFound = true;
			bStreamOut = true;
		}

		if (bIsPublicFound) {
			strlcpy(szPreviousFuncName, funcname, sizeof(szPreviousFuncName));
		}
	}

	return pfn_amx_FindPublic(amx, funcname, index);
}

int amx_Push_Hook(AMX *amx, cell value)
{
	// Are we retrieving parameters ?
	if (bGiveDamage || bTakeDamage) {
		switch (bytePushCount) {
			case 4:
				pDamage.wPlayerId = value;
				break;

			case 3:
				pDamage.wSecondPlayerId = value;
				break;

			case 2:
				pDamage.fHealthLoss = amx_ctof(value);
				break;

			case 1:
				pDamage.byteWeaponId = value;
				break;

			case 0:
				pDamage.iBodypart = value;
				break;
		}
		// Increase the parameters count
		bytePushCount++;
	} else if (bWeaponShot) {
		switch (bytePushCount) {
			case 6:
				pWeaponShot.wPlayerId = value;
				break;

			case 5:
				pWeaponShot.byteWeaponId = value;
				break;

			case 4:
				pWeaponShot.iHitType = value;
				break;

			case 3:
				pWeaponShot.iHitId = value;
				break;

			case 2:
				pWeaponShot.vecHit.fX = amx_ctof(value);
				break;

			case 1:
				pWeaponShot.vecHit.fY = amx_ctof(value);
				break;

			case 0:
				pWeaponShot.vecHit.fZ = amx_ctof(value);
				break;
		}
		// Increase the parameters count
		bytePushCount++;
	} else if (bStreamIn) {
		switch (bytePushCount) {
			case 1:
				pStream.wPlayerId = value;
				break;

			case 0:
				pStream.wForPlayerId = value;
				break;
		}
		// Increase the parameters count
		bytePushCount++;
	} else if (bStreamOut) {
		switch (bytePushCount) {
			case 1:
				pStream.wPlayerId = value;
				break;

			case 0:
				pStream.wForPlayerId = value;
				break;
		}
		// Increase the parameters count
		bytePushCount++;
	}

	pfn_amx_Push = (amx_Push_t)(subhook_get_trampoline(hookPush));

	return pfn_amx_Push(amx, value);
}

int amx_Exec_Hook(AMX *amx, long *retval, int index)
{
	pfn_amx_Exec = (amx_Exec_t)(subhook_get_trampoline(hookExec));

	int ret = 0;

	if (bGiveDamage) {
		bGiveDamage = false;

		// get the npc data
		CPlayerData *pPlayerData = pServer->GetPlayerManager()->GetAt(pDamage.wSecondPlayerId);

		// check on invulnerable
		if (!pPlayerData || !pPlayerData->IsInvulnerable()) {
			// call hooked callback
			ret = pfn_amx_Exec(amx, retval, index);

			// call custom callback
			if (pPlayerData) {
				pPlayerData->ProcessDamage(pDamage.wPlayerId, pDamage.fHealthLoss, pDamage.byteWeaponId, pDamage.iBodypart);
			}
		}

		bHookIsExec = true;
	} else if (bTakeDamage) {
		bTakeDamage = false;

		// call hooked callback
		ret = pfn_amx_Exec(amx, retval, index);

		// get the player data
		CPlayerData *pPlayerData = pServer->GetPlayerManager()->GetAt(pDamage.wSecondPlayerId);

		// call custom callback
		if (pPlayerData) {
			CCallbackManager::OnGiveDamage(pDamage.wPlayerId, pDamage.wSecondPlayerId, pDamage.byteWeaponId, pDamage.iBodypart, pDamage.fHealthLoss);
		}

		bHookIsExec = true;
	} else if (bWeaponShot) {
		bWeaponShot = false;

		// call hooked callback
		ret = pfn_amx_Exec(amx, retval, index);

		// call custom callback
		if (pWeaponShot.iHitType == BULLET_HIT_TYPE_VEHICLE) {
			WORD wPlayerId = pServer->GetVehicleSeatPlayerId(pWeaponShot.iHitId, 0);

			CPlayerData *pPlayerData = pServer->GetPlayerManager()->GetAt(wPlayerId);
			if (pPlayerData) {
				pPlayerData->ProcessVehicleDamage(pWeaponShot.wPlayerId, pWeaponShot.iHitId, pWeaponShot.byteWeaponId, pWeaponShot.vecHit);
			}
		}

		bHookIsExec = true;
	} else if (bStreamIn) {
		bStreamIn = false;

		CPlayerData *pPlayerData = pServer->GetPlayerManager()->GetAt(pStream.wPlayerId);
		if (pPlayerData) {
			pPlayerData->ProcessStreamIn(pStream.wForPlayerId);
			bFindPublicIsBlocked = true;
		} else {
			// call hooked callback
			ret = pfn_amx_Exec(amx, retval, index);
		}

		bHookIsExec = true;
	} else if (bStreamOut) {
		bStreamOut = false;

		CPlayerData *pPlayerData = pServer->GetPlayerManager()->GetAt(pStream.wPlayerId);
		if (pPlayerData) {
			// call custom callback
			pPlayerData->ProcessStreamOut(pStream.wForPlayerId);
			bFindPublicIsBlocked = true;
		} else {
			// call hooked callback
			ret = pfn_amx_Exec(amx, retval, index);
		}

		bHookIsExec = true;
	} else {
		ret = pfn_amx_Exec(amx, retval, index);
	}

	return ret;
}

void CHooks::InstallHooks()
{
	// Reset public flag
	bHookIsExec = false;
	bFindPublicIsBlocked = false;
	bIsPublicFound = false;
	bGiveDamage = false;
	bTakeDamage = false;
	bWeaponShot = false;
	bStreamIn = false;
	bStreamOut = false;

	// Find the function pointers
	BYTE *pFindPublic = *(BYTE **)((DWORD)pAMXFunctions + PLUGIN_AMX_EXPORT_FindPublic * 4);
	BYTE *pPush = *(BYTE **)((DWORD)pAMXFunctions + PLUGIN_AMX_EXPORT_Push * 4);
	BYTE *pExec = *(BYTE **)((DWORD)pAMXFunctions + PLUGIN_AMX_EXPORT_Exec * 4);

	// Hook for amx_FindPublic
	hookFindPublic = subhook_new(pFindPublic, (BYTE *)&amx_FindPublic_Hook);
	subhook_install(hookFindPublic);

	// Hook for amx_Push
	hookPush = subhook_new(pPush, (BYTE *)&amx_Push_Hook);
	subhook_install(hookPush);

	// Hook for amx_Exec
	hookExec = subhook_new(pExec, (BYTE *)&amx_Exec_Hook);
	subhook_install(hookExec);
}
