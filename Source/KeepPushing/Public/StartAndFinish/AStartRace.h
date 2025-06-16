#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AStartRace.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceStartSimple);

UCLASS()
class KEEPPUSHING_API AAStartRace : public AActor
{
	GENERATED_BODY()

public:
	AAStartRace();

	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceStartSimple OnStart;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Race")
	int32 _countdownStart = 3;

	int32 _currentCount;
	FTimerHandle _countdownTimer;

	void BeginCountdown();
	void UpdateCountdown();
	void FinishCountdown();

	void SetPlayerInputEnabled(bool enabled);

	// (Optionnel) pour Widget
	UFUNCTION(BlueprintImplementableEvent)
	void OnCountdownUpdated(int32 secondsLeft);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCountdownGo();
};
