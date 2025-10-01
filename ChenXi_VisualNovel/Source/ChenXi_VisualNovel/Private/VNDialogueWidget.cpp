// VNDialogWidget.cpp

#include "VNDialogueWidget.h"

void UVNDialogueWidget::DisplayDialogueLine(const FDialogLine& DialogLine)
{
	// C++ 只是简单地将 FDialogLine 数据拆解，然后调用蓝图事件
	// 注意：这里传递的是 FString 和 FText 的值，是安全的。

	// 调用蓝图事件，让蓝图去实现具体的文本框更新、角色立绘切换等视觉逻辑。
	OnNewDialogueLine(DialogLine.CharacterName, DialogLine.DialogueText);
    
	UE_LOG(LogTemp, Log, TEXT("UI Widget received: [%s]"), *DialogLine.CharacterName);
}