#include "Game.pch.h"
#include "Trashcan.h"
#include "Holdables\HoldableItem.h"
#include "Holdables\Ingredient.h"
#include "Holdables\Skillet.h"
#include "Holdables\Plate.h"
#include "Components\3D\VertexAnimator.h"
#include "Components/Physics/Collisions/Collider.h"

void Trashcan::InteractWithHeldItem(Player* aPlayer, HoldableItem* anitem)
{
	if (!aPlayer) return;
	HoldableItem* item = aPlayer->GetHeldItem();
	if (Ingredient* ingredient = dynamic_cast<Ingredient*>(item))
	{
		aPlayer->SetHeldItem(nullptr);
		ingredient->GetGameObject().Destroy();
		Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_DISPOSE_FOOD));
	}
	else if (Skillet* skillet = dynamic_cast<Skillet*>(item))
	{
		if (Ingredient* ingredient = skillet->TakeFood())
		{
			ingredient->GetGameObject().Destroy();
			Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_DISPOSE_FOOD));
		}
	}
	else if (Plate* plate = dynamic_cast<Plate*>(item))
	{
		plate->Clear();
		Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(std::string(AUDIO_DISPOSE_FOOD));
	}
	auto vertexAnimator = myGameObject->GetComponent<VertexAnimator>();
	if (vertexAnimator)
	{
		vertexAnimator->PlayVAT(1);
		vertexAnimator->PlayVAT(0, 1.50f);
	}

}