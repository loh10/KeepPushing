#include "Traps/NotDeadlyTraps/JumperTrap2.h"

#include "Car/Car.h"
#include "Components/BoxComponent.h"


AJumperTrap2::AJumperTrap2()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	_trigger->SetupAttachment(_rootScene);

	_trigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_trigger->SetCollisionObjectType(ECC_WorldDynamic);
	_trigger->SetCollisionResponseToAllChannels(ECR_Overlap);
	_trigger->SetGenerateOverlapEvents(true);
	_trigger->OnComponentBeginOverlap.AddDynamic(this, &AJumperTrap2::OnComponentBeginOverlap);
}

void AJumperTrap2::BeginPlay()
{
	Super::BeginPlay();
	
	_isActive = false;
	_isAscending = true;
	_isOscillating = false;

	_animElapsed = 0.f;
	
	_initialLocation = GetActorLocation();
}

void AJumperTrap2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!_isActive)
		return;

	if (_isAscending)
	{
		_animElapsed += DeltaTime;
		float Alpha = FMath::Clamp(_animElapsed / _ascendDuration, 0.f, 1.f);
		FVector TargetLoc = FMath::Lerp(_initialLocation, _initialLocation + FVector(0, 0, _heightTarget), Alpha);
		SetActorLocation(TargetLoc);

		if (Alpha >= 1.f)
		{
			if (_targetActor)
			{
				JumpPlayer(_jumpForceEnd);
			}
			
			_isAscending = false;
			_isOscillating = true;
			_animElapsed = 0.f;

			// Lancer l'animation d'oscillation ici si tu utilises Blueprint
		}
		return;
	}

	if (_isOscillating)
	{
		_animElapsed += DeltaTime;
		if (_animElapsed >= _oscillateDuration)
		{
			_isOscillating = false;
			_animElapsed = 0.f;
			// Commencer la descente
		}
		return;
	}

	// Descente
	_animElapsed += DeltaTime;
	float Alpha = FMath::Clamp(_animElapsed / _descendDuration, 0.f, 1.f);
	FVector StartLoc = _initialLocation + FVector(0, 0, _heightTarget);
	FVector TargetLoc = FMath::Lerp(StartLoc, _initialLocation, Alpha);
	SetActorLocation(TargetLoc);

	if (Alpha >= 1.f)
	{
		_isActive = false;
		_isAscending = true;
		
		_animElapsed = 0.f;
		SetActorLocation(_initialLocation);
	}
}

void AJumperTrap2::ActivateTrap()
{
	_isActive = true;
	JumpPlayer(_jumpForceStart);
}

void AJumperTrap2::AffectPlayer(AActor* actor)
{
	if (!actor)
		return;

	ACar* vehicle = Cast<ACar>(actor);

	if (!vehicle)
		return;

	float speedFactor = FMath::Clamp(1.f - (_reduceSpeedPercent / 100.f), 0.f, 1.f);
	vehicle->MultiplySpeed(speedFactor);
}

void AJumperTrap2::JumpPlayer(float jumpForce)
{
	UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(_targetActor->GetRootComponent());
	if (rootComp && rootComp->IsSimulatingPhysics())
	{
		FVector impulse = FVector(0.f, 0.f, jumpForce);
		rootComp->AddImpulse(impulse, NAME_None, true);

		//UE_LOG(LogTemp, Warning, TEXT("JumperTrap applied impulse to %s"), *player->GetName());
	}
}

void AJumperTrap2::OnComponentBeginOverlap(
		UPrimitiveComponent* overlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex,
		bool bFromSweep,
		const FHitResult& sweepResult
	)
{
	if (!otherActor || otherActor == this)
	{
		return;
	}

	_targetActor = otherActor;
	
	FTimerHandle _delayTimer;
	
	GetWorldTimerManager().SetTimer(
			_delayTimer,
			FTimerDelegate::CreateUObject(this, &AJumperTrap2::AffectPlayer, _targetActor),
			.1f,
			false
		);
	
	if (_delayBeforeActivation > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			_delayTimerHandle,
			FTimerDelegate::CreateUObject(this, &AJumperTrap2::ActivateTrap),
			_delayBeforeActivation,
			false
		);
	}
	else
	{
		ActivateTrap();
	}
}
