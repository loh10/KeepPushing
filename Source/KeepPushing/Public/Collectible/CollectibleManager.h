// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Collectible.h"
#include "CollectibleData.h"
#include "GameFramework/Actor.h"
#include "CollectibleManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdtatCollectionData, const FCollectibleData&, UpdatedCollectionData);

UCLASS(meta = (BlueprintSpawnableComponent))
class KEEPPUSHING_API ACollectibleManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACollectibleManager();
	
	UPROPERTY(BlueprintAssignable, Category = "Collectible")
	FUpdtatCollectionData UpdateCollectionData;

	UFUNCTION(BlueprintCallable, Category = "Collectible")
	int GetMaxCollectibleForLevel(UWorld* Level);

	UFUNCTION(BlueprintCallable, Category = "Collectible")
	int GetCollectibleForLevel(UWorld* Level);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void RegisterAllCollectibles();

	UFUNCTION()
	void OnCollectibleCollected(ACollectible* CollectedItem);
	void SaveCollectionData();
	void LoadCollectionData();

	UFUNCTION()
	void BindCollectibleEvents();

	UPROPERTY(EditAnywhere)
	int MaxCollectibles = 100;

	UPROPERTY(EditAnywhere)
	int ActualCollectibles = 0;

	UPROPERTY(EditAnywhere)
	FCollectibleData CollectionData;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collectibles")
	TMap<UWorld*, int> MaxCollectibleForMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collectibles")
	TArray<ACollectible*> AllCollectibles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
	TArray<UWorld*> LevelToIgnoreResearch;
};
