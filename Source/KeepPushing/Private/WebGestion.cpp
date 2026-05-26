// Fill out your copyright notice in the Description page of Project Settings.

#include "WebGestion.h"
#include "HttpModule.h"
#include "Json.h"
#include "JsonUtilities.h"

namespace
{
	// Some PHP/API setups prepend UTF-8 BOM; Unreal JSON parser expects JSON to start directly.
	static void SanitizeJsonResponse(FString& InOutJson)
	{
		if (!InOutJson.IsEmpty() && InOutJson[0] == 0xFEFF)
		{
			InOutJson.RightChopInline(1, false);
		}
	}
}

void UWebGestion::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Optionnel : utiliser une URL depuis les paramètres du projet
	// Vous pouvez la définir dans les paramètres du projet ou la modifier ici
	UE_LOG(LogTemp, Warning, TEXT("WebGestion initialized. Server: %s"), *ServerURL);
}

void UWebGestion::GetBDDLeaderboardAndCacheIt(int32 Niveau, int32 MaxLimit)
{
	// Si une requête pour ce niveau est déjà en cours, on évite d'en lancer une seconde identique.
	if (LoadingLevels.Contains(Niveau))
	{
		UE_LOG(LogTemp, Warning, TEXT("GetBDDLeaderboardAndCacheIt called but a request for level %d is already in progress. Ignoring duplicate call."), Niveau);
		return;
	}

	// Marquer le niveau comme en cours de chargement
	LoadingLevels.Add(Niveau);

	// Retour immédiat : on récupère la cache local pour l'instant et on lance la requête asynchrone.
	const int32 CachedCount = CachedLeaderboards.FindRef(Niveau).Num();
	UE_LOG(LogTemp, Warning, TEXT("GetBDDLeaderboardAndCacheIt called for level %d - returning cached data (%d scores). Loading started..."), Niveau, CachedCount);

	if (!FHttpModule::Get().IsHttpEnabled())
	{
		UE_LOG(LogTemp, Error, TEXT("HTTP module is not enabled!"));
		LoadingLevels.Remove(Niveau);
		OnLeaderboardLoaded.Broadcast(false, Niveau, CachedLeaderboards.FindRef(Niveau));
		return;
	}

	// Construire l'URL avec paramètres GET
	FString URL = FString::Printf(TEXT("%s/get_leaderboard.php?niveau=%d&limit=%d"), *ServerURL, Niveau, MaxLimit);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Enregistrer la requête en cours pour pouvoir retrouver le niveau dans le callback
	OngoingRequests.Add(Request, Niveau);

	// Bind le callback
	// Utiliser BindUObject car FHttpRequestCompleteDelegate n'est pas un délégué dynamique
	Request->OnProcessRequestComplete().BindUObject(this, &UWebGestion::OnLeaderboardResponse);

	UE_LOG(LogTemp, Warning, TEXT("Requesting leaderboard (level %d): %s"), Niveau, *URL);
	Request->ProcessRequest();
}

TArray<FLeaderboardScore> UWebGestion::GetLeaderboard(int32 Niveau) const
{
	// Retourne la copie du leaderboard en cache pour le niveau demandé
	const TArray<FLeaderboardScore>* Found = CachedLeaderboards.Find(Niveau);
	if (Found)
	{
		return *Found;
	}
	return TArray<FLeaderboardScore>();
}

TArray<FLeaderboardScore> UWebGestion::GetCachedLeaderboard(int32 Niveau) const
{
	const TArray<FLeaderboardScore>* Found = CachedLeaderboards.Find(Niveau);
	if (Found)
	{
		return *Found;
	}
	return TArray<FLeaderboardScore>();
}

int32 UWebGestion::GetCachedLeaderboardCount(int32 Niveau) const
{
	const TArray<FLeaderboardScore>* Found = CachedLeaderboards.Find(Niveau);
	return Found ? Found->Num() : 0;
}

bool UWebGestion::IsLeaderboardLoadingForLevel(int32 Niveau) const
{
	return LoadingLevels.Contains(Niveau);
}

void UWebGestion::SetPlayerInfo(const FString& Pseudo, const FString& Email)
{
	hasReset = false;
	StoredPseudo = Pseudo;
	StoredEmail = Email;
	UE_LOG(LogTemp, Warning, TEXT("Player info registered. Pseudo: %s, Email: %s"), *StoredPseudo, *StoredEmail);
}

void UWebGestion::SubmitScore(int32 Niveau, int32 Minutes, int32 Seconds, int32 Milliseconds)
{
	PassageCounter++;
	
	if (PassageCounter > 4)
	{
		PassageCounter = 0;
		hasReset = true;
	}

	
	if (!FHttpModule::Get().IsHttpEnabled())
	{
		UE_LOG(LogTemp, Error, TEXT("HTTP module is not enabled!"));
		OnScoreSubmitted.Broadcast(false, TEXT("HTTP disabled"));
		return;
	}

	// Vérifier que les informations du joueur sont enregistrées
	if (StoredPseudo.IsEmpty() || StoredEmail.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Player info not set! Call SetPlayerInfo() first."));
		OnScoreSubmitted.Broadcast(false, TEXT("Player info missing"));
		return;
	}

	// Calculer le temps en millisecondes
	int32 TotalTimeMs = Minutes * 60000 + Seconds * 1000 + Milliseconds;

	// Construire le JSON payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("pseudo"), StoredPseudo);
	JsonObject->SetStringField(TEXT("email"), StoredEmail);
	JsonObject->SetNumberField(TEXT("niveau"), Niveau);
	JsonObject->SetNumberField(TEXT("time_ms"), TotalTimeMs);

	// Sérialiser en JSON
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	// Créer la requête HTTP
	FString URL = FString::Printf(TEXT("%s/submit_score.php"), *ServerURL);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(JsonString);

	// Bind le callback
	// Utiliser BindUObject car FHttpRequestCompleteDelegate n'est pas un délégué dynamique
	Request->OnProcessRequestComplete().BindUObject(this, &UWebGestion::OnSubmitScoreResponse);

	UE_LOG(LogTemp, Warning, TEXT("Submitting score: Pseudo=%s, Email=%s, Niveau=%d, Time=%dms"), *StoredPseudo, *StoredEmail, Niveau, TotalTimeMs);
	Request->ProcessRequest();
}

void UWebGestion::OnLeaderboardResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TArray<FLeaderboardScore> Scores;

	// Déterminer le niveau associé à cette requête
	int32 Niveau = -1;
	if (int32* FoundLevel = OngoingRequests.Find(Request))
	{
		Niveau = *FoundLevel;
		OngoingRequests.Remove(Request);
	}

	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Leaderboard request failed for level %d!"), Niveau);
		if (Niveau != -1) LoadingLevels.Remove(Niveau);
		OnLeaderboardLoaded.Broadcast(false, Niveau, Scores);
		return;
	}

	int32 ResponseCode = Response->GetResponseCode();
	FString ResponseString = Response->GetContentAsString();
	SanitizeJsonResponse(ResponseString);

	UE_LOG(LogTemp, Warning, TEXT("Leaderboard Response Code: %d"), ResponseCode);
	UE_LOG(LogTemp, Warning, TEXT("Leaderboard Response: %s"), *ResponseString);

	if (ResponseCode != 200)
	{
		UE_LOG(LogTemp, Error, TEXT("Leaderboard HTTP error for level %d: %d"), Niveau, ResponseCode);
		if (Niveau != -1) LoadingLevels.Remove(Niveau);
		OnLeaderboardLoaded.Broadcast(false, Niveau, Scores);
		return;
	}

	// Parser le JSON
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse leaderboard JSON for level %d! Raw response: %s"), Niveau, *ResponseString);
		if (Niveau != -1) LoadingLevels.Remove(Niveau);
		OnLeaderboardLoaded.Broadcast(false, Niveau, Scores);
		return;
	}

	// Vérifier si la réponse est OK
	bool bOk = JsonObject->GetBoolField(TEXT("ok"));
	if (!bOk)
	{
		FString ErrorMsg = JsonObject->GetStringField(TEXT("error"));
		UE_LOG(LogTemp, Error, TEXT("Leaderboard error for level %d: %s"), Niveau, *ErrorMsg);
		if (Niveau != -1) LoadingLevels.Remove(Niveau);
		OnLeaderboardLoaded.Broadcast(false, Niveau, Scores);
		return;
	}

	// Extraire le tableau leaderboard
	TArray<TSharedPtr<FJsonValue>> LeaderboardArray = JsonObject->GetArrayField(TEXT("leaderboard"));

	for (int32 i = 0; i < LeaderboardArray.Num(); ++i)
	{
		TSharedPtr<FJsonObject> ScoreObj = LeaderboardArray[i]->AsObject();
		if (ScoreObj.IsValid())
		{
			FLeaderboardScore Score;
			Score.Rank = i + 1;
			Score.Pseudo = ScoreObj->GetStringField(TEXT("pseudo"));
			Score.Email = ScoreObj->GetStringField(TEXT("email"));
			Score.TotalTimeMs = (int32)ScoreObj->GetNumberField(TEXT("best_time_ms"));

			// Parser le sous-objet "time"
			TSharedPtr<FJsonObject> TimeObj = ScoreObj->GetObjectField(TEXT("time"));
			if (TimeObj.IsValid())
			{
				Score.Minutes = (int32)TimeObj->GetNumberField(TEXT("minutes"));
				Score.Seconds = (int32)TimeObj->GetNumberField(TEXT("seconds"));
				Score.Milliseconds = (int32)TimeObj->GetNumberField(TEXT("milliseconds"));
			}

			Scores.Add(Score);
		}
	}

	if (Niveau != -1)
	{
		CachedLeaderboards.Add(Niveau, Scores);
		LoadingLevels.Remove(Niveau);
	}
	UE_LOG(LogTemp, Warning, TEXT("Leaderboard loaded for level %d: %d scores"), Niveau, Scores.Num());
	UE_LOG(LogTemp, Warning, TEXT("=== LEADERBOARD DATA READY ==="));
	OnLeaderboardLoaded.Broadcast(true, Niveau, Scores);
}

void UWebGestion::OnSubmitScoreResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Submit score request failed!"));
		OnScoreSubmitted.Broadcast(false, TEXT("Request failed"));
		return;
	}

	int32 ResponseCode = Response->GetResponseCode();
	FString ResponseString = Response->GetContentAsString();
	SanitizeJsonResponse(ResponseString);

	UE_LOG(LogTemp, Warning, TEXT("Submit Score Response Code: %d"), ResponseCode);
	UE_LOG(LogTemp, Warning, TEXT("Submit Score Response: %s"), *ResponseString);

	if (ResponseCode != 200)
	{
		UE_LOG(LogTemp, Error, TEXT("Submit score HTTP error: %d"), ResponseCode);
		OnScoreSubmitted.Broadcast(false, FString::Printf(TEXT("HTTP Error: %d"), ResponseCode));
		return;
	}

	// Parser le JSON
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse submit score JSON!"));
		OnScoreSubmitted.Broadcast(false, TEXT("JSON parse error"));
		return;
	}

	// Vérifier si la réponse est OK
	bool bOk = JsonObject->GetBoolField(TEXT("ok"));
	if (bOk)
	{
		int32 ScoreId = (int32)JsonObject->GetNumberField(TEXT("score_id"));
		FString SuccessMsg = FString::Printf(TEXT("Score submitted! ID: %d"), ScoreId);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *SuccessMsg);
		OnScoreSubmitted.Broadcast(true, SuccessMsg);
	}
	else
	{
		FString ErrorMsg = JsonObject->GetStringField(TEXT("error"));
		UE_LOG(LogTemp, Error, TEXT("Submit score error: %s"), *ErrorMsg);
		OnScoreSubmitted.Broadcast(false, ErrorMsg);
	}
}