#pragma once

enum mp_break_t {
	MULTIPLAYER_BREAK_DEFAULT,
	MULTIPLAYER_BREAK_SERVERSIDE,
	MULTIPLAYER_BREAK_CLIENTSIDE,
	MULTIPLAYER_BREAK_BOTH
};

enum propdata_interactions_t {
	PROPINTER_PHYSGUN_WORLD_STICK,		// "onworldimpact"	"stick"
	PROPINTER_PHYSGUN_FIRST_BREAK,		// "onfirstimpact"	"break"
	PROPINTER_PHYSGUN_FIRST_PAINT,		// "onfirstimpact"	"paintsplat"
	PROPINTER_PHYSGUN_FIRST_IMPALE,		// "onfirstimpact"	"impale"
	PROPINTER_PHYSGUN_LAUNCH_SPIN_NONE,	// "onlaunch"		"spin_none"
	PROPINTER_PHYSGUN_LAUNCH_SPIN_Z,	// "onlaunch"		"spin_zaxis"
	PROPINTER_PHYSGUN_BREAK_EXPLODE,	// "onbreak"		"explode_fire"
	PROPINTER_PHYSGUN_DAMAGE_NONE,		// "damage"			"none"
	PROPINTER_FIRE_FLAMMABLE,			// "flammable"			"yes"
	PROPINTER_FIRE_EXPLOSIVE_RESIST,	// "explosive_resist"	"yes"
	PROPINTER_FIRE_IGNITE_HALFHEALTH,	// "ignite"				"halfhealth"
	PROPINTER_PHYSGUN_CREATE_FLARE,		// "onpickup"		"create_flare"
	PROPINTER_PHYSGUN_ALLOW_OVERHEAD,	// "allow_overhead"	"yes"
	PROPINTER_WORLD_BLOODSPLAT,			// "onworldimpact", "bloodsplat"
	PROPINTER_PHYSGUN_NOTIFY_CHILDREN,	// "onfirstimpact" cause attached flechettes to explode
	PROPINTER_NUM_INTERACTIONS,			// If we get more than 32 of these, we'll need a different system
};

class IMultiplayerPhysics {
public:
	virtual int GetMultiplayerPhysicsMode(void) = 0;
};

class IBreakableWithPropData {
public:
	// Damage modifiers
	virtual void            SetDmgModBullet(float flDmgMod) = 0;
	virtual void            SetDmgModClub(float flDmgMod) = 0;
	virtual void            SetDmgModExplosive(float flDmgMod) = 0;
	virtual float           GetDmgModBullet(void) = 0;
	virtual float           GetDmgModClub(void) = 0;
	virtual float           GetDmgModExplosive(void) = 0;

	// Explosive
	virtual void            SetExplosiveRadius(float flRadius) = 0;
	virtual void            SetExplosiveDamage(float flDamage) = 0;
	virtual float           GetExplosiveRadius(void) = 0;
	virtual float           GetExplosiveDamage(void) = 0;

	// Physics damage tables
	virtual void            SetPhysicsDamageTable(char* iszTableName) = 0;
	virtual char*           GetPhysicsDamageTable(void) = 0;

	// Breakable chunks
	virtual void            SetBreakableModel(char* iszModel) = 0;
	virtual char*           GetBreakableModel(void) = 0;
	virtual void            SetBreakableSkin(int iSkin) = 0;
	virtual int             GetBreakableSkin(void) = 0;
	virtual void            SetBreakableCount(int iCount) = 0;
	virtual int             GetBreakableCount(void) = 0;
	virtual void            SetMaxBreakableSize(int iSize) = 0;
	virtual int             GetMaxBreakableSize(void) = 0;

	// LOS blocking
	virtual void            SetPropDataBlocksLOS(bool bBlocksLOS) = 0;
	virtual void            SetPropDataIsAIWalkable(bool bBlocksLOS) = 0;

	// Interactions
	virtual void            SetInteraction(propdata_interactions_t Interaction) = 0;
	virtual bool            HasInteraction(propdata_interactions_t Interaction) = 0;

	// Multi player physics mode
	virtual void            SetPhysicsMode(int iMode) = 0;
	virtual int             GetPhysicsMode() = 0;

	// Multi player breakable spawn behavior
	virtual void            SetMultiplayerBreakMode(mp_break_t mode) = 0;
	virtual mp_break_t      GetMultiplayerBreakMode(void) const = 0;

	// Used for debugging
	virtual void            SetBasePropData(char* iszBase) = 0;
	virtual char*           GetBasePropData(void) = 0;
};

struct surfacephysicsparams_t
{
	// vphysics physical properties
	float			friction;
	float			elasticity;				// collision elasticity - used to compute coefficient of restitution
	float			density;				// physical density (in kg / m^3)
	float			thickness;				// material thickness if not solid (sheet materials) in inches
	float			dampening;
};

struct surfaceaudioparams_t // +16
{
	float    reflectivity;             // like elasticity, but how much sound should be reflected by this surface
	float    hardnessFactor;           // like elasticity, but only affects impact sound choices
	float    roughnessFactor;          // like friction, but only affects scrape sound choices   
	float    roughThreshold;           // surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
	float    hardThreshold;            // surface hardness > this causes "hard" impacts, < this causes "soft" impacts
	float    hardVelocityThreshold;    // collision velocity > this causes "hard" impacts, < this causes "soft" impacts   
	float    highPitchOcclusion;       //a value betweeen 0 and 100 where 0 is not occluded at all and 100 is silent (except for any additional reflected sound)
	float    midPitchOcclusion;
	float    lowPitchOcclusion;
};

struct surfacesoundnames_t // +36
{
	unsigned short    walkStepLeft;
	unsigned short    walkStepRight;
	unsigned short	  runStepLeft;
	unsigned short	  runStepRight;
	unsigned short    impactSoft;
	unsigned short    impactHard;
	unsigned short    scrapeSmooth;
	unsigned short    scrapeRough;
	unsigned short    bulletImpact;
	unsigned short    rolling;
	unsigned short    breakSound;
	unsigned short    strainSound;
};

struct surfacegameprops_t // +56
{
	//// game movement data
	//float			maxSpeedFactor;			// Modulates player max speed when walking on this surface
	//float			jumpFactor;				// Ini jdicates how much higher the player should jump when on the surface
	//float			unk1;
	//float			flPenetrationModifier;
	//float			flDamageModifier;
	//// Game-specific data
	//unsigned short	material; // +84
	//						  // Indicates whether or not the player is on a ladder.
	//unsigned char	climbable;

	float maxSpeedFactor;
	float jumpFactor;
	float flPenetrationModifier;
	float flDamageModifier;
	uint16_t material;
	uint8_t climbable;

	unsigned char	pad;
};
//-----------------------------------------------------------------------------
// Purpose: Each different material has an entry like this
//-----------------------------------------------------------------------------
struct surfacedata_t
{
	surfacephysicsparams_t	physics;	// physics parameters
	surfaceaudioparams_t	audio;		// audio parameters
	surfacesoundnames_t		sounds;		// names of linked sounds
	surfacegameprops_t		game;		// Game data / properties
};

class IPhysicsSurfaceProps
{
public:
	virtual ~IPhysicsSurfaceProps(void) {}

	// parses a text file containing surface prop keys
	virtual int		ParseSurfaceData(const char *pFilename, const char *pTextfile) = 0;
	// current number of entries in the database
	virtual int		SurfacePropCount(void) const = 0;

	virtual int		GetSurfaceIndex(const char *pSurfacePropName) const = 0;
	virtual void	GetPhysicsProperties(int surfaceDataIndex, float *density, float *thickness, float *friction, float *elasticity) const = 0;

	virtual surfacedata_t	*GetSurfaceData(int surfaceDataIndex) = 0;
	virtual const char		*GetString(unsigned short stringTableIndex) const = 0;


	virtual const char		*GetPropName(int surfaceDataIndex) const = 0;

	// sets the global index table for world materials
	// UNDONE: Make this per-CPhysCollide
	virtual void	SetWorldMaterialIndexTable(int *pMapArray, int mapSize) = 0;

	// NOTE: Same as GetPhysicsProperties, but maybe more convenient
	virtual void	GetPhysicsParameters(int surfaceDataIndex, surfacephysicsparams_t *pParamsOut) const = 0;
};
