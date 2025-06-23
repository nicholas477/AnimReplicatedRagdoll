// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimReplicatedRagdollTypes.h"

#include "RRSkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/Core/Serialization/QuantizedVectorSerialization.h"

void FReplicatedRagdollData::CapturePose(const USkeletalMeshComponent* SkeletalMesh, bool bOptimizeCapture)
{
	check(SkeletalMesh != nullptr);
	const TArray<FTransform> BoneTransforms = SkeletalMesh->GetComponentSpaceTransforms();

	const ENetMode NetMode = SkeletalMesh->GetNetMode();
	if (bOptimizeCapture && (NetMode == ENetMode::NM_DedicatedServer || NetMode == ENetMode::NM_ListenServer) && (BoneTransforms.Num() == ComponentSpaceTransforms.Num()))
	{
		for (int32 i = 0; i < BoneTransforms.Num(); ++i)
		{
			FReplicatedRagdollTransform& Transform = ComponentSpaceTransforms[i];
			const FTransform& NewTransform = BoneTransforms[i];
			const bool LocationsEqual = (NewTransform.GetLocation() - Transform.BoneTransform.GetLocation()).IsNearlyZero(1.f);
			const bool RotationsEqual = (NewTransform.GetRotation().Rotator() - Transform.BoneTransform.GetRotation().Rotator()).IsNearlyZero(1.f);
			if (!LocationsEqual || !RotationsEqual)
			{
				Transform.BoneTransform = NewTransform;
				MarkItemDirty(Transform);
			}
		}
	}
	else
	{
		ComponentSpaceTransforms.SetNum(BoneTransforms.Num());
		for (int32 i = 0; i < BoneTransforms.Num(); ++i)
		{
			ComponentSpaceTransforms[i].BoneTransform = BoneTransforms[i];
		}
		MarkArrayDirty();
	}
}

void FReplicatedRagdollData::ApplyPose(USkeletalMeshComponent* SkeletalMesh)
{
	check(SkeletalMesh != nullptr);
	TArray<FTransform>& BoneTransforms = SkeletalMesh->GetEditableComponentSpaceTransforms();

	if (BoneTransforms.Num() == ComponentSpaceTransforms.Num())
	{
		for (int32 i = 0; i < ComponentSpaceTransforms.Num(); ++i)
		{
			BoneTransforms[i] = ComponentSpaceTransforms[i].BoneTransform;
		}
		((URRSkeletalMeshComponent*)SkeletalMesh)->ApplyEditedComponentSpaceTransforms();
	}
}

bool FReplicatedRagdollTransform::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	FRotator BoneRotation = BoneTransform.GetRotation().Rotator();
	BoneRotation.SerializeCompressed(Ar);

	if (Ar.IsLoading())
	{
		BoneTransform.SetRotation(BoneRotation.Quaternion());
	}

	FVector Location = BoneTransform.GetLocation();
	SerializePackedVector<1, 24>(Location, Ar);

	if (Ar.IsLoading())
	{
		BoneTransform.SetLocation(Location);
	}

	return true;
}
