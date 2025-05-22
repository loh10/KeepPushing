#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Platform.generated.h"



UENUM(BlueprintType)
enum class EAxisMovement : uint8
{
	X UMETA(DisplayName = "X"),
	Y UMETA(DisplayName = "Y"),
	Z UMETA(DisplayName = "Z")
};




UCLASS()
class KEEPPUSHING_API APlatform : public AActor
{
	GENERATED_BODY()

public:
	APlatform();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* mesh; // Composants visibles et collisions (à ajouter)

	
	FVector startPosition; // Position de départ pour plateformes mouvantes
};
