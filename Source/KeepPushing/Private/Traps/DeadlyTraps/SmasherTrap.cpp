#include "Traps/DeadlyTraps/SmasherTrap.h"
#include "DrawDebugHelpers.h"
#include "Car/Car.h"

ASmasherTrap::ASmasherTrap()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_leftBlock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftBlock"));
	_leftBlock->SetCollisionProfileName(FName("BlockOnlyPawn"));
	_leftBlock->SetMobility(EComponentMobility::Movable);
	//_leftBlock->SetRelativeLocation();
	_leftBlock->SetupAttachment(_rootScene);

	_rightBlock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightBlock"));
	_rightBlock->SetCollisionProfileName(FName("BlockOnlyPawn"));
	_rightBlock->SetMobility(EComponentMobility::Movable);
	_rightBlock->SetupAttachment(_rootScene);
}

void ASmasherTrap::BeginPlay()
{
	Super::BeginPlay();

	_leftStart = _leftBlock->GetComponentLocation();
	_rightStart = _rightBlock->GetComponentLocation();

	_leftBlock->OnComponentHit.AddDynamic(this, &ASmasherTrap::OnComponentHit);
	_rightBlock->OnComponentHit.AddDynamic(this, &ASmasherTrap::OnComponentHit);
	
	Activate();
	
	{
		FBox rightBox = _rightBlock->GetStaticMesh()->GetBoundingBox();
		FBox leftBox = _leftBlock->GetStaticMesh()->GetBoundingBox();

		FVector middle  = (_leftStart + _rightStart) / 2.f;

		float gap = 100.f; // Par exemple, 20 Unreal Units d’écart

		// Vectors qui vont du centre vers chaque bloc
		FVector dirLeft = (_leftStart - middle).GetSafeNormal();
		FVector dirRight = (_rightStart - middle).GetSafeNormal();

		float leftExtent = leftBox.GetExtent().X;
		float rightExtent = rightBox.GetExtent().X;

		// Le stop doit être le centre + la moitié du bloc + la moitié de l’autre bloc + gap/2
		_leftStop = middle + dirLeft * (leftExtent + rightExtent + gap);
		_rightStop = middle + dirRight * (leftExtent + rightExtent + gap);
	}

#if WITH_EDITOR	
	DrawDebugLine(GetWorld(), _leftStart, _leftStop, FColor::Blue, true);
	DrawDebugLine(GetWorld(), _rightStart, _rightStop, FColor::Red, true);
#endif
}

void ASmasherTrap::Tick(float DeltaTime)
{

	if (!_isActivated)
	{
		/*_timer -= DeltaTime;
		if (_timer <= 0.f)
		{
			Activate();
			return;
		}*/
		Super::Tick(DeltaTime);
		return;
	}
	
	const FVector leftTarget = (_direction > 0.f) ? _leftStop : _leftStart;
	const FVector rightTarget = (_direction > 0.f) ? _rightStop : _rightStart;

	const FVector leftMove = (leftTarget - _leftBlock->GetComponentLocation()).GetSafeNormal();
	const FVector rightMove = (rightTarget - _rightBlock->GetComponentLocation()).GetSafeNormal();

	const float leftDist = FVector::Dist(_leftBlock->GetComponentLocation(), leftTarget);
	const float rightDist = FVector::Dist(_rightBlock->GetComponentLocation(), rightTarget);

	_leftBlock->AddWorldOffset(leftMove * FMath::Clamp(_moveSpeed * DeltaTime, 0, leftDist));
	_rightBlock->AddWorldOffset(rightMove * FMath::Clamp(_moveSpeed * DeltaTime, 0, rightDist));


	if (leftDist < 1.f || rightDist < 1.f)
	{
		_direction *= -1.f;

		if (_direction > 0.f)
			Deactivate();
	}
}

void ASmasherTrap::Activate()
{
	UE_LOG(LogTemp, Error, TEXT("SMASHER: ACTIVATED!"));

	_isActivated = true;

	Super::Activate();
}

void ASmasherTrap::Deactivate()
{
	UE_LOG(LogTemp, Error, TEXT("SMASHER: DEACTIVATED!"));

	_isActivated = false;
	_timer = _activationDuration;

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

	const ACar* vehicle = Cast<ACar>(otherActor);

	if (vehicle == nullptr) return;

	KillPlayer(otherActor);
}

void ASmasherTrap::KillPlayer(AActor* victim)
{
	OnTrapKillPlayer.Broadcast(victim);
	UE_LOG(LogTemp, Error, TEXT("SMASHER: PLAYER CRUSHED!"));
	/*GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Green,
				FString::Printf(TEXT("SMASHER: PLAYER CRUSHED %s"), *victim->GetName())
				);*/
}