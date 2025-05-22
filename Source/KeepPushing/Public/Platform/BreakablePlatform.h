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
	float _timerToDestroy = 3.f;
	float _currentTimer = 0.f;
	float blinkTimer = 0.f;
	
	bool _isActivated = false;
	bool _isBlinking = false;

	
	void StartBlinking(); // Clignotement etc. à gérer (peut être un bool pour clignoter)

	
	UPROPERTY(EditAnywhere, Category="Breakable")
	USoundBase* _warningSound; // Son à jouer (référence à un son à configurer dans Blueprint)

	
	UFUNCTION()
	void OnComponentHit(
			UPrimitiveComponent* hitComponent,
			AActor* otherActor,
			UPrimitiveComponent* otherComp,
			FVector normalImpulse,
			const FHitResult& hit
		);

	
	void DestroyPlatform();
};
