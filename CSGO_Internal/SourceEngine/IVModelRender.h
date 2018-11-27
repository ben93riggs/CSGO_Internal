//#include "SourceEngine/Definitions.hpp"
//#include "CSGOStructs.hpp"

	struct mstudioanimdesc_t;
	struct mstudiobodyparts_t;
	struct mstudiotexture_t;

	struct model_t
	{
		char name[255];
	};

	class Quaternion;

	class RadianEuler
	{
	public:
		inline RadianEuler(void) { }
		inline RadianEuler(float X, float Y, float Z) { x = X; y = Y; z = Z; }
		inline RadianEuler(Quaternion const &q);	// evil auto type promotion!!!
		inline RadianEuler(QAngle const &angles);	// evil auto type promotion!!!

													// Initialization
		inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f) { x = ix; y = iy; z = iz; }

		//	conversion to qangle
		QAngle ToQAngle(void) const;
		bool IsValid() const;
		void Invalidate();

		inline float *Base() { return &x; }
		inline const float *Base() const { return &x; }

		// array access...
		float operator[](int i) const;
		float& operator[](int i);

		float x, y, z;
	};

	class Quaternion				// same data-layout as engine's vec4_t,
	{								//		which is a float[4]
	public:
		inline Quaternion(void) {}
		inline Quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) { }
		inline Quaternion(RadianEuler const &angle);	// evil auto type promotion!!!

		inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f) { x = ix; y = iy; z = iz; w = iw; }

		bool IsValid() const;
		void Invalidate();

		bool operator==(const Quaternion &src) const;
		bool operator!=(const Quaternion &src) const;

		float* Base() { return (float*)this; }
		const float* Base() const { return (float*)this; }

		// array access...
		float operator[](int i) const;
		float& operator[](int i);

		float x, y, z, w;
	};

	struct ModelRenderInfo_t
	{
		Vector origin;
		QAngle angles;
		char unknown[0x4];
		IClientRenderable *pRenderable;
		const model_t *pModel;
		const matrix3x4_t *pModelToWorld;
		const matrix3x4_t *pLightingOffset;
		const Vector *pLightingOrigin;
		int flags;
		int entity_index;
		int skin;
		int body;
		int hitboxset;
		ModelInstanceHandle_t instance;
		ModelRenderInfo_t()
		{
			pModelToWorld = nullptr;
			pLightingOffset = nullptr;
			pLightingOrigin = nullptr;
		}
	};

	struct mstudiobbox_t
	{
		int		bone;
		int		group; // intersection group
		Vector	bbmin; // bounding box 
		Vector	bbmax;
		int		hitboxnameindex; // offset to the name of the hitbox.
		int		pad[3];
		float	radius;
		int		pad2[4];

		char* getHitboxName()
		{
			if (hitboxnameindex == 0)
				return "";

			return ((char*)this) + hitboxnameindex;
		}
	};

	struct mstudiohitboxset_t
	{
		int sznameindex;
		char* const pszName(void) const { return ((char*)this) + sznameindex; }
		int numhitboxes;
		int hitboxindex;
		mstudiobbox_t* pHitbox(int i) const { return (mstudiobbox_t*)(((BYTE*)this) + hitboxindex) + i; };
	};

	struct mstudiobone_t
	{
		int					sznameindex;
		inline char * const pszName(void) const { return ((char *)this) + sznameindex; }
		int		 			parent;		// parent bone
		int					bonecontroller[6];	// bone controller index, -1 == none

												// default values
		Vector				pos;
		Quaternion			quat;
		RadianEuler			rot;
		// compression scale
		Vector				posscale;
		Vector				rotscale;

		matrix3x4_t			poseToBone;
		Quaternion			qAlignment;
		int					flags;
		int					proctype;
		int					procindex;		// procedural rule
		mutable int			physicsbone;	// index into physically simulated bone
		inline void *pProcedure() const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
		int					surfacepropidx;	// index into string tablefor property name
		inline char * const pszSurfaceProp(void) const { return ((char *)this) + surfacepropidx; }
		inline int			GetSurfaceProp(void) const { return surfacepropLookup; }

		int					contents;		// See BSPFlags.h for the contents flags
		int					surfacepropLookup;	// this index must be cached by the loader, not saved in the file
		int					unused[7];		// remove as appropriate
	};

	struct mstudiobonecontroller_t
	{
		int					bone;	// -1 == 0
		int					type;	// X, Y, Z, XR, YR, ZR, M
		float				start;
		float				end;
		int					rest;	// byte index value at rest
		int					inputfield;	// 0-3 user set controller, 4 mouth
		int					unused[8];
	};

	struct mstudioseqdesc_t
	{
		int	szlabelindex;
		inline char * const pszLabel(void) const { return ((char *)this) + szlabelindex; }

		int	szactivitynameindex;
		inline char * const pszActivityName(void) const { return ((char *)this) + szactivitynameindex; }

		int	flags;		// looping/non-looping flags

		int	activity;	// initialized at loadtime to game DLL values
		int	actweight;

		int	numevents;
		int	eventindex;
		inline void *pEvent(int i) const { return (((byte *)this) + eventindex) + i; };

		Vector	bbmin;		// per sequence bounding box
		Vector	bbmax;

		//-------------------------------------------------------------------------
		// Purpose: returns a model animation from the sequence group size and
		//          blend index
		// Note: this replaces GetAnimValue() that was previously in bone_setup
		// Note: this also acts as a SetAnimValue() as it returns a reference to
		//       the anim value in question
		//-------------------------------------------------------------------------
		inline unsigned short& pAnimValue(int nIndex0, int nIndex1) const
		{
			// Clamp indexes
			if (nIndex0 >= groupsize[0])
				nIndex0 = groupsize[0] - 1;

			if (nIndex1 >= groupsize[1])
				nIndex1 = groupsize[1] - 1;

			return *pBlend(nIndex1 * groupsize[0] + nIndex0);
		}

		int	numblends;

		int blendindex;
		inline unsigned short *pBlend(int i) const { return (unsigned short *)(((byte *)this) + blendindex) + i; };

		int seqgroup; // sequence group for demand loading

		int	groupsize[2];
		int	paramindex[2];	// X, Y, Z, XR, YR, ZR
		float	paramstart[2];	// local (0..1) starting value
		float	paramend[2];	// local (0..1) ending value
		int	paramparent;

		float	fadeintime;	// ideal cross fate in time (0.2 default)
		float	fadeouttime;	// ideal cross fade out time (0.2 default)

		int	entrynode;	// transition node at entry
		int	exitnode;	// transition node at exit
		int	nodeflags;	// transition rules

		float	entryphase;	// used to match entry gait
		float	exitphase;	// used to match exit gait

		float	lastframe;	// frame that should generation EndOfSequence

		int	nextseq;	// auto advancing sequences
		int	pose;		// index of delta animation between end and nextseq

		int	numikrules;

		int	numautolayers;
		int	autolayerindex;
		inline void *pAutolayer(int i) const { return (((byte *)this) + autolayerindex) + i; };

		int	weightlistindex;
		float	*pBoneweight(int i) const { return ((float *)(((byte *)this) + weightlistindex) + i); };
		float	weight(int i) const { return *(pBoneweight(i)); };

		int	posekeyindex;
		float				*pPoseKey(int iParam, int iAnim) const { return (float *)(((byte *)this) + posekeyindex) + iParam * groupsize[0] + iAnim; }
		float				poseKey(int iParam, int iAnim) const { return *(pPoseKey(iParam, iAnim)); }

		int	numiklocks;
		int	iklockindex;
		inline void *pIKLock(int i) const { return (((byte *)this) + iklockindex) + i; };

		// Key values
		int	keyvalueindex;
		int	keyvaluesize;
		inline const char * KeyValueText(void) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

		int	unused[3];		// remove/add as appropriate
	};

	struct studiohdr_t
	{
		int					id;
		int					version;

		long				checksum;		// this has to be the same in the phy and vtx files to load!

		inline const char *	pszName(void) const { return name; }
		char				name[64];

		int					length;

		Vector				eyeposition;	// ideal eye position

		Vector				illumposition;	// illumination center

		Vector				hull_min;		// ideal movement hull size
		Vector				hull_max;

		Vector				view_bbmin;		// clipping bounding box
		Vector				view_bbmax;

		int					flags;

		int					numbones;			// bones
		int					boneindex;
		inline mstudiobone_t *pBone(int i) const { Assert(i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };
		int					RemapSeqBone(int iSequence, int iLocalBone) const;	// maps local sequence bone to global bone
		int					RemapAnimBone(int iAnim, int iLocalBone) const;		// maps local animations bone to global bone

		int					numbonecontrollers;		// bone controllers
		int					bonecontrollerindex;
		inline mstudiobonecontroller_t *pBonecontroller(int i) const { Assert(i >= 0 && i < numbonecontrollers); return (mstudiobonecontroller_t *)(((byte *)this) + bonecontrollerindex) + i; };

		int					numhitboxsets;
		int					hitboxsetindex;

		// Look up hitbox set by index
		mstudiohitboxset_t	*GetHitboxSet(int i) const
		{
			Assert(i >= 0 && i < numhitboxsets);
			return (mstudiohitboxset_t*)(((byte*)this) + hitboxsetindex) + i;
		};

		// Calls through to hitbox to determine size of specified set
		inline mstudiobbox_t *pHitbox(int i, int set) const
		{
			mstudiohitboxset_t const *s = GetHitboxSet(set);
			if (!s)
				return NULL;

			return s->pHitbox(i);
		};

		// Calls through to set to get hitbox count for set
		inline int			iHitboxCount(int set) const
		{
			mstudiohitboxset_t const *s = GetHitboxSet(set);
			if (!s)
				return 0;

			return s->numhitboxes;
		};

		// file local animations? and sequences
		//private:
		int					numlocalanim;			// animations/poses
		int					localanimindex;		// animation descriptions
		inline mstudioanimdesc_t *pLocalAnimdesc(int i) const { return NULL; };

		int					numlocalseq;				// sequences
		int					localseqindex;
		inline mstudioseqdesc_t *pLocalSeqdesc(int i) const { return NULL; };

		//public:
		bool				SequencesAvailable() const
		{
			return true;
		}

		int					GetNumSeq() const;
		mstudioanimdesc_t	&pAnimdesc(int i) const;

		//int	numseq;		// sequences
		//int	seqindex;
		//mstudioseqdesc_t	*pSeqdesc(int i) const { if (i < 0 || i >= numseq) i = 0; return (mstudioseqdesc_t *)(((byte *)this) + seqindex) + i; };
		int					iRelativeAnim(int baseseq, int relanim) const;	// maps seq local anim reference to global anim index
		int					iRelativeSeq(int baseseq, int relseq) const;		// maps seq local seq reference to global seq index

		//mstudioseqdesc_t* pSeqdesc(int i) const
		//{
		//	if (i < 0 || i >= numseq) i = 0;
		//	return (mstudioseqdesc_t*)(((byte*)this) + seqindex) + i;
		//};

		mstudioseqdesc_t* pSeqdesc(int a2)
		{
			int v2 = a2;
			int v3 = *(DWORD*)this;

			if (a2 < 0 || a2 >= *(DWORD*)(v3 + 188))
				v2 = 0;
			
			return (mstudioseqdesc_t*)(v3 + *(DWORD*)(v3 + 192) + 212 * v2);
		}
																				//private:
		mutable int			activitylistversion;	// initialization flag - have the sequences been indexed?
		mutable int			eventsindexed;
		//public:
		int					GetSequenceActivity(int iSequence);
		void				SetSequenceActivity(int iSequence, int iActivity);
		int					GetActivityListVersion(void);
		void				SetActivityListVersion(int version) const;
		int					GetEventListVersion(void);
		void				SetEventListVersion(int version);

		// raw textures
		int					numtextures;
		int					textureindex;
		inline mstudiotexture_t *pTexture(int i) const { return NULL; };


		// raw textures search paths
		int					numcdtextures;
		int					cdtextureindex;
		inline char			*pCdtexture(int i) const { return (((char *)this) + *((int *)(((byte *)this) + cdtextureindex) + i)); };

		// replaceable textures tables
		int					numskinref;
		int					numskinfamilies;
		int					skinindex;
		inline short		*pSkinref(int i) const { return (short *)(((byte *)this) + skinindex) + i; };

		int					numbodyparts;
		int					bodypartindex;
		inline mstudiobodyparts_t	*pBodypart(int i) const { return NULL; };
	};

	enum OverrideType_t
	{
		OVERRIDE_NORMAL = 0,
		OVERRIDE_BUILD_SHADOWS,
		OVERRIDE_DEPTH_WRITE,
	};
	
	class IVModelRender
	{
	public:
		int DrawModel(int flags, IClientRenderable *pRenderable, ModelInstanceHandle_t instance, int entity_index, const model_t *model, Vector const& origin, QAngle const& angles, int skin, int body, int hitboxset, const matrix3x4_t *modelToWorld = NULL, const matrix3x4_t *pLightingOffset = NULL)
		{
			typedef int(__thiscall* fnDrawModel)(void*, int, IClientRenderable*, ModelInstanceHandle_t, int, const model_t*, Vector const&, QAngle const&, int, int, int, const matrix3x4_t*, const matrix3x4_t*);
			return CallVFunction<fnDrawModel>(this, 0)(this, flags, pRenderable, instance, entity_index, model, origin, angles, skin, body, hitboxset, modelToWorld, pLightingOffset);
		}
		
		void ForcedMaterialOverride(IMaterial *material, OverrideType_t type = OVERRIDE_NORMAL, int idk = NULL)
		{
			if (!this)
				return;

			typedef void(__thiscall* Fn)(void*, IMaterial*, OverrideType_t, int);
			return CallVFunction<Fn>(this, 1)(this, material, type, idk);
		}

		void DrawModelExecute(void* ctx, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld = NULL)
		{
			typedef void(__thiscall* oDrawModelExecute)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);
			return CallVFunction<oDrawModelExecute>(this, 21)(this, ctx, state, pInfo, pCustomBoneToWorld);
		}
	};

	class CModelInfo
	{
	public:
		int	GetModelIndex(const char *name)
		{
			typedef int(__thiscall* oGetModelName)(PVOID, const char *);
			return CallVFunction< oGetModelName >(this, 2)(this, name);
		}
		const char *GetModelName(const model_t *mod)
		{
			typedef const char *(__thiscall* oGetModelName)(PVOID, const model_t*);
			return CallVFunction< oGetModelName >(this, 3)(this, mod);
		}

		studiohdr_t	*GetStudiomodel(const model_t *mod)
		{
			typedef studiohdr_t *(__stdcall* oGetStudiomodel)(const model_t*);
			return CallVFunction< oGetStudiomodel >(this, 30)(mod);
		}
	};