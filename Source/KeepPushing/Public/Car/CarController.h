#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StartAndFinish/StartAndFinish.h"
#include "CarController.generated.h"

class UTimerUserWidget;
class ATimer;

UCLASS()
class KEEPPUSHING_API ACarController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void BindStartAndStopTimer();

	UFUNCTION()
	void BindTriggerEvent(const EStartFinishType Type);
	
public:
	UPROPERTY(EditAnywhere, Category = "CarController|UI")
	TSubclassOf<UTimerUserWidget> TimerWidgetClass;
	
private:
	TObjectPtr<ATimer> Timer;
};
