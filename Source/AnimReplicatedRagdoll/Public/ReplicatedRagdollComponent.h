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

	bool bEvaluateAnimation;

protected:
	mutable FRWLock DataLock;
	FReplicatedRagdollData CurrentData;
	FReplicatedRagdollData Data;
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

	UFUNCTION(BlueprintPure, Category = "Ragdoll")
	virtual UAnimInstance* GetAnimationInstance() const;

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
	void CaptureRagdoll(bool bOptimizeCapture = true);

	UFUNCTION(BlueprintCallable, Category = "Ragdoll")
	void ApplyRagdoll();

	// Called on tick to decide if the ragdoll should be captured
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ragdoll")
	bool ShouldCaptureRagdoll();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ragdoll")
	bool ShouldApplyRagdoll();

	TSharedPtr<FRagdollAnimData> GetAnimDataHandle() const { return AnimDataHandle; }
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Ragdoll", ReplicatedUsing=OnRep_AnimData)
	FReplicatedRagdollData AnimData;

	TSharedPtr<FRagdollAnimData> AnimDataHandle;

	UFUNCTION()
	void OnRep_AnimData();
};
