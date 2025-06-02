#pragma once

#include "CoreMinimal.h"
#include "GhostModeData.h"
#include "GameFramework/Actor.h"
#include "GhostModeManager.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class KEEPPUSHING_API AGhostModeManager : public AActor
{
	GENERATED_BODY()

public:
	AGhostModeManager();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "GhostMode")
	void InitializeGhostMode();

	UFUNCTION(BlueprintCallable, Category = "GhostMode")
	void SaveCurrentTracePoint(const float currentTimer);
	
	UFUNCTION(BlueprintCallable, Category = "GhostMode")
	void LoadTracesPoints();

	UFUNCTION(BlueprintCallable, Category = "GhostMode")
	void SaveTracesPoints();
private:
	void FollowCar(const float deltaTime);

	void RegisterSplinePoint(const float deltaTime);
	
public:
	UPROPERTY(EditAnywhere, Category = "GhostMode")
	TSubclassOf<AActor> GhostCar;

	UPROPERTY(EditAnywhere, Category = "GhostMode", meta = (ClampMin = 0.001, ClampMax = 1.f))
	float RegisterPointDelay = .1f;
	
	UPROPERTY(EditAnywhere, Category = "GhostMode")
	FString CurrentMapName;

	UPROPERTY(BlueprintReadWrite, Category = "GhostMode")
	TWeakObjectPtr<AActor> FollowedCar;
	
	TMap<FString, struct FTrace> SaveRacePoints;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> _ghostCarPtr;

	TArray<FGhostModePoint> _currentPoints;

	TArray<FGhostModePoint> _followGhostCarPoint;

	float _currentSplineKey = 0.f;

	float _currentRegisterPointDelay = .1f;

	float _currentRegisterSplineKey = 0.f;

	int32 _currentFollowPoint = 0;

	float _currentTimeMove = 0.f;

	FVector _startLocation;

	FRotator _startRotation;
};
