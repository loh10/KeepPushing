#include "Timer/Timer.h"

#include "Blueprint/UserWidget.h"
#include "Math/UnrealMathUtility.h"
#include "Timer/TimerUserWidget.h"

ATimer::ATimer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATimer::BeginPlay()
{
	Super::BeginPlay();	
	Elapsed = 0.f;
	bRunning = false;
}

void ATimer::StartTimer() { bRunning = true; }
void ATimer::StopTimer()  { bRunning = false; }
void ATimer::ResetTimer() { Elapsed = 0.f; }

void ATimer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bRunning)
	{
		Elapsed += DeltaTime;
	}
}

FString ATimer::GetFormattedTime() const
{
	int32 totalCentiseconds = FMath::FloorToInt(Elapsed * 1000);
	int32 TotalSec = totalCentiseconds / 1000;
	
	int32 Min = TotalSec / 60;
	int32 Sec = TotalSec % 60;
	int32 Centi = totalCentiseconds % 1000;

	return FString::Printf(TEXT("%02d:%02d:%03d"), Min, Sec, Centi);
}
