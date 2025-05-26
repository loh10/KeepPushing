#include "Timer/TimerComponent.h"
#include "Math/UnrealMathUtility.h"

UTimerComponent::UTimerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTimerComponent::BeginPlay()
{
	Super::BeginPlay();
	Elapsed = 0.f;
	bRunning = false;
}

void UTimerComponent::StartTimer() { bRunning = true; }
void UTimerComponent::StopTimer()  { bRunning = false; }
void UTimerComponent::ResetTimer() { Elapsed = 0.f; }

void UTimerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bRunning)
	{
		Elapsed += DeltaTime;
	}
}

FString UTimerComponent::GetFormattedTime() const
{
	int32 TotalSec = FMath::FloorToInt(Elapsed);
	int32 Min = TotalSec / 60;
	int32 Sec = TotalSec % 60;
	return FString::Printf(TEXT("%02d:%02d"), Min, Sec);
}
