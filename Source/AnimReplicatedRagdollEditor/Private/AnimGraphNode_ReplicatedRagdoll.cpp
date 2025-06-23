// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimGraphNode_ReplicatedRagdoll.h"
#include "AnimationGraphSchema.h"

#define LOCTEXT_NAMESPACE "AnimGraphNode_ReplicatedRagdoll"

FText UAnimGraphNode_ReplicatedRagdoll::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("ReplicatedRagdollControllerDescription", "Replicated Ragdoll");
}

void UAnimGraphNode_ReplicatedRagdoll::CreateOutputPins()
{
	CreatePin(EGPD_Output, UAnimationGraphSchema::PC_Struct, FComponentSpacePoseLink::StaticStruct(), TEXT("ComponentPose"));
}

#undef LOCTEXT_NAMESPACE
