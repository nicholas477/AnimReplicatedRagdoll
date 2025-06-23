// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_Base.h"
#include "AnimNode_ReplicatedRagdoll.h"
#include "AnimGraphNode_ReplicatedRagdoll.generated.h"

/**
 * 
 */
UCLASS()
class ANIMREPLICATEDRAGDOLLEDITOR_API UAnimGraphNode_ReplicatedRagdoll : public UAnimGraphNode_Base
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_ReplicatedRagdoll Node;

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//virtual FLinearColor GetNodeTitleColor() const override;
	//virtual FText GetTooltipText() const override;
	//virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//virtual void GetInputLinkAttributes(FNodeAttributeArray& OutAttributes) const override;
	//virtual void GetOutputLinkAttributes(FNodeAttributeArray& OutAttributes) const override;
	// End of UEdGraphNode interface

	// UAnimGraphNode_Base interface
	virtual void CreateOutputPins() override;
	//virtual void PostProcessPinName(const UEdGraphPin* Pin, FString& DisplayName) const override;
	// End of UAnimGraphNode_Base interface
};
