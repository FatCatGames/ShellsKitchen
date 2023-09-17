#pragma once
#include "Debugging\DebugShapes.h"
#include "Graphics\Rendering\DX11\DX11.h"
#include "..\External\HBAOPlus\GFSDK_SSAO.h"

class Camera;
class DeferredRenderer;
class ForwardRenderer;
class PostProcessRenderer;
class DebugRenderer;
class ShadowRenderer;

class Texture;
class PixelShader;
class VertexShader;
class Buffers;
class ModelInstance;
class UIRenderer;
class UISprite;
class Editor;
class SceneLightData;
class Scene;
class NavMesh;
class ParticleSystem;
class Light;

enum Renderer
{
	Deferred,
	Forward
};

class GraphicsEngine
{
	SIZE myWindowSize{ 0,0 };
	HWND myWindowHandle{};

	// Container window message pump.
	static LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

public:
	static inline GraphicsEngine* GetInstance() { return Instance; }
	bool Initialize(unsigned someX, unsigned someY, unsigned someWidth, unsigned someHeight, bool enableDeviceDebug, bool isEditorMode, HICON anIcon = 0);
	bool Start();

	void BeginFrame();
	void EndFrame();
	void RenderFrame();
	void EmptyVectors();

	void SetCamera(Camera* aCamera);
	inline Camera* GetMainCamera() { return myCamera; }
	ForwardRenderer& GetForwardRenderer();
	DeferredRenderer& GetDeferredRenderer();
	PostProcessRenderer& GetPostProcessRenderer();

	void SetLights(SceneLightData* aSceneLightData);
	void SetEnvironmentLight(Light* anEnvLight);
	void SetClearColor(const Color& aColor);
	void AddToRenderQueue(ModelInstance* aModel, Renderer aRendererToUse = Deferred);
	void AddToRenderQueue(ParticleSystem* aParticleSystem);
	void AddToRenderQueue(UISprite* aSprite);
	void AddToRenderQueue(DebugLine aLine);
	void AddToRenderQueue(DebugCube aCube);
	void AddToRenderQueue(BillBoard aBillBoard);
	void AddToRenderQueue(NavMesh* aNavmesh);
	void AddToRenderQueue(Light* aShadowCastingLight);
	void RenderOutline(ModelInstance* aModel);
	void RemoveFromRenderQueue(ModelInstance* aModel);
	void RemoveFromRenderQueue(ParticleSystem* aParticleSystem);
	void RemoveFromRenderQueue(UISprite* aSprite);
	void SetDebugDrawerToggle(bool aFlag) { myDebugDrawerToggle = aFlag; }
	bool GetDebugDrawerToggle() { return myDebugDrawerToggle; }

	void SetDebugDrawerMode(int aDebugMode);

	inline double GetFrameTime() const { return myRenderFrameTime; }
	inline double GetBeginFrameTime() const { return myBeginFrameTime; }
	inline double GetEndFrameTime() const { return myEndFrameTime; }
	inline double GetImGuiFrameTime() const { return myImGuiRenderFrame; }
	std::shared_ptr<Texture> GetPreviousDepth() { return myPreviousDepthTexture; }
	std::shared_ptr<Texture> GetLightMap() { return myLightMap; }

	std::shared_ptr<Buffers> GetBuffers() { return myBuffers; }

	bool HasFinishedFrame() const { return myRenderFinished; }

	void FrameSwap();


	[[nodiscard]] HWND FORCEINLINE GetWindowHandle() const { return myWindowHandle; }
	[[nodiscard]] SIZE FORCEINLINE GetWindowSize() const { return myWindowSize; }
	DX11 myFramework;
	std::unique_ptr<Texture> myPostProcessTarget;
	static void RunFullScreenShader(ID3D11ShaderResourceView* const* aSource, ID3D11RenderTargetView* const* aTarget, std::shared_ptr<PixelShader> aPixelShader, UINT aDepthStencilState = 2); //2 is DSS_Off
	inline void SetGammaCorrectionEnabled(bool aValue) { myGammaCorrectEnabled = aValue; }
	inline bool GetGammaCorrectionEnabled() { return myGammaCorrectEnabled; }

	inline void SetAntiAliasingEnabled(bool aValue) { myAntiAliasEnabled = aValue; }
	inline bool GetAntiAliasingEnabled() { return myAntiAliasEnabled; }
	static std::shared_ptr<VertexShader> FullscreenVS;

private:
	static GraphicsEngine* Instance;
	void CreateShaders();

	std::shared_ptr<Buffers> myBuffers;
	SceneLightData* mySceneLightData{ nullptr };
	std::vector<ModelInstance*> myMeshesToRenderDeferred;
	std::vector<ModelInstance*> myMeshesToRenderForward;
	std::vector<ModelInstance*> myMeshesToRenderOutline;
	std::vector<ModelInstance*> myMeshesToRenderOutlinePlayMode;
	std::vector<ParticleSystem*> myParticleSystemsToRender;
	std::vector<Light*> myShadowsToRender;
	std::vector<UISprite*> mySpritesToRender;
	std::vector<DebugLine> myTimeBasedDebugLinesToRender;
	std::vector<DebugLine> myDebugLinesToRender;
	std::vector<DebugCube> myDebugCubesToRender;
	std::vector<NavMesh*> myNavmeshesToRender;

	std::vector<ModelInstance*> myMeshesToRenderDeferredSwap;
	std::vector<ModelInstance*> myMeshesToRenderForwardSwap;
	std::vector<ModelInstance*> myMeshesToRenderOutlineSwap;
	std::vector<ModelInstance*> myMeshesToRenderOutlinePlayModeSwap;
	std::vector<ParticleSystem*> myParticleSystemsToRenderSwap;
	std::vector<Light*> myShadowsToRenderSwap;
	std::vector<UISprite*> mySpritesToRenderSwap;
	std::vector<DebugLine> myDebugLinesToRenderSwap;
	std::vector<DebugCube> myDebugCubesToRenderSwap;
	std::vector<NavMesh*> myNavmeshesToRenderSwap;
	std::vector<BillBoard> myBillBoardsToRender;

	std::shared_ptr<Texture> myPreviousDepthTexture;
	std::shared_ptr<Texture> myLightMap;
	std::shared_ptr<Texture> myShadowsBluredTexture;
	std::shared_ptr<Texture> myItermediateBluringTexture;
	int myIndex = 0;

	Camera* myCamera{ nullptr };
	std::shared_ptr<DeferredRenderer> myDeferredRenderer;
	std::shared_ptr<ForwardRenderer> myForwardRenderer;
	std::shared_ptr<PostProcessRenderer> myPostProcessRenderer;
	std::shared_ptr<UIRenderer> myUIRenderer;
	std::shared_ptr<DebugRenderer> myDebugRenderer;
	std::shared_ptr<ShadowRenderer> myShadowRenderer;
	bool myIsEditorMode;

	std::shared_ptr<PixelShader> myAntiAliasPS;
	std::shared_ptr<PixelShader> myCopyPS;
	std::shared_ptr<PixelShader> myBackgroundPS;
	std::shared_ptr<PixelShader> myGammaCorrectionPS;
	std::shared_ptr<PixelShader> myOutlinePS;
	std::shared_ptr<PixelShader> myBlurShadowsPS;

	std::shared_ptr<Texture> myIntermediateTexture;
	std::shared_ptr<Texture> myEnviormentTexture;
	Color myClearColor = { 0.1f, 0.1f, 0.1f, 1 };
	bool myGammaCorrectEnabled = true;
	bool myAntiAliasEnabled = true;
	int myDebugRenderMode = 0;
	bool myDebugDrawerToggle = false;
	double myRenderFrameTime = 0;
	double myDeferredFrameTime = 0;
	double myForwardFrameTime = 0;
	double myImGuiRenderFrame = 0;
	double myEndFrameTime;
	double myBeginFrameTime;

	bool myRenderFinished = true;



	GFSDK_SSAO_Parameters Params;
	GFSDK_SSAO_Output_D3D11 myOutput;
	GFSDK_SSAO_InputData_D3D11 myInput;
	GFSDK_SSAO_CustomHeap CustomHeap;
	GFSDK_SSAO_Context_D3D11* pAOContext;
};

