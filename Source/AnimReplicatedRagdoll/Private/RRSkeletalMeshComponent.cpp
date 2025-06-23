// Fill out your copyright notice in the Description page of Project Settings.


#include "RRSkeletalMeshComponent.h"

void URRSkeletalMeshComponent::ApplyEditedComponentSpaceTransforms()
{
	// Flip buffers once to copy the directly-written component space transforms
	bNeedToFlipSpaceBaseBuffers = true;
	bHasValidBoneTransform = false;
	FlipEditableSpaceBases();
	bHasValidBoneTransform = true;

	InvalidateCachedBounds();
	UpdateBounds();
	MarkRenderTransformDirty();
	MarkRenderDynamicDataDirty();

	for (auto& FollowerComponent : GetFollowerPoseComponents())
	{
		if (FollowerComponent.IsValid())
		{
			FollowerComponent->UpdateFollowerComponent();
		}
	}
}
