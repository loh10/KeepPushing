// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingUI.h"

#include "Kismet/GameplayStatics.h"
#include "StartAndFinish/StartAndFinish.h"
#include "Timer/Timer.h"

void UEndingUI::NativeConstruct()
{
    Super::NativeConstruct();
    TArray<AActor*> StartAndFinishGate;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStartAndFinish::StaticClass(), StartAndFinishGate);

    for (AActor* StartFinishActor : StartAndFinishGate)
    {
        if (AStartAndFinish* Gate = Cast<AStartAndFinish>(StartFinishActor))
        {
            Gate->OnTriggerEvent.AddDynamic(this, &UEndingUI::BindEndingEvent);
        }
    }
}

void UEndingUI::BindEndingEvent(const EStartFinishType Type)
{
    if (Type == EStartFinishType::Finish)
    {
        GetTimer();
        
    }
}

void UEndingUI::GetTimer()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATimer::StaticClass(),FoundActors);
    Timer = Cast<ATimer>(FoundActors[0]);
    if (Timer)
    {
        FString TimerText = Timer->GetFormattedTime();
        UE_LOG(LogTemp, Warning, TEXT("Timer: %s"), *TimerText);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Timer not found!"));
    }
}