// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#include "UI/VNPrimaryGameLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"
#include "Components/NamedSlot.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Engine/Texture2D.h"

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

// --- 新增：粘贴以下所有代码到 .cpp 文件末尾 ---

void UVNPrimaryGameLayout::HandleCharacterCommand(const FDialogLine& Row)
{
	// 如果动画名为空，或插槽为Hidden，则不执行任何操作
	if (Row.CharacterAnimation.IsNone() || Row.CharacterSlot == ECharacterSlot::Hidden)
	{
		return;
	}

	// 1. 根据插槽找到目标Image控件
	UImage* TargetImage = GetImageForSlot(Row.CharacterSlot);
	if (!TargetImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleCharacterCommand: Invalid CharacterSlot."));
		return;
	}

	// 2. 根据命令找到目标动画
	UWidgetAnimation* TargetAnimation = GetAnimationForCommand(Row.CharacterAnimation, Row.CharacterSlot);
	if (!TargetAnimation)
	{
		// 警告：策划在DataTable里填了一个我们C++不认识的动画名
		UE_LOG(LogTemp, Warning, TEXT("HandleCharacterCommand: Invalid CharacterAnimation name: %s"), *Row.CharacterAnimation.ToString());
		return;
	}

	// 3. 如果是 "FadeIn" 动画，我们需要先设置立绘图片
	if (Row.CharacterAnimation.ToString().StartsWith(TEXT("FadeIn")))
	{
		// 检查立绘资源是否已设置
		if (Row.CharacterSprite.IsNull())
		{
			UE_LOG(LogTemp, Warning, TEXT("HandleCharacterCommand: FadeIn requested but CharacterSprite is null."));
			return;
		}

		// 同步加载 (在正式项目中，你可能希望换成异步加载来防止卡顿)
		UTexture2D* SpriteToSet = Row.CharacterSprite.LoadSynchronous();
		if (SpriteToSet)
		{
			TargetImage->SetBrushFromTexture(SpriteToSet);
			// 再次确保在播放FadeIn前，不透明度为0
			TargetImage->SetRenderOpacity(0.0f);
		}
	}

	// 4. 播放动画
	if (IsAnimationPlaying(TargetAnimation))
	{
		StopAnimation(TargetAnimation);
	}
	
	PlayAnimation(TargetAnimation);
}

// --- 辅助函数的实现 ---

UImage* UVNPrimaryGameLayout::GetImageForSlot(ECharacterSlot InSlot) const
{
	switch (InSlot)
	{
		case ECharacterSlot::Left:   return CharImage_Left;
		case ECharacterSlot::Center: return CharImage_Center;
		case ECharacterSlot::Right:  return CharImage_Right;
		default:                     return nullptr;
	}
}

UWidgetAnimation* UVNPrimaryGameLayout::GetAnimationForCommand(FName AnimName, ECharacterSlot InSlot) const
{
    // 你的动画名 (FName) 只需要填 "FadeIn" 或 "FadeOut"
	if (AnimName == FName(TEXT("FadeIn")))
	{
		switch (InSlot)
		{
			case ECharacterSlot::Left:   return FadeIn_Left;
			case ECharacterSlot::Center: return FadeIn_Center;
			case ECharacterSlot::Right:  return FadeIn_Right;
		}
	}
	else if (AnimName == FName(TEXT("FadeOut")))
	{
		switch (InSlot)
		{
			case ECharacterSlot::Left:   return FadeOut_Left;
			case ECharacterSlot::Center: return FadeOut_Center;
			case ECharacterSlot::Right:  return FadeOut_Right;
		}
	}
	
	return nullptr;
}
// --- 新增结束 ---
