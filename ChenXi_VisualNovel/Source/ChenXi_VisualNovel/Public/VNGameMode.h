// VNGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "UObject/SoftObjectPtr.h"

#include "UVNSaveGame.h"
#include "Kismet/GameplayStatics.h"

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


	/**
	 * 【新增】获取对话历史记录
	 * @return 返回历史记录数组的常量引用
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Dialogue")
	const TArray<FDialogLine>& GetDialogueHistory() const { return DialogueHistory; }

	/**
	 * 【新增】供 C++ 内部调用的函数，用于添加一行到历史记录
	 * @param Line 要添加的对话行
	 */
	void AddToHistory(const FDialogLine& Line)
	{
		DialogueHistory.Add(Line);
	}

	/**
	 * 【新增】清空对话历史
	 * 在开始新游戏或返回主菜单时调用
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Dialogue")
	void ClearHistory()
	{
		DialogueHistory.Empty();
	}
	
	// --- [新增] 存档/读档功能 ---

	/**
	 * 保存游戏到指定的槽位
	 * @param SlotName - 存档槽的名称 (例如 "Slot1", "QuickSave")
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Game")
	void SaveGame(const FString& SlotName);

	/**
	 * 从指定槽位加载游戏
	 * @param SlotName - 存档槽的名称
	 * @param OutLoadedLine - [输出] 加载成功后，返回当前行的对话数据以供UI显示
	 * @return bool - 是否加载成功 (UI根据这个来判断是否要显示 OutLoadedLine)
	 */
	UFUNCTION(BlueprintCallable, Category = "VN|Game")
	bool LoadGame(const FString& SlotName, FDialogLine& OutLoadedLine);

protected:
	/** 游戏是否已开始 (用于判断是否显示主菜单) */
	UPROPERTY(BlueprintReadOnly, Category = "VN|Game")
	bool bGameStarted = false;

	/** 加载对话数据 */
	void LoadDialogueData();

	/** 存储已显示过的对话历史 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VN|Dialogue")
	TArray<FDialogLine> DialogueHistory;
};