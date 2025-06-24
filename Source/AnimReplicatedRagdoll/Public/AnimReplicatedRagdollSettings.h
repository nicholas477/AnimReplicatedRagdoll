// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AnimReplicatedRagdollSettings.generated.h"

/**
 * 
 */
UCLASS(config = Engine, defaultconfig)
class ANIMREPLICATEDRAGDOLL_API UAnimReplicatedRagdollSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere)
	EVectorQuantization LocationQuantizationLevel = EVectorQuantization::RoundOneDecimal;

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere)
	ERotatorQuantization RotationQuantizationLevel = ERotatorQuantization::ByteComponents;

	static UAnimReplicatedRagdollSettings* Get() { return GetMutableDefault<ThisClass>(); }
};
