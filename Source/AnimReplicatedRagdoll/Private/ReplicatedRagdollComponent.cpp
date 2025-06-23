// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicatedRagdollComponent.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

UReplicatedRagdollComponent::UReplicatedRagdollComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UReplicatedRagdollComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner()->HasAuthority() && ShouldCaptureRagdoll() && GetNetMode() != ENetMode::NM_Standalone)
	{
		CaptureRagdoll();
	}
}

void UReplicatedRagdollComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AnimData, Params);
}

void UReplicatedRagdollComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (!AnimDataHandle.IsValid())
	{
		AnimDataHandle = MakeShared<decltype(AnimDataHandle)::ElementType>();
		AnimDataHandle->bEvaluateAnimation = !GetOwner()->HasAuthority();
	}
}

void UReplicatedRagdollComponent::Serialize(FArchive& Ar)
{
	if (Ar.IsSaving() && Ar.ArIsSaveGame)
	{
		CaptureRagdoll(false);
	}

	Super::Serialize(Ar);

	if (Ar.IsLoading() && Ar.ArIsSaveGame)
	{
		ApplyRagdoll();
	}
}

USkeletalMeshComponent* UReplicatedRagdollComponent::GetSkeletalMesh() const
{
	return Cast<USkeletalMeshComponent>(GetAttachParent());
}

UAnimInstance* UReplicatedRagdollComponent::GetAnimationInstance() const
{
	if (USkeletalMeshComponent* SkeletalMesh = GetSkeletalMesh())
	{
		return SkeletalMesh->GetAnimInstance();
	}
	return nullptr;
}

void UReplicatedRagdollComponent::CaptureRagdoll(bool bOptimizeCapture)
{
	if (USkeletalMeshComponent* SkeletalMesh = GetSkeletalMesh())
	{
		AnimData.CapturePose(SkeletalMesh, bOptimizeCapture);
		AnimDataHandle->WriteRagdollData(AnimData);
	}
}

void UReplicatedRagdollComponent::ApplyRagdoll()
{
	if (USkeletalMeshComponent* SkeletalMesh = GetSkeletalMesh())
	{
		AnimData.ApplyPose(SkeletalMesh);
	}
}

bool UReplicatedRagdollComponent::ShouldApplyRagdoll_Implementation()
{
	return true;
}

bool UReplicatedRagdollComponent::ShouldCaptureRagdoll_Implementation()
{
	return true;
}

void UReplicatedRagdollComponent::OnRep_AnimData()
{
	//ApplyRagdoll();

	USkeletalMeshComponent* SkeletalMesh = GetSkeletalMesh();
	if (SkeletalMesh == nullptr)
	{
		return;
	}

	if (!AnimDataHandle.IsValid())
	{
		AnimDataHandle = MakeShared<decltype(AnimDataHandle)::ElementType>();
	}
	AnimDataHandle->WriteRagdollData(AnimData);
}
