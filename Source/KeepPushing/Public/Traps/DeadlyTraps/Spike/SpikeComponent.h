#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillPlayer, AActor*, KilledPlayer);

UCLASS()
class KEEPPUSHING_API ASpikeComponent : public AActor
{
	GENERATED_BODY()

public:
	ASpikeComponent();

	void Init(const FVector& direction, UCurveFloat* moveCurve, float lifeTime);

    UPROPERTY(BlueprintAssignable, Category="Spike")
	FOnKillPlayer OnKillCar;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	USceneComponent* _rootScene = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spike")
	UStaticMeshComponent* _mesh = nullptr;

	FVector _moveDirection = FVector::ZeroVector;

	UPROPERTY()
	UCurveFloat* _movementCurve = nullptr;
	
	float _timeElapsed = 0.f;
	float _lifeTime = 2.f;
	
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* hitComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		FVector normalImpulse,
		const FHitResult& hit
	);
};