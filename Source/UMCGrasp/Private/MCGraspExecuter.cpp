// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspExecuter.h"


// Sets default values for this component's properties
UMCGraspExecuter::UMCGraspExecuter()
{

}


// Called when the game starts
void UMCGraspExecuter::InitiateExecuter(ASkeletalMeshActor* Parent, const float& inSpringBase, const float& inSpringMultiplier, const float& inDamping, const float& inForceLimit)
{
	Hand = Parent;
	SpringBase = inSpringBase;
	SpringMultiplier = inSpringMultiplier;
	Damping = inDamping;
	ForceLimit = inForceLimit;

	USkeletalMeshComponent* const SkelComp = Hand->GetSkeletalMeshComponent();
	SkelComp->SetSimulatePhysics(true);
	SkelComp->SetEnableGravity(false);

	if (SkelComp->GetPhysicsAsset())
	{
		//sets up the constraints so they can be moved 
		for (FConstraintInstance* Constraint : SkelComp->Constraints) {
			Constraint->SetAngularSwing1Limit(ACM_Free, 0);
			Constraint->SetAngularSwing2Limit(ACM_Free, 0);
			Constraint->SetAngularTwistLimit(ACM_Free, 0);
			Constraint->SetAngularVelocityDriveTwistAndSwing(false, false);
			Constraint->SetAngularVelocityDriveSLERP(true);
			Constraint->SetAngularDriveParams(Spring, Damping, ForceLimit);
		}
	}
	bIsInitiated = true;
}

void UMCGraspExecuter::UpdateGrasp(const float Input)
{
	if (!bIsInitiated)
	{
		return;
	}

	if (GraspingData.GetNumberOfEpisodes() <= 1)
	{
		return;
	}

	Spring = SpringBase * (SpringMultiplier + Input);

	// Checks if input is relevant
	if (Input > 0.001)
	{
		if (!bIsGrasping) {
			bIsGrasping = true;
		}

		// Calculations for moving in multiple steps
		// StepSize defines how much of the 0-1 input is between each step
		// For 3 Steps for example there is a input of 0,5 seperating each 
		float StepSize = 1 / ((float)GraspingData.GetNumberOfEpisodes() - 1);
		// Calculates how many steps we have passed, given then current input
		// When rounded down we know which step came before with this input
		float StepIteratorCountFloat = Input / StepSize;

		// We will be rounding down and a input of exactly 1 causes problems there
		if (Input >= 1)
		{
			StepIteratorCountFloat = 0.999999 / StepSize;
		}

		// Uses the float to int cast to round down
		int StepIteratorCountInt = (int)StepIteratorCountFloat;

		// We calculate how far the input is past the step that came before it
		float NewInput = StepIteratorCountFloat - (float)StepIteratorCountInt;

		// Manipulate Orientation Drives
		FMCEpisodeData TargetOrientation;
		LerpHandOrientation(&TargetOrientation, GraspingData.GetPositionDataWithIndex(StepIteratorCountInt), GraspingData.GetPositionDataWithIndex(StepIteratorCountInt + 1), NewInput);
		DriveToHandOrientationTarget(&TargetOrientation);

	}
	else {
		if (bIsGrasping)
		{
			// Stop grasping
			StopGrasping();
		}
	}
}

void UMCGraspExecuter::StopGrasping()
{
	// Stop Grasp
	FMCEpisodeData Save = GraspingData.GetPositionDataWithIndex(0);
	DriveToHandOrientationTarget(&Save);
	if (bIsInQueue)
	{
		GraspingData = GraspQueue;
		bIsInQueue = false;
	}
	bIsGrasping = false;
}

void UMCGraspExecuter::LerpHandOrientation(FMCEpisodeData* Target, FMCEpisodeData Initial, FMCEpisodeData Closed, const float Input)
{
	TArray<FString> TempArray;
	Initial.GetMap()->GenerateKeyArray(TempArray);
	for (FString s : TempArray) {
		Target->AddNewBoneData(s, FMath::LerpRange(
			Initial.GetBoneData(s)->AngularDriveInput,
			Closed.GetBoneData(s)->AngularDriveInput, Input));
	}
}

void UMCGraspExecuter::DriveToHandOrientationTarget(FMCEpisodeData* Target)
{
	FConstraintInstance* Constraint = nullptr;
	TArray<FString> TempArray;
	Target->GetMap()->GenerateKeyArray(TempArray);
	for (FString s : TempArray) {
		Constraint = BoneNameToConstraint(s);
		if (Constraint) {
			Constraint->SetAngularDriveParams(Spring, Damping, ForceLimit);
			Constraint->SetAngularOrientationTarget(Target->GetMap()->Find(s)->AngularDriveInput.Quaternion());
		}
	}
}

FConstraintInstance* UMCGraspExecuter::BoneNameToConstraint(FString BoneName)
{
	FConstraintInstance* Constraint = nullptr;
	UActorComponent* component = Hand->GetComponentByClass(USkeletalMeshComponent::StaticClass());
	USkeletalMeshComponent* skeletalComponent = Cast<USkeletalMeshComponent>(component);

	//finds the constraint responsible for moving this bone
	for (FConstraintInstance* NewConstraint : skeletalComponent->Constraints) 
	{
		if (NewConstraint->ConstraintBone1.ToString() == BoneName) {
			Constraint = NewConstraint;
		}
	}
	return Constraint;
}

void UMCGraspExecuter::SetGraspingData(FMCAnimationData Data) {
	// if player is grasping put new grasp in queue, else change grasp immediately
	if (bIsGrasping)
	{
		GraspQueue = Data;
		bIsInQueue = true;
		return;
	}
	GraspingData = Data;
}
