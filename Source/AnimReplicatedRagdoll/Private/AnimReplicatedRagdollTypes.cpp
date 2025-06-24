// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimReplicatedRagdollTypes.h"

#include "RRSkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Net/Core/Serialization/QuantizedVectorSerialization.h"
#include "AnimReplicatedRagdollSettings.h"

// Returns how much location/rotation difference there must be for a bone's transform to get replicated
static TPair<float, float> GetMinTransformDelta(EVectorQuantization VectorQuantization, ERotatorQuantization RotationQuantization)
{
	float VectorDelta = 1.f;
	float RotationDelta = 1.f;

	switch (VectorQuantization)
	{
	case EVectorQuantization::RoundTwoDecimals:
		VectorDelta = 0.01f;
		break;
	case EVectorQuantization::RoundOneDecimal:
		VectorDelta = 0.1f;
		break;
	case EVectorQuantization::RoundWholeNumber:
		VectorDelta = 1.f;
		break;
	}

	switch (RotationQuantization)
	{
	case ERotatorQuantization::ByteComponents:
		RotationDelta = 360.f / 256.f;
		break;

	case ERotatorQuantization::ShortComponents:
		RotationDelta = 360.f / 65536.f;
		break;
	}

	return { VectorDelta, RotationDelta };
}

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
			const TPair<float, float> Epsilon = GetMinTransformDelta(UAnimReplicatedRagdollSettings::Get()->LocationQuantizationLevel, UAnimReplicatedRagdollSettings::Get()->RotationQuantizationLevel);
			const bool LocationsEqual = (NewTransform.GetLocation() - Transform.BoneTransform.GetLocation()).IsNearlyZero(Epsilon.Key);
			const bool RotationsEqual = (NewTransform.GetRotation().Rotator() - Transform.BoneTransform.GetRotation().Rotator()).IsNearlyZero(Epsilon.Value);
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

	const UAnimReplicatedRagdollSettings* Settings = UAnimReplicatedRagdollSettings::Get();

	switch (Settings->RotationQuantizationLevel)
	{
	case ERotatorQuantization::ByteComponents:
		BoneRotation.SerializeCompressed(Ar);
		break;

	case ERotatorQuantization::ShortComponents:
		BoneRotation.SerializeCompressedShort(Ar);
		break;

	default:
		break;
	}

	if (Ar.IsLoading())
	{
		BoneTransform.SetRotation(BoneRotation.Quaternion());
	}

	FVector Location = BoneTransform.GetLocation();

	switch (Settings->LocationQuantizationLevel)
	{
	case EVectorQuantization::RoundWholeNumber:
		SerializePackedVector<1, 24>(Location, Ar);
		break;

	case EVectorQuantization::RoundOneDecimal:
		SerializePackedVector<10, 27>(Location, Ar);
		break;

	case EVectorQuantization::RoundTwoDecimals:
		SerializePackedVector<100, 30>(Location, Ar);
		break;

	default:
		break;
	}

	if (Ar.IsLoading())
	{
		BoneTransform.SetLocation(Location);
	}

	return true;
}
