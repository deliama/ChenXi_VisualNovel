// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#include "UI/VNActivatableWidget.h"
#include "Editor/WidgetCompilerLog.h"
#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VNActivatableWidget)

UVNActivatableWidget::UVNActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UVNActivatableWidget::GetDesiredInputConfig() const
{
	// Convert our custom input mode to CommonUI input mode
	switch (InputConfig)
	{
		case EVNWidgetInputMode::Default:
			return TOptional<FUIInputConfig>();

		case EVNWidgetInputMode::GameAndMenu:
			// 第三个参数设置为 false，防止捕获视口时隐藏光标
			return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode, false);

		case EVNWidgetInputMode::Game:
			return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode, false);

		case EVNWidgetInputMode::Menu:
			return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, false);

		default:
			return TOptional<FUIInputConfig>();
	}
}

void UVNActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	// 自动播放激活动画
	PlayActivationAnimation();

	// 调用蓝图实现的激活事件
	ReceiveWidgetActivated();

	UE_LOG(LogTemp, Log, TEXT("VNActivatableWidget: %s activated"), *GetName());
}

void UVNActivatableWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	// 自动播放停用动画
	PlayDeactivationAnimation();

	// 调用蓝图实现的停用事件
	ReceiveWidgetDeactivated();

	UE_LOG(LogTemp, Log, TEXT("VNActivatableWidget: %s deactivated"), *GetName());
}

void UVNActivatableWidget::PlayActivationAnimation()
{
	// 查找名为 "FadeIn" 的动画
	static const FName FadeInAnimName = FName(TEXT("FadeIn"));

	// 遍历所有动画，查找匹配的动画
	if (UWidgetBlueprintGeneratedClass* WidgetClass = Cast<UWidgetBlueprintGeneratedClass>(GetClass()))
	{
		for (UWidgetAnimation* Animation : WidgetClass->Animations)
		{
			if (Animation && Animation->GetFName() == FadeInAnimName)
			{
				// 找到了 FadeIn 动画，播放它
				PlayAnimation(Animation);
				UE_LOG(LogTemp, Log, TEXT("VNActivatableWidget: Playing FadeIn animation for %s"), *GetName());
				return;
			}
		}
	}

	// 如果没有找到动画，不做任何操作（静默失败）
}

void UVNActivatableWidget::PlayDeactivationAnimation()
{
	// 查找名为 "FadeOut" 的动画
	static const FName FadeOutAnimName = FName(TEXT("FadeOut"));

	// 遍历所有动画，查找匹配的动画
	if (UWidgetBlueprintGeneratedClass* WidgetClass = Cast<UWidgetBlueprintGeneratedClass>(GetClass()))
	{
		for (UWidgetAnimation* Animation : WidgetClass->Animations)
		{
			if (Animation && Animation->GetFName() == FadeOutAnimName)
			{
				// 找到了 FadeOut 动画，播放它
				PlayAnimation(Animation);
				UE_LOG(LogTemp, Log, TEXT("VNActivatableWidget: Playing FadeOut animation for %s"), *GetName());
				return;
			}
		}
	}

	// 如果没有找到动画，不做任何操作（静默失败）
}

#if WITH_EDITOR
void UVNActivatableWidget::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledWidgetTree(BlueprintWidgetTree, CompileLog);

	// Check if widget has buttons but uses Game input mode (which wouldn't receive clicks)
	if (InputConfig == EVNWidgetInputMode::Game)
	{
		CompileLog.Warning(FText::FromString(TEXT("This widget is set to 'Game' input mode but may contain interactive elements that won't receive input.")));
	}
}
#endif
