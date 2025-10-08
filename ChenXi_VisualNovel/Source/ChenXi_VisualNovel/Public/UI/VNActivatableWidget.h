// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "VNActivatableWidget.generated.h"


/**
 * Widget 输入模式 - 定义 Widget 激活时如何处理输入
 */
UENUM(BlueprintType)
enum class EVNWidgetInputMode : uint8
{
	/** 使用默认输入配置 */
	Default,

	/** 输入同时传递给游戏和 UI */
	GameAndMenu,

	/** 输入只传递给游戏（UI 透传） */
	Game,

	/** 输入只传递给 UI（阻止游戏输入） */
	Menu
};

/**
 * VN 可激活 Widget - 所有可激活 Widget 的基类
 *
 * 参考 Lyra 的 LyraActivatableWidget
 * 提供功能：
 * - 激活时自动管理输入配置
 * - 鼠标捕获控制
 * - 可在蓝图中配置的输入模式
 */
UCLASS(Abstract, Blueprintable)
class CHENXI_VISUALNOVEL_API UVNActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UVNActivatableWidget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~ End UCommonActivatableWidget interface

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif

protected:
	/**
	 * 此 UI 激活时使用的期望输入模式
	 * 例如，是否希望按键仍然传递到游戏/玩家控制器？
	 */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EVNWidgetInputMode InputConfig = EVNWidgetInputMode::Default;

	/**
	 * 游戏获得输入时的期望鼠标行为
	 */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
