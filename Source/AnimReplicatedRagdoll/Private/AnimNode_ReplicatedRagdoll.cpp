// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNode_ReplicatedRagdoll.h"

#include "Animation/AnimInstanceProxy.h"

void FAnimNode_ReplicatedRagdoll::PreUpdate(const UAnimInstance* InAnimInstance)
{
	// cache the currently used skeletal mesh's bone names
	if (InAnimInstance->GetSkelMeshComponent() && InAnimInstance->GetSkelMeshComponent()->IsRegistered())
	{
		USkeletalMeshComponent* SkeletalMeshComponent = InAnimInstance->GetSkelMeshComponent();
		if (SkeletalMeshComponent)
		{
			for (TObjectPtr<USceneComponent> Component : SkeletalMeshComponent->GetAttachChildren())
			{
				if (UReplicatedRagdollComponent* RagdollComponent = Cast<UReplicatedRagdollComponent>(Component))
				{
					AnimDataHandle = RagdollComponent->GetAnimDataHandle();

					if (AnimDataHandle.IsValid())
					{
						if (AnimDataHandle->ReadCurrentRagdollData().ComponentSpaceTransforms.Num() > 0)
						{
							FReplicatedRagdollData CurrentData;
							CurrentData.CapturePose(SkeletalMeshComponent);
							AnimDataHandle->WriteCurrentRagdollData(CurrentData);
						}
						else
						{
							AnimDataHandle->WriteCurrentRagdollData(AnimDataHandle->ReadRagdollData());
						}
					}

					break;
				}
			}
		}

	}
}

void FAnimNode_ReplicatedRagdoll::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	FAnimNode_Base::Initialize_AnyThread(Context);

	ComponentPose.Initialize(Context);
}

void FAnimNode_ReplicatedRagdoll::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)
	FAnimNode_Base::CacheBones_AnyThread(Context);
	//InitializeBoneReferences(Context.AnimInstanceProxy->GetRequiredBones());
	ComponentPose.CacheBones(Context);
}

void FAnimNode_ReplicatedRagdoll::EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateComponentSpace_AnyThread)
	ANIM_MT_SCOPE_CYCLE_COUNTER_VERBOSE(ReplicatedRagdoll, !IsInGameThread());

	Super::EvaluateComponentSpace_AnyThread(Output);
	ComponentPose.EvaluateComponentSpace(Output);

	if (AnimDataHandle.IsValid())
	{
		if (!AnimDataHandle->ReadEvaluateAnimation() || AnimDataHandle->ReadRagdollData().ComponentSpaceTransforms.Num() == 0)
		{
			return;
		}

		FReplicatedRagdollData Transforms = AnimDataHandle->GetInterpedRagdollData(Output.AnimInstanceProxy->GetDeltaSeconds(), InterpSpeed);
		if (Transforms.ComponentSpaceTransforms.Num() == 0)
		{
			// if we don't have interped data then just go to the target
			Transforms = AnimDataHandle->ReadRagdollData();
		}

		{
			TArray<FBoneTransform> BoneTransforms;
			BoneTransforms.Reserve(Transforms.ComponentSpaceTransforms.Num());

			int32 i = 0;
			for (const FReplicatedRagdollTransform& RagdollTransform : Transforms.ComponentSpaceTransforms)
			{
				FBoneTransform BoneTransform;
				BoneTransform.BoneIndex = FCompactPoseBoneIndex(i);
				BoneTransform.Transform = RagdollTransform;
				i++;

				BoneTransforms.Add(BoneTransform);
			}

			Output.Pose.LocalBlendCSBoneTransforms(BoneTransforms, 1.0f);
		}
	}
}

void FAnimNode_ReplicatedRagdoll::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread);
	Super::Update_AnyThread(Context);

	GetEvaluateGraphExposedInputs().Execute(Context);

	ComponentPose.Update(Context);
}
