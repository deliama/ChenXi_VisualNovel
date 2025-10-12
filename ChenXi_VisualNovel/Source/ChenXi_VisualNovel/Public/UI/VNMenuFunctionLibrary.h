// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VNMenuFunctionLibrary.generated.h"

class UCommonActivatableWidget;

/**
 * 菜单相关的工具函数库
 *
 * 提供通用的菜单操作,任何蓝图都可以调用
 * 用于主菜单、暂停菜单、设置菜单等的通用逻辑
 */
UCLASS()
class CHENXI_VISUALNOVEL_API UVNMenuFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 打开菜单
	 * 将指定的菜单Widget推入Menu Stack层
	 *
	 * @param WorldContextObject 世界上下文
	 * @param MenuClass 菜单Widget类(必须继承自 UCommonActivatableWidget)
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Menu", meta = (WorldContext = "WorldContextObject"))
	static void OpenMenu(UObject* WorldContextObject, TSubclassOf<UCommonActivatableWidget> MenuClass);

	/**
	 * 关闭当前菜单
	 * 从Menu Stack中弹出指定菜单
	 *
	 * @param MenuWidget 要关闭的菜单Widget
	 * @param bWithAnimation 是否播放关闭动画(延迟关闭)
	 * @param AnimationDuration 动画时长(秒)
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Menu")
	static void CloseMenu(UCommonActivatableWidget* MenuWidget, bool bWithAnimation = true, float AnimationDuration = 0.3f);

	/**
	 * 开始新游戏
	 * 便捷函数:关闭所有菜单并通知GameMode开始新游戏
	 *
	 * @param WorldContextObject 世界上下文
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Menu", meta = (WorldContext = "WorldContextObject"))
	static void StartNewGame(UObject* WorldContextObject);

	/**
	 * 返回主菜单
	 * 便捷函数:清理游戏状态并显示主菜单
	 *
	 * @param WorldContextObject 世界上下文
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Menu", meta = (WorldContext = "WorldContextObject"))
	static void ReturnToMainMenu(UObject* WorldContextObject);

	/**
	 * 退出游戏
	 * 便捷函数:退出应用程序
	 *
	 * @param WorldContextObject 世界上下文
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Menu", meta = (WorldContext = "WorldContextObject"))
	static void QuitGame(UObject* WorldContextObject);
};
