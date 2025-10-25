
#pragma once

#include "CoreMinimal.h"
#include "UI/VNActivatableWidget.h" // 继承自我们的可激活基类
#include "VNGameMode.h" // 需要 FDialogLine 结构体
#include "VNHistoryWidget.generated.h"

/**
 * 对话历史记录 UI
 * 继承自 VNActivatableWidget，可以被推入 UI 堆栈
 */
UCLASS(Abstract, Blueprintable)
class CHENXI_VISUALNOVEL_API UVNHistoryWidget : public UVNActivatableWidget
{
	GENERATED_BODY()

public:
	/**
	 * 蓝图实现事件：当需要显示历史记录时调用
	 * C++ 将在 Widget 被创建时调用此事件，并将数据传递给蓝图
	 * @param HistoryLines 包含所有历史对话的数组
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "VN|History", meta = (DisplayName = "On Display History"))
	void OnDisplayHistory(const TArray<FDialogLine>& HistoryLines);
};