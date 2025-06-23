// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "RRSkeletalMeshComponent.generated.h"

/**
 * Don't use this class. This class is just used to get around 
 * ApplyEditedComponentSpaceTransforms() being editor-only.
 */
UCLASS(Abstract)
class ANIMREPLICATEDRAGDOLL_API URRSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	/** Called after modifying Component Space Transforms externally */
	void ApplyEditedComponentSpaceTransforms();
};
