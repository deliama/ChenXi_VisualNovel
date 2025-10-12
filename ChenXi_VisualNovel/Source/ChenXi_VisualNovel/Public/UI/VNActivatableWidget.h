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
 * - 自动播放约定名称的动画（FadeIn/FadeOut）
 */
UCLASS(Abstract, Blueprintable)
class CHENXI_VISUALNOVEL_API UVNActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UVNActivatableWidget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	//~ End UCommonActivatableWidget interface

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif

protected:
	/**
	 * 蓝图实现事件: Widget 激活时调用
	 * 在自动播放 FadeIn 动画之后调用
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "VN|Widget", meta = (DisplayName = "On Widget Activated"))
	void ReceiveWidgetActivated();

	/**
	 * 蓝图实现事件: Widget 停用时调用
	 * 在自动播放 FadeOut 动画之前调用
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "VN|Widget", meta = (DisplayName = "On Widget Deactivated"))
	void ReceiveWidgetDeactivated();

	/**
	 * 自动播放激活动画（如果存在名为 "FadeIn" 的动画）
	 */
	void PlayActivationAnimation();

	/**
	 * 自动播放停用动画（如果存在名为 "FadeOut" 的动画）
	 */
	void PlayDeactivationAnimation();

protected:
	/**
	 * 此 UI 激活时使用的期望输入模式
	 * 例如，是否希望按键仍然传递到游戏/玩家控制器？
	 */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EVNWidgetInputMode InputConfig = EVNWidgetInputMode::Default;

	/**
	 * 游戏获得输入时的期望鼠标行为
	 * VN游戏使用 CaptureDuringMouseDown: 鼠标可见，每次点击都能发送到游戏，避免双击问题
	 * 参考: https://x157.github.io/UE5/LyraStarterGame/Tutorials/How-to-Take-Control-of-the-Mouse.html
	 */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CaptureDuringMouseDown;
};
