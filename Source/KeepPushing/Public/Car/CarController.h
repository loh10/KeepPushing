#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StartAndFinish/StartAndFinish.h"
#include "CarController.generated.h"

class UEndingUI;
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


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CarController|UI")
	TSubclassOf<UEndingUI> EndingUIClass;

	UPROPERTY()
	UEndingUI* EndingUIInstance;

	void ShowEndingUI();
	void HideEndingUI();

    
private:
	TObjectPtr<ATimer> Timer;

    
};