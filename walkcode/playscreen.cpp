// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              playscreen.cpp - EXILE play screen object                *
// *************************************************************************

#include <windows.h>
#include <stdio.h>

#include "revenant.h"
#include "timer.h"
#include "mainwnd.h"
#include "animation.h"
#include "animimagebody.h"
#include "bitmap.h"
#include "bmsurface.h"
#include "display.h"
#include "playscreen.h"
#include "spell.h"
#include "mappane.h"
#include "automap.h"
#include "inventory.h"
#include "tile.h"
#include "editor.h"
#include "dls.h"
#include "cursor.h"
#include "equip.h"
#include "player.h"
#include "multictrl.h"
#include "spell.h"
#include "statpane.h"
#include "multi.h"
#include "textbar.h"
#include "statusbar.h"
#include "sound.h"
#include "dialog.h"
#include "savegame.h"
#include "character.h"
#include "script.h"
#include "ctrlmap.h"
#include "area.h"

extern TEditToolsPane ToolBar;

PTBitmap bitmap;
PTBitmap PointerCursor;
PTBitmap HandCursor;

int DynamicIntensity = 0;

/* Quick structure for the little bitmaps of the interface which hang over the map pane. */
#define NUMOVERHANGS	17
#define NUMMAINHANGS	13
#define STARTMULTIHANGS	NUMMAINHANGS
#define ENDMULTIHANGS	NUMOVERHANGS
struct { int x, y, w, h; } oh[NUMOVERHANGS] =
{ { 32, 16, 46, 9 }, { 32, 25, 9, 37 }, { 278, 16, 84, 30 },	// top pieces
  { 562, 16, 46, 9 }, { 599, 25, 9, 37 },		// more top pieces
  { 32, 169, 7, 14 }, { 601, 169, 7, 14 },		// middle pieces
  { 32, 294, 3, 9 }, { 32, 318, 26, 18 },		// left bottom
  { 104, 332, 233, 4 },							// middle (text bar)
  { 406, 328, 98, 8 },							// right/middle bottom
  { 582, 318, 26, 18 }, { 605, 294, 3, 9 },		// right bottom

  { 459, 346, 41, 41 }, { 586, 346, 41, 41 },	// multipane top
  { 459, 433, 41, 41 }, { 586, 433, 41, 41 }	// multipane bottom
};
PTBitmap overhang[NUMOVERHANGS];

// **********************
// * Game control codes *
// **********************

#define CTRL_NORMALMODE    1
#define CTRL_COMBATMODE    2
#define CTRL_INVENTORYMODE 4

// {"Name", "INI tag", modeflags, codes(1-3): { keys(1-3): {} }, upcode, downcode} 
SControlEntry GameControls[] = {
{"Invoke 1", "Invoke1", CTRL_NORMALMODE | CTRL_COMBATMODE, {{VK_F1}}, GAMECMD_INVOKE1, 0},
{"Invoke 2", "Invoke2", CTRL_NORMALMODE | CTRL_COMBATMODE, {{VK_F2}}, GAMECMD_INVOKE2, 0},
{"Invoke 3", "Invoke3", CTRL_NORMALMODE | CTRL_COMBATMODE, {{VK_F3}}, GAMECMD_INVOKE3, 0},
{"Invoke 4", "Invoke4", CTRL_NORMALMODE | CTRL_COMBATMODE, {{VK_F4}}, GAMECMD_INVOKE4, 0},
{"Combat Mode", "CombatMode", CTRL_NORMALMODE | CTRL_COMBATMODE, {{VK_RETURN}}, GAMECMD_COMBAT, 0},
{"Full Screen", "FullScreen", ALLMODES, {{VK_SPACE}}, GAMECMD_FULLSCREEN, 0},
{"Sneak", "Sneak", CTRL_NORMALMODE, {{'S'}}, GAMECMD_MOVEDOWN, GAMECMD_MOVEUP, CMDFLAG_SNEAK},
{"Run", "Run", CTRL_NORMALMODE, {{'R'}}, GAMECMD_MOVEDOWN, GAMECMD_MOVEUP, CMDFLAG_RUN},
{"Use", "Use", CTRL_NORMALMODE, {{'U'},{'T'}}, GAMECMD_USE, 0},
{"Get", "Get", CTRL_NORMALMODE, {{'G'}}, GAMECMD_GET, 0},
{"Jump", "Jump", CTRL_NORMALMODE, {{'J'}}, GAMECMD_JUMP, 0},
{"Inventory", "Inventory", CTRL_NORMALMODE, {{'I'}}, GAMECMD_INVENTORY, 0},
{"Inventory Use", "InvUse", CTRL_INVENTORYMODE, {{'U'}}, GAMECMD_INVUSE, 0},
{"Inventory Move", "InvMove", CTRL_INVENTORYMODE, {{'M'}}, GAMECMD_INVMOVE, 0},
{"Inventory Drop", "InvDrop", CTRL_INVENTORYMODE, {{'D'}}, GAMECMD_INVDROP, 0},
{"Inventory Exit", "InvExit", CTRL_INVENTORYMODE, {{VK_ESCAPE}}, GAMECMD_INVEXIT, 0},
{"Combat Combo1", "CombatCombo1", CTRL_COMBATMODE, {{VK_CONTROL, 'A'}}, GAMECMD_COMBO1, 0},
{"Combat Combo2", "CombatCombo2", CTRL_COMBATMODE, {{VK_CONTROL, 'S'}}, GAMECMD_COMBO2, 0},
{"Combat Combo3", "CombatCombo3", CTRL_COMBATMODE, {{VK_CONTROL, 'D'}}, GAMECMD_COMBO3, 0},
{"Combat Combo4", "CombatCombo4", CTRL_COMBATMODE, {{VK_SHIFT, 'A'}}, GAMECMD_COMBO4, 0},
{"Combat Combo5", "CombatCombo5", CTRL_COMBATMODE, {{VK_SHIFT, 'S'}}, GAMECMD_COMBO5, 0},
{"Combat Combo6", "CombatCombo6", CTRL_COMBATMODE, {{VK_SHIFT, 'D'}}, GAMECMD_COMBO6, 0},
{"Combat Block", "CombatBlock", CTRL_COMBATMODE, {{'Q'}}, GAMECMD_BLOCK, 0},
{"Combat Dodge", "CombatDodge", CTRL_COMBATMODE, {{'W'}}, GAMECMD_DODGE, 0},
{"Combat Leap", "CombatLeap", CTRL_COMBATMODE, {{'F'}}, GAMECMD_LEAPDOWN, GAMECMD_LEAPUP, CMDFLAG_LEAP},
{"Combat Swing", "CombatSwing", CTRL_COMBATMODE, {{'A'}}, GAMECMD_SWING, 0},
{"Combat Thrust", "CombatThrust", CTRL_COMBATMODE, {{'S'}}, GAMECMD_THRUST, 0},
{"Combat Chop", "CombatChop", CTRL_COMBATMODE, {{'D'}}, GAMECMD_CHOP, 0},
{"Left", "Left", ALLMODES, {{VK_LEFT},{VK_JOYLEFT}}, GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_LEFT},
{"Right", "Right", ALLMODES, {{VK_RIGHT}, {VK_JOYRIGHT}}, GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_RIGHT},
{"Up", "Up", ALLMODES, {{VK_UP},{VK_JOYUP}}, GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_UP},
{"Down", "Down", ALLMODES, {{VK_DOWN},{VK_JOYDOWN}}, GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_DOWN},
{"Up Left", "UpLeft", ALLMODES, {{VK_HOME}, {VK_JOYUPLEFT}}, GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_UPLEFT},
{"Up Right", "UpRight", ALLMODES, {{VK_PRIOR}, {VK_JOYUPRIGHT}}, GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_UPRIGHT},
{"Down Left", "DownLeft", ALLMODES, {{VK_END}, {VK_JOYDOWNLEFT}}, GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_DOWNLEFT},
{"Down Right", "DownRight", ALLMODES, {{VK_NEXT}, {VK_JOYDOWNRIGHT}}, GAMECMD_DIRDOWN, GAMECMD_DIRUP, CMDFLAG_DOWNRIGHT}
};
#define NUMGAMECONTROLS sizearray(GameControls)

// *********************
// * PlayScreen Screen *
// *********************

TPlayScreen::TPlayScreen()
{
	nextscreen   = NULL;
}

BOOL TPlayScreen::Initialize()
{
	Status("Initializing TPlayScreen\n");

	// Set control/demo modes
	controlon = TRUE;
	demomode = FALSE;
	
	// Don't load a new game right off (set to FALSE when starting)
	loadnewgame = FALSE;  

	// Don't save the map
	savemap = FALSE;

	// Set game time to 0
	gameframes = 0;
	sessionstart = 0;
	lastsessionframes = 0;

	// The main game resource, where most of the interface stuff (cursors, backgrounds, buttons) is stored
	GameData = TMulti::LoadMulti("playscrn.dat");

  // Create background areas for screen
	Status("Creating background areas\n");
	CreateBackgroundAreas();

  // Setup control mapper
	ControlMap.Initialize(NUMGAMECONTROLS, GameControls);
	ControlMap.Load("Controls");

  // Initialize the script manager
	Status("Initializing game script manager\n");
	ScriptManager.Initialize();

  // Load the area def
	Status("Initializing game areas\n");
	AreaManager.Initialize();

  // Map pane
	Status("Setup up game panes in %s mode\n", FullScreen?"FULLSCREEN":"NONFULLSCREEN");
	if (FullScreen)
		MapPane.Resize(0, 0, Display->Width(), Display->Height());
	else
		MapPane.Resize(FRAMEMAPPANEX, FRAMEMAPPANEY, FRAMEMAPPANEWIDTH, FRAMEMAPPANEHEIGHT);
	if (!MapPane.Initialize())
		FatalError("Trouble initializing map pane");
	AddPane(&MapPane);

  // Init the AutoMap pane before MapPane since it allocates buffers used by MapPane
	if (!AutoMap.Initialize())
		FatalError("Trouble initializing automap pane");

  // Character inventory
	if (!Inventory.Initialize())
		FatalError("Trouble initializing inventory pane");

  // Health bar
	if (!HealthBar.Initialize())
		FatalError("Trouble initializing health bar");

	if (!StaminaBar.Initialize())
		FatalError("Trouble initializing stamina bar");

  // Quick spell buttons
	if (!QuickSpells.Initialize())
		FatalError("Trouble initializing quick spell buttons");

  // Text bar
	if (!TextBar.Initialize())
		FatalError("Trouble initializing text bar");

  // Dialog Pane
	if (!DialogPane.Initialize())
		FatalError("Trouble initializing dialog pane");

  // Various multifunction panels
	if (!EquipPane.Initialize())
		FatalError("Trouble initializing equipment pane");

  // Spell creation pane
	if (!SpellPane.Initialize())
		FatalError("Trouble initializing spell pane");

  // Character stat pane
	if (!StatPane.Initialize())
		FatalError("Trouble initializing stat pane");

  // The multicontrol button pane	
	if (!MultiCtrl.Initialize())
		FatalError("Trouble initializing multicontrol pane");

  // Set initial full screen state 
	SetFullScreen(FullScreen);

	// Mouse cursors
	PointerCursor = GameData->Bitmap("cursor");
	if (!PointerCursor)
		return FALSE;

	HandCursor = GameData->Bitmap("handcursor");
	if (!HandCursor)
		return FALSE;

	SetMouseBitmap(PointerCursor);

	// Sound system
	if (!SoundPlayer.Initialize())
		FatalError("Trouble initalizing soundplayer - is SOUND.DEF present?");

  // Load game
	Status("Loading save game\n");
	if (!Editor && !StartInEditor)
		SaveGame.ReadGame();

  // Set up editor
  	if (StartInEditor)
	{
		Status("Initializing editor\n");
		StartEditor(StartInEditor);
	}

	// Setup screen interface
	Status("Loading interface/starting game...\n");
	bitmap = TBitmap::Load(100);
	if (!bitmap)
		return FALSE;

	multidirty = FALSE;

	numpostcharanims = 0;
	numpostchartexts = 0;

	// Hang on to the parts which hang over the map pane
	for (int i = 0; i < NUMOVERHANGS; i++)
	{
		if (!(overhang[i] = TBitmap::NewBitmap(oh[i].w, oh[i].h, BM_8BIT | BM_PALETTE)))
			return FALSE;

		overhang[i]->Put(0, 0, bitmap, oh[i].x, oh[i].y, oh[i].w, oh[i].h);
		if (bitmap->palettesize)
			memcpy(overhang[i]->palette.ptr(), bitmap->palette.ptr(), bitmap->palettesize);
	}

	Display->Put(0, 0, bitmap, DM_NOCLIP | DM_BACKGROUND);
	delete bitmap;

  // Set next pane to null
	nextpane = NULL;

	return TRUE;
}

void TPlayScreen::Close()
{
	if (Editor)
	{
		ShutDownEditor();
		Editor = TRUE;			// other stuff needs to know that we were in the editor
		TObjectClass::SaveClasses(FALSE);
	}

	MapPane.Close();
	Inventory.Close();
	QuickSpells.Close();
	HealthBar.Close();
	StaminaBar.Close();
	TextBar.Close();
	AutoMap.Close();
	EquipPane.Close();
	SpellPane.Close();
	StatPane.Close();
	MultiCtrl.Close();
	SoundPlayer.Close();
	DialogPane.Close();

	for (int i = 0; i < NUMOVERHANGS; i++)
		delete overhang[i];

	SetMouseBitmap(NULL);

  // Close the area manager
	AreaManager.Close();	

  // Close the script manager
	ScriptManager.Close();

  // Close the control mapper
	ControlMap.Close();	

	delete GameData;
}

void TPlayScreen::DrawBackground()
{
	if (nextpane)
	{
		DrawOverhangs();
		nextpane->Initialize();
		nextpane = NULL;
	}

	if (dirty)
	{
		bitmap = TBitmap::Load(100);
		if (!bitmap)
			FatalError("loading interface bitmap");

		Display->Reset();
		Display->Put(0, 0, bitmap, DM_NOCLIP | DM_BACKGROUND);
		delete bitmap;
	}

	TScreen::DrawBackground();

 // Overhaning parts of multipanel if needed
	if (multidirty && !Editor && !IsFullScreen())
	{
		for (int i = STARTMULTIHANGS; i < ENDMULTIHANGS; i++)
			Display->Put(oh[i].x, oh[i].y, overhang[i], DM_TRANSPARENT | DM_BACKGROUND);

		MultiCtrl.RedrawOverhangButtons();
		MultiCtrl.SetClipRect();
		MultiCtrl.DrawBackground();		// ugly but it works

		multidirty = FALSE;
	}
}

void TPlayScreen::Pulse()
{
  // Before doing anything.. see if we should load a new game?
	if (loadnewgame)
	{
		SaveGame.ReadGame();
		loadnewgame = FALSE;
	}
	
   // Should we save the map right now before drawing the screen
	if (savemap)
	{
		MapPane.SaveAllSectors();
		MapPane.SaveCurMap();  // Copies new sector files to main game map dir
		MapPane.ClearCurMap(); // Clears all map sectors from the 'curmap' directory
		savemap = FALSE;
	}

   // Now call base class pulse function
	TScreen::Pulse();

	// Call the area pulse funciton
	AreaManager.Pulse();
}

void TPlayScreen::Animate(BOOL draw)
{
	if (dirty)
		return;

	TScreen::Animate(draw);

	// Call the area animate funciton
	AreaManager.Animate(draw);

	// Draw post-character animations (spell effects, text, etc)
	if (draw && !InCompleteExclusion())
	{
		MapPane.SetClipRect();
		for (int i = 0; i < numpostcharanims; i++)
		{
			if (postanim[i].drawmode & (DM_ZBUFFER | DM_ZSTATIC))
				Display->ZPut(postanim[i].x, postanim[i].y, postanim[i].z, postanim[i].bm, postanim[i].drawmode);
			else
				Display->PutDim(postanim[i].x, postanim[i].y, postanim[i].bm, postanim[i].drawmode, postanim[i].dim);
		}

		// This is a tad tricky - draw the mouse shadow first, then the overhangs, and then
		// (later on, in TScreen::TimerTick()) the actual cursor.  This gets the effect of
		// the mouse shadow being 'in' the map pane (underneath the interface overhangs)
		// but doesn't obscure the cursor itself.

		DrawMouseShadow();

		MapPane.SetClipRect();
		for (i = 0; i < numpostchartexts; i++)
		{
			SColor color;
			color.red = color.green = color.blue = 0;

			Display->WriteText(posttext[i].text, posttext[i].x, posttext[i].y - 1,
									99, DialogFontShadow,
									&color, DM_TRANSPARENT | DM_ALIAS,
									posttext[i].wrapwidth, 0, JUSTIFY_CENTER | JUSTIFY_CLIP);
			Display->WriteText(posttext[i].text, posttext[i].x, posttext[i].y,
									99, DialogFont,
									&posttext[i].color, DM_TRANSPARENT | DM_ALIAS,
									posttext[i].wrapwidth, 0, JUSTIFY_CENTER | JUSTIFY_CLIP);

			delete posttext[i].text;
		}

		DrawOverhangs(TRUE);
	}

	numpostcharanims = 0;
	numpostchartexts = 0;

	// Update the AutoMap
	if (!Editor)
		AutoMap.RecordTravels();

	// Now increase the game frame (don't increase game frames when paused, editor, etc.)
	if (!Editor && 
	  !MapPane.IsHidden() && MapPane.IsOpen() && !InCompleteExclusion())
	{
		gameframes++;
		gametime = 
			(int)((__int64)((__int64)GameFrame() * (__int64)100 / (__int64)FRAMERATE));
		timeofday = 
			(int)((__int64)(((__int64)gametime * (__int64)(24 * 60) / 
				(__int64)Rules.daylength) % (__int64)(24 * 60)));
	}
}

// Sets or clears fullscreen mode
void TPlayScreen::SetFullScreen(BOOL on)
{
  // Init map pane
	if (on)
	{
		MapPane.Resize(0, 0, Display->Width(), Display->Height());

		RemovePane(&Inventory);
		RemovePane(&HealthBar);
		RemovePane(&StaminaBar);
		RemovePane(&QuickSpells);
		RemovePane(&TextBar);
		RemovePane(&DialogPane);
		RemovePane(&EquipPane);
		RemovePane(&SpellPane);
		RemovePane(&StatPane);
		RemovePane(&AutoMap);
		RemovePane(&MultiCtrl);

		Redraw();
	}
	else
	{
		MapPane.Resize(FRAMEMAPPANEX, FRAMEMAPPANEY, FRAMEMAPPANEWIDTH, FRAMEMAPPANEHEIGHT);
	
		AddPane(&Inventory);
		AddPane(&HealthBar);
		AddPane(&StaminaBar);
		AddPane(&QuickSpells);
		AddPane(&TextBar);
		AddPane(&DialogPane);
		AddPane(&EquipPane);
		AddPane(&SpellPane);
		AddPane(&StatPane);
		AddPane(&AutoMap);
		AddPane(&MultiCtrl);


		Redraw();
	}

	fullscreen = FullScreen = on;
}

// Sets game to run in demo mode
void TPlayScreen::SetDemoMode(BOOL on)
{
	demomode = on;
	SetControlOn(!on);
}

// Sets game control on or off
void TPlayScreen::SetControlOn(BOOL on)
{
	controlon = on;
	if (controlon)
		demomode = FALSE;
	MapPane.SetIgnoreInput(!on);
}

void TPlayScreen::KeyPress(int key, BOOL down)
{
	BYTE *screen;

	TScreen::KeyPress(key, down);

	if (down)
	{
		switch (key)
		{
		  case VK_F5:
			if (Editor)
			{
			  SaveGame.WriteGame();
			}
			break;
		  case VK_F8:
			if (CtrlDown)
			{
				FILE *fp;
				fp = fopen("SCREEN.RAW", "wb");
				screen = (BYTE *)(Display->FrontBuffer()->Lock());
				Display->FrontBuffer()->Unlock();
				if (fp && screen)
				{
					WORD pixel;
					for (int y = 0; y < 480; y++)
						for (int x = 0; x < 640; x++)
						{
							pixel = *((WORD *)screen);
							screen += 2;
							if (Display->BitsPerPixel() == 15)
							{
								fputc(((pixel >> 10) & 0x1f) << 3, fp);
								fputc(((pixel >> 5) & 0x1f) << 3, fp);
								fputc((pixel & 0x1f) << 3, fp);
							}
							else
							{
								fputc(((pixel >> 11) & 0x1f) << 3, fp);
								fputc(((pixel >> 5) & 0x3f) << 2, fp);
								fputc((pixel & 0x1f) << 3, fp);
							}
						}

					fclose(fp);
				}
			}
			else
			{
				if (Player)
				{
					((PTCharacter)Player)->RestoreHealth();
					HealthBar.ChangeLevel(Player->Health());
				}
			}

			break;
		  case VK_F12:
			if (Editor)
			{
				ShutDownEditor();
				Redraw();
			}
			else
 				StartEditor();

			break;
		  case 'R':
			if (CtrlDown)
				MapPane.RedrawAll();
			break;
		}
	}

  // Temporary invokes!
	if (down && key >= '0' && key <= '9' && !Editor)
	{
		if(CtrlDown)
			key += 10;

		int i[2];
		i[0] = key - '0';
		i[1] = 0xff;
		Player->Invoke(i, 2);
	}

  // Gameplay Commands
	if (!Editor)
	{
		DWORD mode;
		if (Player && Player->IsFighting())
			mode = CTRL_COMBATMODE;
		else
			mode = CTRL_NORMALMODE;
		GAMECOMMAND cmd = (GAMECOMMAND)ControlMap.GetCommand(key, down, mode);
		Command(cmd);
	}
}

void TPlayScreen::Joystick(int key, BOOL down)
{
	TScreen::Joystick(key, down);

  // Gameplay Commands
	if (!Editor)
	{
		DWORD mode;
		if (Player && Player->IsFighting())
			mode = CTRL_COMBATMODE;
		else
			mode = CTRL_NORMALMODE;
		GAMECOMMAND cmd = (GAMECOMMAND)ControlMap.GetCommand(key, down, mode);
		Command(cmd);
	}
}

void TPlayScreen::MouseClick(int button, int x, int y)
{
  // Stop self running demo
	if (Player && Player->GetScript() &&  
		Player->GetScript()->Running() &&
		Player->GetScript()->GetTrigger() == TRIGGER_ALWAYS)
	{
		Player->GetScript()->End();
		if (IsDemoMode())  // If in the middle of uninterruptible part, restart game
		{
			SetDemoMode(FALSE);
			SaveGame.ReadGame();
			return;
		}
	}

	if (button == MB_LEFTDOWN || button == MB_RIGHTDOWN || button == MB_MIDDLEDOWN)
		TextBar.Clear();

	TScreen::MouseClick(button, x, y);
}

void TPlayScreen::Command(GAMECOMMAND command)
{
	int angle = -1;
	DWORD state, changed;

  // Get command flags
	ControlMap.GetCommandFlags(state, changed);

  // Stop self running demo
	if (Player && Player->GetScript() &&  
		Player->GetScript()->Running() &&
		Player->GetScript()->GetTrigger() == TRIGGER_ALWAYS)
	{
		Player->GetScript()->End();
		if (IsDemoMode())  // If in the middle of uninterruptible part, restart game
		{
			SetDemoMode(FALSE);
			SaveGame.ReadGame();
		}
	}

	if (MapPane.IsIgnoringInput())
		return;

	switch (command)
	{
	  case GAMECMD_COMBAT:
	  {
		if (Player)
		{
			if (Player->IsFighting())
				Player->EndCombat();
			else
				Player->BeginCombat();
		}
		break;
	  }
	  case GAMECMD_FULLSCREEN:
	  {
		if (IsFullScreen())
			SetFullScreen(FALSE);
		else
			SetFullScreen(TRUE);
		break;
	  }
	  case GAMECMD_USE:
	  {
		if (Player)
			Player->TryUse();
	    break;	
	  }
	  case GAMECMD_GET:
	  {
		if (Player)
			Player->TryGet();
	    break;	
 	  }
	  case GAMECMD_JUMP:
	  {
		if (Player)
			Player->Jump();
	    break;	
	  }
	  case GAMECMD_INVENTORY:
	  {
	    break;	
	  }
	  case GAMECMD_INVOKE1:
	  case GAMECMD_INVOKE2:
	  case GAMECMD_INVOKE3:
	  case GAMECMD_INVOKE4:
 	  {
		QuickSpells.Invoke(command - GAMECMD_INVOKE1 + 1);
	    break;	
	  }
	  case GAMECMD_INVUSE:
	  {
	    break;	
	  }
	  case GAMECMD_INVMOVE:
	  {
	    break;	
	  }
	  case GAMECMD_INVDROP:
	  {
	    break;	
	  }
	  case GAMECMD_INVEXIT:
	  {
	    break;	
	  }
	  case GAMECMD_SWING:
	  {
		if (Player)
			Player->Swing();
	    break;	
	  }
	  case GAMECMD_THRUST:
	  {
		if (Player)
			Player->Thrust();
	    break;	
	  }
	  case GAMECMD_CHOP:
	  {
		if (Player)
			Player->Chop();
	    break;	
	  }
	  case GAMECMD_COMBO1:
	  case GAMECMD_COMBO2:
	  case GAMECMD_COMBO3:
	  case GAMECMD_COMBO4:
	  case GAMECMD_COMBO5:
	  case GAMECMD_COMBO6:
	  {
		if (Player)
			Player->Combo(command - GAMECMD_COMBO1 + 1);
	    break;	
	  }
	  case GAMECMD_BLOCK:
	  {
		if (Player)
			Player->Block();
	    break;
	  }
	  case GAMECMD_DODGE:
	  {
		if (Player)
			Player->Dodge();
	    break;
	  }
	  case GAMECMD_DIRDOWN:
	  case GAMECMD_DIRUP:
	  {
		// Direction keys are state flags 0-7, so we can check by doing this little loop below
		int angle = 0;		// Check each key, and set angle if down
		int c;

		// Check changed	
		for (c = 1; c < (1 << 8) && !(c & (state & changed)); c <<= 1, angle += 32);
		
		// No changed key down, check rest of keys
		if (angle > 255)
		{
			angle = 0;
			for (c = 1; c < (1 << 8) && !(c & state); c <<= 1, angle += 32);

			if (angle > 255)
				angle = -1; // No direction keys down
		}

		// Now move char
		if (Player)
		{	
			if (angle >= 0)
			{
				if (Player->IsFighting() && (state & CMDFLAG_LEAP))
					Player->Leap(angle);
				else
					Player->Go(angle);
			}
			else
				Player->Stop();
		}

		break;
	  }
	  case GAMECMD_MOVEDOWN:
	  case GAMECMD_MOVEUP:
	  {
		if (Player && !Player->IsFighting())
		{
			if (state & CMDFLAG_RUN)
				Player->SetRunMode();
			else if (state & CMDFLAG_SNEAK)
				Player->SetSneakMode();
			else
				Player->SetWalkMode();
		}
	    break;
	  }
	  case GAMECMD_LEAPDOWN:
	  case GAMECMD_LEAPUP:
	  {
	    break; // Handle using 'state' below
	  }  
    } // End of switch  	

}

void TPlayScreen::CreateBackgroundAreas()
{
	if (!IsFullScreen()) // Create border background areas
	{
		Display->CreateBackgroundArea(0, 0, Display->Width(), FRAMEMAPPANEY);
			Display->CreateBackgroundArea(0, FRAMEMAPPANEY, FRAMEMAPPANEX, FRAMEMAPPANEHEIGHT);
		Display->CreateBackgroundArea(FRAMEMAPPANEX+FRAMEMAPPANEWIDTH, FRAMEMAPPANEY,
						Display->Width() - (FRAMEMAPPANEX+FRAMEMAPPANEWIDTH), FRAMEMAPPANEHEIGHT);
		Display->CreateBackgroundArea(0, FRAMEMAPPANEY+FRAMEMAPPANEHEIGHT, Display->Width(),
						Display->Height() - FRAMEMAPPANEY - FRAMEMAPPANEHEIGHT);
	}
}

void TPlayScreen::DrawOverhangs(BOOL temporary)
{
	if (IsFullScreen())
		return;

	Display->Reset();

	// Overhanging parts of interface screen to background
	for (int i = 0; i < NUMMAINHANGS; i++)
		Display->Put(oh[i].x, oh[i].y, overhang[i], DM_TRANSPARENT | (temporary ? 0 : DM_BACKGROUND));
}

void TPlayScreen::HideLowerPanes()
{
	Inventory.Hide();
	TextBar.Hide();
	QuickSpells.Hide();
	MultiCtrl.SetIgnoreInput(TRUE);
}

void TPlayScreen::ShowLowerPanes()
{
	Inventory.Show();
	TextBar.Show();
	QuickSpells.Show();
	MultiCtrl.SetIgnoreInput(FALSE);
}

void TPlayScreen::NewGame()
{
	loadnewgame = TRUE;
}

void TPlayScreen::SaveMap()
{
	savemap = TRUE;
}

// ------------------ Game Time Stuff --------------------

// Returns the number of frames elapsed since the beginning of the current game.
// This value is saved in the save game file, and restored whenever a game is loaded.
int TPlayScreen::GameFrame()
{
	return gameframes - sessionstart + lastsessionframes;
}

// This is the time in milliseconds since the beginning of the current game.  This
// gets saved and restored in the save game file as well.  This time is calculated 
// not by the system clock, but by dividing the GameFrames() value by the number 
// of frames per second.
int TPlayScreen::GameTime()
{
  // Note: use 64 bit ints so calculations don't wrap around	
	return gametime;
}

// Set the game time.  This is called from the save game/new game sytem.
void TPlayScreen::SetGameTime(int gametime)
{
	sessionstart = gameframes;
	lastsessionframes = (int)((__int64)((__int64)gametime * (__int64)FRAMERATE / (__int64)100));
	gametime = 
		(int)((__int64)((__int64)GameFrame() * (__int64)100 / (__int64)FRAMERATE));
	timeofday = 
		(int)((__int64)(((__int64)gametime * (__int64)(24 * 60) / 
			(__int64)Rules.daylength) % (__int64)(24 * 60)));
}

// Returns the time of day in minutes
int TPlayScreen::TimeOfDay()
{
	return timeofday;
}

// Returns the amount of ambient light 0-255 for the day
int TPlayScreen::Daylight()
{
	int time = TimeOfDay();
	int step;
	int morningbeg = (6 * 60) - (Rules.twilight >> 1);
	int morningend = morningbeg + Rules.twilight - 1;
	int eveningbeg = (18 * 60) - (Rules.twilight >> 1);
	int eveningend = eveningbeg + Rules.twilight - 1;

	if (time > eveningend || time < morningbeg)
		return 0;		// Night
	else if (time > morningend && time < eveningbeg)
		return 255;		// Day
	else if (time >= morningbeg && time <= morningend)
	{
		step = (time - morningbeg) * Rules.twilightsteps / Rules.twilight;
		return step * 255 / Rules.twilightsteps;
	}
	else
	{
		step = (Rules.twilight - (time - eveningbeg)) * Rules.twilightsteps / Rules.twilight;
		return step * 255 / Rules.twilightsteps;
	}
}

// Returns a DAY_MIDNIGHT, DAY_MORNING, DAY_DAYTIME, DAY_NOON, DAY_EVENING, DAY_NIGHT, flags
int TPlayScreen::DayTimeFlag()
{
	int time = TimeOfDay();
	int morningbeg = (6 * 60) - (Rules.twilight >> 1);
	int morningend = morningbeg + Rules.twilight - 1;
	int eveningbeg = (18 * 60) - (Rules.twilight >> 1);
	int eveningend = eveningbeg + Rules.twilight - 1;

	if (time == 0)
		return DAY_MIDNIGHT;
	else if (time == 12 * 60)
		return DAY_NOON;
	if (time > eveningend || time < morningbeg)
		return DAY_NIGHT;
	else if (time > morningend && time < eveningbeg)
		return DAY_DAYTIME;	
	else if (time >= morningbeg && time <= morningend)
		return DAY_MORNING;
	else
		return DAY_EVENING;
}

// Conversion function to convert game minutes (not real minutes) to frames
int ConvertFramesToMinutes(int frames)
{
	return frames * (24 * 60) / Rules.daylength; 
}

// Conversion function to convert frames to game minutes (not real minutes)
int ConvertMinutesToFrames(int time)
{
	return time * Rules.daylength / (24 * 60);
}
