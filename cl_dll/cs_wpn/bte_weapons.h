#pragma once

class CBasePlayer;
class CBasePlayerWeapon;

class CBTEClientWeapons
{
private:
	CBTEClientWeapons();

public:
	void Init();

	void PrepEntity(CBasePlayer *pWeaponOwner);
	void ActiveWeapon(const char *name);
	CBasePlayerWeapon *GetActiveWeaponEntity()
	{
		return m_pActiveWeapon;
	}

private:
	CBasePlayerWeapon *m_pActiveWeapon;

public:
	// singleton accessor
	friend CBTEClientWeapons &BTEClientWeapons();
};