

#pragma once

#include "CoreMinimal.h"
#include "UI/VNActivatableWidget.h"  // 改为引入 VNActivatableWidget
// 引入 FDialogLine 结构体所在的头文件
#include "VNGameMode.h"
#include "Engine/Texture2D.h"
#include "VNDialogueWidget.generated.h"

//打字机完成时的广播委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTypewriterFinished);

/**
 * Visual Novel 对话框 UI 的 C++ 基类
 * 定义了接收对话数据和处理输入反馈的接口
 *
 * 继承自 VNActivatableWidget，获得以下能力：
 * - 自动输入管理（激活时阻止游戏输入）
 * - 堆栈管理（可以推入/弹出 Stack）
 * - 激活/停用状态管理
 */
UCLASS()
class CHENXI_VISUALNOVEL_API UVNDialogueWidget : public UVNActivatableWidget  // 改为继承 VNActivatableWidget
{
	// 请确保这里的 VNDEMO_API 替换为你项目实际的 API 宏！
	GENERATED_BODY()

public:
	//用于追踪打字机状态，允许在蓝图中读写
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="VN|Dialogue")
	bool bIsTypewriterActive;
    
	/**
	 * @brief 供 C++ 或蓝图调用的函数，用于向 UI 传递下一行对话数据
	 * @param DialogLine 当前需要显示的对话数据结构体
	 */
	void DisplayDialogueLine(const FDialogLine& DialogLine);

	//供蓝图实现的跳过打字机
	UFUNCTION(BlueprintCallable,Category="VN|Dialogue")
	void SkipTypewriter();

	/** 当打字机动画播放完毕时广播 */
	UPROPERTY(BlueprintAssignable, Category = "VN|Dialogue")
	FOnTypewriterFinished OnTypewriterFinished;

protected:
    
	// /**
	//  * @brief 蓝图可实现事件：接收新的对话数据并更新屏幕显示
	//  * 这是 C++ 逻辑与蓝图视觉设计的连接点。
	//  * * @param CharacterName 当前说话角色的名字
	//  * @param DialogueText 当前对话文本的内容
	//  */
	// UFUNCTION(BlueprintImplementableEvent, Category = "VN|Dialog")
	// void OnNewDialogueLine(const FString& CharacterName, const FText& DialogueText);

	// 蓝图实现的事件，用于在UI上更新文本
	UFUNCTION(BlueprintImplementableEvent, Category = "VN|Dialogue")
	void OnUpdateDialogueText(const FText& CurrentText);

	// 蓝图实现的事件，用于接收角色名
	UFUNCTION(BlueprintImplementableEvent, Category = "VN|Dialogue")
	void OnNewCharacterName(const FString& CharacterName);

private:
	// --- 从蓝图移至C++的变量 ---

	// 存储完整的对话文本
	FText FullDialogueText;

	// 当前显示到第几个字符
	int32 CurrentCharIndex;

	// 打字机速度，可以在蓝图子类中被覆盖设置
	UPROPERTY(EditDefaultsOnly, Category="VN|Dialogue")
	float TypewriterSpeed = 0.05f;

	// 打字机计时器的句柄
	FTimerHandle TypewriterTimerHandle;

	// --- C++内部函数 ---

	// 开始打字机效果
	void StartTypewriter();

	// 计时器调用的函数，用于追加单个字符
	void AppendCharacter();
};