#include "Car/Car.h"

#include "Car/CarController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"
#include "Traps/DeadlyTraps/SmasherTrap.h"
#include "KeepPushing/Public/LifeZone/LifeZone.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundManager.h"


class ACar;

#pragma region Unreal Function
ACar::ACar()
{
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	SetRootComponent(Box);

	Chassie = CreateDefaultSubobject<UStaticMeshComponent>("Chassie");
	Chassie->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	Chassie->SetupAttachment(Box);

	FL_Wheel = CreateDefaultSubobject<USceneComponent>("FL_Wheel");
	FL_Wheel->SetupAttachment(Box);
	FR_Wheel = CreateDefaultSubobject<USceneComponent>("FR_Wheel");
	FR_Wheel->SetupAttachment(Box);
	BL_Wheel = CreateDefaultSubobject<USceneComponent>("BL_Wheel");
	BL_Wheel->SetupAttachment(Box);
	BR_Wheel = CreateDefaultSubobject<USceneComponent>("BR_Wheel");
	BR_Wheel->SetupAttachment(Box);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(Box);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	SuspensionArray.Empty(4);
	SuspensionArray.Add(FL_Wheel);
	SuspensionArray.Add(FR_Wheel);
	SuspensionArray.Add(BL_Wheel);
	SuspensionArray.Add(BR_Wheel);

	DashParticlePos1 = CreateDefaultSubobject<UParticleSystemComponent>("DashParticlePos1");
	DashParticlePos1->SetupAttachment(Box);

	DashParticlePos2 = CreateDefaultSubobject<UParticleSystemComponent>("DashParticlePos2");
	DashParticlePos2->SetupAttachment(Box);
}

void ACar::BeginPlay()
{
	Super::BeginPlay();
	Box->SetCenterOfMass(FVector(0.f, 0.f, -120.f));

	_startTransform = Box->GetComponentTransform();
	for (TActorIterator<ASmasherTrap> It(GetWorld()); It; ++It)
	{
		It->OnTrapKillPlayer.AddDynamic(this, &ACar::Kill);
	}

	for (TActorIterator<ALifeZone> It(GetWorld()); It; ++It)
	{
		It->OnVoidZoneTouched.AddDynamic(this, &ACar::Kill);
	}
}

void ACar::Kill(AActor* victim)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "You died");
	Box->SetWorldTransform(_startTransform);
	Box->SetAllPhysicsLinearVelocity(FVector::Zero());
	Box->SetAllPhysicsAngularVelocityInDegrees(FVector::Zero());
	Box->SetWorldTransform(_startTransform);

	USoundManager::Get(this)->Play2DSound("Car_Die");
}

void ACar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateGroundState();
    StabilizeCar();
	PreventRolling();
    ApplyHighSpeedForces();
    ClampAngularVelocity();

	FVector CurrentVelocity = Box->GetPhysicsLinearVelocity();
	float CurrentSpeed = CurrentVelocity.Size();

	if (CurrentSpeed > TopSpeed)
	{
		FVector ClampedVelocity = CurrentVelocity.GetSafeNormal() * TopSpeed;
		Box->SetPhysicsLinearVelocity(ClampedVelocity);
	}

}

// Sets up the car's input bindings.
void ACar::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(Input))
	{
		EnhancedInput->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ACar::ThrottleActionTriggered);
		EnhancedInput->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ACar::ThrottleActionComplete);

		EnhancedInput->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ACar::BrakeActionTriggered);
		EnhancedInput->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ACar::BrakeActionComplete);

		EnhancedInput->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ACar::TurnActionTriggered);

		EnhancedInput->BindAction(DriftAction, ETriggerEvent::Started, this, &ACar::DriftActionPressed);
		EnhancedInput->BindAction(DriftAction, ETriggerEvent::Completed, this, &ACar::DriftActionReleased);

		EnhancedInput->BindAction(FlipAction, ETriggerEvent::Started, this, &ACar::FlipActionPressed);

		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACar::JumpActionPressed);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACar::JumpActionReleased);

		EnhancedInput->BindAction(DashAction, ETriggerEvent::Started, this, &ACar::DashActionPressed);
		EnhancedInput->BindAction(DashAction, ETriggerEvent::Completed, this, &ACar::DashActionReleased);
	}
}

#pragma endregion

// Multiplies the car's speed by a given factor.
void ACar::MultiplySpeed(float Factor)
{
	if (!Box->IsSimulatingPhysics())
		return;

	FVector Velocity = Box->GetPhysicsLinearVelocity();
	Box->SetPhysicsLinearVelocity(Velocity * Factor);
}

// Disables all input and forces applied to the car.
void ACar::DisableCarInput()
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		DisableInput(PC);
	}

	AccelerationInput = 0.f;
	BrakeInput = 0.f;
	SteeringInput = 0.f;
	bIsDrifting = false;
	bIsJumping = false;
	bIsDashing = false;
	bCanDash = false;

	Box->SetPhysicsLinearVelocity(FVector::ZeroVector);
	Box->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	UE_LOG(LogTemp, Warning, TEXT("Car input and forces disabled."));
}

// Updates the car's ground state by checking if wheels are on the ground.
void ACar::UpdateGroundState()
{
    bFullGrounded = true;
    currentWheelOnGround = SuspensionArray.Num();

    for (const TWeakObjectPtr<USceneComponent>& Element : SuspensionArray)
    {
        HandleWheelForce(Element.Get());
    }

    bFullGrounded = currentWheelOnGround > 0;
    if (bFullGrounded)
    {
        bHasDashed = false;
    }
}


void ACar::HandleJump()
{
	if (bFullGrounded)
	{
		FVector Velocity = Box->GetPhysicsLinearVelocity();
		Velocity.Z = 0.f;
		Box->SetPhysicsLinearVelocity(Velocity);

		const float DesiredJumpHeight = 200.f; // 2 mètres

		float Gravity = FMath::Abs(GetWorld()->GetGravityZ());

		float JumpSpeed = FMath::Sqrt(2.f * Gravity * DesiredJumpHeight);

		FVector JumpImpulse = FVector(0.f, 0.f, JumpSpeed);
		Box->AddImpulse(JumpImpulse, NAME_None, true);

		// Jouer le son de saut
		USoundManager::Get(this)->Play2DSound("Car_Jump");
	}
}

// Handles the car's dash by applying a forward force and playing effects.
void ACar::HandleDash()
{
	if (!bFullGrounded)
	{
		CalcDashForce();
		bCanDash = false;
		bHasDashed = true;

		if (DashParticles)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				DashParticles,
				DashParticlePos1,
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true);

			UNiagaraFunctionLibrary::SpawnSystemAttached(
				DashParticles,
				DashParticlePos2,
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true);
		}
		USoundManager::Get(this)->Play2DSound("Car_Dash");
	}
}

// Stabilizes the car by applying torque to correct its roll angle.
void ACar::StabilizeCar()
{
	const FVector UpVector = Box->GetUpVector();
	const float RollAngle = FMath::Acos(FVector::DotProduct(UpVector, FVector::UpVector));

	if (RollAngle > KINDA_SMALL_NUMBER)
	{
		// Stronger anti-roll force
		const FVector StabilizingTorque = FVector::CrossProduct(UpVector, FVector::UpVector) * RollAngle * StabilizationForce * 3.0f;
		Box->AddTorqueInRadians(StabilizingTorque);
	}

	FVector AngularVel = Box->GetPhysicsAngularVelocityInRadians();

	FVector CounterTorque = FVector::ZeroVector;
	CounterTorque.X = -AngularVel.X * 500000.0f;
	CounterTorque.Y = -AngularVel.Y * 300000.0f;

	Box->AddTorqueInRadians(CounterTorque);
}


void ACar::PreventRolling()
{
	FVector AngularVelocity = Box->GetPhysicsAngularVelocityInRadians();

	// Clamp roll and pitch angular velocity
	AngularVelocity.X = FMath::Clamp(AngularVelocity.X, -1.0f, 1.0f); // Roll
	AngularVelocity.Y = FMath::Clamp(AngularVelocity.Y, -1.0f, 1.0f); // Pitch

	Box->SetPhysicsAngularVelocityInRadians(AngularVelocity);

	// Keep car upright
	FVector UpVector = Box->GetUpVector();
	FVector WorldUp = FVector::UpVector;

	if (FVector::DotProduct(UpVector, WorldUp) < 0.7f) // Car is tilting too much
	{
		FVector CorrectiveTorque = FVector::CrossProduct(UpVector, WorldUp) * 2000000.0f;
		Box->AddTorqueInRadians(CorrectiveTorque);
	}
}

// Applies forces to stabilize the car at high speeds.
void ACar::ApplyHighSpeedForces()
{
    const float Speed = Box->GetPhysicsLinearVelocity().Size();

    if (Speed > HighSpeedThreshold)
    {
        const FVector LateralVelocity = FVector::DotProduct(Box->GetPhysicsLinearVelocity(), Box->GetRightVector()) * Box->GetRightVector();
        const FVector CounterForce = -LateralVelocity * SpeedStabilizationFactor;
        Box->AddForce(CounterForce);
    }

    if (Speed > 0.f)
    {
        const FVector Downforce = FVector::DownVector * Speed * DownforceFactor;
        Box->AddForce(Downforce);
    }
}

// Clamps the car's angular velocity to prevent excessive spinning.
void ACar::ClampAngularVelocity()
{
    FVector AngularVelocity = Box->GetPhysicsAngularVelocityInRadians();
    AngularVelocity.X = FMath::Clamp(AngularVelocity.X, -MaxAngularVelocity, MaxAngularVelocity);
    AngularVelocity.Y = FMath::Clamp(AngularVelocity.Y, -MaxAngularVelocity, MaxAngularVelocity);
    Box->SetPhysicsAngularVelocityInRadians(AngularVelocity);
}

// Calculates and applies the dash force based on the car's current speed.
void ACar::CalcDashForce()
{
	// Obtenez la vitesse actuelle de la voiture
	const FVector CurrentVelocity = Box->GetPhysicsLinearVelocity();
	const FVector ForwardVector = Box->GetForwardVector();

	// Calculez la vitesse avant (projection de la vitesse sur le vecteur avant)
	const float ForwardSpeed = FVector::DotProduct(CurrentVelocity, ForwardVector);

	// Calculez la vitesse de dash en multipliant la vitesse avant par DashMultiplier
	float DashSpeed = FMath::Max(ForwardSpeed,500);

	// Réinitialisez la vitesse actuelle pour éviter les composantes latérales
	Box->SetAllPhysicsLinearVelocity(FVector::Zero());


	// Appliquez la force de dash strictement dans la direction avant
	const FVector BaseDashForce = ForwardVector * DashSpeed * DashMultiplier;
	const FVector MinDash = ForwardVector * DashSpeed ;
	const FVector DashForceVector= (BaseDashForce.Size() > MinDash.Size()) ? BaseDashForce : MinDash;
	Box->AddForce(DashForceVector, NAME_None, true);
}

// Called when the car is possessed by a controller.
void ACar::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ACarController* PC = Cast<ACarController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

// Handles the suspension force for a specific wheel.
void ACar::HandleWheelForce(const USceneComponent* CurrentWheel)
{
	const FVector Start = CurrentWheel->K2_GetComponentLocation();
	const FVector UpVector = CurrentWheel->GetUpVector();

	const FVector MultVector = UKismetMathLibrary::Multiply_VectorVector(
		UKismetMathLibrary::Multiply_VectorVector(UpVector, FVector(-1.f)),
		FVector(SuspensionRestDistance));

	const FVector End = Start + MultVector;

	FHitResult HitResult;
	if (UKismetSystemLibrary::LineTraceSingle(this, Start, End, TraceTypeQuery1,
	                                          false, {}, EDrawDebugTrace::None, HitResult, true))
	{
		if (HitResult.bBlockingHit)
		{
			const float HitDistance = HitResult.Distance;

			CalculateSuspension(CurrentWheel, HitDistance);
			CalcAcceleration(CurrentWheel);
			CalcBrake(CurrentWheel);
			CalculateLateralSlipping(CurrentWheel);
		}
		else
		{
			currentWheelOnGround--;
		}
	}
	else
	{
		currentWheelOnGround--;
	}
}

// Calculates and applies the suspension force for a wheel.
void ACar::CalculateSuspension(const USceneComponent* CurrentWheel, const float OutDistance)
{
	const FVector WorldLocation = CurrentWheel->K2_GetComponentLocation();
	const FVector UpVector = CurrentWheel->GetUpVector();

	const FVector Velocity = Box->GetPhysicsLinearVelocityAtPoint(WorldLocation);
	FVector Force = UKismetMathLibrary::Multiply_VectorVector(UpVector,
	                                                          FVector(
		                                                          (SuspensionRestDistance - OutDistance) * SpringForce)
	                                                          - (SpringDamper *
		                                                          UKismetMathLibrary::Dot_VectorVector(
			                                                          Velocity, UpVector)));

	Force = UKismetMathLibrary::Multiply_VectorVector(Force, FVector(UGameplayStatics::GetWorldDeltaSeconds(this)));
	Box->AddForceAtLocation(Force, WorldLocation);

}

// Calculates and applies acceleration force for a wheel.
void ACar::CalcAcceleration(const USceneComponent* CurrentWheel)
{
	const FVector Velocity = Box->GetPhysicsLinearVelocity();
	const FVector WorldLocation = CurrentWheel->GetComponentLocation();
	const float CurrentForwardSpeed = UKismetMathLibrary::Dot_VectorVector(Box->GetForwardVector(), Velocity);

	if (AccelerationInput > 0.f)
	{
		if (CurrentForwardSpeed <= TopSpeed)
		{
			FVector Force = UKismetMathLibrary::Multiply_VectorVector(CurrentWheel->GetForwardVector(),
			                                                          FVector(AccelerationForce));

			Force = UKismetMathLibrary::Multiply_VectorVector(Force,
			                                                  FVector(
				                                                  AccelerationInput * AvailableTorqueCurve->
				                                                  GetFloatValue(
					                                                  UKismetMathLibrary::Clamp(
						                                                  UKismetMathLibrary::Abs(CurrentForwardSpeed) /
						                                                  TopSpeed, 0., 1.))));

			Box->AddForceAtLocation(Force, WorldLocation);
		}
	}
	else
	{
		if (CurrentForwardSpeed > 5.f)
		{
			const FVector Force = FVector(AccelerationForce) * (CurrentWheel->GetForwardVector() * FVector(-1.f));
			Box->AddForceAtLocation(Force, WorldLocation);

		}
	}
}

// Calculates and applies braking force for a wheel.
void ACar::CalcBrake(const USceneComponent* CurrentWheel)
{
	const FVector Forward = CurrentWheel->GetForwardVector();
	const FVector Velocity = Box->GetPhysicsLinearVelocity();
	const FVector WorldLocation = CurrentWheel->GetComponentLocation();

	if (BrakeInput > 0.f)
	{
		const float CurrentForwardSpeed = UKismetMathLibrary::Dot_VectorVector(
			Box->GetForwardVector() * FVector(-1.f), Velocity);

		if (CurrentForwardSpeed <= TopSpeed)
		{
			FVector Force = UKismetMathLibrary::Multiply_VectorVector(Forward, FVector(BrakeForce));
			Force = UKismetMathLibrary::Multiply_VectorVector(Force,
			                                                  FVector(BrakeInput * AvailableTorqueCurve->GetFloatValue(
				                                                  UKismetMathLibrary::Clamp(
					                                                  UKismetMathLibrary::Abs(CurrentForwardSpeed) /
					                                                  TopSpeed, 0., 1.))));
			Force *= -1.f;

			Box->AddForceAtLocation(Force, WorldLocation);
		}
	}
	else
	{
		const float CurrentForwardSpeed = UKismetMathLibrary::Dot_VectorVector(Box->GetForwardVector(), Velocity);

		if (CurrentForwardSpeed < -5.f)
		{
			const FVector Force = FVector(OppositeBreakForce) * Forward;
			Box->AddForceAtLocation(Force, WorldLocation);

		}
	}
}

// Calculates and applies lateral slipping force for a wheel.
void ACar::CalculateLateralSlipping(const USceneComponent* CurrentWheel)
{
	GripFactor = bIsDrifting ? 0.1f : 0.5f;

	const FVector WorldLocation = CurrentWheel->GetComponentLocation();
	const FVector RightVector = CurrentWheel->GetRightVector();
	const FVector Velocity = Box->GetPhysicsLinearVelocityAtPoint(WorldLocation);

	float Force = (UKismetMathLibrary::Dot_VectorVector(Velocity, RightVector) * -1.f) * GripFactor;
	Force /= UGameplayStatics::GetWorldDeltaSeconds(this);

	const FVector VectorForce = FVector(Force) * FVector(TireMass) * RightVector;
	Box->AddForceAtLocation(VectorForce, WorldLocation);

}

// Handles the car's jump force for a specific wheel.
void ACar::CalcJump(const USceneComponent* CurrentWheel, const float OutDistance)
{
	if (bIsJumping)
	{
		const FVector WorldLocation = CurrentWheel->GetComponentLocation();
		FVector Velocity = Box->GetPhysicsLinearVelocity();

		// Set a constant upward velocity for a consistent jump
		Velocity.Z = JumpForce / Box->GetMass(); // Adjust based on mass for consistency
		Box->SetPhysicsLinearVelocity(Velocity);

		USoundManager::Get(this)->Play2DSound("Car_Jump");
	}
}

#pragma region Input

void ACar::ThrottleActionTriggered(const FInputActionValue& Value)
{
	AccelerationInput = Value.Get<float>();
}

void ACar::ThrottleActionComplete(const FInputActionValue& Value)
{
	AccelerationInput = Value.Get<float>();
}

void ACar::BrakeActionTriggered(const FInputActionValue& Value)
{
	BrakeInput = Value.Get<float>();
}

void ACar::BrakeActionComplete(const FInputActionValue& Value)
{
	BrakeInput = Value.Get<float>();
}

void ACar::TurnActionTriggered(const FInputActionValue& Value)
{
	SteeringInput = Value.Get<float>();

	if (FMath::IsNearlyZero(SteeringInput))
		return;

	const float Speed = Box->GetComponentVelocity().Size();

	if (Speed < 50.f)
		return;



	const float DeltaYaw = SteeringInput * TurnTorque * UGameplayStatics::GetWorldDeltaSeconds(this);

	FRotator NewRotation = Box->GetComponentRotation();
	NewRotation.Yaw += DeltaYaw;

	Box->SetWorldRotation(NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
}


void ACar::DriftActionPressed(const FInputActionValue& Value)
{
	bIsDrifting = true;
}

void ACar::DriftActionReleased(const FInputActionValue& Value)
{
	bIsDrifting = false;
}

void ACar::FlipActionPressed(const FInputActionValue& Value)
{
	const FRotator Rotation = GetActorRotation();
	SetActorLocationAndRotation(GetActorLocation() + FVector(0.f, 0.f, 50.f),
								FRotator(0.f, Rotation.Yaw, Rotation.Roll));
}

void ACar::JumpActionPressed(const FInputActionValue& Value)
{
	HandleJump();
}

void ACar::JumpActionReleased(const FInputActionValue& Value)
{
	bIsJumping = false;
}

void ACar::DashActionPressed(const FInputActionValue& Value)
{
	if (!bFullGrounded && !bHasDashed)
	{
		bIsDashing = true;
		HandleDash();
	}
}

void ACar::DashActionReleased(const FInputActionValue& Value)
{
	bIsDashing = false;
}


#pragma  endregion