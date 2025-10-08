// VNBackgroundWidget.cpp

#include "UI/VNBackgroundWidget.h"

void UVNBackgroundWidget::SetBackground(TSoftObjectPtr<UTexture2D> NewBackground)
{
	// 如果背景没有变化，不做任何操作
	if (NewBackground == CurrentBackground)
	{
		UE_LOG(LogTemp, Log, TEXT("VNBackgroundWidget: Background unchanged, skipping"));
		return;
	}

	// 如果新背景为空，也记录日志但允许切换（可能是清空背景）
	if (NewBackground.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("VNBackgroundWidget: Setting background to NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("VNBackgroundWidget: Changing background to %s"), *NewBackground.ToString());
	}

	// 更新当前背景引用
	CurrentBackground = NewBackground;

	// 调用蓝图事件，让蓝图处理实际的图片切换和动画
	ReceiveBackgroundChanged(NewBackground);
}
