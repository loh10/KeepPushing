// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "WebGestion.generated.h"

// Structure pour un score du leaderboard
USTRUCT(BlueprintType)
struct FLeaderboardScore
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Leaderboard")
	int32 Rank = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Leaderboard")
	FString Pseudo = "";

	UPROPERTY(BlueprintReadOnly, Category = "Leaderboard")
	FString Email = "";

	UPROPERTY(BlueprintReadOnly, Category = "Leaderboard")
	int32 Minutes = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Leaderboard")
	int32 Seconds = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Leaderboard")
	int32 Milliseconds = 0;

	// Temps total en millisecondes
	UPROPERTY(BlueprintReadOnly, Category = "Leaderboard")
	int32 TotalTimeMs = 0;
};

// Delegate pour les callbacks : inclut le niveau demandé pour permettre plusieurs leaderboards simultanément
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLeaderboardLoaded, bool, bSuccess, int32, Niveau, const TArray<FLeaderboardScore>&, Scores);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreSubmitted, bool, bSuccess, const FString&, Message);

/**
 * 
 */
UCLASS()
class KEEPPUSHING_API UWebGestion : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// --- BLUEPRINT CALLABLE FUNCTIONS ---

	/**
	 * Lance une requête HTTP pour récupérer le leaderboard depuis la BDD et met en cache le résultat.
	 * Ne retourne rien immédiatement : après réception, l'événement OnLeaderboardLoaded sera déclenché.
	 * @param Niveau Le niveau du jeu (1, 2, etc.)
	 * @param MaxLimit Le nombre maximum de scores à récupérer (défaut 10)
	 */
	UFUNCTION(BlueprintCallable, Category = "KeepPushing|Web")
	void GetBDDLeaderboardAndCacheIt(int32 Niveau, int32 MaxLimit = 10);

	/**
	 * Retourne le leaderboard actuellement en cache (mis à jour par GetBDDLeaderboardAndCacheIt).
	 * Utile en Blueprint pour récupérer les données après avoir écouté OnLeaderboardLoaded.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KeepPushing|Web")
	TArray<FLeaderboardScore> GetLeaderboard(int32 Niveau) const;

	/**
	 * Enregistre les informations du joueur (pseudo et email)
	 * À appeler depuis le menu principal
	 * @param Pseudo Le pseudo du joueur
	 * @param Email L'email du joueur
	 */
	UFUNCTION(BlueprintCallable, Category = "KeepPushing|Web")
	void SetPlayerInfo(const FString& Pseudo, const FString& Email);

	/**
	 * Soumet un score au serveur avec les infos pré-enregistrées
	 * @param Niveau Le niveau complété
	 * @param Minutes Minutes du temps
	 * @param Seconds Secondes du temps
	 * @param Milliseconds Millisecondes du temps
	 */
	UFUNCTION(BlueprintCallable, Category = "KeepPushing|Web")
	void SubmitScore(int32 Niveau, int32 Minutes, int32 Seconds, int32 Milliseconds);

	// --- DELEGATES FOR BLUEPRINT ---
	UPROPERTY(BlueprintAssignable, Category = "KeepPushing|Web")
	FOnLeaderboardLoaded OnLeaderboardLoaded;

	UPROPERTY(BlueprintAssignable, Category = "KeepPushing|Web")
	FOnScoreSubmitted OnScoreSubmitted;

	// Getter pour accéder au dernier leaderboard chargé (cache local) - MISE À JOUR PROGRESSIVE
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KeepPushing|Web")
	TArray<FLeaderboardScore> GetCachedLeaderboard(int32 Niveau) const;
    
	// Obtient le nombre de scores en cache pour un niveau donné
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KeepPushing|Web")
	int32 GetCachedLeaderboardCount(int32 Niveau) const;
	
	// Vérifie si le leaderboard est actuellement en cours de chargement
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KeepPushing|Web")
	bool IsAnyLeaderboardLoading() const { return LoadingLevels.Num() > 0; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "KeepPushing|Web")
	bool IsLeaderboardLoadingForLevel(int32 Niveau) const;
	
	UFUNCTION(BlueprintCallable, Category = "KeepPushing|Web")
	bool GetReset() const { return hasReset; }

private:
	// --- INTERNAL HTTP CALLBACKS ---
	void OnLeaderboardResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnSubmitScoreResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// --- MEMBERS ---
	FString ServerURL = "http://localhost/api";
	// Cache par niveau
	TMap<int32, TArray<FLeaderboardScore>> CachedLeaderboards;
	
	// Informations du joueur pré-enregistrées
	FString StoredPseudo = "";
	FString StoredEmail = "";
	
	// Ensemble des niveaux actuellement en cours de chargement
	TSet<int32> LoadingLevels;

	// Map des requêtes HTTP en cours -> niveau demandé (pour associer la réponse)
	TMap<TSharedPtr<IHttpRequest, ESPMode::ThreadSafe>, int32> OngoingRequests;
	
	//Conteur de passage pour oblié la réauthentification
	int32 PassageCounter = 0;
	bool hasReset = true;
};