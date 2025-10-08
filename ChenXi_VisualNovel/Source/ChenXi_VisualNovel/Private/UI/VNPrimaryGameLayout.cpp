// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#include "UI/VNPrimaryGameLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"
#include "Components/NamedSlot.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VNPrimaryGameLayout)

UVNPrimaryGameLayout::UVNPrimaryGameLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVNPrimaryGameLayout::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("VNPrimaryGameLayout: NativeConstruct called"));
}

bool UVNPrimaryGameLayout::PushWidgetToLayerStack(FGameplayTag LayerTag, UCommonActivatableWidget* ActivatableWidget)
{
	if (!ActivatableWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNPrimaryGameLayout: Attempted to push null widget to layer %s"), *LayerTag.ToString());
		return false;
	}

	// Find or create the layer stack
	UCommonActivatableWidgetStack* LayerStack = FindOrCreateLayerStack(LayerTag);
	if (!LayerStack)
	{
		UE_LOG(LogTemp, Error, TEXT("VNPrimaryGameLayout: Failed to find or create layer stack for tag %s"), *LayerTag.ToString());
		return false;
	}

	// Push the widget instance to the stack
	LayerStack->AddWidgetInstance(*ActivatableWidget);

	UE_LOG(LogTemp, Log, TEXT("VNPrimaryGameLayout: Pushed widget %s to layer %s"),
		*ActivatableWidget->GetName(), *LayerTag.ToString());

	return true;
}

UCommonActivatableWidgetStack* UVNPrimaryGameLayout::FindOrCreateLayerStack(FGameplayTag LayerTag)
{
	// Check if stack already exists
	if (TObjectPtr<UCommonActivatableWidgetStack>* ExistingStack = LayerStacks.Find(LayerTag))
	{
		return *ExistingStack;
	}

	// Stack not found - in a production implementation, we might create it dynamically
	// For now, return nullptr and require Blueprint setup
	UE_LOG(LogTemp, Warning, TEXT("VNPrimaryGameLayout: Layer stack for tag %s not registered. Did you call RegisterLayer from Blueprint?"),
		*LayerTag.ToString());

	return nullptr;
}

void UVNPrimaryGameLayout::RegisterLayerStack(FGameplayTag LayerTag, UCommonActivatableWidgetStack* LayerStack)
{
	if (!LayerTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("VNPrimaryGameLayout: Attempted to register stack layer with invalid tag"));
		return;
	}

	if (!LayerStack)
	{
		UE_LOG(LogTemp, Error, TEXT("VNPrimaryGameLayout: Attempted to register null layer stack for tag %s"), *LayerTag.ToString());
		return;
	}

	LayerStacks.Add(LayerTag, LayerStack);

	UE_LOG(LogTemp, Log, TEXT("VNPrimaryGameLayout: Registered stack layer for tag %s"), *LayerTag.ToString());
}

void UVNPrimaryGameLayout::AddNativeWidgetToLayer(FGameplayTag LayerTag, UUserWidget* Widget)
{
	if (!Widget)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNPrimaryGameLayout: Attempted to add null widget to native layer %s"), *LayerTag.ToString());
		return;
	}

	// Find the named slot for this layer
	TObjectPtr<UNamedSlot>* SlotPtr = NativeLayers.Find(LayerTag);
	if (!SlotPtr || !(*SlotPtr))
	{
		UE_LOG(LogTemp, Error, TEXT("VNPrimaryGameLayout: Native layer %s not registered. Call RegisterNativeLayer from Blueprint."), *LayerTag.ToString());
		return;
	}

	UNamedSlot* NamedSlotWidget = *SlotPtr;

	// Add widget to the named slot
	NamedSlotWidget->ClearChildren();
	NamedSlotWidget->AddChild(Widget);

	UE_LOG(LogTemp, Log, TEXT("VNPrimaryGameLayout: Added widget %s to native layer %s"),
		*Widget->GetName(), *LayerTag.ToString());
}

void UVNPrimaryGameLayout::RemoveNativeWidgetFromLayer(FGameplayTag LayerTag, UUserWidget* Widget)
{
	if (!Widget)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNPrimaryGameLayout: Attempted to remove null widget from native layer %s"), *LayerTag.ToString());
		return;
	}

	TObjectPtr<UNamedSlot>* SlotPtr = NativeLayers.Find(LayerTag);
	if (!SlotPtr || !(*SlotPtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("VNPrimaryGameLayout: Native layer %s not found"), *LayerTag.ToString());
		return;
	}

	UNamedSlot* NamedSlotWidget = *SlotPtr;
	NamedSlotWidget->RemoveChild(Widget);

	UE_LOG(LogTemp, Log, TEXT("VNPrimaryGameLayout: Removed widget %s from native layer %s"),
		*Widget->GetName(), *LayerTag.ToString());
}

void UVNPrimaryGameLayout::RegisterNativeLayer(FGameplayTag LayerTag, UNamedSlot* NamedSlot)
{
	if (!LayerTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("VNPrimaryGameLayout: Attempted to register native layer with invalid tag"));
		return;
	}

	if (!NamedSlot)
	{
		UE_LOG(LogTemp, Error, TEXT("VNPrimaryGameLayout: Attempted to register null named slot for tag %s"), *LayerTag.ToString());
		return;
	}

	NativeLayers.Add(LayerTag, NamedSlot);

	UE_LOG(LogTemp, Log, TEXT("VNPrimaryGameLayout: Registered native layer for tag %s"), *LayerTag.ToString());
}
