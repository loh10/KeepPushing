#pragma once

#include "InputMappingContext.h"
#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
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
	void UpdateGroundState();
	void HandleDash();
	void HandleJump();
	void StabilizeCar();
	void ApplyHighSpeedForces();
	void ClampAngularVelocity();

	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

	UFUNCTION()
	void Kill(AActor* victim);

	UFUNCTION(BlueprintCallable, Category = "Car|Runtime")

	void MultiplySpeed(float Factor);
	UFUNCTION(BlueprintCallable, Category = "Car|Input")
	void DisableCarInput();

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

	void CalcDashForce();

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

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Indicates whether the car can dash."))
	bool bCanDash;

private:
	UPROPERTY(EditAnywhere, Category = "Forward / Backward", meta = (ToolTip = "Input value for acceleration."))
	float AccelerationInput;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward", meta = (ToolTip = "Input value for braking."))
	float BrakeInput;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward", meta = (ToolTip = "Maximum speed the car can reach."))
	float TopSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Car Physics",
		meta = (ToolTip = "Force applied to stabilize the car when it tilts."))
	float StabilizationForce = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Car Physics",
		meta = (ToolTip = "Speed threshold above which additional stabilization is applied."))
	float HighSpeedThreshold = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Car Physics",
		meta = (ToolTip = "Factor controlling the stabilization force based on speed."))
	float SpeedStabilizationFactor = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Car Physics",
		meta = (ToolTip = "Downward force applied to stabilize the car at high speeds."))
	float DownforceFactor = 50.f;

	UPROPERTY(EditAnywhere, Category = "Car Physics",
		meta = (ToolTip = "Maximum angular velocity allowed to prevent flipping."))
	float MaxAngularVelocity = 5.f;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward",
		meta = (ToolTip = "Curve defining the available torque based on speed."))
	TObjectPtr<UCurveFloat> AvailableTorqueCurve;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Curve defining the force applied during a dash."))
	TObjectPtr<UCurveFloat> DashCurve;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward", meta = (ToolTip = "Force applied to accelerate the car."))
	float AccelerationForce = 8000.f;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward", meta = (ToolTip = "Force applied to decelerate the car."))
	float DecelerationForce = 10000.f;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward", meta = (ToolTip = "Force applied when braking."))
	float BrakeForce = 6000.f;

	UPROPERTY(EditAnywhere, Category = "Forward / Backward",
		meta = (ToolTip = "Force applied to counteract movement in the opposite direction."))
	float OppositeBreakForce = 10000.f;

	UPROPERTY(EditAnywhere, Category = "Suspension", meta = (ToolTip = "Resting distance of the suspension."))
	float SuspensionRestDistance = 35.f;

	UPROPERTY(EditAnywhere, Category = "Suspension", meta = (ToolTip = "Force applied by the suspension spring."))
	float SpringForce = 25000.f;

	UPROPERTY(EditAnywhere, Category = "Suspension",
		meta = (ToolTip = "Damping force applied by the suspension spring."))
	float SpringDamper = 18000.f;

	UPROPERTY(EditAnywhere, Category = "Suspension", meta = (ToolTip = "Array of suspension components for the car."))
	TArray<TWeakObjectPtr<USceneComponent>> SuspensionArray;

	UPROPERTY(EditAnywhere, Category = "Lateral Slipping",
		meta = (ToolTip = "Mass of the tires, affecting lateral slipping."))
	float TireMass = 1.f;

	UPROPERTY(EditAnywhere, Category = "Lateral Slipping", meta = (ToolTip = "Torque applied when turning the car."))
	float TurnTorque = 1000000.f;

	UPROPERTY(EditAnywhere, Category = "Lateral Slipping", meta = (ToolTip = "Input value for steering."))
	float SteeringInput;

	UPROPERTY(EditAnywhere, Category = "Lateral Slipping",
		meta = (ToolTip = "Factor controlling the grip of the tires."))
	float GripFactor = .5f;

	UPROPERTY(EditAnywhere, Category = "Lateral Slipping",
		meta = (ToolTip = "Indicates whether the car is currently drifting."))
	bool bIsDrifting;

	UPROPERTY(EditAnywhere, Category = "Component", meta = (ToolTip = "Camera component attached to the car."))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, Category = "Component", meta = (ToolTip = "Spring arm component for the camera."))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, Category = "Component", meta = (ToolTip = "Back-right wheel component."))
	TObjectPtr<USceneComponent> BR_Wheel;

	UPROPERTY(EditAnywhere, Category = "Component", meta = (ToolTip = "Back-left wheel component."))
	TObjectPtr<USceneComponent> BL_Wheel;

	UPROPERTY(EditAnywhere, Category = "Component", meta = (ToolTip = "Front-right wheel component."))
	TObjectPtr<USceneComponent> FR_Wheel;

	UPROPERTY(EditAnywhere, Category = "Component", meta = (ToolTip = "Front-left wheel component."))
	TObjectPtr<USceneComponent> FL_Wheel;

	UPROPERTY(EditAnywhere, Category = "Component", meta = (ToolTip = "Box component representing the car's body."))
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(EditAnywhere, Category = "Component", meta = (ToolTip = "Static mesh representing the car's chassis."))
	TObjectPtr<UStaticMeshComponent> Chassie;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<USceneComponent> DashParticlePos1;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<USceneComponent> DashParticlePos2;
	
	UPROPERTY(EditAnywhere, Category = "VFX", meta = (ToolTip = "Particle emitter for the dash"))
	TObjectPtr<UNiagaraSystem> DashParticles;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Force applied when the car jumps."))
	float JumpForce = 2200000.f;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Indicates whether all wheels are grounded."))
	bool bFullGrounded;


	UPROPERTY(EditAnywhere, meta = (ToolTip = "Indicates whether the car is currently jumping."))
	bool bIsJumping;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Indicates whether the car is currently dashing."))
	bool bIsDashing;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Force vector applied during a dash."))
	FVector DashForce = FVector(13000.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Initial transform of the car."))
	FTransform _startTransform;

	int currentWheelOnGround = 4;
	bool bHasDashed = false;
};
