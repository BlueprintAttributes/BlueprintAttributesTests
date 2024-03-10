// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#include "AttributeSet.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/AutomationTest.h"
#include "Subsystems/GBAEditorSubsystem.h"

BEGIN_DEFINE_SPEC(FGBAEditorSubsystemSpec, "BlueprintAttributes.Editor.GBAEditorSubsystem", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	const FString FixtureAttributeSetLoadPath = TEXT("/BlueprintAttributesTests/Fixtures/GBAEditorSubsystem/GBA_Reff_Test.GBA_Reff_Test_C");
	TWeakObjectPtr<UBlueprint> BlueprintWeakPtr = nullptr;

	const FName OldName = TEXT("Ref_01");
	const FName NewName = TEXT("Renamed_Ref_01");

	/**
	 * Simulates the user pressing the blueprint's compile button (will load the
	 * blueprint first if it isn't already).
	 * 
	 * @param InBlueprint The Blueprint that you wish to compile
	 * @return False if we failed to load the blueprint, true otherwise
	 */
	bool CompileBlueprint(UBlueprint* InBlueprint)
	{
		if (!InBlueprint || !InBlueprint->ParentClass)
		{
			AddError(FString::Printf(TEXT("Failed to compile invalid blueprint, or blueprint parent no longer exists (InBlueprint: %s)"), *GetNameSafe(InBlueprint)));
			return false;
		}

		UPackage* const BlueprintPackage = InBlueprint->GetOutermost();
		// compiling the blueprint will inherently dirty the package, but if there 
		// weren't any changes to save before, there shouldn't be after
		bool const bStartedWithUnsavedChanges = (BlueprintPackage != nullptr) ? BlueprintPackage->IsDirty() : true;

		FKismetEditorUtilities::CompileBlueprint(InBlueprint, EBlueprintCompileOptions::SkipGarbageCollection);

		if (BlueprintPackage != nullptr)
		{
			BlueprintPackage->SetDirtyFlag(bStartedWithUnsavedChanges);
		}

		return true;
	}

	void CheckBlueprintGraphs(const UBlueprint* InBlueprint)
	{
		AddInfo(FString::Printf(TEXT("Check Blueprint: %s"), *GetNameSafe(InBlueprint)));
		if (!InBlueprint)
		{
			AddError(FString::Printf(TEXT("Failed to check invalid blueprint (InBlueprint: %s)"), *GetNameSafe(InBlueprint)));
			return;
		}


		TArray<UEdGraph*> Graphs;
		InBlueprint->GetAllGraphs(Graphs);

		const UEdGraph* Graph = InBlueprint->GetLastEditedUberGraph();
		if (!Graph)
		{
			AddError(FString::Printf(TEXT("Failed to check blueprint (InBlueprint: %s), LastEditedUberGraph is not valid"), *GetNameSafe(InBlueprint)));
			return;
		}
		
		AddInfo(FString::Printf(TEXT("LastEditedUberGraph: %s"), *GetNameSafe(Graph)));
		if (Graph->GetName() != TEXT("EventGraph"))
		{
			AddError(FString::Printf(TEXT("Failed to check blueprint (InBlueprint: %s), LastEditedUberGraph is not EventGraph"), *GetNameSafe(InBlueprint)));
			return;
		}
		
		AddInfo(FString::Printf(TEXT("Graphs: %d"), Graphs.Num()));
		
		TArray<UK2Node_CallFunction*> Nodes;
		Graph->GetNodesOfClass(Nodes);

		AddInfo(FString::Printf(TEXT("Nodes: %d"), Nodes.Num()));
		for (const UK2Node_CallFunction* Node : Nodes)
		{
			AddInfo(FString::Printf(TEXT("\t Node: %s"), *GetNameSafe(Node)));
			if (!Node)
			{
				continue;
			}

			TArray<UEdGraphPin*> Pins = Node->Pins;

			UEdGraphPin** PinPtr = Pins.FindByPredicate([](const UEdGraphPin* InPin)
			{
				return InPin->Direction == EGPD_Input
					&& InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct
					&& InPin->PinType.PinSubCategoryObject == FGameplayAttribute::StaticStruct();
			});

			if (!PinPtr || !*PinPtr)
			{
				continue;
			}

			const UEdGraphPin* Pin = *PinPtr;
			check(Pin);

			FString PackageName;
			FString AttributeName;
			UGBAEditorSubsystem::ParseAttributeFromDefaultValue(Pin->GetDefaultAsString(), PackageName, AttributeName);
			AddInfo(FString::Printf(TEXT("\t Pin: %s, PackageName: %s, AttributeName: %s"), *Pin->GetName(), *PackageName, *AttributeName));

			const FString BlueprintName = GetNameSafe(Node->GetBlueprint());
			const FString GraphName = GetNameSafe(Node->GetGraph());
			const FString NodeName = Node->GetNodeTitle(ENodeTitleType::ListView).ToString();
			const FString PinPath = FString::Printf(TEXT("%s > %s > %s"), *BlueprintName, *GraphName, *NodeName);
			TestEqual(FString::Printf(TEXT("Check Pin value of %s"), *PinPath), AttributeName, NewName.ToString());
		}
	}

END_DEFINE_SPEC(FGBAEditorSubsystemSpec)

void FGBAEditorSubsystemSpec::Define()
{
	Describe(TEXT("Handle K2Nodes on Attribute rename"), [this]()
	{
		BeforeEach([this]()
		{
			const UClass* FixtureClass = StaticLoadClass(UAttributeSet::StaticClass(), nullptr, *FixtureAttributeSetLoadPath);
			if (!FixtureClass)
			{
				AddError(FString::Printf(TEXT("Unable to load fixture class from %s"), *FixtureAttributeSetLoadPath));
				return;
			}

			BlueprintWeakPtr = UBlueprint::GetBlueprintFromClass(FixtureClass);
			if (!FixtureClass)
			{
				AddError(FString::Printf(TEXT("Unable to load Blueprint from class %s"), *FixtureClass->GetName()));
			}
		});
		
		It(TEXT("should update all K2 Nodes with a FGameplayAttribute Pin parameter"), [this]()
		{
			if (!BlueprintWeakPtr.IsValid())
			{
				AddError(FString::Printf(TEXT("Unable to load Blueprint from invalid weak ptr")));
				return;
			}

			UBlueprint* Blueprint = BlueprintWeakPtr.Get();
			check(Blueprint);

			AddInfo(FString::Printf(TEXT("Renaming attribute from %s to %s in %s"), *OldName.ToString(), *NewName.ToString(), *Blueprint->GetName()));
			FBlueprintEditorUtils::RenameMemberVariable(Blueprint, OldName, NewName);
			CompileBlueprint(Blueprint);

			CheckBlueprintGraphs(Blueprint);
		});

		AfterEach([this]()
		{
			AddInfo(FString::Printf(TEXT("Restore %s BP to original state"), *GetNameSafe(BlueprintWeakPtr.Get())));
			if (!BlueprintWeakPtr.IsValid())
			{
				AddError(FString::Printf(TEXT("AfterEach clean up failed to load Blueprint from invalid weak ptr")));
			}
			
			UBlueprint* Blueprint = BlueprintWeakPtr.Get();
			check(Blueprint);
			
			FBlueprintEditorUtils::RenameMemberVariable(Blueprint, NewName, OldName);
			CompileBlueprint(Blueprint);
		});
	});
}
