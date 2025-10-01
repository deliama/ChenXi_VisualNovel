// VNGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "VNGameMode.generated.h"

// 故事文本结构体 (用于存储在DataTable中)
USTRUCT(BlueprintType)
struct FDialogLine : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 对话人物的名字
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN|Dialog")
	FString CharacterName;

	// 对话内容
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN|Dialog")
	FText DialogueText;

	// 对话时播放的音频/语音（可选）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN|Audio")
	TSoftObjectPtr<class USoundWave> VoiceLine;

	// 角色立绘/表情ID (用于通知UI显示)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN|Visual")
	FName CharacterSpriteID; 
};

/**
 * Visual Novel Game Mode: 负责游戏流程、数据加载和状态管理
 */
UCLASS()
class CHENXI_VISUALNOVEL_API AVNGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AVNGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// 存储对话数据表的引用 (在蓝图中设置)
	UPROPERTY(EditDefaultsOnly, Category = "VN|Data")
	UDataTable* DialogDataTable;

	// 当前对话的索引
	int32 CurrentDialogIndex;

	// 存储从DataTable加载的所有对话行
	TArray<FDialogLine> StoryLines;

public:
	// 对外暴露的函数：开始游戏和加载对话
	UFUNCTION(BlueprintCallable, Category = "VN|Flow")
	void StartDialog();

	// 对外暴露的函数：获取下一行对话
	// 返回一个常量指针，确保外部无法修改对话数据
	UFUNCTION(BlueprintCallable, Category = "VN|Dialog")
	bool GetNextDialogLine(FDialogLine& OutDialogLine);

};