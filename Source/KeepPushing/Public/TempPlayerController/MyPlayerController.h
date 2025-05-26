#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Timer/TimerComponent.h"
#include "Timer/TimerUserWidget.h"
#include "MyPlayerController.generated.h"

UCLASS()
class KEEPPUSHING_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Timer")
	UTimerComponent* TimerComp;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UTimerUserWidget> TimerWidgetClass;

private:
	UPROPERTY()
	UTimerUserWidget* TimerWidget;
};
