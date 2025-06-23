#include "Traps/NotDeadlyTraps/JumperTrap.h"

#include "Car/Car.h"
#include "Components/BoxComponent.h"


class ACar;

AJumperTrap::AJumperTrap()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	_trigger->SetupAttachment(_rootScene);

}

void AJumperTrap::BeginPlay()
{
	Super::BeginPlay();

	if (_trigger)
	{
		_trigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		_trigger->SetCollisionObjectType(ECC_WorldDynamic);
		_trigger->SetCollisionResponseToAllChannels(ECR_Overlap);
		_trigger->SetGenerateOverlapEvents(true);
		_trigger->OnComponentBeginOverlap.AddDynamic(this, &AJumperTrap::OnComponentBeginOverlap);
	}

	_hasJumped = false;
}

void AJumperTrap::AffectPlayer(AActor* other)
{
	// Example: Launch the player upwards (works for Pawn or custom car actor)
	if (!other) return;

	ACar* vehicle = Cast<ACar>(other);

	if (!vehicle)
		return;

	UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(other->GetRootComponent());
	if (!rootComp || !rootComp->IsSimulatingPhysics())
		return;
	
	float SpeedFactor = FMath::Clamp(1.f - (_reduceSpeedPercent / 100.f), 0.f, 1.f);
	vehicle->MultiplySpeed(SpeedFactor);
	
	if (_hasJumped)
		return;
	
	FVector impulse = FVector(0.f, 0.f, _jumpForce);
	rootComp->AddImpulse(impulse*100, NAME_None, true);

	vehicle->bCanDash = false;

	_hasJumped = true;
}

void AJumperTrap::OnComponentBeginOverlap(
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

	FTimerHandle _delayTimer;
	
	GetWorldTimerManager().SetTimer(
			_delayTimer,
			FTimerDelegate::CreateUObject(this, &AJumperTrap::AffectPlayer, otherActor),
			.1f,
			false
		);
}
