#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LifeZone.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoidZoneOverlap, AActor*, OverlappedActor);

UCLASS()
class KEEPPUSHING_API ALifeZone : public AActor
{
	GENERATED_BODY()

public:
	ALifeZone();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* _triggerZone;

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* overlappedComp,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex);

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnVoidZoneOverlap OnVoidZoneTouched;

	virtual void Tick(float DeltaTime) override;
};
