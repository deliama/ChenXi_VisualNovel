// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "VNPrimaryGameLayout.generated.h"

class UCommonActivatableWidgetStack;
class UCommonActivatableWidget;
class UNamedSlot;

/**
 * 主游戏布局 - 管理所有 UI 层级的根 Widget
 *
 * 视觉小说混合架构：
 * - 3 层使用 CommonUI Stack（对话框、菜单、系统）- 需要输入管理
 * - 7 层使用原生 Widget（背景、角色等）- 仅显示
 *
 * Widget 层级通过 GameplayTag 标识（例如：UI.Layer.Dialogue）
 */
UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class CHENXI_VISUALNOVEL_API UVNPrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UVNPrimaryGameLayout(const FObjectInitializer& ObjectInitializer);

	//~ Begin UUserWidget interface
	virtual void NativeConstruct() override;
	//~ End UUserWidget interface

	// ========== Stack 层（需要输入管理的 Widget） ==========

	/**
	 * 将可激活 Widget 推入指定的 Stack 层
	 * 用于对话框、菜单、系统层等需要输入焦点的界面
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Layer | Stack")
	bool PushWidgetToLayerStack(FGameplayTag LayerTag, UCommonActivatableWidget* ActivatableWidget);

	/**
	 * 查找或创建指定层的 Widget 堆栈
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Layer | Stack")
	UCommonActivatableWidgetStack* FindOrCreateLayerStack(FGameplayTag LayerTag);

	/**
	 * 注册 Stack Widget 到指定的层级标签
	 * 从蓝图调用，将命名的 Widget 堆栈绑定到标签
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Layer | Stack")
	void RegisterLayerStack(FGameplayTag LayerTag, UCommonActivatableWidgetStack* LayerStack);

	// ========== 原生层（仅显示的 Widget） ==========

	/**
	 * 添加原生 Widget 到指定层（无输入管理）
	 * 用于背景、角色、特效层等仅显示内容的界面
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Layer | Native")
	void AddNativeWidgetToLayer(FGameplayTag LayerTag, UUserWidget* Widget);

	/**
	 * 从指定层移除原生 Widget
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Layer | Native")
	void RemoveNativeWidgetFromLayer(FGameplayTag LayerTag, UUserWidget* Widget);

	/**
	 * 注册命名槽位（Overlay）到指定的层级标签
	 * 从蓝图调用，将命名的覆盖槽位绑定到标签
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Layer | Native")
	void RegisterNativeLayer(FGameplayTag LayerTag, UNamedSlot* NamedSlot);

protected:
	// 层级标签到 CommonUI Widget 堆栈的映射（用于交互层）
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetStack>> LayerStacks;

	// 层级标签到原生 Widget 容器的映射（用于显示层）
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UNamedSlot>> NativeLayers;
};
