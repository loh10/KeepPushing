#include "Traps/NotDeadlyTraps/JumperTrap.h"

#include "Components/BoxComponent.h"


AJumperTrap::AJumperTrap()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AJumperTrap::BeginPlay()
{
	Super::BeginPlay();

	Super::BeginPlay();
	
	if (_trigger)
	{
		_trigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		_trigger->SetCollisionObjectType(ECC_WorldDynamic);
		_trigger->SetCollisionResponseToAllChannels(ECR_Block);
		_trigger->SetGenerateOverlapEvents(false);
		_trigger->OnComponentHit.AddDynamic(this, &AJumperTrap::OnComponentHit);
	}
}

void AJumperTrap::AffectPlayer(AActor* player)
{
	// Example: Launch the player upwards (works for Pawn or custom car actor)
	if (!player) return;

	UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(player->GetRootComponent());
	if (rootComp && rootComp->IsSimulatingPhysics())
	{
		FVector impulse = FVector(0.f, 0.f, _jumpForce);
		rootComp->AddImpulse(impulse, NAME_None, true);

		UE_LOG(LogTemp, Warning, TEXT("JumperTrap applied impulse to %s"), *player->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("JumperTrap: %s has no simulating physics!"), *player->GetName());
	}
	
	/*APawn* pawn = Cast<APawn>(player);
	
	if (pawn)
	{
		UPrimitiveComponent* pawnRoot = Cast<UPrimitiveComponent>(pawn->GetRootComponent());
		if (pawnRoot && pawnRoot->IsSimulatingPhysics())
		{
			FVector impulse = FVector(0.f, 0.f, _jumpForce);
			pawnRoot->AddImpulse(impulse, NAME_None, true);
		}
	}*/
}

void AJumperTrap::OnComponentHit(
	UPrimitiveComponent* hitComponent,
	AActor* otherActor,
	UPrimitiveComponent* otherComp,
	FVector normalImpulse,
	const FHitResult& hit
)
{
	if (!otherActor || otherActor == this)
	{
		return;
	}
	
	AffectPlayer(otherActor);

	// You can play a sound, spawn effects, etc. here
	UE_LOG(LogTemp, Warning, TEXT("JumperTrap activated by: %s"), *otherActor->GetName());
}
