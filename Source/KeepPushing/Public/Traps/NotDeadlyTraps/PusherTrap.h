#pragma once

#include "CoreMinimal.h"
#include "NotDeadlyTrap.h"
#include "GameFramework/Actor.h"
#include "PusherTrap.generated.h"

UCLASS()
class KEEPPUSHING_API APusherTrap : public ANotDeadlyTrap
{
	GENERATED_BODY()

public:
	APusherTrap();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pusher")
	UStaticMeshComponent* _pusherMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pusher")
	float _basePushForce = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pusher")
	float _minPushForce = 700.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pusher")
	float _referenceSpeed = 1200.f; 	// Vitesse de référence (à calibrer)

	UFUNCTION()
	void OnPusherHit(
		UPrimitiveComponent* hitComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		FVector normalImpulse,
		const FHitResult& hit
	);
	
	virtual void AffectPlayer(AActor* player) override;
};
