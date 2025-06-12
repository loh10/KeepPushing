#pragma once

#include "CoreMinimal.h"
#include "NotDeadlyTrap.h"
#include "GameFramework/Actor.h"
#include "JumperTrap2.generated.h"

UCLASS()
class KEEPPUSHING_API AJumperTrap2 : public ANotDeadlyTrap
{
	GENERATED_BODY()

public:
	AJumperTrap2();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	UBoxComponent* _trigger;
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumperTrap")
	float _jumpForce = 2000.f;

	virtual void AffectPlayer(AActor* actor) override;

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
