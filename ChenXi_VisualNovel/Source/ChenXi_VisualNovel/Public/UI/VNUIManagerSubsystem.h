// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VNUIManagerSubsystem.generated.h"

class UVNPrimaryGameLayout;
class APlayerController;
class UVNBackgroundWidget;
class UVNDialogueWidget;
class UTexture2D;
class UCommonActivatableWidget;

/**
 * UI 管理器子系统 - 管理 UI 生命周期
 *
 * 为视觉小说简化的 UI 管理
 * 职责：
 * - 创建和管理主游戏布局
 * - 提供访问根布局 Widget 的接口
 * - 管理背景 Widget 的创建和切换
 *
 * 配置方式：
 * 1. 创建蓝图派生类 BP_VNUIManagerSubsystem（可选）
 * 2. 在蓝图的 Class Defaults 中设置 Root Layout Class 和 Background Widget Class
 * 3. 系统会自动查找并使用蓝图派生类的配置
 */
UCLASS(Blueprintable)
class CHENXI_VISUALNOVEL_API UVNUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UVNUIManagerSubsystem();

	//~ Begin USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End USubsystem interface

	/** 获取根布局 Widget */
	UFUNCTION(BlueprintCallable, Category = "VN UI")
	UVNPrimaryGameLayout* GetRootLayout() const { return RootLayout; }

	/** 如果不存在则创建根布局（当玩家控制器就绪时调用） */
	UFUNCTION(BlueprintCallable, Category = "VN UI")
	void CreateRootLayoutIfNeeded(APlayerController* PlayerController);

	/**
	 * 切换背景图片
	 * 如果背景 Widget 不存在，会自动创建并添加到 Background 层
	 * @param NewBackground 新的背景纹理（软引用）
	 */
	UFUNCTION(BlueprintCallable, Category = "VN UI")
	void SetBackground(TSoftObjectPtr<UTexture2D> NewBackground);

	/**
	 * 显示对话框
	 * 如果对话框 Widget 不存在，会自动创建并推入 Dialogue 层的 Stack
	 */
	UFUNCTION(BlueprintCallable, Category = "VN UI")
	void ShowDialogue(APlayerController* PlayerController);

	/**
	 * 获取对话框 Widget 实例
	 */
	UFUNCTION(BlueprintCallable, Category = "VN UI")
	UVNDialogueWidget* GetDialogueWidget() const { return DialogueWidget; }

	/**
	 * 显示主菜单
	 * 将主菜单推入 Menu 层的 Stack
	 */
	UFUNCTION(BlueprintCallable, Category = "VN UI")
	void ShowMainMenu(APlayerController* PlayerController);

	/*
	 *隐藏（停用）对话框
	 *从堆栈中弹出并清除本地引用
	 */
	UFUNCTION(BlueprintCallable, Category = "VN UI")
	void HideDialogue();

protected:
	/** 根布局 Widget 类（在蓝图派生类的 Class Defaults 中配置） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Classes")
	TSubclassOf<UVNPrimaryGameLayout> RootLayoutClass;

	/** 背景 Widget 类（在蓝图派生类的 Class Defaults 中配置） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Classes")
	TSubclassOf<UVNBackgroundWidget> BackgroundWidgetClass;

	/** 对话框 Widget 类（在蓝图派生类的 Class Defaults 中配置） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Classes")
	TSubclassOf<UVNDialogueWidget> DialogueWidgetClass;

	/** 主菜单 Widget 类（在蓝图派生类的 Class Defaults 中配置） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Classes")
	TSubclassOf<UCommonActivatableWidget> MainMenuWidgetClass;

private:
	/** 游戏的根布局 Widget */
	UPROPERTY(Transient)
	TObjectPtr<UVNPrimaryGameLayout> RootLayout;

	/** 背景 Widget 实例 */
	UPROPERTY(Transient)
	TObjectPtr<UVNBackgroundWidget> BackgroundWidget;

	/** 对话框 Widget 实例 */
	UPROPERTY(Transient)
	TObjectPtr<UVNDialogueWidget> DialogueWidget;

	/** 创建背景 Widget（如果还不存在） */
	void CreateBackgroundWidgetIfNeeded();
};
