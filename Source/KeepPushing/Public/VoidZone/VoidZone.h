#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoidZone.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoidZoneOverlap, AActor*, OverlappedActor);

UCLASS()
class KEEPPUSHING_API AVoidZone : public AActor
{
	GENERATED_BODY()

public:
	AVoidZone();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* _triggerZone;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* overlappedComp,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex,
		bool bFromSweep,
		const FHitResult& sweepResult);

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnVoidZoneOverlap OnVoidZoneTouched;

	virtual void Tick(float DeltaTime) override;
};
