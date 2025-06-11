#include "Traps/NotDeadlyTraps/JumperTrap.h"

#include "Components/BoxComponent.h"


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

		//UE_LOG(LogTemp, Warning, TEXT("JumperTrap applied impulse to %s"), *player->GetName());
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("JumperTrap: %s has no simulating physics!"), *player->GetName());
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
	
	AffectPlayer(otherActor);

	// You can play a sound, spawn effects, etc. here
	//UE_LOG(LogTemp, Warning, TEXT("JumperTrap activated by: %s"), *otherActor->GetName());
}
