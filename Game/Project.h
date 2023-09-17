#pragma once

class NetMessageHandler;
class GameManager;
class NetworkGameManager;
class UIManager;
class LevelSelectManager;
class Project
{
public:
	static void Setup();
	static void Update();
	static void Start();
	static double RenderProjectFrameTime(float aTotalFrameTime);
	static std::shared_ptr<LevelSelectManager> levelSelectManager;
	static std::shared_ptr<GameManager> gameManager;
	static std::shared_ptr<UIManager> uiManager;
	static std::shared_ptr<NetMessageHandler> netMessageHandler;
	static std::shared_ptr<NetworkGameManager> netGameManger;

	static double myUpdateFrameTime;
};