// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "ReplicatedRagdollComponent.h"
#include "AnimNode_ReplicatedRagdoll.generated.h"

/**
 * 
 */
USTRUCT(BlueprintInternalUseOnly)
struct ANIMREPLICATEDRAGDOLL_API FAnimNode_ReplicatedRagdoll : public FAnimNode_Base
{
	GENERATED_BODY()
public:
	// Input link
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
	FComponentSpacePoseLink ComponentPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Evaluation, meta = (PinShownByDefault))
	float InterpSpeed = 25.f;

	virtual bool HasPreUpdate() const override { return true; }
	virtual void PreUpdate(const UAnimInstance* InAnimInstance) override;
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;

protected:
	TSharedPtr<FRagdollAnimData> AnimDataHandle;
};
