#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "client.h"
#include "player_zombie_skill.h"

#include <string>

#include "gamemode/mod_zb2.h"

const float ZOMBIECRAZY_DURATION = 10.0f;
const float ZOMBIECRAZY_COOLDOWN = 10.0f;

void IZombieSkill::Think()
{
	if (m_iZombieSkillStatus == SKILL_STATUS_USING && gpGlobals->time > m_flTimeZombieSkillEnd)
	{
		OnSkillEnd();
	}

	if (m_iZombieSkillStatus == SKILL_STATUS_FREEZING && gpGlobals->time > m_flTimeZombieSkillNext)
	{
		m_iZombieSkillStatus = SKILL_STATUS_READY;
		OnSkillReady();
	}

}

IZombieSkill::IZombieSkill(CBasePlayer *player) : BasePlayerExtra(player), m_iZombieSkillStatus(SKILL_STATUS_READY)
{
	
}

void IZombieSkill::InitHUD()
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	MESSAGE_END();
}

void ZombieSkill_Precache()
{
	PRECACHE_SOUND("zombi/zombi_pressure.wav");
	PRECACHE_SOUND("zombi/zombi_pre_idle_1.wav");
	PRECACHE_SOUND("zombi/zombi_pre_idle_2.wav");
}

CZombieSkill_ZombieCrazy::CZombieSkill_ZombieCrazy(CBasePlayer *player) : IZombieSkill(player)
{
	
}

void CZombieSkill_ZombieCrazy::InitHUD()
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_TANK);
	WRITE_BYTE(ZOMBIE_SKILL_CRAZY);
	MESSAGE_END();
}

void CZombieSkill_ZombieCrazy::Think()
{
	IZombieSkill::Think();

	if (m_iZombieSkillStatus == SKILL_STATUS_USING && gpGlobals->time > m_flTimeZombieSkillEffect)
	{
		OnCrazyEffect();
	}
}

void CZombieSkill_ZombieCrazy::Activate()
{
	if (m_iZombieSkillStatus != SKILL_STATUS_READY)
	{
		switch (m_iZombieSkillStatus)
		{
		case SKILL_STATUS_USING:
		case SKILL_STATUS_FREEZING:
			char buf[16];
			sprintf(buf, "%d", static_cast<int>(m_flTimeZombieSkillNext - gpGlobals->time));
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER,
				"The 'Berserk' skill can't be used because the skill is in cooldown. [Remaining Cooldown Time: %s1 sec.]",
				buf
			); // #CSO_WaitCoolTimeNormal

			break;
		case SKILL_STATUS_USED:
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "The 'Sprint' skill can only be used once per round."); // #CSO_CantSprintUsed
		}

		return;
	}

	if (m_pPlayer->pev->health <= 500.0f)
		return;

	m_iZombieSkillStatus = SKILL_STATUS_USING;
	m_flTimeZombieSkillEnd = gpGlobals->time + ZOMBIECRAZY_DURATION;
	m_flTimeZombieSkillNext = gpGlobals->time + ZOMBIECRAZY_COOLDOWN;
	m_flTimeZombieSkillEffect = gpGlobals->time + 3.0f;

	m_pPlayer->pev->renderfx = kRenderFxGlowShell;
	m_pPlayer->pev->rendercolor = { 255,0,0 };
	m_pPlayer->pev->renderamt = 1;
	m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 105;
	m_pPlayer->pev->health -= 500.0f;
	m_pPlayer->ResetMaxSpeed();

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zombi/zombi_pressure.wav", VOL_NORM, ATTN_NORM);

	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_ACTIVATE);
	WRITE_BYTE(ZOMBIE_SKILL_CRAZY);
	WRITE_SHORT(ZOMBIECRAZY_DURATION);
	WRITE_SHORT(ZOMBIECRAZY_COOLDOWN);
	MESSAGE_END();
}

void CZombieSkill_ZombieCrazy::ResetMaxSpeed()
{
	m_pPlayer->pev->maxspeed = 390;
}

void CZombieSkill_ZombieCrazy::OnSkillEnd()
{
	m_iZombieSkillStatus = SKILL_STATUS_FREEZING;

	m_pPlayer->pev->renderfx = kRenderFxNone;
	m_pPlayer->pev->rendercolor = { 255,255,255 };
	m_pPlayer->pev->renderamt = 16;
	m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 90;
	m_pPlayer->ResetMaxSpeed();
}

void CZombieSkill_ZombieCrazy::OnCrazyEffect()
{
	m_flTimeZombieSkillEffect = gpGlobals->time + 3.0f;

	if (RANDOM_LONG(0, 1))
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zombi/zombi_pre_idle_1.wav", VOL_NORM, ATTN_NORM);
	else
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "zombi/zombi_pre_idle_2.wav", VOL_NORM, ATTN_NORM);
}

float CZombieSkill_ZombieCrazy::GetDamageRatio()
{
	if(m_iZombieSkillStatus == SKILL_STATUS_USING)
		return 1.6f;
	return 1.0f;
}