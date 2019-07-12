// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      weapon.h - TWeapon module                        *
// *************************************************************************

#ifndef _WEAPON_H
#define _WEAPON_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

// Weapon type defines
#define WT_HAND		    0			// Hand, claw, tail, etc.	
#define WT_SHORTBLADE	1			// Daggers, knives
#define WT_LONGBLADE	2			// Swords
#define WT_BLUDGEON		3			// Clubs, maces, hammers
#define WT_AXE			4			// Axes
#define WT_BOW			5			// Bows, crossbows
#define WT_LAST			5			// Last weapon type

// Weapon mask defines (Note: make sure these don't match Windows WM_ messages)
#define WM_HAND			0x0000
#define WM_SHORTBLADE	0x0001
#define WM_LONGBLADE	0x0002
#define WM_BLUDGEON		0x0004
#define WM_AXE			0x0008
#define WM_BOW			0x0010

#define MAX_WEAPONTYPES	6

_CLASSDEF(TWeapon)
class TWeapon : public TObjectInstance
{
  public:
	TWeapon(PTObjectImagery newim) : TObjectInstance(newim) { ClearWeapon(); }
	TWeapon(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { ClearWeapon(); }

	void ClearWeapon();

	virtual BOOL Use(PTObjectInstance user, int with = -1);
	virtual int CursorType(PTObjectInstance with = NULL);

	virtual void Load(RTInputStream is, int version, int objversion);
	virtual void Save(RTOutputStream os);

  // Weapon stats
	STATFUNC(EqSlot)
	STATFUNC(Type)
	STATFUNC(Damage)
	STATFUNC(Combining)
	STATFUNC(Poison)
	STATFUNC(Value)

  protected:
	int poison;				// how much posion is currently on the blade
};

DEFINE_BUILDER("WEAPON", TWeapon)

#endif