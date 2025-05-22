#pragma once

#include "CoreMinimal.h"
#include "Platform.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"








UCLASS()
class KEEPPUSHING_API AMovingPlatform : public APlatform
{
	GENERATED_BODY()

public:
	AMovingPlatform();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving")
	EAxisMovement axisToMove = EAxisMovement::X;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving")
	float offsetTarget = 500.f; // distance de déplacement

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving")
	float movementSpeed = 5.f; // durée aller-retour

	
	float direction = 1.f; // 1 ou -1 pour aller et retour
	
	
	float moveTimer = 0.f;

	
	void Move(float deltaTime);
};
