#include "Traps/DeadlyTraps/SmasherTrap.h"
#include "DrawDebugHelpers.h"

ASmasherTrap::ASmasherTrap()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_leftBlock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftBlock"));
	_leftBlock->SetupAttachment(RootComponent);

	_rightBlock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightBlock"));
	_rightBlock->SetupAttachment(RootComponent);
}

void ASmasherTrap::BeginPlay()
{
	Super::BeginPlay();

	_leftStart = _leftBlock->GetRelativeLocation();
	_rightStart = _rightBlock->GetRelativeLocation();

	_leftBlock->OnComponentHit.AddDynamic(this, &ASmasherTrap::OnComponentHit);
	_rightBlock->OnComponentHit.AddDynamic(this, &ASmasherTrap::OnComponentHit);
	
	Activate();
	
	UE_LOG(LogTemp, Warning, TEXT("_isClosing = %s"), _isClosing ? TEXT("true") : TEXT("false"));

}

void ASmasherTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_isActivated && _isClosing)
	{
		FVector directionToOther = (_rightBlock->GetRelativeLocation() - _leftBlock->GetRelativeLocation()).GetSafeNormal();

		_leftBlock->AddRelativeLocation(directionToOther * _direction * _moveSpeed * DeltaTime);
		_rightBlock->AddRelativeLocation(-directionToOther * _direction * _moveSpeed * DeltaTime);

		float leftDirectionToStart = _leftStart.X - _leftBlock->GetRelativeLocation().X;
		float rightDirectionToStart = _rightStart.X - _rightBlock->GetRelativeLocation().X;
		
		// Ouverture : retour à la position de départ
		if (_direction > 0.f && leftDirectionToStart < 1.f || rightDirectionToStart < 1.f)
		{
			_direction = 1.f;
		}
	}


#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		GetActorTransform().TransformPosition(_leftStart),
		25.f,  // rayon
		12,    // segments
		FColor::Red,
		false, // persistant
		-1.f,  // durée
		0      // profondeur de priorité
	);
	DrawDebugSphere(
		GetWorld(),
		GetActorTransform().TransformPosition(_rightStart),
		25.f,
		12,
		FColor::Blue,
		false,
		-1.f,
		0
	);
#endif
}

void ASmasherTrap::Activate()
{
	UE_LOG(LogTemp, Error, TEXT("SMASHER: ACTIVATED!"));

	_isClosing = true;
	_direction = 1.f;

	Super::Activate();
}

void ASmasherTrap::Deactivate()
{
	UE_LOG(LogTemp, Error, TEXT("SMASHER: DEACTIVATED!"));

	_isClosing = false;
	_direction = -1.f;
	
	Super::Deactivate();
}

void ASmasherTrap::OnComponentHit(
			UPrimitiveComponent* hitComponent,
			AActor* otherActor,
			UPrimitiveComponent* otherComp,
			FVector normalImpulse,
			const FHitResult& hit
			)
{
	if (!_isActivated || otherActor == nullptr || otherActor == this)
	{
		_direction = -1.f;
		//UE_LOG(LogTemp, Warning, TEXT("SMASHER: HIT DETECTED, BUT IGNORED!"));	
		return;
	}
	
	if (otherActor == _leftBlock->GetOwner() || otherActor == _rightBlock->GetOwner() ||
		otherComp == _leftBlock || otherComp == _rightBlock) // Si on touche l'autre bloc
	{
		//Deactivate();
		_direction = -1.f;
		//UE_LOG(LogTemp, Warning, TEXT("Smasher: Blocks collided, returning to origin."));
		return;
	}

	//UE_LOG(LogTemp, Error, TEXT("SMASHER: Hit detected!"));
	// Si on touche le player (ou autre chose), on applique la mort si besoin (par exemple Cast<APawn>(otherActor)
	KillPlayer(otherActor);
}

void ASmasherTrap::KillPlayer(AActor* victim)
{
	OnTrapKillPlayer.Broadcast(victim);
	//UE_LOG(LogTemp, Error, TEXT("SMASHER: PLAYER CRUSHED!"));
}

bool ASmasherTrap::CheckVectorEpsilon(const FVector& start, const FVector& target, float epsilon)
{
	return FVector::DistSquared(start, target) < FMath::Square(epsilon);
}
