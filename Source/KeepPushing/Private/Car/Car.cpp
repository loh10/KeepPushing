#include "Car/Car.h"

#include "Car/CarController.h"
#include "EnhancedInputSubsystems.h" 
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"
#include "Traps/DeadlyTraps/SmasherTrap.h"

class ASmasherTrap;

ACar::ACar()
{
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(Box);

	Chassie = CreateDefaultSubobject<UStaticMeshComponent>("Chassie");
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
}

void ACar::BeginPlay()
{
	Super::BeginPlay();
	_startTransform = Box->GetComponentTransform();
	for (TActorIterator<ASmasherTrap> It(GetWorld()); It; ++It)
	{
		It->OnTrapKillPlayer.AddDynamic(this, &ACar::PlayerDeath);
	}
	
}

void ACar::PlayerDeath(AActor* victim)
{
	Box->SetWorldTransform(_startTransform);
	Box->SetAllPhysicsLinearVelocity(FVector::Zero());
	Box->SetAllPhysicsAngularVelocityInDegrees(FVector::Zero());
	Box->SetWorldTransform(_startTransform);
}

void ACar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	bFullGrounded = true;
	for (const TWeakObjectPtr<USceneComponent>& Element : SuspensionArray)
	{
		HandleWheelForce(Element.Get());
	}

	if (bFullGrounded)
	{
		bCanDash = true;
	}

	if (!bFullGrounded && bIsDashing && bCanDash)
	{
		Box->SetAllPhysicsLinearVelocity(FVector::Zero());

		const FVector ForceVector = Box->GetForwardVector();
		Box->AddForce(ForceVector * FVector(DashForce.X, DashForce.Y, DashForce.Z), EName::None, true);
		bCanDash = false;
	}
	else if (bIsJumping && bFullGrounded)
	{
		const FVector CurrentVelocity = Box->GetComponentVelocity();
		Box->SetAllPhysicsLinearVelocity(CurrentVelocity * FVector(1., 1., 0.));
		Box->AddForceAtLocation(FVector::UpVector * JumpForce, Box->GetComponentLocation());
	}
}

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
		false, {}, EDrawDebugTrace::ForOneFrame,HitResult, true))
	{
		if (HitResult.bBlockingHit)
		{
			const float HitDistance = HitResult.Distance;

			CalculateSuspension(CurrentWheel, HitDistance);
			CalcAcceleration(CurrentWheel);
			CalcBrake(CurrentWheel);
			CalculateLateralSlipping(CurrentWheel);
			// CalcJump(CurrentWheel, HitDistance);
		}
		else
		{
			bFullGrounded = false;
		}
	}
	else
	{
		bFullGrounded = false;
	}
}

void ACar::CalculateSuspension(const USceneComponent* CurrentWheel, const float OutDistance)
{
	const FVector WorldLocation = CurrentWheel->K2_GetComponentLocation();
	const FVector UpVector = CurrentWheel->GetUpVector();

	const FVector Velocity = Box->GetPhysicsLinearVelocityAtPoint(WorldLocation);
	FVector Force = UKismetMathLibrary::Multiply_VectorVector(UpVector,
		FVector((SuspensionRestDistance - OutDistance) * SpringForce) - (SpringDamper *
			UKismetMathLibrary::Dot_VectorVector(Velocity, UpVector)));

	Force = UKismetMathLibrary::Multiply_VectorVector(Force, FVector(UGameplayStatics::GetWorldDeltaSeconds(this)));
	Box->AddForceAtLocation(Force, WorldLocation);

	UKismetSystemLibrary::DrawDebugArrow(this, WorldLocation,
		Force * FVector(0.005f) + WorldLocation, 0.f,FColor::Green);
}

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
				FVector(AccelerationInput * AvailableTorqueCurve->GetFloatValue(
			UKismetMathLibrary::Clamp(UKismetMathLibrary::Abs(CurrentForwardSpeed) / TopSpeed, 0., 1.))));
			
			Box->AddForceAtLocation(Force, WorldLocation);
			UKismetSystemLibrary::DrawDebugArrow(this, WorldLocation,
				WorldLocation + UKismetMathLibrary::Multiply_VectorVector(Force, FVector(0.01)),
				25.f, FColor::Blue);
		}
	}
	else
	{
		if (CurrentForwardSpeed > 5.f)
		{
			const FVector Force = FVector(AccelerationForce) * (CurrentWheel->GetForwardVector() * FVector(-1.f));
			Box->AddForceAtLocation(Force, WorldLocation);
			
			UKismetSystemLibrary::DrawDebugArrow(this,
				WorldLocation, WorldLocation + (Force * FVector(.01)),
				25.f, FColor::Blue);
		}
	}
}

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
			UKismetMathLibrary::Clamp(UKismetMathLibrary::Abs(CurrentForwardSpeed) / TopSpeed, 0., 1.))));
			Force *= -1.f;
			
			Box->AddForceAtLocation(Force, WorldLocation);
			UKismetSystemLibrary::DrawDebugArrow(this, WorldLocation,
				WorldLocation + UKismetMathLibrary::Multiply_VectorVector(Force, FVector(0.01)),
				25.f, FColor::Blue);
		}
	}
	else
	{
		const float CurrentForwardSpeed = UKismetMathLibrary::Dot_VectorVector(Box->GetForwardVector(), Velocity);
		
		if (CurrentForwardSpeed < -5.f)
		{
			const FVector Force = FVector(OppositeBreakForce) * Forward;
			Box->AddForceAtLocation(Force, WorldLocation);
			
			UKismetSystemLibrary::DrawDebugArrow(this,
				WorldLocation, WorldLocation + (Force * FVector(.01)),
				25.f, FColor::Blue);
		}
	}
}

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

	UKismetSystemLibrary::DrawDebugArrow(this, WorldLocation,
		WorldLocation + (VectorForce * FVector(.1f)), 25.f, FColor::Red);
}

void ACar::CalcJump(const USceneComponent* CurrentWheel, const float OutDistance)
{
	if (bIsJumping && (SuspensionRestDistance - OutDistance) > 1.f)
	{
		const FVector WorldLocation = CurrentWheel->GetComponentLocation();
		const FVector Velocity = Box->GetComponentVelocity();

		Box->SetAllPhysicsLinearVelocity(FVector(Velocity.X, Velocity.Y, Velocity.Z));
		Box->AddForceAtLocation(FVector(JumpForce) * FVector::UpVector, WorldLocation);
	}
}

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
	const FVector PlaneVelocity = FVector(Box->GetPhysicsLinearVelocity().X, Box->GetPhysicsLinearVelocity().Y, 0.f);
	if (PlaneVelocity.Length() < 10.f) {
		return;
	}
	
	SteeringInput = Value.Get<float>();
	

	if (UKismetMathLibrary::InRange_FloatFloat(BrakeInput, 0., 1.) ||
		UKismetMathLibrary::InRange_FloatFloat(AccelerationInput, 0., 1.))
	{
		Box->AddTorqueInRadians(FVector(0., 0., SteeringInput * TurnTorque));
	}
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
	bIsJumping = true;
}

void ACar::JumpActionReleased(const FInputActionValue& Value)
{
	bIsJumping = false;
}

void ACar::DashActionPressed(const FInputActionValue& Value)
{
	bIsDashing = true;
}

void ACar::DashActionReleased(const FInputActionValue& Value)
{
	bIsDashing = false;
}
