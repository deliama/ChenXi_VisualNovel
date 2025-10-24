#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Engine/Texture2D.h"
#include "UObject/SoftObjectPtr.h"
#include "UVNSaveGame.generated.h"

class USoundCue;

/**
 * 视觉小说存档数据结构
 */
UCLASS()
class CHENXI_VISUALNOVEL_API UVNSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UVNSaveGame();

	// 关键的游戏进度：我们保存的是玩家 *当前正在看* 的那一行对话的索引
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	int32 SavedDialogIndex;

	// 当前持续播放的BGM
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	TSoftObjectPtr<USoundCue> SavedBGMTrack;

	// 当前持续显示的背景图片
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	TSoftObjectPtr<UTexture2D> SavedBackgroundImage;

	// 游戏是否已开始（通常为true）
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	bool bSavedGameStarted;
    
	// 存档时间戳，方便UI显示
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FDateTime Timestamp;

	// 存档槽位名称 (方便管理)
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FString SlotName;
};