#pragma once

#include "InputMappingContext.h"
#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Car.generated.h"

class UCameraComponent;

UCLASS()
class KEEPPUSHING_API ACar : public APawn
{
	GENERATED_BODY()

public:
	ACar();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

	UFUNCTION()
	void Kill(AActor* victim);	

private:
	void HandleWheelForce(const USceneComponent* CurrentWheel);

	void CalculateSuspension(const USceneComponent* CurrentWheel, const float OutDistance);

	void CalcAcceleration(const USceneComponent* CurrentWheel);

	void CalcBrake(const USceneComponent* CurrentWheel);

	void CalculateLateralSlipping(const USceneComponent* CurrentWheel);

	void CalcJump(const USceneComponent* CurrentWheel, const float OutDistance);
	
	void ThrottleActionTriggered(const FInputActionValue& Value);

	void ThrottleActionComplete(const FInputActionValue& Value);

	void BrakeActionTriggered(const FInputActionValue& Value);

	void BrakeActionComplete(const FInputActionValue& Value);
	
	void TurnActionTriggered(const FInputActionValue& Value);

	void DriftActionPressed(const FInputActionValue& Value);

	void DriftActionReleased(const FInputActionValue& Value);

	void FlipActionPressed(const FInputActionValue& Value);

	void JumpActionPressed(const FInputActionValue& Value);

	void JumpActionReleased(const FInputActionValue& Value);

	void DashActionPressed(const FInputActionValue& Value);

	void DashActionReleased(const FInputActionValue& Value);
	
public:
	UPROPERTY(EditAnywhere, Category = "Car|Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category = "Car|Input")
	TObjectPtr<UInputAction> ThrottleAction;

	UPROPERTY(EditAnywhere, Category = "Car|Input")
	TObjectPtr<UInputAction> BrakeAction;

	UPROPERTY(EditAnywhere, Category = "Car|Input")
	TObjectPtr<UInputAction> TurnAction;

	UPROPERTY(EditAnywhere, Category = "Car|Input")
	TObjectPtr<UInputAction> DriftAction;

	UPROPERTY(EditAnywhere, Category = "Car|Input")
	TObjectPtr<UInputAction> FlipAction;

	UPROPERTY(EditAnywhere, Category = "Car|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Car|Input")
	TObjectPtr<UInputAction> DashAction;
	
private:
	UPROPERTY(EditAnywhere, Category = "Forward / Backward")
	float AccelerationInput;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward")
	float BrakeInput;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward")
	float TopSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward")
	TObjectPtr<UCurveFloat> AvailableTorqueCurve;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward")
	float AccelerationForce = 8000.f;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward")
	float DecelerationForce = 10000.f;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward")
	float BrakeForce = 6000.f;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward")
	float OppositeBreakForce = 10000.f;

	UPROPERTY(EditAnywhere, Category = "Suspension")
	float SuspensionRestDistance = 35.f;

	UPROPERTY(EditAnywhere, Category = "Suspension")
	float SpringForce = 25000.f;

	UPROPERTY(EditAnywhere, Category = "Suspension")
	float SpringDamper = 18000.f;

	UPROPERTY(EditAnywhere, Category = "Suspension")
	TArray<TWeakObjectPtr<USceneComponent>> SuspensionArray;

	UPROPERTY(EditAnywhere, Category = "Lateral Slipping")
	float TireMass = 1.f;

	UPROPERTY(EditAnywhere, Category = "Lateral Slipping")
	float TurnTorque = 1000000.f;

	UPROPERTY(EditAnywhere, Category = "Lateral Slipping")
	float SteeringInput;

	UPROPERTY(EditAnywhere, Category = "Lateral Slipping")
	float GripFactor = .5f;

	UPROPERTY(EditAnywhere, Category = "Lateral Slipping")
	bool bIsDrifting;

	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> BR_Wheel;
	
	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> BL_Wheel;
	
	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> FR_Wheel;
	
	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> FL_Wheel;

	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> Chassie;
	
	UPROPERTY(EditAnywhere)
	float JumpForce = 2200000.f;

	UPROPERTY(EditAnywhere)
	bool bFullGrounded;

	UPROPERTY(EditAnywhere)
	bool bCanDash;

	UPROPERTY(EditAnywhere)
	bool bIsJumping;

	UPROPERTY(EditAnywhere)
	bool bIsDashing;
	
	UPROPERTY(EditAnywhere)
	FVector DashForce = FVector(13000.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere)
	FTransform _startTransform;
};

