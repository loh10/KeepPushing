#pragma once

#include "CoreMinimal.h"
#include "NotDeadlyTrap.h"
#include "GameFramework/Actor.h"
#include "JumperTrap.generated.h"

UCLASS()
class KEEPPUSHING_API AJumperTrap : public ANotDeadlyTrap
{
	GENERATED_BODY()

public:
	AJumperTrap();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	UBoxComponent* _trigger;
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumperTrap")
	float _jumpForce = 2000.f;

	virtual void AffectPlayer(AActor* player) override;

	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* overlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex,
		bool bFromSweep,
		const FHitResult& sweepResult
	);
};
