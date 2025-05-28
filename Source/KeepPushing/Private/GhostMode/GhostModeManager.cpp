#include "GhostMode/GhostModeManager.h"

#include "Car/Car.h"
#include "GhostMode/GhostModeData.h"
#include "GhostMode/GhostTraceSaver.h"
#include "Kismet/GameplayStatics.h"
#include "Timer/Timer.h"

namespace GhostModeManagerConstant
{
	static const FString SaverFileName = "GhostTraceSaver";
	static constexpr int32 SaverUserIndex = 0;
}

UE_DISABLE_OPTIMIZATION

AGhostModeManager::AGhostModeManager()
{
	PrimaryActorTick.bCanEverTick = true;
	_currentRegisterPointDelay = RegisterPointDelay;
}

void AGhostModeManager::BeginPlay()
{
	Super::BeginPlay();
	FollowedCar = UGameplayStatics::GetActorOfClass(this, ACar::StaticClass());
	
	LoadTracesPoints();
	InitializeGhostMode();
}

void AGhostModeManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (const ATimer* Timer = Cast<ATimer>(UGameplayStatics::GetActorOfClass(this, ATimer::StaticClass()))) {
		const float CurrentTimerTime = Timer->GetElapsed();
		if (CurrentTimerTime == 0.f)
		{
			return;
		}

		SaveCurrentTracePoint(CurrentTimerTime);
		SaveTracesPoints();
	}
}

void AGhostModeManager::InitializeGhostMode()
{
	if (!FollowedCar.IsValid()) {
		return;
	}
	
	if (const FTrace* Trace = SaveRacePoints.Find(CurrentMapName))
	{
		_followGhostCarPoint = Trace->Points;
		SetActorTransform(FollowedCar->GetTransform());

		_ghostCarPtr = GetWorld()->SpawnActor<AActor>(GhostCar, FollowedCar->GetActorTransform());
		_startLocation = _ghostCarPtr->GetActorLocation();
		_startRotation = _ghostCarPtr->GetActorRotation();
	}
}

void AGhostModeManager::SaveCurrentTracePoint(const float currentTimer)
{
	if (const FTrace* Trace = SaveRacePoints.Find(CurrentMapName)) {
		if (Trace->Time > currentTimer) {
			return;
		}
	}
	
	SaveRacePoints.Add(CurrentMapName, FTrace(_currentPoints, currentTimer));
}

void AGhostModeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!FollowedCar.IsValid()) {
		return;
	}
	
	RegisterSplinePoint(DeltaTime);

	if (IsValid(_ghostCarPtr)) {
		_followGhostCarPoint = SaveRacePoints[CurrentMapName].Points;
		FollowCar(DeltaTime);
	}

	for (auto& p : _followGhostCarPoint)
	{
		DrawDebugLine(GetWorld(), p.Position, p.Position + FVector::UpVector * 100.f, FColor::Red);
	}
}

void AGhostModeManager::FollowCar(const float deltaTime)
{
	if (!_followGhostCarPoint.IsValidIndex(_currentFollowPoint))
	{
		return;
	}

	_currentTimeMove = FMath::Min(_currentTimeMove + (deltaTime / RegisterPointDelay), 1.f);

	const FVector targetLocation = _followGhostCarPoint[_currentFollowPoint].Position;
	const FRotator targetRotation = _followGhostCarPoint[_currentFollowPoint].Rotator;
	_ghostCarPtr->SetActorLocation(FMath::Lerp(_startLocation, targetLocation, _currentTimeMove));
	_ghostCarPtr->SetActorRotation(FMath::Lerp(_startRotation, targetRotation, _currentTimeMove));

	if (_currentTimeMove >= 1.f)
	{
		_currentFollowPoint++;
		_currentTimeMove = 0.f;

		_startLocation = _ghostCarPtr->GetActorLocation();
		_startRotation = _ghostCarPtr->GetActorRotation();
	}
}

void AGhostModeManager::RegisterSplinePoint(const float deltaTime)
{
	_currentRegisterPointDelay -= deltaTime;
	if (_currentRegisterPointDelay < 0.f)
	{
		const FTransform cTransform = FollowedCar->GetActorTransform();
		_currentPoints.Add(FGhostModePoint(cTransform));

		_currentRegisterPointDelay = RegisterPointDelay;
	}

	_currentRegisterSplineKey += deltaTime;
}

void AGhostModeManager::LoadTracesPoints()
{
	if (UGameplayStatics::DoesSaveGameExist(GhostModeManagerConstant::SaverFileName,
										GhostModeManagerConstant::SaverUserIndex))
	{
		UGhostTraceSaver* Loaded = Cast<UGhostTraceSaver>(
			UGameplayStatics::LoadGameFromSlot(GhostModeManagerConstant::SaverFileName,
											   GhostModeManagerConstant::SaverUserIndex));

		SaveRacePoints.Empty();
		for (const auto& [Name, Trace] : Loaded->TracesPoints)
		{
			SaveRacePoints.Add(Name, Trace);
		}
	}
}

void AGhostModeManager::SaveTracesPoints()
{
	UGhostTraceSaver* Saver = Cast<UGhostTraceSaver>(
		UGameplayStatics::CreateSaveGameObject(UGhostTraceSaver::StaticClass()));

	for (const auto& [Name, Point] : SaveRacePoints)
	{
		Saver->TracesPoints.Add(Name, Point);	
	}
	UGameplayStatics::SaveGameToSlot(Saver, GhostModeManagerConstant::SaverFileName,
									 GhostModeManagerConstant::SaverUserIndex);
}

UE_ENABLE_OPTIMIZATION
