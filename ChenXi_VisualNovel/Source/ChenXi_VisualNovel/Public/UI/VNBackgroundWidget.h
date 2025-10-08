// VNBackgroundWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "VNBackgroundWidget.generated.h"

/**
 * Visual Novel 背景显示 Widget
 *
 * 职责：
 * - 显示单张背景图片
 * - 支持切换背景图片（可在蓝图中添加淡入淡出效果）
 * - 放置在 Background Named Slot 层
 */
UCLASS()
class CHENXI_VISUALNOVEL_API UVNBackgroundWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * 切换背景图片
	 * @param NewBackground 新的背景纹理（软引用）
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Background")
	void SetBackground(TSoftObjectPtr<UTexture2D> NewBackground);

protected:
	/**
	 * 蓝图实现事件：当背景需要切换时调用
	 * 蓝图可以在这里实现淡入淡出、过渡动画等效果
	 *
	 * @param NewBackground 新的背景纹理
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Background Changed"), Category = "VN|Background")
	void ReceiveBackgroundChanged(const TSoftObjectPtr<UTexture2D>& NewBackground);

private:
	// 当前显示的背景（用于防止重复切换）
	UPROPERTY()
	TSoftObjectPtr<UTexture2D> CurrentBackground;
};
