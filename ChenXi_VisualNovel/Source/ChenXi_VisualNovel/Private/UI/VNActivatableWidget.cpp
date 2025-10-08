// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#include "UI/VNActivatableWidget.h"
#include "Editor/WidgetCompilerLog.h"

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
			return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);

		case EVNWidgetInputMode::Game:
			return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);

		case EVNWidgetInputMode::Menu:
			return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);

		default:
			return TOptional<FUIInputConfig>();
	}
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
