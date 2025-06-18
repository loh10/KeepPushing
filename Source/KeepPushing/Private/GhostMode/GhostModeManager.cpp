#include "GhostMode/GhostModeManager.h"

#include "Car/Car.h"
#include "GhostMode/GhostModeData.h"
#include "GhostMode/GhostTraceSaver.h"
#include "Kismet/GameplayStatics.h"
#include "StartAndFinish/StartAndFinish.h"
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

	CurrentMapName = GetWorld()->GetMapName();
	CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, CurrentMapName);

	//DeleteAllSaves();
	//DeleteSaveByName(CurrentMapName);
	LoadTracesPoints();
	InitializeGhostMode();

	TArray<AActor*> foundTriggers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStartAndFinish::StaticClass(), foundTriggers);
	for (AActor* actor : foundTriggers)
	{
		AStartAndFinish* trigger = Cast<AStartAndFinish>(actor);
		trigger->OnRaceEndSimple.AddDynamic(this, &AGhostModeManager::EndPlayTriggered);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Found Start/Finish Trigger"));
	}
}

void AGhostModeManager::EndPlayTriggered()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("End Play"));
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
		if (Trace->Time < currentTimer) {
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

void AGhostModeManager::DeleteSaveByName(const FString& SaveSlotName, int32 UserIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		if (UGameplayStatics::DeleteGameInSlot(SaveSlotName, UserIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Save '%s' deleted successfully."), *SaveSlotName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to delete save '%s'."), *SaveSlotName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save '%s' does not exist."), *SaveSlotName);
	}
}

void AGhostModeManager::DeleteAllSaves()
{
	const FString SaveDir = FPaths::ProjectSavedDir() + TEXT("SaveGames/");

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.DirectoryExists(*SaveDir))
	{
		TArray<FString> Files;
		PlatformFile.FindFiles(Files, *SaveDir, TEXT("*.sav"));

		for (const FString& File : Files)
		{
			if (PlatformFile.DeleteFile(*File))
			{
				UE_LOG(LogTemp, Warning, TEXT("Deleted save file: %s"), *File);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to delete: %s"), *File);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save directory does not exist: %s"), *SaveDir);
	}
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
