#pragma once
struct Users
{
	int id;
	Transform transform;
	Vector3f color = { 1,1,1 };
};

class OnlineSceneManager
{
public:
	void Connect();
	void Render();
	void SendMovmentMessages();
	//void SendObjectMovmentMessages();
	void Update();
	void SendObjectSpawnMessages(GameObject* aGameObject);
	void SendAddComponent(Component* aComponent);

	void SendRotateMessages();

private:
	std::vector<Users> myUsers;
};

