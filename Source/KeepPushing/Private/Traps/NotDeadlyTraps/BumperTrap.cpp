#include "Traps/NotDeadlyTraps/BumperTrap.h"


ABumperTrap::ABumperTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	_pusherMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PusherMesh"));
	_pusherMesh->SetupAttachment(RootComponent);

	_pusherMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_pusherMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void ABumperTrap::BeginPlay()
{
	Super::BeginPlay();

	_pusherMesh->OnComponentHit.AddDynamic(this, &ABumperTrap::OnPusherHit);
}

void ABumperTrap::OnPusherHit(
	UPrimitiveComponent* hitComponent,
	AActor* otherActor,
	UPrimitiveComponent* otherComp,
	FVector normalImpulse,
	const FHitResult& hit
)
{
	if (otherActor && otherActor != this)
	{
		AffectPlayer(otherActor);

		// Ajoute ici animation/sfx :
		// UGameplayStatics::PlaySoundAtLocation(this, YourSound, GetActorLocation());

		UE_LOG(LogTemp, Warning, TEXT("PusherTrap: Hit by %s"), *otherActor->GetName());
	}
}

void ABumperTrap::AffectPlayer(AActor* player)
{
	if (!player) return;

	UPrimitiveComponent* playerComp = Cast<UPrimitiveComponent>(player->GetRootComponent());
	if (playerComp && playerComp->IsSimulatingPhysics())
	{
		// Utilise la vélocité réelle du joueur pour direction et force
		FVector velocity = playerComp->GetComponentVelocity();
		FVector pushDir = velocity.IsNearlyZero() ? 
			(player->GetActorLocation() - GetActorLocation()).GetSafeNormal() :
			velocity.GetSafeNormal();

		float speed = velocity.Size();

		// Force dépendante de la vitesse (mais jamais sous le minimum)
		float pushForce = _basePushForce * (speed / _referenceSpeed);
		pushForce = FMath::Max(pushForce, _minPushForce);

		playerComp->AddImpulse(pushDir * pushForce, NAME_None, true);

		UE_LOG(LogTemp, Warning, TEXT("PusherTrap: Applied force %f in dir %s (speed %f)"), pushForce, *pushDir.ToString(), speed);
	}
}