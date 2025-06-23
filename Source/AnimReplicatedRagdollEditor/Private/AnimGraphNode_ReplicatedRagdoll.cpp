// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimGraphNode_ReplicatedRagdoll.h"
#include "AnimationGraphSchema.h"

void UAnimGraphNode_ReplicatedRagdoll::CreateOutputPins()
{
	CreatePin(EGPD_Output, UAnimationGraphSchema::PC_Struct, FComponentSpacePoseLink::StaticStruct(), TEXT("ComponentPose"));
}
