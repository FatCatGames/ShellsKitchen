#include "Game.pch.h"
#include "Project.h"
//end Shaidors

#include "Components\ComponentHandler.h"
#include "Components\Physics\Collisions\CollisionManager.h"
#include "Assets\AssetLoader.h"
#include "ComponentTools\SceneManager.h"

//Components
#include "Components\GamePlay\IngredientIcon.h"
#include "Components\GamePlay\BurnIndicator.h"
#include "Components\GamePlay\SelfDestroy.h"
#include "Components\GamePlay\LevelTimer.h"
#include "Components\GamePlay\LevelCoinCounter.h"
#include "Components\GamePlay\CoinCounterComponent.h"
#include "Components\GamePlay\OrderUI.h"
#include "Components\GamePlay\TutorialBoard.h"
#include "Components\GamePlay\TrashcanIcon.h"
#include "Components\GamePlay\FloatingScore.h"
#include "Components\Camera\RayPickerComponent.h"
#include "Components\LevelEditorComponent\KitchenWindowDummyComponent.h"
#include "Components\Player\Player.h"
#include "Components\Player\PlayerPlaceholder.h"

//Interactables
#include "Components\Interactables\Surfaces\CuttingBoard.h"
#include "Components\Interactables\Surfaces\Stove.h"
#include "Components\Interactables\Holdables\Ingredient.h"
#include "Components\Interactables\Surfaces\IngredientContainer.h"
#include "Components\Interactables\Trashcan.h"
#include "Components\Interactables\Holdables\Skillet.h"
#include "Components\Interactables\Holdables\Pot.h"
#include "Components\Interactables\Surfaces\Counter.h"
#include "Components\Interactables\Surfaces\DeliveryDesk.h"
#include "Components\Interactables\Holdables\Plate.h"
#include "Components\Interactables\Surfaces\PlateHolder.h"

//environment
#include "Components/Environment/Teleporter.h"
#include "Components/Environment/MovingInteractable.h"
#include "Components/Environment/MovingInteractableItems.h"
#include "Components\Environment\Shark.h"
#include "Components\Environment\Sharkfin.h"
#include "Components\Environment\WalkingCustomers.h"
#include "Components\Environment\WalkingCustomer.h"
#include "Components\Environment\LineupCustomers.h"
#include "Components\Environment\LineupCustomer.h"
#include "Components\Environment\CloudSpawner.h"
#include "Components\Environment\RippleSpawner.h"
#include "Components\Environment\AutoFoam.h"

//MainMenu and Controller component
#include "Components\MainMenu\PlayerModelComponent.h"
#include "Components\MainMenu\NumberOfPlayersComponent.h"
#include "Components\MainMenu\MenuButton.h"
#include "Components\MainMenu\InteractableUIObjectComponent.h"
#include "Components\MainMenu\P1MenuComponent.h"
#include "Components\MainMenu\P2MenuComponent.h"
#include "Components/MainMenu/P1CustomizeMenuComponent.h"
#include "Components/MainMenu/P2CustomizeMenuComponent.h"
#include "Components/MainMenu/CounterComponent.h"
#include "Components\MainMenu\LevelCounterComponent.h"
#include "Components\MainMenu\PauseMenuComponent.h"
#include "Components\MainMenu\SettingsMenuComponent.h"
#include "Components\Scoreboard\Scoreboard.h"
#include "Components/MainMenu/CountdownComponent.h"

//Audio
#include "Components\Audio\AudioPlayer.h"
#include "Components\Audio\LevelSelectAudioPlayer.h"
#include "Components\Audio\TutorialBoardAudioPlayer.h"

//LevelSelect components
#include "Components\LevelSelect\LevelSelectPlayer.h"
#include "Components\LevelSelect\LevelSelectLevel.h"
#include "Components\LevelSelect\LevelSelectCamera.h"
#include "Components\LevelSelect\LevelSelectPreview.h"
#include "Components\LevelSelect\LevelSelectDataHandler.h"
#include "Components\LevelSelect\LevelNumber.h"



//The Man, the Myth, the Legend
#include "Components/BobSpinComponent.h"
#include "Components/ExplodeComponent.h"
#include "Components/MovingPlatform.h"
#include "Components/ProgressBar.h"

//Not too fond of settings not loading correctly
#include "ProjectSettings.h"

#include "Shaders/UI/Progressbar/UIProgressBarPS.h"
#include "Shaders/UI/CookingProgressBar/UICookingProgressBarPS.h"
#include "Shaders/UI/ErodingMenu/UIErodingMenuPS.h"
#include "Shaders/UI/Counter/UICounterPS.h"
#include "Shaders/UI/Timer/UITimerPS.h"
#include "Shaders/UI/Order/UIOrderPS.h"
#include "Shaders/UI/Points/UIPointsPS.h"
#include "Shaders/Interaction/HighlightPS.h"
#include "Shaders/Interaction/ProgressBarPS.h"
#include "Shaders/WaterStylizedPS.h"

#include "Components/Camera/DistanceBasedZoomCamera.h"


std::shared_ptr< LevelSelectManager> Project::levelSelectManager;
std::shared_ptr<GameManager> Project::gameManager;
std::shared_ptr<NetMessageHandler> Project::netMessageHandler;
std::shared_ptr<NetworkGameManager> Project::netGameManger;
std::shared_ptr<UIManager> Project::uiManager;
double Project::myUpdateFrameTime;

void Project::Setup()
{
#pragma region Components setup

	ComponentHandler::RegisterComponent<Player>();
	ComponentHandler::RegisterComponent<PlayerPlaceholder>();
	ComponentHandler::RegisterComponent<LevelSelectPlayer>();
	ComponentHandler::RegisterComponent<LevelSelectCamera>();
	ComponentHandler::RegisterComponent<LevelSelectLevel>();
	ComponentHandler::RegisterComponent<LevelSelectPreview>();
	ComponentHandler::RegisterComponent<LevelSelectDataHandler>();
	ComponentHandler::RegisterComponent<LevelNumber>();
	ComponentHandler::RegisterComponent<RayPickerComponent>();
	ComponentHandler::RegisterComponent<UISprite>();
	ComponentHandler::RegisterComponent<TutorialBoard>();
	ComponentHandler::RegisterComponent<SelfDestroy>();
	ComponentHandler::RegisterComponent<LevelTimer>();
	ComponentHandler::RegisterComponent<LevelCoinCounter>();
	ComponentHandler::RegisterComponent<OrderUI>();
	ComponentHandler::RegisterComponent<ProgressBar>();
	ComponentHandler::RegisterComponent<Scoreboard>();
	ComponentHandler::RegisterComponent<IngredientIcon>();
	ComponentHandler::RegisterComponent<BurnIndicator>();
	ComponentHandler::RegisterComponent<CoinCounterComponent>();
	ComponentHandler::RegisterComponent<TrashcanIcon>();
	ComponentHandler::RegisterComponent<FloatingScore>();

	//Interactables
	ComponentHandler::RegisterComponent<Stove>();
	ComponentHandler::RegisterComponent<CuttingBoard>();
	ComponentHandler::RegisterComponent<Counter>();
	ComponentHandler::RegisterComponent<Ingredient>();
	ComponentHandler::RegisterComponent<IngredientContainer>();
	ComponentHandler::RegisterComponent<Trashcan>();
	ComponentHandler::RegisterComponent<Skillet>();
	ComponentHandler::RegisterComponent<Pot>();
	ComponentHandler::RegisterComponent<Plate>();
	ComponentHandler::RegisterComponent<DeliveryDesk>();
	ComponentHandler::RegisterComponent<PlateHolder>();
	
	//Environment
	ComponentHandler::RegisterComponent<Teleporter>();
	ComponentHandler::RegisterComponent<MovingInteractable>();
	ComponentHandler::RegisterComponent<MovingInteractableItems>();
	ComponentHandler::RegisterComponent<Shark>();
	ComponentHandler::RegisterComponent<Sharkfin>();
	ComponentHandler::RegisterComponent<WalkingCustomers>();
	ComponentHandler::RegisterComponent<WalkingCustomer>();
	ComponentHandler::RegisterComponent<LineupCustomers>();
	ComponentHandler::RegisterComponent<LineupCustomer>();
	ComponentHandler::RegisterComponent<CloudSpawner>();
	ComponentHandler::RegisterComponent<RippleSpawner>();
	ComponentHandler::RegisterComponent<AutoFoam>();

	//MainMenu
	ComponentHandler::RegisterComponent<NumberOfPlayersComponent>();
	ComponentHandler::RegisterComponent<PlayerModelComponent>();
	ComponentHandler::RegisterComponent<MenuButton>();
	ComponentHandler::RegisterComponent<InteractableUIObjectComponent>();
	ComponentHandler::RegisterComponent<P1MenuComponent>();
	ComponentHandler::RegisterComponent<P2MenuComponent>();
	ComponentHandler::RegisterComponent<P1CustomizeMenuComponent>();
	ComponentHandler::RegisterComponent<P2CustomizeMenuComponent>();
	ComponentHandler::RegisterComponent<CounterComponent>();
	ComponentHandler::RegisterComponent<LevelCounterComponent>();
	ComponentHandler::RegisterComponent<PauseMenuComponent>();
	ComponentHandler::RegisterComponent<SettingsMenuComponent>();
	ComponentHandler::RegisterComponent<CountdownComponent>();

	ComponentHandler::RegisterComponent<DistanceBasedZoomCamera>();

	//Audio
	ComponentHandler::RegisterComponent<AudioPlayer>();
	ComponentHandler::RegisterComponent<LevelSelectAudioPlayer>();
	ComponentHandler::RegisterComponent<TutorialBoardAudioPlayer>();


	//Level editor dummy component for saving data
	ComponentHandler::RegisterComponent<KitchenWindowDummyComponent>();

	//kys
	ComponentHandler::RegisterComponent<BobSpinComponent>();
	ComponentHandler::RegisterComponent<MovingPlatform>();

#pragma endregion

	levelSelectManager = std::make_shared<LevelSelectManager>();
	gameManager = std::make_shared<GameManager>();
	uiManager = std::make_shared<UIManager>();

	Engine::GetInstance()->GetCollisionManager()->AddLayer("Player");
	Engine::GetInstance()->GetCollisionManager()->AddLayer("PlayerInteract");
	Engine::GetInstance()->GetCollisionManager()->AddLayer("AiFish");
	Engine::GetInstance()->GetCollisionManager()->AddLayer("Floor");
	Engine::GetInstance()->GetCollisionManager()->AddLayer("Ingredient");
	Engine::GetInstance()->GetCollisionManager()->AddLayer("IngredientInteract");
	ProjectSettings::LoadProjectSettings();

	levelSelectManager->Init();
	gameManager->Init();
	uiManager->Init();

	//shared_ptr<PixelShader> WaterStylizedPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/WaterStylizedPS.sh");
	std::string shaderData;
	HRESULT result;
	ifstream psFile;
	//psFile.open("Resources/BuiltIn/Shaders/WaterStylizedPS.cso", ios::binary);
	//shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	//result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &WaterStylizedPS->pixelShader);
	//WaterStylizedPS->ConstructConstantBuffers();
	//psFile.close();
	//assert(!FAILED(result) && "Loading WaterStylizedPS failed!");

	shared_ptr<PixelShader> FoamLinePS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/FoamLinePS.sh");
	psFile.open("Resources/BuiltIn/Shaders/FoamLinePS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &FoamLinePS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading FoamLinePS failed!");

	shared_ptr<PixelShader> LevelselectWaterPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/LevelselectWaterPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/LevelselectWaterPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &LevelselectWaterPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading LevelselectWaterPS failed!");

	shared_ptr<PixelShader> UnderWaterPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/UnderWaterPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/UnderWaterPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &UnderWaterPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading UnderWaterPS failed!");

	shared_ptr<PixelShader> LevelSelectUnderWaterPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/LevelSelectUnderWaterPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/LevelSelectUnderWaterPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &LevelSelectUnderWaterPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading LevelSelectUnderWaterPS failed!");

	shared_ptr<VertexShader> UnderWaterVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/UnderWaterVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/UnderWaterVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	UnderWaterVS->ConstructConstantBuffers();
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &UnderWaterVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading UnderWaterVS failed!");

	shared_ptr<PixelShader> FoodPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/FoodPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/FoodPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &FoodPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading FoodPS failed!");

	shared_ptr<PixelShader> BoatWavesPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/BoatWavesPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/BoatWavesPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	BoatWavesPS->ConstructConstantBuffers();
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &BoatWavesPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading BoatWavesPS failed!");

	shared_ptr<VertexShader> BoatWavesVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/BoatWavesVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/BoatWavesVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	BoatWavesVS->ConstructConstantBuffers();
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &BoatWavesVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading BoatWavesVS failed!");
	
	shared_ptr<PixelShader> BubblesPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/BubblesPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/BubblesPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &BubblesPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading BubblesPS failed!");

	shared_ptr<PixelShader> TeleportPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/TeleportPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/TeleportPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &TeleportPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading Teleport pixel shader failed!");

	shared_ptr<PixelShader> AutoFoamPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/AutoFoamPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/AutoFoamPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &AutoFoamPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading AutoFoamPSfailed!");



	shared_ptr<VertexShader> BubblesVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/BubblesVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/BubblesVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &BubblesVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading BubblesVS failed!");

	shared_ptr<VertexShader> TeleportVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/TeleportVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/TeleportVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &TeleportVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading Teleport vertex shader failed!");

	shared_ptr<GeometryShader> BubblesGS = CreateAsset<GeometryShader>("Resources/BuiltIn/Shaders/BubblesGS.sh");
	psFile.open("Resources/BuiltIn/Shaders/BubblesGS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateGeometryShader(shaderData.data(), shaderData.size(), nullptr, &BubblesGS->geometryShader);
	psFile.close();
	assert(!FAILED(result) && "Loading BubblesGS failed!");

	shared_ptr<PixelShader> WaterSplashRingPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/WaterSplashRingPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/WaterSplashRingPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &WaterSplashRingPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading WaterSplashRingPS failed!");

	shared_ptr<VertexShader> WaterSplashRingVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/WaterSplashRingVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/WaterSplashRingVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &WaterSplashRingVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading WaterSplashRingVS failed!");

	shared_ptr<GeometryShader> WaterSplashRingGS = CreateAsset<GeometryShader>("Resources/BuiltIn/Shaders/WaterSplashRingGS.sh");
	psFile.open("Resources/BuiltIn/Shaders/WaterSplashRingGS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateGeometryShader(shaderData.data(), shaderData.size(), nullptr, &WaterSplashRingGS->geometryShader);
	psFile.close();
	assert(!FAILED(result) && "Loading WaterSplashRingGS failed!");

	shared_ptr<PixelShader> PortalSuckInSnurrPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/PortalSuckInSnurrPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/PortalSuckInSnurrPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &PortalSuckInSnurrPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading PortalSuckInSnurrPS failed!");

	shared_ptr<PixelShader> CloudPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/CloudPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/CloudPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &CloudPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading CloudPS failed!");

	shared_ptr<VertexShader> PortalSuckInSnurrVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/PortalSuckInSnurrVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/PortalSuckInSnurrVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &PortalSuckInSnurrVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading PortalSuckInSnurrVS failed!");

	shared_ptr<GeometryShader> BirdsGS = CreateAsset<GeometryShader>("Resources/BuiltIn/Shaders/BirdsGS.sh");
	psFile.open("Resources/BuiltIn/Shaders/BirdsGS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateGeometryShader(shaderData.data(), shaderData.size(), nullptr, &BirdsGS->geometryShader);
	psFile.close();
	assert(!FAILED(result) && "Loading BirdsGS failed!");

	shared_ptr<VertexShader> FlagPoleWindVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/FlagPoleWindVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/FlagPoleWindVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &FlagPoleWindVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading FlagPoleWindVS failed!");

	shared_ptr<VertexShader> PalmtreeWindVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/PalmtreeWindVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/PalmtreeWindVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &PalmtreeWindVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading PalmtreeWindVS failed!");

	shared_ptr<VertexShader> WindVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/WindVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/WindVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &WindVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading WindVS failed!");

	shared_ptr<VertexShader> MarketstandWindVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/MarketstandWindVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/MarketstandWindVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &MarketstandWindVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading MarketstandWindVS failed!");

	shared_ptr<VertexShader> HatMovementVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/HatMovementVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/HatMovementVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &HatMovementVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading HatMovementVS failed!");

	shared_ptr<VertexShader> HatPropellerVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/HatPropellerVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/HatPropellerVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &HatPropellerVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading HatPropellerVS failed!");

	shared_ptr<VertexShader> CloudVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/CloudVS.sh");
	psFile.open("Resources/BuiltIn/Shaders/CloudVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &CloudVS->vertexShader);
	psFile.close();
	assert(!FAILED(result) && "Loading CloudVS failed!");

	shared_ptr<PixelShader> TAresearchPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/TAresearchPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/TAresearchPS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &TAresearchPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading TAresearchPS failed!");

	shared_ptr<HighlightPS> highlightPS = std::make_shared<HighlightPS>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/HighlightPS.sh", highlightPS);

	shared_ptr<UIProgressBarPS> uiProgressBarPS = std::make_shared<UIProgressBarPS>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/UIProgressBarPS.sh", uiProgressBarPS);

	shared_ptr<UICookingProgressBarPS> uiCookingProgressBarPS = std::make_shared<UICookingProgressBarPS>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/UICookingProgressBarPS.sh", uiCookingProgressBarPS);

	shared_ptr<UIErodingMenuPS> uiErodingMenuPS = std::make_shared<UIErodingMenuPS>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/UIErodingMenuPS.sh", uiErodingMenuPS);

	shared_ptr<UICounterPS> uiCounterPS = std::make_shared<UICounterPS>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/UICounterPS.sh", uiCounterPS);

	shared_ptr<UITimerPS> uiTimerPS = std::make_shared<UITimerPS>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/UITimerPS.sh", uiTimerPS);

	shared_ptr<UIOrderPS> uiOrderPS = std::make_shared<UIOrderPS>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/UIOrderPS.sh", uiOrderPS);

	shared_ptr<UIPointsPS> uiPointsPS = std::make_shared<UIPointsPS>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/UIPointsPS.sh", uiPointsPS);

	shared_ptr<ProgressBarPS> progressBarPS = std::make_shared<ProgressBarPS>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/ProgressBarPS.sh", progressBarPS);

	shared_ptr<WaterStylized> waterStylized = std::make_shared<WaterStylized>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/WaterStylizedPS.sh", waterStylized);

	if (!EDITORMODE)
	{	
		AssetLoader::LoadAsset("Assets/Scenes/SplashScreen.scene");
		AssetLoader::LoadAsset("Assets/UI/MainMenu/StartupImage.dds");
		AssetLoader::ThreadedLoading();

		auto& worldInstancing = AssetRegistry::GetInstance()->GetAsset<Scene>("SplashScreen");
		Engine::GetInstance()->GetSceneManager()->LoadScene(worldInstancing);
	}
}


void Project::Update()
{
	if (PLAYMODE)
	{
		PerformanceManager::BeginTimer("Project", "Whole Frame");

		PerformanceManager::BeginTimer("Game Manager", "Project");
		gameManager->Update();
		PerformanceManager::StopTimer("Game Manager");

		PerformanceManager::BeginTimer("UI Manager", "Project");
		uiManager->Update();
		PerformanceManager::StopTimer("UI Manager");

		PerformanceManager::BeginTimer("Level Select Manager", "Project");
		levelSelectManager->Update();
		PerformanceManager::StopTimer("Level Select Manager");
	
		PerformanceManager::StopTimer("Project");
	}
}

void Project::Start()
{
	GameManager::GetInstance()->ResetLevel();
}

double Project::RenderProjectFrameTime(float aTotalFrameTime)
{
	std::string data = std::to_string((myUpdateFrameTime) * 1000.0f) + " ms";
	bool open = ImGui::TreeNodeEx("Project Frame: ", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf);
	ImGui::SameLine();
	ImGui::Text(&data[0]);
	data = std::to_string((myUpdateFrameTime / aTotalFrameTime) * 100.0f) + " %%";
	ImGui::SameLine();
	ImGui::Text(&data[0]);
	if (open) 
	{
		ImGui::TreePop();
	}
	return myUpdateFrameTime;
}
