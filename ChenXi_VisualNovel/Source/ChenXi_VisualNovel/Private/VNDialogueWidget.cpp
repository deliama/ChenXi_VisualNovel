// VNDialogWidget.cpp

#include "VNDialogueWidget.h"
#include "TimerManager.h"

void UVNDialogueWidget::DisplayDialogueLine(const FDialogLine& DialogLine)
{
	// C++ 只是简单地将 FDialogLine 数据拆解，然后调用蓝图事件
	// 注意：这里传递的是 FString 和 FText 的值，是安全的。

	// // 调用蓝图事件，让蓝图去实现具体的文本框更新、角色立绘切换等视觉逻辑。
	// OnNewDialogueLine(DialogLine.CharacterName, DialogLine.DialogueText);
	// 存储新数据并通知蓝图更新角色名
	FullDialogueText = DialogLine.DialogueText;
	OnNewCharacterName(DialogLine.CharacterName);

	// 开始打字机效果
	StartTypewriter();
    
	UE_LOG(LogTemp, Log, TEXT("UI Widget received: [%s]"), *DialogLine.CharacterName);
}

void UVNDialogueWidget::StartTypewriter()
{
	// 激活状态并重置
	bIsTypewriterActive = true;
	CurrentCharIndex = 0;

	// 清空UI上的旧文本
	OnUpdateDialogueText(FText::GetEmpty());

	// 设置一个循环计时器，重复调用AppendCharacter函数
	GetWorld()->GetTimerManager().SetTimer(
		TypewriterTimerHandle, 
		this, 
		&UVNDialogueWidget::AppendCharacter, 
		TypewriterSpeed, 
		true // true表示循环
	);
}

void UVNDialogueWidget::AppendCharacter()
{
	CurrentCharIndex++;
	
	// 从完整文本中获取子字符串并更新UI
	FString CurrentString = FullDialogueText.ToString().Left(CurrentCharIndex);
	OnUpdateDialogueText(FText::FromString(CurrentString));

	// 检查是否已经显示完毕
	if (CurrentCharIndex >= FullDialogueText.ToString().Len())
	{
		// 清除计时器并更新状态
		GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);
		bIsTypewriterActive = false;
	}
}

void UVNDialogueWidget::SkipTypewriter()
{
	// 只有在打字机激活时才执行跳过逻辑
	if (bIsTypewriterActive)
	{
		// 清除计时器
		GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);

		// 直接显示完整文本
		OnUpdateDialogueText(FullDialogueText);

		// 更新状态
		bIsTypewriterActive = false;
	}
}