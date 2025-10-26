// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/VNPrimaryGameLayout.h" 
#include "UI/VNBackgroundWidget.h" 
#include "VNDialogueWidget.h"
#include "VNUIManagerSubsystem.generated.h"

class UVNPrimaryGameLayout;
class APlayerController;
class UVNBackgroundWidget;
class UVNDialogueWidget;
class UTexture2D;
class UCommonActivatableWidget;
class UVNHistoryWidget;

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

	// --- 新增 Getters ---
	/** 获取根布局 Widget 实例 */
	UFUNCTION(BlueprintPure, Category = "VN|UI Manager")
	UVNPrimaryGameLayout* GetPrimaryGameLayout() const { return RootLayout; }

	/** 获取背景 Widget 实例 */
	UFUNCTION(BlueprintPure, Category = "VN|UI Manager")
	UVNBackgroundWidget* GetBackgroundWidget() const { return BackgroundWidget; }
	// --- 新增结束 ---

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

	/**
	 * 【新增】显示对话历史记录
	 * 将历史记录 Widget 推入 Menu 层的 Stack
	 */
	UFUNCTION(BlueprintCallable, Category = "VN UI")
	void ShowHistory(APlayerController* PlayerController);

protected:
	/** 根布局 Widget 类（在蓝图派生类的 Class Defaults 中配置） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Classes")
	TSoftClassPtr<UVNPrimaryGameLayout> RootLayoutClass;

	/** 背景 Widget 类（在蓝图派生类的 Class Defaults 中配置） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Classes")
	TSoftClassPtr<UVNBackgroundWidget> BackgroundWidgetClass;

	/** 对话框 Widget 类（在蓝图派生类的 Class Defaults 中配置） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Classes")
	TSoftClassPtr<UVNDialogueWidget> DialogueWidgetClass;

	/** 主菜单 Widget 类（在蓝图派生类的 Class Defaults 中配置） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Classes")
	TSoftClassPtr<UCommonActivatableWidget> MainMenuWidgetClass;

	/** 【新增】历史记录 Widget 类（在 C++ 的 Initialize 中硬编码路径） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Classes")
	TSoftClassPtr<UVNHistoryWidget> HistoryWidgetClass;

private:
	/** 游戏的根布局 Widget */
	UPROPERTY()
	TObjectPtr<UVNPrimaryGameLayout> RootLayout;

	/** 背景 Widget 实例 */
	UPROPERTY()
	TObjectPtr<UVNBackgroundWidget> BackgroundWidget;

	/** 对话框 Widget 实例 */
	UPROPERTY()
	TObjectPtr<UVNDialogueWidget> DialogueWidget;

	/** 创建背景 Widget（如果还不存在） */
	void CreateBackgroundWidgetIfNeeded();
};
