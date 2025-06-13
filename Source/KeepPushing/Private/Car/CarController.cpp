#include "Car/CarController.h"

#include "EndingUI.h"
#include "Kismet/GameplayStatics.h"
#include "StartAndFinish/StartAndFinish.h"
#include "Timer/TimerUserWidget.h"

ACarController::ACarController()
{
    static ConstructorHelpers::FClassFinder<UEndingUI> WidgetClassFinder(TEXT("/Game/EndUI"));
    if (WidgetClassFinder.Succeeded())
    {
        EndingUIClass = WidgetClassFinder.Class;
    }
}

void ACarController::BeginPlay()
{
    Super::BeginPlay();

    UTimerUserWidget* Widget = Cast<UTimerUserWidget>(CreateWidget(this, TimerWidgetClass));
    Widget->AddToViewport();

    Timer = GetWorld()->SpawnActor<ATimer>();
    Widget->InitTimer(Timer);
    BindStartAndStopTimer();

    if (EndingUIClass)
    {
        EndingUIInstance = CreateWidget<UEndingUI>(this, EndingUIClass);
        if (EndingUIInstance)
        {
            EndingUIInstance->AddToViewport();
            HideEndingUI();
        }
    }
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
            ShowEndingUI();
            break;
        }
    }
}

void ACarController::ShowEndingUI()
{
    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);

    SetInputMode(InputMode);
    
    if (EndingUIInstance && EndingUIInstance->IsInViewport())
    {
        EndingUIInstance->SetVisibility(ESlateVisibility::Visible);
        InputMode.SetWidgetToFocus(EndingUIInstance->TakeWidget());
        SetInputMode(InputMode);
    }

}

void ACarController::HideEndingUI()
{
    if (EndingUIInstance && EndingUIInstance->IsInViewport())
    {
        EndingUIInstance->SetVisibility(ESlateVisibility::Hidden);
    }
}