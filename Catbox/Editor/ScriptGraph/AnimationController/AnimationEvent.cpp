#include "stdafx.h"
#include "AnimationEvent.h"
#include "Animator.h"
#include "Assets\AnimationController.h"
#include <Audio\AudioMonoComponent.h>

void AnimationEvent::Execute(Animator* anActor)
{
	if (myType == AnimationEventType::Prefab && myPrefabData.prefab)
	{
		auto newObject = InstantiatePrefab(myPrefabData.prefab);
		if (myPrefabData.worldSpace)
		{
			newObject->GetTransform()->SetWorldPos(myPrefabData.position);
		}
		else
		{
			if (myPrefabData.bone)
			{
				auto& boneTransform = anActor->GetSkeleton()->boneTransforms[myPrefabData.bone->id];
				Vector3f x = boneTransform.right() * myPrefabData.position.x;
				Vector3f y = boneTransform.up() * myPrefabData.position.y;
				Vector3f z = boneTransform.forward() * myPrefabData.position.z;
				newObject->GetTransform()->SetWorldPos(boneTransform.worldPos() + x + y + z);
				newObject->GetTransform()->Translate(myPrefabData.position);

			}
			else
			{
				Transform* t = anActor->GetTransform();
				newObject->GetTransform()->SetWorldPos(t->worldPos());
				newObject->GetTransform()->Translate(t->right() * myPrefabData.position.x);
				newObject->GetTransform()->Translate(t->up() * myPrefabData.position.y);
				newObject->GetTransform()->Translate(t->forward() * myPrefabData.position.z);
			}
		}

		if (myPrefabData.useBoneRotation && myPrefabData.bone)
		{
			auto& boneTransform = anActor->GetSkeleton()->boneTransforms[myPrefabData.bone->id];
			newObject->GetTransform()->SetWorldRot(boneTransform.worldRot());
		}
		newObject->GetTransform()->Rotate(myPrefabData.rotation);
	}
	else if (myType == AnimationEventType::PlaySound)
	{
		AudioMonoComponent* audioPlayer = anActor->GetGameObject().GetComponent<AudioMonoComponent>();
		if (audioPlayer)
		{
			anActor->GetGameObject().GetComponent<AudioMonoComponent>()->PlaySoundEffectAsOneShot(mySoundData.sfxIndex);
		}
		else
		{
			Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByID(mySoundData.sfxIndex);
			printerror("Tried to play sound on " + anActor->GetGameObject().GetName() + " but it has no AudioMonoComponent!");
		}
	}
}

static std::string searchTerm;

void AnimationEvent::RenderInProperties(AnimationController* aController)
{
	ImGui::Text(("Event " + std::to_string(myID)).c_str());
	ImGui::Spacing();
	ImGui::SetNextItemWidth(100);
	int frame = myFrame;
	if (Catbox::InputInt(("Frame##" + std::to_string(myID)).c_str(), &frame, 1, 10))
	{
		if (frame < 0) frame = 0;
		myFrame = frame;
	}

	const char* types[]
	{
		"Prefab",
		"Sound"
	};

	int typeIndex = static_cast<int>(myType);
	ImGui::SetNextItemWidth(100);
	if (ImGui::Combo(("Event Type##" + std::to_string(myID)).c_str(), &typeIndex, types, 2))
	{
		myType = (AnimationEventType)typeIndex;
	}


	ImGui::SetNextItemWidth(100);
	if (myType == AnimationEventType::Prefab)
	{
		bool edit = false;
		auto newPrefab = AssetRegistry::GetInstance()->AssetDropdownMenu<Prefab>(myPrefabData.prefab.get(), ("Prefab##" + std::to_string(myID)).c_str(), edit);
		if (edit)
		{
			myPrefabData.prefab = newPrefab;
		}

		Catbox::Checkbox(("World Space##" + std::to_string(myID)).c_str(), &myPrefabData.worldSpace);
		std::string posLabel = "Position";
		if (!myPrefabData.worldSpace) posLabel += " offset";

		std::string rotLabel = "Rotation";
		if (myPrefabData.useBoneRotation) rotLabel += " offset";


		Catbox::DragFloat3((posLabel + std::to_string(myID)).c_str(), &myPrefabData.position);
		Catbox::Checkbox(("Inherit bone rotation##" + std::to_string(myID)).c_str(), &myPrefabData.useBoneRotation);
		Catbox::DragFloat3((rotLabel + std::to_string(myID)).c_str(), &myPrefabData.rotation);

		if (!myPrefabData.worldSpace)
		{
			auto& skeleton = aController->GetSkeleton();
			int index = 0;
			for (size_t i = 0; i < skeleton->bones.size(); i++)
			{
				if (myPrefabData.bone)
				{
					if (skeleton->bones[i].id == myPrefabData.bone->id) index = static_cast<int>(i) + 1;
				}
			}

			if (ImGui::BeginCombo(("Bone##" + std::to_string(myID)).c_str(), index == 0 ? "None" : skeleton->boneNames[index - 1].c_str()))
			{
				if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
					ImGui::SetKeyboardFocusHere(0);
				ImGui::InputText("Search", &searchTerm);

				if (ImGui::Selectable("None", false))
				{
					myPrefabData.bone = nullptr;
				}

				for (int i = 0; i < skeleton->boneNames.size(); i++)
				{
					if (Catbox::ToLowerString(skeleton->boneNames[i]).find(Catbox::ToLowerString(searchTerm)) != std::string::npos)
					{
						if (ImGui::Selectable(skeleton->boneNames[i].c_str(), false))
						{
							myPrefabData.bone = &skeleton->bones[i];
						}
					}
				}

				ImGui::EndCombo();
			}
		}
	}
	else if (myType == AnimationEventType::PlaySound)
	{
		Catbox::InputInt(("Sfx Index##" + std::to_string(myID)).c_str(), &mySoundData.sfxIndex);
	}
}
