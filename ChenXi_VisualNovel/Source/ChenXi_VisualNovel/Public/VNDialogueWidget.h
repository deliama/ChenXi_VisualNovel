

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
// 引入 FDialogLine 结构体所在的头文件
#include "VNGameMode.h" 
#include "VNDialogueWidget.generated.h"

/**
 * Visual Novel 对话框 UI 的 C++ 基类
 * 定义了接收对话数据和处理输入反馈的接口
 */
UCLASS()
class CHENXI_VISUALNOVEL_API UVNDialogueWidget : public UUserWidget
{
	// 请确保这里的 VNDEMO_API 替换为你项目实际的 API 宏！
	GENERATED_BODY()

public:
    
	/**
	 * @brief 供 C++ 或蓝图调用的函数，用于向 UI 传递下一行对话数据
	 * @param DialogLine 当前需要显示的对话数据结构体
	 */
	void DisplayDialogueLine(const FDialogLine& DialogLine);

protected:
    
	/**
	 * @brief 蓝图可实现事件：接收新的对话数据并更新屏幕显示
	 * 这是 C++ 逻辑与蓝图视觉设计的连接点。
	 * * @param CharacterName 当前说话角色的名字
	 * @param DialogueText 当前对话文本的内容
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "VN|Dialog")
	void OnNewDialogueLine(const FString& CharacterName, const FText& DialogueText);
    
};