// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     mappane.h - TMapPane object                       *
// *************************************************************************

#ifndef _MAPPANE_H
#define _MAPPANE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

#ifndef _SECTOR_H
#include "sector.h"
#endif

#ifndef _MULTISURFACE_H
#include "multisurface.h"
#endif

#ifndef _LIGHTDEF_H
#include "lightdef.h"
#endif

// Grid snap
#define GRIDSHIFT	4
#define GRIDSIZE	(1 << GRIDSHIFT)
#define GRIDMASK	(0xFFFFFFFF - (GRIDSIZE - 1))

// Cursor modes
#define MODE_TARGET		0
#define MODE_CLONE		1
#define MODE_MOVE		2

#define NUMMODES		3

// ****************
// * TMapIterator *
// ****************

#define CHECK_NONE		(0)				// Don't bother checking flags, rects, etc.
#define CHECK_SECTRECT	(1 << 0)		// check sector rectangle
#define CHECK_SCRRECT	(1 << 1)		// check screen rectangle for intersection
#define CHECK_RECT		(CHECK_SECTRECT | CHECK_SCRRECT)
#define CHECK_LIGHT		(1 << 2)		// check light rectangle for intersection
#define CHECK_INVIS		(1 << 3)		// check OF_INVISIBLE
#define CHECK_MOVING	(1 << 4)		// check OF_MOVING
#define CHECK_NOINVENT	(1 << 5)		// no inventory recurse
#define CHECK_MAPRECT	(1 << 6)		// check screen rectangle for intersection

// This is the standard local range for an object when iterating through its neighbors
// Using this range prevents object searches from becoming too big when large numbers of
// sectors are loaded up during multiplayer, or MUD game modes since each player/object/etc.
// only looks within its local area of the map for its own stuff.
#define LOCALRANGE 1024

_CLASSDEF(TMapIterator)
class TMapIterator
{
  public:
	TMapIterator(PSRect sr = NULL, int fl = CHECK_NONE, int objset = OBJSET_ALL)
	  { Initialize(sr, fl, objset); }
	  // Calls initialize function
	TMapIterator(RS3DPoint pos, int range = LOCALRANGE, int fl = CHECK_NOINVENT | CHECK_MAPRECT, int objset = OBJSET_ALL);
	  // Initializes iterator with a range value
	TMapIterator(RTObjectInstance oi, int fl = CHECK_NONE, int objset = OBJSET_ALL);
	  // Initializes iterator with a range value
	void Initialize(PSRect sr = NULL, int fl = CHECK_NONE, int objset = OBJSET_ALL);
	  // Initializes iterator
	PTObjectInstance Item() { return item; }
	  // Returns current item.
	PTObjectInstance NextItem();
	  // Advance to the next item, and return it (NULL if end of list)
	BOOL operator ++ (int) { return (NextItem() != NULL); }
	  // Allows moving forwards through array in sequential order
	//BOOL operator -- (int) { return PrevItem(); }
	  // Allows moving backwards through array in sequential order
//	operator BOOL () { return (item != NULL); }
	  // Returns FALSE if at end of item list
	operator PTObjectInstance () { return item; }
	  // Typecast operator for object
	PTObjectInstance operator -> () { return item; }
	  // Allows iterator to be used as pointer
	RTObjectInstance operator * () { return *item; }
	  // Dereferencing operator

	int SectorX() { return sx; }
	int SectorY() { return sy; }
	int SectorIndex() { return index; }
	PTObjectInstance Parent() { return parent; }
	int InventoryIndex() { if (parent) return invindex - 1; return invindex; }

	void Nuke();
		// Delete this object

  protected:
	int objset;					// Object set to iterate through
	int index;					// Object index (in set)
	PTObjectInstance item;		// Current object
	int sx, sy;					// sector x and y
	PTSector sector;			// Current sector
	int numitems;				// Number of items in current sector set
	PTObjectInstance parent;	// parent object, or NULL when in map pane
	int invindex;				// index into parent object's inventory

	int flags;					// flags for which objects are valid
	SRect r;					// screen rectangle for CHECK_RECT and CHECK_LIGHT
};

// ********************************
// * TMapPane - Map gameplay pane *
// ********************************

// The TMapPane object implements the display, scrolling, animation, and game
// control for the game map.  This includes loading and saving sectors, adding,
// removing, moving, and using objects, animating objects, and scrolling.

#define MAXMAPBGRECTS 64

_STRUCTDEF(SBgUpdateRect)
struct SBgUpdateRect
{
	SRect rect;
	int bgdraw;
};

#define CENTERON_OBJ	1
#define CENTERON_POS	2
#define CENTERON_SCROLL	4

_STRUCTDEF(SCenterOnState)
struct SCenterOnState
{
	int flags;
	PTObjectInstance obj;
	S3DPoint pos;
	int level;
};

_CLASSDEF(TMapPane)
class TMapPane : public TPane
{
	friend class TMapIterator;

  // Function Members
  public:

  // Initialize and close the pane
	virtual BOOL Initialize();
		// Initializes pane and prepares to load map resources
	virtual void Close();
		// Releases pane data

  // Virtual handler functions	
	virtual void KeyPress(int key, BOOL down);
	virtual void MouseClick(int button, int x, int y);
	virtual void MouseMove(int button, int x, int y);
	virtual void Update() { TPane::Update(); RedrawAll(); }

	virtual void CreateBackgroundBuffers();
		// Set up main background buffer and map pane surfaces
	virtual void FreeBackgroundBuffers();
		// Delete background buffer and previously allocated video surfaces

	void RedrawAll();	
	  // Redraws entire screen

	void MoveObjScreen(int index, int x, int y);
		// Object drag function for mouse
	void GetMouseMapPos(S3DPoint &p, int zoffset = 50);
	  // Gets the world position (given character's z pos) that mouse is hovering over
	int GetMouseMapAngle();
	  // Gets the angle of the mouse from the character's current position
	void UpdateMouseMovement(int x, int y);
	  // Update moving Player around

  // Map management functions
	void LoadCurMap(char *from = NULL);
	  // Loads the current map in the "curmap" directory from the given directory 
	  // (i.e. "savegame.001"), or clears the "curmap" directory if NULL, forces
	  // reload of all sectors.
	void SaveCurMap(char *to = NULL);
	  // Saves map in curmap to the given game subdirectory 
	  //(i.e. "savegame.001") or "map" if null
	void ClearCurMap();
	  // Deletes all files in the "curmap" directory, and forces sectors to reload.

  // Lighting functions
	void DrawDLight();
	  // Called by DrawBackground() to draw the dynamic light to the display
	void SetDLightIntensity(BYTE newintensity) 
	  { dlight.intensity = dlight.color.red = dlight.color.green = dlight.color.blue = newintensity; }
	  // Sets the dynamic light intensity
	BYTE GetDLightIntensity() { return dlight.intensity; }
	  // Gets the dynamic light intensity
	void SetDLightPos(RS3DPoint newpos) { dlight.pos = newpos; }
	  // Sets light relative position
	RS3DPoint GetDLightPos(RS3DPoint newpos) { return dlight.pos; }
	  // Sets light relative position
	void SetAmbientLight(int light, BOOL stopfade = TRUE);
	  // Sets ambient light intensity.  If fading, sets target to new value unless stopfade
	  // is true, in which case the value is set immediately.
	int GetAmbientLight() { return ambient; }
	  // Gets the ambient light intensity 
	void SetAmbientColor(SColor &color, BOOL stopfade = TRUE);
	  // Sets ambient light color.  If fading, sets target to new value unless stopfade
	  // is true, in which case the value is set immediately.
	SColor &GetAmbientColor() { return ambcolor; }
	  // Gets the ambient light color
	void SetAmbient(int light, SColor &color, BOOL stopfade = TRUE)
	  { SetAmbientLight(light, stopfade); SetAmbientColor(color, stopfade); }
	  // Sets total ambient environment in one shot
	void FadeAmbient(int light, SColor &color, int frames, int steps);
	  // Fades to the given ambient light in 'frames' frames with
	  // 'steps' number of discreet steps.
	void UpdateLights();
	  // Set up baselights

  // Background drawing functions
	virtual void DrawBackground();
	  // TScreen virtual function which handles drawing to background.. root function for all
	  // the crap below...
	void AddBgUpdateRect(SRect &r, int bgdraw = BGDRAW_UNLIT);
	  // Adds a background update rectangle to be added to the update rect queue
	void AddObjectUpdateRect(int index);
	  // User function called to add a background update rectangle for the given object
	
	void BeginUpdateThread();
	  // Called by Initialize function to begin update thread
	void EndUpdateThread();
	  // Called by Close function to end update thread
	int GetUpdateSleep();
	  // Returns the amount of milliseconds per frame to sleep for update thread
	void UpdateTimeSlice(BOOL draw);
	  // Called by the Animate() function to release time to the update thread
	void AdjustTimeSlice(int milliseconds);
	  // Called whenever update has to wait for a queued rect
	void UpdateEdges(int updatex, int updatey);
	  // Called by DrawBackground() to figure out which rectangles to send to the Draw
	  // DrawBackgroundUpdates edges of scrollbuffer
	void QueueUpdateRect(RSRect rect, int bgdraw);
	  // Adds the update rect to the update rect queue... Call BeginUpdate() to start drawing...
	  // (This func will wait for last queue to finish if previous queue is still drawing)
	void PutQueueRectsToDisplay();
	  // Called by UpdateEdges() function to cause rects to draw to screen after they're
	  // rendered by the update thread
	void ClearQueueRects();
	  // Waits till current render is done, then clears out rects in the queue.
	void BeginUpdate();
	  // After all rectangles have been added to queue, will start the queue drawing
	BOOL UpdateDone();
	  // Returns TRUE if the last queued set of update rectangles is done
	void CancelUpdate();
	  // Cancels last queue of update rectangles.
	DWORD WaitUpdate();
	  // Waits for last queue of update rectangles to finish
	void PauseUpdate(char *file, int line);
	  // Call to pause the update thread
	void ResumeUpdate();
	  // Call to resume the update thread
	void FlushUpdate(BOOL updatetimeslice = FALSE);
	  // Call to flush the current update queue, put rects to screen, and update current
	  // valid scroll rect from the queued rect.
	void LockSectors(char *file, int line);
	  // Prevents update thread from accessing sector arrays.  Call when changing sector arrays.
	void UnlockSectors();
	  // Unlocks update system access to sector arrays
	void LockObjects(char *file, int line);
	  // Prevents update thread code from accessing object data. Call when changing object states or imagery
	void UnlockObjects();
	  // Unlock objects

   // Update buffering functions
	void DrawUpdateRect(RSRect dr, int bgdraw);
	  // Draws an update rectangle using the ordinary tile draw system

	static unsigned _stdcall UpdateThread(void *);
	  // Loader thread (calls the loader functions below
	void UpdateLoop();
	  // Main update thread loop
	void GetUpdateObjs(SRect &r);
	  // Go's through sector list and gets all objects to draw (CALLED ONLY BY UPDATE THREAD!!)
	void DrawUnlitObjects(SRect &r);
	  // Draws object to the unlit buffer (before lighting) (CALLED ONLY BY UPDATE THREAD!!)
	void DrawStaticLights(SRect &r);
	  // Draws static lights and copies info to lit buffer (CALLED ONLY BY UPDATE THREAD!!)
	void TransferUnlitToLit(SRect &r);
	  // Does conversion of unlit buffer with lighting to the lit buffer
	void DrawLitObjects(SRect &r);
	  // Draws objects to the lit buffer (CALLED ONLY BY UPDATE THREAD!!)
	void DrawSelectedObjects(SRect &r);
	  // Draws selected objects to the lit buffer (CALLED ONLY BY UPDATE THREAD!!)

	virtual void Pulse();
	  // Called by screen to pulse system (called before DrawBackground())
	void MoveObjects();
	  // Causes all objects to move (based on their moveanle and nextmove members)
	void PulseObjects();
	  // Sends animation pulse to all objects with OF_PULSE set
	virtual void Animate(BOOL draw);
	  // Does animations for all objects with OF_ANIMATE set
	void AnimateObjects(BOOL draw);
	  // Does foreground/2D animations (calls Animate() function for objects on screen)
	void NextFrameObjects();
	  // Advances all animated objects (regardless of whether they are on screen)
	  // to the next frame of thier animation.  This allows offscreen objects to move,
	  // and animations to show the correct passage of time when off screen.
	void AnimateBackground();
	  // Does background animations
	void AnimateSelectedObjects();
	  // Draws selected objects that are moving (usually being dragged)

  // Sector Functions
  	void UpdateSectors();
		// Reload sectors based on pane x, y position
	void SaveAllSectors();
		// Save all sectors to disk without deallocating
	void FreeAllSectors();
		// Save all sectors to disk and then deallocate them
	void ReloadSectors();
		// Free and then reload all sectors

  // World position functions
	void SetMapPos(RS3DPoint newpos);
	void GetMapPos(RS3DPoint retpos) { retpos = center; }
	void SetMapLevel(int newlev) { newlevel = newlev; if (newlev != level) RedrawAll(); }
	int GetMapLevel() { return level; }

  // Center On Functions (causes game map to center on point or object)
	void CenterOnObj(PTObjectInstance obj, BOOL scroll)
		{ centeron.obj = obj; centeron.flags = CENTERON_OBJ | (scroll ? CENTERON_SCROLL:0); }
	  // Center on object
	PTObjectInstance GetCenterOnObj() { return centeron.obj; }
	  // Gets centered object
	void CenterOnPos(RS3DPoint pos, int level, BOOL scroll)
		{ centeron.pos = pos; centeron.level = level; centeron.flags = CENTERON_POS | (scroll ? CENTERON_SCROLL:0); }
	  // Center on a point
	void GetCenterOnPos(RS3DPoint pos, int &level) { pos = centeron.pos; level = centeron.level; }
	  // Gets current center on point
	BOOL IsScrollCenterOn() { return centeron.flags & CENTERON_SCROLL; }
	  // True if we smooth scroll to object or point
	void SaveCenterOnState(PSCenterOnState state)
		{ memcpy(state, &centeron, sizeof(SCenterOnState)); }
	  // Saves the current center on state to structure
	void RestoreCenterOnState(PSCenterOnState state)
		{ memcpy(&centeron, state, sizeof(SCenterOnState)); }
	  // Restores the current center on state from structure

  // Scroll pos
	int PosX() { return posx; }
	int PosY() { return posy; }

  // Add/Remove objects
	int MakeIndex();
	  // Makes a unique id map index for an object
	int NewObject(PSObjectDef def);
	  // Create a new object with the given objdef
	int AddObject(PTObjectInstance oi);
	  // Adds the given object to the map and returns unique id
	void ObjectFlagsChanged(PTObjectInstance oi, DWORD oldflags, DWORD newflags);
	  // Allows map to update lists, etc. when an objects flags change (mainly
	  // for OF_LIGHT, OF_PULSE, and OF_ANIMATE changing objects location on 
	  // OBJSET_xxx sector arrays.
	PTObjectInstance RemoveObject(int index);
	  // Removes the given object from the map, recursing through inventories
	PTObjectInstance RemoveObject(PTObjectInstance inst)
		{ return RemoveObject(inst->GetMapIndex()); }
	  // Removes the given object from the map, recursing through inventories
	void DeleteObject(PTObjectInstance obj);
	  // Removes and deletes an object
	PTObjectInstance RemoveFromSector(PTObjectInstance inst, int sx, int sy, int sectindex);
	  // Removes the given object from the sector array (called by RemoveObject())
	void DeleteSector(PTSector sect);
	  // Removes and deletes a sector (hey.. don't call this)
	void ReloadImagery();
	  // Forces imagery system to reload imagery.
	int AddShadow(PTObjectInstance oi);
	  // Add shadow for object, if there is one

  // Object find functions
  // ---------------------
  // NOTE: Use the 'objset' variable to greatly reduce search times for common sets of
  // objects like MOVING objects and CHARACTERS. 
  
	PTObjectInstance FindObject(char *name, int occurance = 1, int objset = OBJSET_ALL);
		// Returns a pointer to the occurance of object answering to name
	PTObjectInstance FindClosestObject(char *name, S3DPoint pos, BOOL partial, int objset = OBJSET_ALL);
		// Returns a pointer to the object answering to name closest to center
	PTObjectInstance FindClosestObject(char *name, PTObjectInstance from = NULL, BOOL partial = FALSE, int objset = OBJSET_ALL);
		// Returns a pointer to the object answering to name closest to center
	int FindObjectsInRange(S3DPoint pos, int *array, int width, int height = 0, int objclass = -1, int maxnum = MAXFOUNDOBJS, int objset = OBJSET_ALL);
      // Finds objects within given range. If height not given uses width as radius
	PTObjectInstance ObjectInCube(PS3DRect cube, int objset = OBJSET_ALL);
		// Returns pointer to object in cube
	PTObjectInstance OnObject(int screenx, int screeny, PTObjectInstance with = NULL);
		// Returns the index of the object the mouse is on
	PTObjectInstance GetInstance(int index, int objset = OBJSET_ALL);
		// Returns the instance given an object index
	void FindClickPos(int x, int y, RS3DPoint start, RS3DPoint target);
		// Find the position clicked on, taking into account the walkmap heights
	BOOL SwapDrawOrder(PTObjectInstance inst0, PTObjectInstance inst1);
		// Exchange the sector positions of the two objects
	void PushToFront(PTObjectInstance inst);
		// Push object to the top of the sector
	void PushToBack(PTObjectInstance inst);
		// Push object to the bottom of the sector

  // Walk map functions
	void TransferWalkmap(PTObjectInstance inst) { WalkmapHandler(inst, WALK_TRANSFER); }
		// Transfer object's walkmap to the sector walkmap
	BOOL SaveWalkmap(PTObjectInstance oi)
		{ WalkmapHandler(oi, WALK_CAPTURE); oi->SaveHeader(); return TRUE; }
		// Write walkmap for object to disk (editor use only)
	void ClearWalkmap(PTObjectInstance oi) { WalkmapHandler(oi, WALK_CLEAR); }
		// Clear out area of walkmap that object resides in
	void ExtractWalkmap(PTObjectInstance inst) { WalkmapHandler(inst, WALK_EXTRACT); }
		// Remove object's walkmap from the sector walkmap (by redrawing its rect)
	int GetWalkHeight(RS3DPoint pos);
		// Gets walk height for a given walk map
	int GetWalkGridHeight(int x, int y);
		// Gets walk height given walk grid x and y
	int GetWalkHeightArea(RS3DPoint pos, int width = 0, int height = 0);
		// Gets walk height for a given walk map CENTERED on pos with given with and height
		// If width == 0, pos is treated as a point
		// Otherwise pos, width, and height define the box of walk area CENTERED on pos
	void GetWalkHeightRadius(RS3DPoint pos, int radius, 
		int &mindelta, int &maxdelta, int &curheight);
		// Returns the minimum delta (fall), and maximum delta (rise) between any two walk grids 
		// within the radius, and also the height at the current position.
		// The mindelta value detects drops in the heightmap within the radius. (holes, downstairs)
		// The maxdelta value detects rises in the heightmap (walls, columns, obstructions)
		// The 'curheight' value gives the height under 'pos'
		// Note that the 'deltas' only indicate differences in hieght between adjacent walk
		// grids, so 'radius' may be scaled arbirarily large and still work correctly.
		// This routine is somewhat processor intensive, so only use for moving chars and objs.
	BOOL LineOfSight(RS3DPoint pos, RS3DPoint to, PS3DPoint obst = NULL);
		// Returns line of sight flags for line between positions
	void CalculateWalkmap();
		// Call to recalculate the walkmap for the current sector
	void AdjustWalkmap(int deltaz, BOOL absolute = FALSE, BOOL nonzero = FALSE);
		// Move the entire walkmap up or down based on deltaz
	void TransferAllWalkmaps();
		// Re-transfers all the walkmaps (does not clear walkmap first)
	void ClearWalkmaps();
	    // Clears all sector walkmaps
	void RefreshWalkmaps() { ClearWalkmaps(); TransferAllWalkmaps(); }
	    // Refreshes all sector walkmaps (clears, then transfers walkmaps)
	void SnapWalkDisplay(int mapindex);
		// Snap the walk display to the given object (editor only)

  // Object manipulation functions
	void MoveObject(int index, RS3DPoint newpos);
	void SetFlags(int index, WORD newflags);
	void SetState(int index, BYTE newstate);
	void Damage(int intdex, int damage);
	void Throw(int index, RS3DPoint velocity);
	void RepaintObject(int index);

	int MoneyHandler(PTObjectInstance oi, int amount);
		// General purpose money handler, called by the two functions below
	int SubtractMoney(PTObjectInstance oi, int amount);
		// Take away the amount of money specified from oi's inventory
		// If they don't have enough, do nothing and return 0
	int GetTotalMoney(PTObjectInstance oi);
		// Return the total about of money owned by the object

	int CheckPos(PTObjectInstance inst, RS3DPoint newpos, int newlevel = -1);
		// Call before moving inst to newpos - returns index
	int TransferObject(PTObjectInstance inst, int sx, int sy, int newsx, int newsy);
		// Transfer an object to a new sector, returns new itemnum
	BOOL Use(int index, int with);
	int Face(int index, int newfacing);

	BOOL Clicked() { return clicked; }
		// Return whether mouse is clicked down in the mappane or not

  // Modes for editor
	void SetMode(void (*tfunc)(S3DPoint), void (*afunc)(), int mode);
	void ResetDrag() { objx = objy = grabx = graby = onobject = -1; }
	void SetWalkmapReveal(int nx, int ny, int nsx, int nsy)
		{ wmrevealx = nx; wmrevealy = ny; wmrevealsizex = nsx; wmrevealsizey = nsy; }

	DWORD GetNotify() { return notifyflags; }
	void SetNotify( DWORD newflags) { notifyflags |= newflags; }
	void Notify(DWORD notify, void *ptr);

  private:

  // Private functions
	void UpdateMapPos();
		// Called by drawbackground to update the map pane position (Does CenterOn stuff)
		// calls SetMapPos())
	void Update3DScenePos();
		// Called by drawbackground to update the 3d scene position

	void WalkmapHandler(PTObjectInstance inst, int mode);
		// Handles clipping of inst's walkmap to various sectors it may reside in
	void RedrawWalkmapRect(PTObjectInstance oi, int x, int y, int w, int l, PTSector dsect);
		// Redraws the given wm rect - must already be clipped to sector
	void DrawWalkMap(SRect &r);
		// Draws lit walmap crap
	
	void PulseFadeAmbient();
		// Called by Pulse() function to update ambient fade values
  
  // Data Members
	PTSector sectors[SECTORWINDOWX][SECTORWINDOWY];	// Currently loaded sectors
	int oldsectorx, oldsectory;						// Position of sector in last frame
	int sectorx, sectory;				 			// Position of sector in current frame
	int newsectorx, newsectory;			 			// Position of sector in next frame
	int oldlevel, oldposx, oldposy;	   				// Position of screen in last frame
	int level, posx, posy;							// Current position (this frame)
	int newlevel;					 				// New position in map (next frame)
	int oldscrollx, oldscrolly;						// Scroll position of previous frame
	int scrollx, scrolly;							// Scroll position of current frame
	S3DPoint center;								// World coordinates of pane center (current pos)
	SCenterOnState centeron;						// Pane will attempt to scroll to this object or point
	int onobject;									// Object clicked on
	int grabx, graby;								// Click pos
	int objx, objy;									// Offset on the object
	int oldz;										// For z-dragging
	int mx, my;										// Mouse scrolling
	BOOL clicked;									// To allow movement outside of pane
	DWORD notifyflags;								// Notify Objects of changes
	int	lastkey;									// Last virtual keycode generated by mouseclicks

  // Update system members...
	int numbgrects;									// Number of background update rectangles
	SBgUpdateRect bgrects[MAXMAPBGRECTS];			// Background update rectangles
	BOOL isrecqueued;								// True if update system has a rectangle in queue
	SRect queuedrect;								// Current rectangle in queue
	SRect scrollrect;								// Currently valid (drawn) scroll area

	PTMosaicSurface unlitmulti;						// 32 bit unlit buffer
	PTMosaicSurface litmulti;						// 16 bit lit buffer (set to mosaic tile)
	PTMosaicSurface updatemulti;					// Clone of 16 bit surface for updating
	BOOL zbufferiscloned;							// TRUE if zbuffer of unlitmulti is clone of litmulti

	SLightDef dlight;

	void (*targetcallback)(S3DPoint pos);			// Callback target function
	void (*abortcallback)();						// Callback on abort
	BOOL dragmode;									// Drags objects without clicking
	int mode;										// Mode number

	int	lightdist[MAXLIGHTS];						// Array of nearest light distances
	int	lightindex[MAXLIGHTS];						// Array of nearest light indexes

	int wmrevealx, wmrevealy;						// Start of walkmap cutaway
	int wmrevealsizex, wmrevealsizey;				// Size of walkmap cutaway

  // Ambient lighting values	
	int ambient;									// Ambient light value
	SColor ambcolor;								// Ambient light color
	BOOL ambientchanged;							// TRUE when ambient needs to be reset
	int oldambient, newambient;						// Ambient fade old and new values
	SColor oldambcolor, newambcolor;				// Ambient fade old and new color
	int totambfadeframes, ambfadeframes;			// Ambient fade frame values
	int totambfadesteps, ambfadesteps;				// Ambient fade step values
};

inline void PosToScreen(int posx, int posy, int posz, int &screenx, int &screeny)
{
	screenx = posx;
	screeny = (posy >> 1) - posz;
}

inline void ScreenToPos(int screenx, int screeny, int &posx, int &posy, int posz)
{
	posx = screenx;
	posy = (screeny + posz) * 2;
}

// Converts a map pos to a walk grid pos
void PosToWalkGrid(RS3DPoint pos, int &x, int &y);

// Converts a walk grid pos to a map pos
void WalkGridToPos(int x, int y, RS3DPoint pos);

#define PAUSEUPDATE		MapPane.PauseUpdate(__FILE__, __LINE__)
#define RESUMEUPDATE	MapPane.ResumeUpdate()

#define LOCKOBJECTS		MapPane.LockObjects(__FILE__, __LINE__)
#define UNLOCKOBJECTS	MapPane.UnlockObjects()

#define LOCKSECTORS		MapPane.LockSectors(__FILE__, __LINE__)
#define UNLOCKSECTORS	MapPane.UnlockSectors()

#endif

