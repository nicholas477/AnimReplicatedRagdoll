// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "AnimReplicatedRagdollTypes.generated.h"

USTRUCT(BlueprintType)
struct FReplicatedRagdollTransform : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform BoneTransform;

	operator FTransform() const { return BoneTransform; }
	FReplicatedRagdollTransform& operator=(const FTransform& Other) { BoneTransform = Other; return *this; }

	bool Serialize(FArchive& Ar)
	{
		Ar << BoneTransform;
		return true;
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

static FArchive& operator<<(FArchive& Ar, FReplicatedRagdollTransform& Value)
{
	Value.Serialize(Ar);
	return Ar;
}

template<> struct TStructOpsTypeTraits<FReplicatedRagdollTransform> : public TStructOpsTypeTraitsBase2<FReplicatedRagdollTransform>
{
	enum
	{
		WithNetSerializer = true,
		WithSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct FReplicatedRagdollData : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
	TArray<FReplicatedRagdollTransform> ComponentSpaceTransforms;

	void CapturePose(const USkeletalMeshComponent* SkeletalMesh, bool bOptimizeCapture = true);
	void ApplyPose(USkeletalMeshComponent* SkeletalMesh);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FReplicatedRagdollTransform, FReplicatedRagdollData>(ComponentSpaceTransforms, DeltaParms, *this);
	}

	bool Serialize(FArchive& Ar)
	{
		Ar << ComponentSpaceTransforms;
		return true;
	}
};

static FArchive& operator<<(FArchive& Ar, FReplicatedRagdollData& Value)
{
	Value.Serialize(Ar);
	return Ar;
}


template<> struct TStructOpsTypeTraits<FReplicatedRagdollData> : public TStructOpsTypeTraitsBase2<FReplicatedRagdollData>
{
	enum
	{
		WithNetDeltaSerializer = true,
		WithSerializer = true,
	};
};