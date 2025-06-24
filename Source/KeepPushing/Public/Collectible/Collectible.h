#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Collectible.generated.h"

class USphereComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCollected, ACollectible*, CollectedItem);

UCLASS()
class KEEPPUSHING_API ACollectible : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACollectible();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Sphere Collider
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereCollider;

	// Static Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Radius of the Sphere Collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible Settings")
	float Radius;

	// Mesh to be used for the Collectible
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible Settings")
	UStaticMesh* Mesh;

	// Event triggered when collected
	UPROPERTY(BlueprintAssignable, Category = "Collectible")
	FOnCollected Collected;

private:
	// Function called when an overlap begins
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};