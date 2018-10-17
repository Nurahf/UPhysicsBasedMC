// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "MCGripperControlType.h"
#include "MCParallelGripperController.h"
#include "MCParallelGripper.generated.h"

/**
 * Actor component setting up a parallel gripper constraints and its controller
 */
UCLASS( ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC Parallel Gripper" ) )
class UMCPARALLELGRIPPER_API UMCParallelGripper : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMCParallelGripper();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Set default values to the constraints
	void SetupConstraintLimits(UPhysicsConstraintComponent* Constraint);

	// Set default physics and collision values to the static meshes
	void SetupPhysics(UStaticMeshComponent* StaticMeshComponent);

private:
	// Left finger static mesh
	UPROPERTY(EditAnywhere, Category = "Parallel Gripper")
	AStaticMeshActor* LeftFinger;

	// Right finger static mesh
	UPROPERTY(EditAnywhere, Category = "Parallel Gripper")
	AStaticMeshActor* RightFinger;

	// Left finger constraint
	UPROPERTY(EditAnywhere, Category = "Parallel Gripper")
	UPhysicsConstraintComponent* LeftFingerConstraint;
	
	// Right finger constraint
	UPROPERTY(EditAnywhere, Category = "Parallel Gripper")
	UPhysicsConstraintComponent* RightFingerConstraint;

	// Control type
	UPROPERTY(EditAnywhere, Category = "Parallel Gripper")
	EMCGripperControlType ControlType;

	// Parallel grasp controller (take the input from user and maps it to the gripper)
	UMCParallelGripperController* PGController;
};
