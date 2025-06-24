// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AnimReplicatedRagdollTypes.h"
#include "ReplicatedRagdollComponent.generated.h"

struct FRagdollAnimData
{
	FReplicatedRagdollData ReadCurrentRagdollData() const
	{
		FReadScopeLock Lock(DataLock);
		return CurrentData;
	}

	void WriteCurrentRagdollData(const FReplicatedRagdollData& NewData)
	{
		FWriteScopeLock Lock(DataLock);
		CurrentData = NewData;
	}

	FReplicatedRagdollData ReadRagdollData() const
	{
		FReadScopeLock Lock(DataLock);
		return Data;
	}

	void WriteRagdollData(const FReplicatedRagdollData& NewData)
	{
		FWriteScopeLock Lock(DataLock);
		Data = NewData;
	}

	bool ReadEvaluateAnimation() const
	{
		FReadScopeLock Lock(DataLock);
		return bEvaluateAnimation;
	}

	void WriteEvaluateAnimation(bool bNewEvaluateAnimation)
	{
		FWriteScopeLock Lock(DataLock);
		bEvaluateAnimation = bNewEvaluateAnimation;
	}

	FReplicatedRagdollData GetInterpedRagdollData(float DeltaTime, float InterpSpeed) const
	{
		FReplicatedRagdollData OutData;
		const FReplicatedRagdollData CurrentDataCopy = ReadCurrentRagdollData();
		const FReplicatedRagdollData DataCopy = ReadRagdollData();

		if (CurrentDataCopy.ComponentSpaceTransforms.Num() == DataCopy.ComponentSpaceTransforms.Num() && CurrentDataCopy.ComponentSpaceTransforms.Num() > 0)
		{
			OutData.ComponentSpaceTransforms.SetNum(CurrentDataCopy.ComponentSpaceTransforms.Num());

			for (int32 i = 0; i < CurrentDataCopy.ComponentSpaceTransforms.Num(); ++i)
			{
				OutData.ComponentSpaceTransforms[i].BoneTransform.SetLocation(
					FMath::VInterpTo(
						CurrentDataCopy.ComponentSpaceTransforms[i].BoneTransform.GetLocation(),
						DataCopy.ComponentSpaceTransforms[i].BoneTransform.GetLocation(),
						DeltaTime,
						InterpSpeed
					)
				);

				OutData.ComponentSpaceTransforms[i].BoneTransform.SetRotation(
					FMath::RInterpTo(
						CurrentDataCopy.ComponentSpaceTransforms[i].BoneTransform.GetRotation().Rotator(),
						DataCopy.ComponentSpaceTransforms[i].BoneTransform.GetRotation().Rotator(),
						DeltaTime,
						InterpSpeed
					).Quaternion()
				);

				OutData.ComponentSpaceTransforms[i].BoneTransform.SetScale3D(FVector(1.f));
			}
		}
		
		return OutData;
	}

protected:
	mutable FRWLock DataLock;
	FReplicatedRagdollData CurrentData;
	FReplicatedRagdollData Data;
	bool bEvaluateAnimation;
};

UCLASS( ClassGroup=(Ragdoll), meta = (BlueprintSpawnableComponent))
class ANIMREPLICATEDRAGDOLL_API UReplicatedRagdollComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UReplicatedRagdollComponent(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitializeComponent() override;
	virtual void Serialize(FArchive& Ar) override;

	UFUNCTION(BlueprintPure, Category = "Ragdoll")
	virtual USkeletalMeshComponent* GetSkeletalMesh() const;

	// Empties the ragdoll data
	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
	void ClearRagdoll();

	// Reads the bone transforms from the skeletal mesh and copies them into AnimData.
	// 
	// bOptimizeCapture = true does an optimized network capture and only updates 
	// bone transforms if they are more than 1cm or 1degree off.
	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
	void CaptureRagdoll(bool bOptimizeCapture = true);

	// Sets the skeletal mesh bone transforms to the bone transforms in AnimData.
	//
	// This only works if the skeletal mesh is in simulate physics mode.
	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
	void ApplyRagdoll();

	// Called on tick on the server to decide if the ragdoll should be captured
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ragdoll")
	bool ShouldCaptureRagdoll();

	// Called on tick to decide if the ragdoll animation node should apply the replicated
	// bone data.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ragdoll")
	bool ShouldApplyRagdoll();

	TSharedPtr<FRagdollAnimData> GetAnimDataHandle() const { return AnimDataHandle; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ragdoll", ReplicatedUsing=OnRep_AnimData)
	FReplicatedRagdollData AnimData;

	// This is the anim data that the animation system actually uses.
	TSharedPtr<FRagdollAnimData> AnimDataHandle;

	UFUNCTION()
	void OnRep_AnimData();
};
