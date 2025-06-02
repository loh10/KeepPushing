#include "Car/CarController.h"

#include "Kismet/GameplayStatics.h"
#include "StartAndFinish/StartAndFinish.h"
#include "Timer/TimerUserWidget.h"

void ACarController::BeginPlay()
{
	Super::BeginPlay();

	UTimerUserWidget* Widget = Cast<UTimerUserWidget>(CreateWidget(this, TimerWidgetClass));
	Widget->AddToViewport();

	Timer = GetWorld()->SpawnActor<ATimer>();
	Widget->InitTimer(Timer);
	BindStartAndStopTimer();
}

void ACarController::BindStartAndStopTimer()
{
	TArray<AActor*> StartAndFinishGate;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStartAndFinish::StaticClass(), StartAndFinishGate);

	for (AActor* StartFinishActor : StartAndFinishGate)
	{
		if (AStartAndFinish* Gate = Cast<AStartAndFinish>(StartFinishActor))
		{
			Gate->OnTriggerEvent.AddDynamic(this, &ACarController::BindTriggerEvent);
		}
	} 
}

void ACarController::BindTriggerEvent(const EStartFinishType Type)
{
	switch (Type)
	{
	case EStartFinishType::Start:
		{
			Timer->StartTimer();
			break;
		}
	case EStartFinishType::Finish:
		{
			Timer->StopTimer();
			break;
		}
	}
}

