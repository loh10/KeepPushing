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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race|UI")
	float _countdownHideDelay = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Race")
	int32 _countdownStart = 3;

	int32 _currentCount;
	FTimerHandle _countdownTimer;

	void BeginCountdown();
	void UpdateCountdown();
	void FinishCountdown();

	void SetPlayerInputEnabled(bool enabled);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCountdownUpdated(const FText& countdownText);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCountdownGo();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCountdownHide();
	
	UFUNCTION()
	void TriggerCountdownHide();
private :
	int _offsetCountdown= 4;
};
