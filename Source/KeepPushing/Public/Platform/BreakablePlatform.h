#pragma once

#include "CoreMinimal.h"
#include "Platform.h"
#include "GameFramework/Actor.h"
#include "BreakablePlatform.generated.h"



UCLASS()
class KEEPPUSHING_API ABreakablePlatform : public APlatform
{
	GENERATED_BODY()

public:
	ABreakablePlatform();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Breakable")
	float timerToDestroy = 3.f;
	
	float currentTimer = 0.f;

	
	bool isActivated = false; // Plateforme activée (contact joueur)

	
	void StartBlinking(); // Clignotement etc. à gérer (peut être un bool pour clignoter)

	
	UPROPERTY(EditAnywhere, Category="Breakable")
	USoundBase* warningSound; // Son à jouer (référence à un son à configurer dans Blueprint)

	
	UFUNCTION()
	void OnPlayerContact(); // Fonction appelée lors du contact joueur

	
	void DestroyPlatform(); // Destruction plateforme

public:
	
	UFUNCTION()
	void NotifyPlayerContact(); // Appelé quand joueur entre en collision (à brancher au collider)
};
