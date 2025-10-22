// VNGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "UObject/SoftObjectPtr.h"

class USoundBase;
class USoundCue;
class UAudioComponent;

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

	//新增：指定此对话行的背景图片
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN|Visual")
	TSoftObjectPtr<UTexture2D> BackgroundImage;

	//bgm轨道：如果设置了，那么就从这一行开始播放此bgm
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN|Audio")
	TSoftObjectPtr<USoundCue> BGM;

	//特殊音效：如果设置了，则在此行触发一次特殊音效
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VN|Audio")
	TSoftObjectPtr<USoundBase> SFX;
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

	// 处理对话附带的声音事件
	void PlaySoundForLine(const FDialogLine& DialogLine);

	// 追踪当前正在播放的BGM
	UPROPERTY()
	UAudioComponent* CurrentBgmComponent;

	// 追踪当前播放的 BGM 资源（成员变量，每次 PIE 重启时重置）
	UPROPERTY()
	TSoftObjectPtr<USoundCue> CurrentBGMTrack;

	//主菜单BGM
	UPROPERTY(EditDefaultsOnly,Category="VN|Audio")
	TSoftObjectPtr<USoundCue> MainMenuBGM;

public:
	// 对外暴露的函数：开始游戏和加载对话
	UFUNCTION(BlueprintCallable, Category = "VN|Flow")
	void StartDialog();

	// 对外暴露的函数：获取下一行对话
	// 返回一个常量指针，确保外部无法修改对话数据
	UFUNCTION(BlueprintCallable, Category = "VN|Dialog")
	bool GetNextDialogLine(FDialogLine& OutDialogLine);

	/**
	 * 开始新游戏
	 * 从主菜单调用,初始化对话系统并显示对话框
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Game")
	void StartNewGame();

	/**
	 * 返回主菜单
	 * 清理游戏状态并显示主菜单
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Game")
	void ReturnToMainMenu();

protected:
	/** 游戏是否已开始 (用于判断是否显示主菜单) */
	UPROPERTY(BlueprintReadOnly, Category = "VN|Game")
	bool bGameStarted = false;

	/** 加载对话数据 */
	void LoadDialogueData();
};