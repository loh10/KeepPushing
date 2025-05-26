#include "TempPlayerController/MyPlayerController.h"

AMyPlayerController::AMyPlayerController()
{
	TimerComp = CreateDefaultSubobject<UTimerComponent>(TEXT("TimerComponent"));
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (TimerComp)
	{
		TimerComp->StartTimer();
	}

	if (TimerWidgetClass)
	{
		TimerWidget = CreateWidget<UTimerUserWidget>(this, TimerWidgetClass);
		if (TimerWidget)
		{
			TimerWidget->AddToViewport();
			TimerWidget->InitTimer(TimerComp);
		}
	}
}
