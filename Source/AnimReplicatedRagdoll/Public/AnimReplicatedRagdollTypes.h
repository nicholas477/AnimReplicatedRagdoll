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

template<> struct TStructOpsTypeTraits<FReplicatedRagdollTransform> : public TStructOpsTypeTraitsBase2<FReplicatedRagdollTransform>
{
	enum
	{
		WithSerializer = true,
		WithNetSerializer = true,
	};
};

static FArchive& operator<<(FArchive& Ar, FReplicatedRagdollTransform& Value)
{
	Value.Serialize(Ar);
	return Ar;
}

USTRUCT(BlueprintType)
struct FReplicatedRagdollData : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FReplicatedRagdollTransform> ComponentSpaceTransforms;

	void CapturePose(const USkeletalMeshComponent* SkeletalMesh, bool bOptimizeCapture = true);
	void ApplyPose(USkeletalMeshComponent* SkeletalMesh);

	bool Serialize(FArchive& Ar)
	{
		Ar << ComponentSpaceTransforms;
		return true;
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FReplicatedRagdollTransform, FReplicatedRagdollData>(ComponentSpaceTransforms, DeltaParms, *this);
	}
};

template<> struct TStructOpsTypeTraits<FReplicatedRagdollData> : public TStructOpsTypeTraitsBase2<FReplicatedRagdollData>
{
	enum
	{
		WithSerializer = true,
		WithNetDeltaSerializer = true,
	};
};