#include "Traps/NotDeadlyTraps/JumperTrap2.h"

#include "Car/Car.h"
#include "Components/BoxComponent.h"


AJumperTrap2::AJumperTrap2()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	_trigger->SetupAttachment(_rootScene);
}

void AJumperTrap2::BeginPlay()
{
	Super::BeginPlay();

	if (_trigger)
	{
		_trigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		_trigger->SetCollisionObjectType(ECC_WorldDynamic);
		_trigger->SetCollisionResponseToAllChannels(ECR_Overlap);
		_trigger->SetGenerateOverlapEvents(true);
		_trigger->OnComponentBeginOverlap.AddDynamic(this, &AJumperTrap2::OnComponentBeginOverlap);
	}
}

void AJumperTrap2::AffectPlayer(AActor* actor)
{
	// Example: Launch the player upwards (works for Pawn or custom car actor)
	if (!actor) return;


	ACar* vehicle = Cast<ACar>(actor);

	if (!vehicle)
		return;

	vehicle->MultiplySpeed(.1f); // Reduce speed by 90% for a short duration

	
	/*UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(actor->GetRootComponent());
	if (rootComp && rootComp->IsSimulatingPhysics())
	{
		FVector impulse = FVector(0.f, 0.f, _jumpForce);
		rootComp->AddImpulse(impulse, NAME_None, true);
	}*/
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
	
	AffectPlayer(otherActor);

	// You can play a sound, spawn effects, etc. here
	//UE_LOG(LogTemp, Warning, TEXT("JumperTrap activated by: %s"), *otherActor->GetName());
}
