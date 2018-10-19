// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MCGraspController.generated.h"

/**
 * Skeletal grasp controller
 */
UCLASS( ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC Grasp Controller"))
class UMCGRASP_API UMCGraspController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMCGraspController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
private:
	// Update the grasp
	void Update(float Value);

private:
	// Input axis name
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	FName InputAxisName;

	// Angular drive mode
	UPROPERTY(EditAnywhere, Category = "Grasp Controller")
	TEnumAsByte<EAngularDriveMode::Type> AngularDriveMode;

	// Spring value to apply to the angular drive (Position strength)
	UPROPERTY(EditAnywhere, Category = "Grasp Controller", meta = (ClampMin = 0))
	float Spring;

	// Damping value to apply to the angular drive (Velocity strength) 
	UPROPERTY(EditAnywhere, Category = "Grasp Controller", meta = (ClampMin = 0))
	float Damping;

	// Limit of the force that the angular drive can apply
	UPROPERTY(EditAnywhere, Category = "Grasp Controller", meta = (ClampMin = 0))
	float ForceLimit;

	// Skeletal mesh of the owner
	class USkeletalMeshComponent* SkeletalMesh;
};
