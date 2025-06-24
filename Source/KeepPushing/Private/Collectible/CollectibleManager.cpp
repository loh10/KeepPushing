// Fill out your copyright notice in the Description page of Project Settings.


#include "Collectible/CollectibleManager.h"

#include "EngineUtils.h"
#include "Collectible/CollectibleSave.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ACollectibleManager::ACollectibleManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

int ACollectibleManager::GetMaxCollectibleForLevel(UWorld* Level)
{
	return MaxCollectibleForMap[Level];
}

int ACollectibleManager::GetCollectibleForLevel(UWorld* Level)
{
	UCollectibleSave* LoadGameInstance = Cast<UCollectibleSave>(UGameplayStatics::LoadGameFromSlot("GameCollectibleSaver", 1));
	FCollectibleData Data = LoadGameInstance->SavedCollectionData[Level->GetCurrentLevel()->GetName()];
	return Data.ActualCollectibles;
}

// Called when the game starts or when spawned
void ACollectibleManager::BeginPlay()
{
	Super::BeginPlay();

	UWorld* CurrentLevel = GetWorld();
	
	if (LevelToIgnoreResearch.Contains(CurrentLevel))
	{
		LoadCollectionData();
	}
}

void ACollectibleManager::BindCollectibleEvents()
{
	for (ACollectible* Collectible : AllCollectibles)
	{
		if (Collectible)
		{
			Collectible->Collected.AddDynamic(this, &ACollectibleManager::OnCollectibleCollected);
		}
	}
}

void ACollectibleManager::RegisterAllCollectibles()
{
	// Get the current level
	UWorld* CurrentLevel = GetWorld();

	// Check if the current level is in the ignore list
	if (LevelToIgnoreResearch.Contains(CurrentLevel))
	{
		UE_LOG(LogTemp, Warning, TEXT("Current level is in the ignore list. Skipping collectible registration."));
		return;
	}

	// Clear the array before adding new collectibles
	AllCollectibles.Empty();

	// Iterate through all ACollectible actors in the level
	for (TActorIterator<ACollectible> It(GetWorld()); It; ++It)
	{
		if (ACollectible* Collectible = *It)
		{
			AllCollectibles.Add(Collectible);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Registered %d collectibles."), AllCollectibles.Num());
	
	BindCollectibleEvents();
}

// Called every frame
void ACollectibleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACollectibleManager::OnCollectibleCollected(ACollectible* CollectedItem)
{
	MaxCollectibles = AllCollectibles.Num();
	ActualCollectibles ++;
	CollectionData.MaxCollectibles = MaxCollectibles;
	CollectionData.ActualCollectibles = ActualCollectibles;
	UpdateCollectionData.Broadcast(CollectionData);
}

void ACollectibleManager::SaveCollectionData()
{
	// Créer ou charger un objet de sauvegarde
	UCollectibleSave* SaveGameInstance = Cast<UCollectibleSave>(UGameplayStatics::CreateSaveGameObject(UCollectibleSave::StaticClass()));

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Impossible de créer l'objet de sauvegarde."));
		return;
	}

	// Copier les données dans l'objet de sauvegarde
	SaveGameInstance->SavedCollectionData.Add(GetWorld()->GetCurrentLevel()->GetName(), CollectionData);

	// Sauvegarder sur le disque
	if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, "GameCollectibleSaver", 1))
	{
		UE_LOG(LogTemp, Log, TEXT("Données sauvegardées avec succès."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Échec de la sauvegarde des données."));
	}
}

void ACollectibleManager::LoadCollectionData()
{
    // Charger l'objet de sauvegarde depuis le disque
    UCollectibleSave* LoadGameInstance = Cast<UCollectibleSave>(UGameplayStatics::LoadGameFromSlot("GameCollectibleSaver", 1));

    if (LoadGameInstance)
    {
        // Vérifier si les données pour le niveau actuel existent
        if (LoadGameInstance->SavedCollectionData.Contains(GetWorld()->GetCurrentLevel()->GetName()))
        {
            // Copier les données chargées dans la struct
            CollectionData = LoadGameInstance->SavedCollectionData[GetWorld()->GetCurrentLevel()->GetName()];

            // Mettre à jour les valeurs locales
            MaxCollectibles = CollectionData.MaxCollectibles;
            ActualCollectibles = CollectionData.ActualCollectibles;

        	UpdateCollectionData.Broadcast(CollectionData);

            UE_LOG(LogTemp, Log, TEXT("Données chargées avec succès."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Aucune donnée trouvée pour le niveau actuel."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Aucune sauvegarde trouvée."));
    }
}

