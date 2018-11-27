//#pragma once
//#include "IAppSystem.hpp"
//#include "../XorStr.hpp"
//#include <cassert>
//
//#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
//#define MAXSTUDIOSKINS		32
//
//// These are given to FindMaterial to reference the texture groups that show up on the
//#define TEXTURE_GROUP_LIGHTMAP						"Lightmaps"
//#define TEXTURE_GROUP_WORLD							"World textures"
//#define TEXTURE_GROUP_MODEL							"Model textures"
//#define TEXTURE_GROUP_VGUI							"VGUI textures"
//#define TEXTURE_GROUP_PARTICLE						"Particle textures"
//#define TEXTURE_GROUP_DECAL							"Decal textures"
//#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
//#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
//#define TEXTURE_GROUP_OTHER							"Other textures"
//#define TEXTURE_GROUP_PRECACHED						"Precached"				
//#define TEXTURE_GROUP_CUBE_MAP						"CubeMap textures"
//#define TEXTURE_GROUP_RENDER_TARGET					"RenderTargets"
//#define TEXTURE_GROUP_UNACCOUNTED					"Unaccounted textures"	// Textures that weren't assigned a texture group.
////#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		"Static Vertex"
//#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			"Static Indices"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
//#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			"Dynamic Indices"
//#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			"Dynamic Verts"
//#define TEXTURE_GROUP_DEPTH_BUFFER					"DepthBuffer"
//#define TEXTURE_GROUP_VIEW_MODEL					"ViewModel"
//#define TEXTURE_GROUP_PIXEL_SHADERS					"Pixel Shaders"
//#define TEXTURE_GROUP_VERTEX_SHADERS				"Vertex Shaders"
//#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			"RenderTarget Surfaces"
//#define TEXTURE_GROUP_MORPH_TARGETS					"Morph Targets"
//#define TEXTURE_GROUP_STATIC_PROP					"StaticProp textures"
//
////-----------------------------------------------------------------------------
//// forward declarations
////-----------------------------------------------------------------------------
////class IMaterial;
//class IMesh;
//class IVertexBuffer;
//class IIndexBuffer;
//struct MaterialSystem_Config_t;
//class VMatrix;
//class matrix3x4_t;
//class ITexture;
//struct MaterialSystemHWID_t;
//class KeyValues;
//class IShader;
//class IVertexTexture;
//class IMorph;
//class IMatRenderContext;
//class ICallQueue;
//struct MorphWeight_t;
//class IFileList;
//struct VertexStreamSpec_t;
//struct ShaderStencilState_t;
//struct MeshInstanceData_t;
//class IClientMaterialSystem;
//class CPaintMaterial;
//class IPaintMapDataManager;
//class IPaintMapTextureManager;
//class GPUMemoryStats;
//struct AspectRatioInfo_t;
//struct CascadedShadowMappingState_t;
//
//class IMaterialProxyFactory;
//class ITexture;
//class IMaterialSystemHardwareConfig;
//class CShadowMgr;
//class IMaterialVar;
//
//enum CompiledVtfFlags
//{
//	TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
//	TEXTUREFLAGS_TRILINEAR = 0x00000002,
//	TEXTUREFLAGS_CLAMPS = 0x00000004,
//	TEXTUREFLAGS_CLAMPT = 0x00000008,
//	TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
//	TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
//	TEXTUREFLAGS_PWL_CORRECTED = 0x00000040,
//	TEXTUREFLAGS_NORMAL = 0x00000080,
//	TEXTUREFLAGS_NOMIP = 0x00000100,
//	TEXTUREFLAGS_NOLOD = 0x00000200,
//	TEXTUREFLAGS_ALL_MIPS = 0x00000400,
//	TEXTUREFLAGS_PROCEDURAL = 0x00000800,
//	TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
//	TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
//	TEXTUREFLAGS_ENVMAP = 0x00004000,
//	TEXTUREFLAGS_RENDERTARGET = 0x00008000,
//	TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
//	TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
//	TEXTUREFLAGS_SINGLECOPY = 0x00040000,
//	TEXTUREFLAGS_PRE_SRGB = 0x00080000,
//	TEXTUREFLAGS_UNUSED_00100000 = 0x00100000,
//	TEXTUREFLAGS_UNUSED_00200000 = 0x00200000,
//	TEXTUREFLAGS_UNUSED_00400000 = 0x00400000,
//	TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
//	TEXTUREFLAGS_UNUSED_01000000 = 0x01000000,
//	TEXTUREFLAGS_CLAMPU = 0x02000000,
//	TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,
//	TEXTUREFLAGS_SSBUMP = 0x08000000,
//	TEXTUREFLAGS_UNUSED_10000000 = 0x10000000,
//	TEXTUREFLAGS_BORDER = 0x20000000,
//	TEXTUREFLAGS_UNUSED_40000000 = 0x40000000,
//	TEXTUREFLAGS_UNUSED_80000000 = 0x80000000
//};
//
//enum StandardLightmap_t
//{
//	MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE = -1,
//	MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE_BUMP = -2,
//	MATERIAL_SYSTEM_LIGHTMAP_PAGE_USER_DEFINED = -3
//};
//
//enum PreviewImageRetVal_t
//{
//	MATERIAL_PREVIEW_IMAGE_BAD = 0,
//	MATERIAL_PREVIEW_IMAGE_OK,
//	MATERIAL_NO_PREVIEW_IMAGE,
//};
//
//enum MaterialVarFlags_t
//{
//	MATERIAL_VAR_DEBUG = (1 << 0),
//	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
//	MATERIAL_VAR_NO_DRAW = (1 << 2),
//	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
//
//	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
//	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
//	MATERIAL_VAR_SELFILLUM = (1 << 6),
//	MATERIAL_VAR_ADDITIVE = (1 << 7),
//	MATERIAL_VAR_ALPHATEST = (1 << 8),
//	MATERIAL_VAR_MULTIPASS = (1 << 9),
//	MATERIAL_VAR_ZNEARER = (1 << 10),
//	MATERIAL_VAR_MODEL = (1 << 11),
//	MATERIAL_VAR_FLAT = (1 << 12),
//	MATERIAL_VAR_NOCULL = (1 << 13),
//	MATERIAL_VAR_NOFOG = (1 << 14),
//	MATERIAL_VAR_IGNOREZ = (1 << 15),
//	MATERIAL_VAR_DECAL = (1 << 16),
//	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17),
//	MATERIAL_VAR_NOALPHAMOD = (1 << 18),
//	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19),
//	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
//	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
//	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
//	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23),
//	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
//	MATERIAL_VAR_ENVMAPMODE = (1 << 25),
//	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
//	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
//	MATERIAL_VAR_WIREFRAME = (1 << 28),
//
//	// NOTE: Only add flags here that either should be read from
//	// .vmts or can be set directly from client code. Other, internal
//	// flags should to into the flag enum in IMaterialInternal.h
//};
//
//class IMaterial
//{
//public:
//	// Get the name of the material.  This is a full path to 
//	// the vmt file starting from "hl2/materials" (or equivalent) without
//	// a file extension.
//	virtual const char *	GetName() const = 0;
//	virtual const char *	GetTextureGroupName() const = 0;
//
//	// Get the preferred size/bitDepth of a preview image of a material.
//	// This is the sort of image that you would use for a thumbnail view
//	// of a material, or in WorldCraft until it uses materials to render.
//	// separate this for the tools maybe
//	virtual PreviewImageRetVal_t GetPreviewImageProperties(int *width, int *height, ImageFormat *imageFormat, bool* isTranslucent) const = 0;
//
//	// Get a preview image at the specified width/height and bitDepth.
//	// Will do resampling if necessary.(not yet!!! :) )
//	// Will do color format conversion. (works now.)
//	virtual PreviewImageRetVal_t GetPreviewImage(unsigned char *data, int width, int height, ImageFormat imageFormat) const = 0;
//	// 
//	virtual int				GetMappingWidth() = 0;
//	virtual int				GetMappingHeight() = 0;
//
//	virtual int				GetNumAnimationFrames() = 0;
//
//	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
//	virtual bool			InMaterialPage(void) = 0;
//	virtual	void			GetMaterialOffset(float *pOffset) = 0;
//	virtual void			GetMaterialScale(float *pScale) = 0;
//	virtual IMaterial		*GetMaterialPage(void) = 0;
//
//	// find a vmt variable.
//	// This is how game code affects how a material is rendered.
//	// The game code must know about the params that are used by
//	// the shader for the material that it is trying to affect.
//	virtual IMaterialVar *	FindVar(const char *varName, bool *found, bool complain = true) = 0;
//
//	// The user never allocates or deallocates materials.  Reference counting is
//	// used instead.  Garbage collection is done upon a call to 
//	// IMaterialSystem::UncacheUnusedMaterials.
//	virtual void			IncrementReferenceCount(void) = 0;
//	virtual void			DecrementReferenceCount(void) = 0;
//
//	inline void AddRef() { IncrementReferenceCount(); }
//	inline void Release() { DecrementReferenceCount(); }
//
//	// Each material is assigned a number that groups it with like materials
//	// for sorting in the application.
//	virtual int 			GetEnumerationID(void) const = 0;
//
//	virtual void			GetLowResColorSample(float s, float t, float *color) const = 0;
//
//	// This computes the state snapshots for this material
//	virtual void			RecomputeStateSnapshots() = 0;
//
//	// Are we translucent?
//	virtual bool			IsTranslucent() = 0;
//
//	// Are we alphatested?
//	virtual bool			IsAlphaTested() = 0;
//
//	// Are we vertex lit?
//	virtual bool			IsVertexLit() = 0;
//
//	// Gets the vertex format
//	virtual uint64_t		GetVertexFormat() const = 0;
//
//	// returns true if this material uses a material proxy
//	virtual bool			HasProxy(void) const = 0;
//
//	virtual bool			UsesEnvCubemap(void) = 0;
//
//	virtual bool			NeedsTangentSpace(void) = 0;
//
//	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
//	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
//
//	// returns true if the shader doesn't do skinning itself and requires
//	// the data that is sent to it to be preskinned.
//	virtual bool			NeedsSoftwareSkinning(void) = 0;
//
//	// Apply constant color or alpha modulation
//	virtual void			AlphaModulate(float alpha) = 0;
//	virtual void			ColorModulate(float r, float g, float b) = 0;
//
//	// Material Var flags...
//	virtual void			SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
//	virtual bool			GetMaterialVarFlag(MaterialVarFlags_t flag) = 0;
//
//	// Gets material reflectivity
//	virtual void			GetReflectivity(Vector& reflect) = 0;
//
//	// Gets material property flags
//	virtual bool			GetPropertyFlag(MaterialPropertyTypes_t type) = 0;
//
//	// Is the material visible from both sides?
//	virtual bool			IsTwoSided() = 0;
//
//	// Sets the shader associated with the material
//	virtual void			SetShader(const char *pShaderName) = 0;
//
//	// Can't be const because the material might have to precache itself.
//	virtual int				GetNumPasses(void) = 0;
//
//	// Can't be const because the material might have to precache itself.
//	virtual int				GetTextureMemoryBytes(void) = 0;
//
//	// Meant to be used with materials created using CreateMaterial
//	// It updates the materials to reflect the current values stored in the material vars
//	virtual void			Refresh() = 0;
//
//	// GR - returns true is material uses lightmap alpha for blending
//	virtual bool			NeedsLightmapBlendAlpha(void) = 0;
//
//	// returns true if the shader doesn't do lighting itself and requires
//	// the data that is sent to it to be prelighted
//	virtual bool			NeedsSoftwareLighting(void) = 0;
//
//	// Gets at the shader parameters
//	virtual int				ShaderParamCount() const = 0;
//	virtual IMaterialVar	**GetShaderParams(void) = 0;
//
//	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
//	// the material can't be found.
//	virtual bool			IsErrorMaterial() const = 0;
//
//	virtual void			Unused() = 0;
//
//	// Gets the current alpha modulation
//	virtual float			GetAlphaModulation() = 0;
//	virtual void			GetColorModulation(float *r, float *g, float *b) = 0;
//
//	// Is this translucent given a particular alpha modulation?
//	virtual bool			IsTranslucentUnderModulation(float fAlphaModulation = 1.0f) const = 0;
//
//	// fast find that stores the index of the found var in the string table in local cache
//	virtual IMaterialVar *	FindVarFast(char const *pVarName, unsigned int *pToken) = 0;
//
//	// Sets new VMT shader parameters for the material
//	virtual void			SetShaderAndParams(void *pKeyValues) = 0;
//	virtual const char *	GetShaderName() const = 0;
//
//	virtual void			DeleteIfUnreferenced() = 0;
//
//	virtual bool			IsSpriteCard() = 0;
//
//	virtual void			CallBindProxy(void *proxyData) = 0;
//
//	virtual void			RefreshPreservingMaterialVars() = 0;
//
//	virtual bool			WasReloadedFromWhitelist() = 0;
//
//	virtual bool			SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;
//
//	virtual int				GetReferenceCount() const = 0;
//};
//
//struct MaterialSystem_SortInfo_t
//{
//	IMaterial	*material;
//	int			lightmapPageID;
//};
//
//enum MaterialThreadMode_t
//{
//	MATERIAL_SINGLE_THREADED,
//	MATERIAL_QUEUED_SINGLE_THREADED,
//	MATERIAL_QUEUED_THREADED
//};
//
//enum MaterialContextType_t
//{
//	MATERIAL_HARDWARE_CONTEXT,
//	MATERIAL_QUEUED_CONTEXT,
//	MATERIAL_NULL_CONTEXT
//};
//
//enum
//{
//	MATERIAL_ADAPTER_NAME_LENGTH = 512
//};
//
//struct MaterialTextureInfo_t
//{
//	int iExcludeInformation;
//};
//
//struct ApplicationPerformanceCountersInfo_t
//{
//	float msMain;
//	float msMST;
//	float msGPU;
//	float msFlip;
//	float msTotal;
//};
//
//struct ApplicationInstantCountersInfo_t
//{
//	unsigned int m_nCpuActivityMask;
//	unsigned int m_nDeferredWordsAllocated;
//};
//struct MaterialAdapterInfo_t
//{
//	char m_pDriverName[MATERIAL_ADAPTER_NAME_LENGTH];
//	unsigned int m_VendorID;
//	unsigned int m_DeviceID;
//	unsigned int m_SubSysID;
//	unsigned int m_Revision;
//	int m_nDXSupportLevel;			// This is the *preferred* dx support level
//	int m_nMinDXSupportLevel;
//	int m_nMaxDXSupportLevel;
//	unsigned int m_nDriverVersionHigh;
//	unsigned int m_nDriverVersionLow;
//};
//
//struct MaterialVideoMode_t
//{
//	int m_Width;			// if width and height are 0 and you select 
//	int m_Height;			// windowed mode, it'll use the window size
//	ImageFormat m_Format;	// use ImageFormats (ignored for windowed mode)
//	int m_RefreshRate;		// 0 == default (ignored for windowed mode)
//};
//enum HDRType_t
//{
//	HDR_TYPE_NONE,
//	HDR_TYPE_INTEGER,
//	HDR_TYPE_FLOAT,
//};
//
//enum RestoreChangeFlags_t
//{
//	MATERIAL_RESTORE_VERTEX_FORMAT_CHANGED = 0x1,
//	MATERIAL_RESTORE_RELEASE_MANAGED_RESOURCES = 0x2,
//};
//
//enum RenderTargetSizeMode_t
//{
//	RT_SIZE_NO_CHANGE = 0,
//	RT_SIZE_DEFAULT = 1,
//	RT_SIZE_PICMIP = 2,
//	RT_SIZE_HDR = 3,
//	RT_SIZE_FULL_FRAME_BUFFER = 4,
//	RT_SIZE_OFFSCREEN = 5,
//	RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP = 6
//};
//
//enum MaterialRenderTargetDepth_t
//{
//	MATERIAL_RT_DEPTH_SHARED = 0x0,
//	MATERIAL_RT_DEPTH_SEPARATE = 0x1,
//	MATERIAL_RT_DEPTH_NONE = 0x2,
//	MATERIAL_RT_DEPTH_ONLY = 0x3,
//};
//
//typedef void(*MaterialBufferReleaseFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
//typedef void(*MaterialBufferRestoreFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
//typedef void(*ModeChangeCallbackFunc_t)(void);
//typedef void(*EndFrameCleanupFunc_t)(void);
//typedef bool(*EndFramePriorToNextContextFunc_t)(void);
//typedef void(*OnLevelShutdownFunc_t)(void *pUserData);
//
//typedef unsigned short MaterialHandle_t;
//DECLARE_POINTER_HANDLE(MaterialLock_t);
//
//struct FileHandle_t;
//
//class KeyValues
//{
//public:
//	//	By default, the KeyValues class uses a string table for the key names that is
//	//	limited to 4MB. The game will exit in error if this space is exhausted. In
//	//	general this is preferable for game code for performance and memory fragmentation
//	//	reasons.
//	//
//	//	If this is not acceptable, you can use this call to switch to a table that can grow
//	//	arbitrarily. This call must be made before any KeyValues objects are allocated or it
//	//	will result in undefined behavior. If you use the growable string table, you cannot
//	//	share KeyValues pointers directly with any other module. You can serialize them across
//	//	module boundaries. These limitations are acceptable in the Steam backend code 
//	//	this option was written for, but may not be in other situations. Make sure to
//	//	understand the implications before using this.
//	static void SetUseGrowableStringTable(bool bUseGrowableTable);
//
//	KeyValues(const char *setName) {}
//
//	//
//	// AutoDelete class to automatically free the keyvalues.
//	// Simply construct it with the keyvalues you allocated and it will free them when falls out of scope.
//	// When you decide that keyvalues shouldn't be deleted call Assign(NULL) on it.
//	// If you constructed AutoDelete(NULL) you can later assign the keyvalues to be deleted with Assign(pKeyValues).
//	// You can also pass temporary KeyValues object as an argument to a function by wrapping it into KeyValues::AutoDelete
//	// instance:   call_my_function( KeyValues::AutoDelete( new KeyValues( "test" ) ) )
//	//
//	class AutoDelete
//	{
//	public:
//		explicit inline AutoDelete(KeyValues *pKeyValues) : m_pKeyValues(pKeyValues) {}
//		explicit inline AutoDelete(const char *pchKVName) : m_pKeyValues(new KeyValues(pchKVName)) {}
//		inline ~AutoDelete(void) { if (m_pKeyValues) m_pKeyValues->deleteThis(); }
//		inline void Assign(KeyValues *pKeyValues) { m_pKeyValues = pKeyValues; }
//		KeyValues *operator->() { return m_pKeyValues; }
//		operator KeyValues *() { return m_pKeyValues; }
//	private:
//		AutoDelete(AutoDelete const &x); // forbid
//		AutoDelete & operator= (AutoDelete const &x); // forbid
//		KeyValues *m_pKeyValues;
//	};
//
//	// Quick setup constructors
//	KeyValues(const char *setName, const char *firstKey, const char *firstValue);
//	KeyValues(const char *setName, const char *firstKey, const wchar_t *firstValue);
//	KeyValues(const char *setName, const char *firstKey, int firstValue);
//	KeyValues(const char *setName, const char *firstKey, const char *firstValue, const char *secondKey, const char *secondValue);
//	KeyValues(const char *setName, const char *firstKey, int firstValue, const char *secondKey, int secondValue);
//
//	// Section name
//	const char *GetName() const;
//	void SetName(const char *setName);
//
//	// gets the name as a unique int
//	int GetNameSymbol() const { return m_iKeyName; }
//
//	// File access. Set UsesEscapeSequences true, if resource file/buffer uses Escape Sequences (eg \n, \t)
//	void UsesEscapeSequences(bool state); // default false
//	void UsesConditionals(bool state); // default true
//	bool LoadFromFile(void *filesystem, const char *resourceName, const char *pathID = nullptr);
//	bool SaveToFile(void *filesystem, const char *resourceName, const char *pathID = nullptr, bool sortKeys = false, bool bAllowEmptyString = false);
//
//	// Read from a buffer...  Note that the buffer must be null terminated
//	bool LoadFromBuffer(char const *resourceName, const char *pBuffer, void* pFileSystem = nullptr, const char *pPathID = nullptr);
//
//	// Read from a utlbuffer...
//	bool LoadFromBuffer(char const *resourceName, void*buf, void* pFileSystem = nullptr, const char *pPathID = nullptr);
//
//	// Find a keyValue, create it if it is not found.
//	// Set bCreate to true to create the key if it doesn't already exist (which ensures a valid pointer will be returned)
//	KeyValues *FindKey(const char *keyName, bool bCreate = false);
//	KeyValues *FindKey(int keySymbol) const;
//	KeyValues *CreateNewKey();		// creates a new key, with an autogenerated name.  name is guaranteed to be an integer, of value 1 higher than the highest other integer key name
//	void AddSubKey(KeyValues *pSubkey);	// Adds a subkey. Make sure the subkey isn't a child of some other keyvalues
//	void RemoveSubKey(KeyValues *subKey);	// removes a subkey from the list, DOES NOT DELETE IT
//
//											// Key iteration.
//											//
//											// NOTE: GetFirstSubKey/GetNextKey will iterate keys AND values. Use the functions 
//											// below if you want to iterate over just the keys or just the values.
//											//
//	KeyValues *GetFirstSubKey() { return m_pSub; }	// returns the first subkey in the list
//	KeyValues *GetNextKey() { return m_pPeer; }		// returns the next subkey
//	void SetNextKey(KeyValues * pDat);
//	KeyValues *FindLastSubKey();	// returns the LAST subkey in the list.  This requires a linked list iteration to find the key.  Returns NULL if we don't have any children
//
//									//
//									// These functions can be used to treat it like a true key/values tree instead of 
//									// confusing values with keys.
//									//
//									// So if you wanted to iterate all subkeys, then all values, it would look like this:
//									//     for ( KeyValues *pKey = pRoot->GetFirstTrueSubKey(); pKey; pKey = pKey->GetNextTrueSubKey() )
//									//     {
//									//		   Msg( "Key name: %s\n", pKey->GetName() );
//									//     }
//									//     for ( KeyValues *pValue = pRoot->GetFirstValue(); pKey; pKey = pKey->GetNextValue() )
//									//     {
//									//         Msg( "Int value: %d\n", pValue->GetInt() );  // Assuming pValue->GetDataType() == TYPE_INT...
//									//     }
//	KeyValues* GetFirstTrueSubKey();
//	KeyValues* GetNextTrueSubKey();
//
//	KeyValues* GetFirstValue();	// When you get a value back, you can use GetX and pass in NULL to get the value.
//	KeyValues* GetNextValue();
//
//
//	// Data access
//	int   GetInt(const char *keyName = nullptr, int defaultValue = 0);
//	uint64_t GetUint64(const char *keyName = nullptr, uint64_t defaultValue = 0);
//	float GetFloat(const char *keyName = nullptr, float defaultValue = 0.0f);
//	const char *GetString(const char *keyName = nullptr, const char *defaultValue = "");
//	const wchar_t *GetWString(const char *keyName = nullptr, const wchar_t *defaultValue = L"");
//	void *GetPtr(const char *keyName = nullptr, void *defaultValue = (void*)nullptr);
//	bool GetBool(const char *keyName = nullptr, bool defaultValue = false);
//	Color GetColor(const char *keyName = nullptr /* default value is all black */);
//	bool  IsEmpty(const char *keyName = nullptr);
//
//	// Data access
//	int   GetInt(int keySymbol, int defaultValue = 0);
//	float GetFloat(int keySymbol, float defaultValue = 0.0f);
//	const char *GetString(int keySymbol, const char *defaultValue = "");
//	const wchar_t *GetWString(int keySymbol, const wchar_t *defaultValue = L"");
//	void *GetPtr(int keySymbol, void *defaultValue = (void*)nullptr);
//	Color GetColor(int keySymbol /* default value is all black */);
//	bool  IsEmpty(int keySymbol);
//
//	// Key writing
//	void SetWString(const char *keyName, const wchar_t *value);
//	void SetString(const char *keyName, const char *value);
//	void SetInt(const char *keyName, int value);
//	void SetUint64(const char *keyName, uint64_t value);
//	void SetFloat(const char *keyName, float value);
//	void SetPtr(const char *keyName, void *value);
//	void SetColor(const char *keyName, Color value);
//	void SetBool(const char *keyName, bool value) { SetInt(keyName, value ? 1 : 0); }
//
//	// Adds a chain... if we don't find stuff in this keyvalue, we'll look
//	// in the one we're chained to.
//	void ChainKeyValue(KeyValues* pChain);
//
//	void RecursiveSaveToFile(void* buf, int indentLevel, bool sortKeys = false, bool bAllowEmptyString = false);
//
//	bool WriteAsBinary(void*buffer);
//	bool ReadAsBinary(void*buffer, int nStackDepth = 0);
//
//	// Allocate & create a new copy of the keys
//	KeyValues *MakeCopy(void) const;
//
//	// Make a new copy of all subkeys, add them all to the passed-in keyvalues
//	void CopySubkeys(KeyValues *pParent) const;
//
//	// Clear out all subkeys, and the current value
//	void Clear(void);
//
//	// Data type
//	enum types_t
//	{
//		TYPE_NONE = 0,
//		TYPE_STRING,
//		TYPE_INT,
//		TYPE_FLOAT,
//		TYPE_PTR,
//		TYPE_WSTRING,
//		TYPE_COLOR,
//		TYPE_UINT64,
//		TYPE_NUMTYPES,
//	};
//	types_t GetDataType(const char *keyName = nullptr);
//
//	// Virtual deletion function - ensures that KeyValues object is deleted from correct heap
//	void deleteThis();
//
//	void SetStringValue(char const *strValue);
//
//	// unpack a key values list into a structure
//	void UnpackIntoStructure(struct KeyValuesUnpackStructure const *pUnpackTable, void *pDest, size_t DestSizeInBytes);
//
//	// Process conditional keys for widescreen support.
//	bool ProcessResolutionKeys(const char *pResString);
//
//	// Dump keyvalues recursively into a dump context
//	bool Dump(class IKeyValuesDumpContext *pDump, int nIndentLevel = 0);
//
//	// Merge in another KeyValues, keeping "our" settings
//	void RecursiveMergeKeyValues(KeyValues *baseKV);
//
//private:
//	KeyValues(KeyValues&);	// prevent copy constructor being used
//
//							// prevent delete being called except through deleteThis()
//	~KeyValues();
//
//	KeyValues* CreateKey(const char *keyName);
//
//	/// Create a child key, given that we know which child is currently the last child.
//	/// This avoids the O(N^2) behaviour when adding children in sequence to KV,
//	/// when CreateKey() wil have to re-locate the end of the list each time.  This happens,
//	/// for example, every time we load any KV file whatsoever.
//	KeyValues* CreateKeyUsingKnownLastChild(const char *keyName, KeyValues *pLastChild);
//	void AddSubkeyUsingKnownLastChild(KeyValues *pSubKey, KeyValues *pLastChild);
//
//	void RecursiveCopyKeyValues(KeyValues& src);
//	void RemoveEverything();
//	//	void RecursiveSaveToFile( IBaseFileSystem *filesystem, void*buffer, int indentLevel );
//	//	void WriteConvertedString( void*buffer, const char *pszString );
//
//	// NOTE: If both filesystem and pBuf are non-null, it'll save to both of them.
//	// If filesystem is null, it'll ignore f.
//	void RecursiveSaveToFile(void *filesystem, FileHandle_t f, void *pBuf, int indentLevel, bool sortKeys, bool bAllowEmptyString);
//	void SaveKeyToFile(KeyValues *dat, void *filesystem, FileHandle_t f, void *pBuf, int indentLevel, bool sortKeys, bool bAllowEmptyString);
//	void WriteConvertedString(void *filesystem, FileHandle_t f, void *pBuf, const char *pszString);
//
//	void RecursiveLoadFromBuffer(char const *resourceName, void*buf);
//
//	// For handling #include "filename"
//	void AppendIncludedKeys(void* includedKeys);
//	void ParseIncludedKeys(char const *resourceName, const char *filetoinclude,
//		void* pFileSystem, const char *pPathID, void* includedKeys);
//
//	// For handling #base "filename"
//	void MergeBaseKeys(void* baseKeys);
//
//	// NOTE: If both filesystem and pBuf are non-null, it'll save to both of them.
//	// If filesystem is null, it'll ignore f.
//	void InternalWrite(void *filesystem, FileHandle_t f, void *pBuf, const void *pData, int len);
//
//	void Init();
//	const char * ReadToken(void*buf, bool &wasQuoted, bool &wasConditional);
//	void WriteIndents(void *filesystem, FileHandle_t f, void *pBuf, int indentLevel);
//
//	void FreeAllocatedValue();
//	void AllocateValueBlock(int size);
//
//	int m_iKeyName;	// keyname is a symbol defined in KeyValuesSystem
//
//					// These are needed out of the union because the API returns string pointers
//	char *m_sValue;
//	wchar_t *m_wsValue;
//
//	// we don't delete these
//	union
//	{
//		int m_iValue;
//		float m_flValue;
//		void *m_pValue;
//		unsigned char m_Color[4];
//	};
//
//	char	   m_iDataType;
//	char	   m_bHasEscapeSequences; // true, if while parsing this KeyValue, Escape Sequences are used (default false)
//	char	   m_bEvaluateConditionals; // true, if while parsing this KeyValue, conditionals blocks are evaluated (default true)
//	char	   unused[1];
//
//	KeyValues *m_pPeer;	// pointer to next key in list
//	KeyValues *m_pSub;	// pointer to Start of a new sub key list
//	KeyValues *m_pChain;// Search here if it's not in our list
//
//private:
//	// Statics to implement the optional growable string table
//	// Function pointers that will determine which mode we are in
//	static int(*s_pfGetSymbolForString)(const char *name, bool bCreate);
//	static const char *(*s_pfGetStringForSymbol)(int symbol);
//	static void *s_pGrowableStringTable;
//
//public:
//	// Functions that invoke the default behavior
//	static int GetSymbolForStringClassic(const char *name, bool bCreate = true);
//	static const char *GetStringForSymbolClassic(int symbol);
//
//	// Functions that use the growable string table
//	static int GetSymbolForStringGrowable(const char *name, bool bCreate = true);
//	static const char *GetStringForSymbolGrowable(int symbol);
//
//	// Functions to get external access to whichever of the above functions we're going to call.
//	static int CallGetSymbolForString(const char *name, bool bCreate = true) { return s_pfGetSymbolForString(name, bCreate); }
//	static const char *CallGetStringForSymbol(int symbol) { return s_pfGetStringForSymbol(symbol); }
//};
//
//
//#include <Windows.h>
//#include <Psapi.h>
//#include "../Utils.hpp"
//
////#define INRANGE(x,a,b)  (x >= a && x <= b) 
////#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
////#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))
////
////inline uint64_t FindSignature2(const char* szModule, const char* szSignature)
////{
////	//CREDITS: learn_more
////
////	MODULEINFO modInfo;
////	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
////	DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;
////	DWORD endAddress = startAddress + modInfo.SizeOfImage;
////	const char* pat = szSignature;
////	DWORD firstMatch = 0;
////	for (DWORD pCur = startAddress; pCur < endAddress; pCur++) {
////		if (!*pat) return firstMatch;
////		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
////			if (!firstMatch) firstMatch = pCur;
////			if (!pat[2]) return firstMatch;
////			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
////			else pat += 2;    //one ?
////		}
////		else {
////			pat = szSignature;
////			firstMatch = 0;
////		}
////	}
////	return NULL;
////}
////
////inline uint64_t FindSignature(const char* szModule, const char* szSignature)
////{
////	uint64_t temp = FindSignature2(szModule, szSignature);
////	assert(temp);
////	return temp;
////}
//
//
//inline void InitKeyValues(KeyValues* pKeyValues, const char* name)
//{
//	using InitKeyValuesFn = void(__thiscall*)(void* thisptr, const char* name);
//	static InitKeyValuesFn InitKeyValuesEx;
//	if (!InitKeyValuesEx)
//		InitKeyValuesEx = (InitKeyValuesFn)Utils::FindSignature(Utils::get_client_dll(), XorStr("55 8B EC 51 33 C0 C7 45"));
//
//	InitKeyValuesEx(pKeyValues, name);
//}
//
//inline void LoadFromBuffer(KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, void* pFileSystem = nullptr, const char* pPathID = nullptr, void* pfnEvaluateSymbolProc = nullptr)
//{
//	using LoadFromBufferFn = void(__thiscall*)(void* thisptr, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc);
//	static LoadFromBufferFn LoadFromBufferEx;
//	if (!LoadFromBufferEx)
//		LoadFromBufferEx = (LoadFromBufferFn)Utils::FindSignature(Utils::get_client_dll(), XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04"));
//
//	LoadFromBufferEx(pKeyValues, resourceName, pBuffer, pFileSystem, pPathID, pfnEvaluateSymbolProc);
//}
//
//int created = 0;
//
//class IMaterialSystem : public IAppSystem
//{
//public:
//
//	virtual CreateInterfaceFn               Init(char const* pShaderAPIDLL, IMaterialProxyFactory *pMaterialProxyFactory, CreateInterfaceFn fileSystemFactory, CreateInterfaceFn cvarFactory = nullptr) = 0;
//	virtual void                            SetShaderAPI(char const *pShaderAPIDLL) = 0;
//	virtual void                            SetAdapter(int nAdapter, int nFlags) = 0;
//	virtual void                            ModInit() = 0;
//	virtual void                            ModShutdown() = 0;
//	virtual void                            SetThreadMode(MaterialThreadMode_t mode, int nServiceThread = -1) = 0;
//	virtual MaterialThreadMode_t            GetThreadMode() = 0;
//	virtual void                            ExecuteQueued() = 0;
//	virtual void                            OnDebugEvent(const char *pEvent) = 0;
//	virtual IMaterialSystemHardwareConfig*  GetHardwareConfig(const char *pVersion, int *returnCode) = 0;
//	virtual void                            __unknown() = 0;
//	virtual bool                            UpdateConfig(bool bForceUpdate) = 0; //20
//	virtual bool                            OverrideConfig(const MaterialSystem_Config_t &config, bool bForceUpdate) = 0;
//	virtual const MaterialSystem_Config_t&  GetCurrentConfigForVideoCard() const = 0;
//	virtual bool                            GetRecommendedConfigurationInfo(int nDXLevel, KeyValues * pKeyValues) = 0;
//	virtual int                             GetDisplayAdapterCount() const = 0;
//	virtual int                             GetCurrentAdapter() const = 0;
//	virtual void                            GetDisplayAdapterInfo(int adapter, MaterialAdapterInfo_t& info) const = 0;
//	virtual int                             GetModeCount(int adapter) const = 0;
//	virtual void                            GetModeInfo(int adapter, int mode, MaterialVideoMode_t& info) const = 0;
//	virtual void                            AddModeChangeCallBack(ModeChangeCallbackFunc_t func) = 0;
//	virtual void                            GetDisplayMode(MaterialVideoMode_t& mode) const = 0; //30
//	virtual bool                            SetMode(void* hwnd, const MaterialSystem_Config_t &config) = 0;
//	virtual bool                            SupportsMSAAMode(int nMSAAMode) = 0;
//	virtual const MaterialSystemHWID_t&     GetVideoCardIdentifier(void) const = 0;
//	virtual void                            SpewDriverInfo() const = 0;
//	virtual void                            GetBackBufferDimensions(int &width, int &height) const = 0;
//	virtual ImageFormat                     GetBackBufferFormat() const = 0;
//	virtual const AspectRatioInfo_t&        GetAspectRatioInfo() const = 0;
//	virtual bool                            SupportsHDRMode(HDRType_t nHDRModede) = 0;
//	virtual bool                            AddView(void* hwnd) = 0;
//	virtual void                            RemoveView(void* hwnd) = 0; //40
//	virtual void                            SetView(void* hwnd) = 0;
//	virtual void                            BeginFrame(float frameTime) = 0;
//	virtual void                            EndFrame() = 0;
//	virtual void                            Flush(bool flushHardware = false) = 0;
//	virtual unsigned int                    GetCurrentFrameCount() = 0;
//	virtual void                            SwapBuffers() = 0;
//	virtual void                            EvictManagedResources() = 0;
//	virtual void                            ReleaseResources(void) = 0;
//	virtual void                            ReacquireResources(void) = 0;
//	virtual void                            AddReleaseFunc(MaterialBufferReleaseFunc_t func) = 0; //50
//	virtual void                            RemoveReleaseFunc(MaterialBufferReleaseFunc_t func) = 0;
//	virtual void                            AddRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
//	virtual void                            RemoveRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
//	virtual void                            AddEndFrameCleanupFunc(EndFrameCleanupFunc_t func) = 0;
//	virtual void                            RemoveEndFrameCleanupFunc(EndFrameCleanupFunc_t func) = 0;
//	virtual void                            OnLevelShutdown() = 0;
//	virtual bool                            AddOnLevelShutdownFunc(OnLevelShutdownFunc_t func, void *pUserData) = 0;
//	virtual bool                            RemoveOnLevelShutdownFunc(OnLevelShutdownFunc_t func, void *pUserData) = 0;
//	virtual void                            OnLevelLoadingComplete() = 0;
//	virtual void                            ResetTempHWMemory(bool bExitingLevel = false) = 0; //60
//	virtual void                            HandleDeviceLost() = 0;
//	virtual int                             ShaderCount() const = 0;
//	virtual int                             GetShaders(int nFirstShader, int nMaxCount, IShader **ppShaderList) const = 0;
//	virtual int                             ShaderFlagCount() const = 0;
//	virtual const char*                     ShaderFlagName(int nIndex) const = 0;
//	virtual void                            GetShaderFallback(const char *pShaderName, char *pFallbackShader, int nFallbackLength) = 0;
//	virtual IMaterialProxyFactory*          GetMaterialProxyFactory() = 0;
//	virtual void                            SetMaterialProxyFactory(IMaterialProxyFactory* pFactory) = 0;
//	virtual void                            EnableEditorMaterials() = 0;
//	virtual void                            EnableGBuffers() = 0; //70
//	virtual void                            SetInStubMode(bool bInStubMode) = 0;
//	virtual void                            DebugPrintUsedMaterials(const char *pSearchSubString, bool bVerbose) = 0;
//	virtual void                            DebugPrintUsedTextures(void) = 0;
//	virtual void                            ToggleSuppressMaterial(char const* pMaterialName) = 0;
//	virtual void                            ToggleDebugMaterial(char const* pMaterialName) = 0;
//	virtual bool                            UsingFastClipping(void) = 0;
//	virtual int                             StencilBufferBits(void) = 0; //number of bits per pixel in the stencil buffer
//	virtual void                            UncacheAllMaterials() = 0;
//	virtual void                            UncacheUnusedMaterials(bool bRecomputeStateSnapshots = false) = 0;
//	virtual void                            CacheUsedMaterials() = 0; //80
//	virtual void                            ReloadTextures() = 0;
//	virtual void                            ReloadMaterials(const char *pSubString = nullptr) = 0;
//	virtual IMaterial*                      CreateMaterial2(const char *pMaterialName, KeyValues *pVMTKeyValues) = 0;
//	virtual IMaterial*                      FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = nullptr) = 0;
//	virtual MaterialHandle_t                FirstMaterial() const = 0;
//	virtual MaterialHandle_t                NextMaterial(MaterialHandle_t h) const = 0;
//	virtual MaterialHandle_t                InvalidMaterial() const = 0;
//	virtual IMaterial*                      GetMaterial(MaterialHandle_t h) const = 0;
//	virtual int                             GetNumMaterials() const = 0;
//	virtual ITexture*                       FindTexture(char const* pTextureName, const char *pTextureGroupName, bool complain = true) = 0;
//	virtual bool                            IsTextureLoaded(char const* pTextureName) const = 0;
//	virtual ITexture*                       CreateProceduralTexture(const char	*pTextureName, const char *pTextureGroupName, int w, int h, ImageFormat fmt, int nFlags) = 0;
//	virtual void                            BeginRenderTargetAllocation() = 0;
//	virtual void                            EndRenderTargetAllocation() = 0; // Simulate an Alt-Tab in here, which causes a release/restore of all resources
//	/*virtual ITexture*                       CreateRenderTargetTexture(int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat	format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED) = 0;
//	virtual ITexture*                       CreateNamedRenderTargetTextureEx(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, unsigned int renderTargetFlags = 0) = 0;
//	virtual ITexture*                       CreateNamedRenderTargetTexture(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, bool bClampTexCoords = true, bool bAutoMipMap = false) = 0;
//	virtual ITexture*                       CreateNamedRenderTargetTextureEx2(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, unsigned int renderTargetFlags = 0) = 0;
//*/
//
//	IMaterial* IMaterialSystem::CreateMaterial(bool flat, bool ignorez, bool wireframed)
//	{
//		std::string type = (flat) ? XorStr("UnlitGeneric") : XorStr("VertexLitGeneric");
//
//		std::string matdata = XorStr("\"") + type + XorStr("\"\n{\n\t\"$basetexture\" \"vgui/white_additive\"\n\t\"$envmap\"  \"\"\n\t\"$model\" \"1\"\n\t\"$flat\" \"1\"\n\t\"$nocull\"  \"0\"\n\t\"$selfillum\" \"1\"\n\t\"$halflambert\" \"1\"\n\t\"$nofog\"  \"0\"\n\t\"$znearer\" \"0\"\n\t\"$wireframe\" \"") + std::to_string(wireframed) + XorStr("\"\n\t\"$ignorez\" \"") + std::to_string(ignorez) + XorStr("\"\n}\n");
//
//		std::string matname = XorStr("custom_") + std::to_string(created);
//		++created;
//
//		KeyValues* pKeyValues = new KeyValues(matname.c_str());
//		InitKeyValues(pKeyValues, type.c_str());
//		LoadFromBuffer(pKeyValues, matname.c_str(), matdata.c_str());
//
//		typedef IMaterial*(__thiscall* OriginalFn)(void*, const char *pMaterialName, KeyValues *pVMTKeyValues);
//		IMaterial* createdMaterial = CallVFunction<OriginalFn>(this, 83)(this, matname.c_str(), pKeyValues);
//
//		createdMaterial->IncrementReferenceCount();
//
//		return createdMaterial;
//	}
//};