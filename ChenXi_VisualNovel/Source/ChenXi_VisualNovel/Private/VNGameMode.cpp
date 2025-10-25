// VNGameMode.cpp

#include "VNGameMode.h"
#include "VNPlayerController.h" // 需要引入PlayerController，以便在构造函数中设置
#include "VNDialogueWidget.h" // 引入对话框Widget
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "UI/VNUIManagerSubsystem.h" // 引入 UI 管理器子系统

AVNGameMode::AVNGameMode()
{
	// 设置默认的 PlayerController 为我们自定义的 AVNPlayerController
	// 确保你的项目使用这个自定义的 Controller
	PlayerControllerClass = AVNPlayerController::StaticClass(); 
    
	CurrentDialogIndex = 0;
}

void AVNGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 1. 获取 PlayerController
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameMode: Failed to get PlayerController"));
		return;
	}
	
	// 2. 创建根布局
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
		{
			UIManager->CreateRootLayoutIfNeeded(PC);
			UE_LOG(LogTemp, Log, TEXT("VNGameMode: UI Root Layout created"));
	
			CurrentBGMTrack = nullptr; 
			
			if (!MainMenuBGM.IsNull())
			{
				CurrentBGMTrack = MainMenuBGM;
				UE_LOG(LogTemp, Log, TEXT("Switching to Main Menu BGM: %s"), *CurrentBGMTrack.ToString());
	
				if(CurrentBgmComponent && CurrentBgmComponent->IsPlaying())
				{
					CurrentBgmComponent->Stop();
				}
	
				// 播放主菜单BGM
				USoundCue* LoadedBGM = MainMenuBGM.LoadSynchronous();
				if (LoadedBGM)
				{
					CurrentBgmComponent = UGameplayStatics::SpawnSound2D(this, LoadedBGM);
					
				}
			}
			
			// 显示主菜单，等待玩家点击"新游戏"
			UIManager->ShowMainMenu(PC);
			UE_LOG(LogTemp, Log, TEXT("VNGameMode: Main menu displayed"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("VNGameMode: Failed to get VNUIManagerSubsystem"));
		}
	}
}

void AVNGameMode::StartDialog()
{
	// 1. 检查 DataTable 是否设置
	if (!DialogDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DialogDataTable is NULL in AVNGameMode. Please set it in the Blueprint derived class."));
		return;
	}

	// 2. 从 DataTable 加载所有数据行
	StoryLines.Empty(); // 清空，防止重复加载
	// 创建一个用于接收 FTableRowBase* 的临时数组
	TArray<FTableRowBase*> RowBaseArray; 
	FString ContextString = TEXT("FDialogLine Context");
    
	// **核心修正**：调用 GetAllRows，现在参数类型匹配
	DialogDataTable->GetAllRows(ContextString, RowBaseArray);

	if (RowBaseArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("DialogDataTable is empty or failed to load any rows."));
		return;
	}

	// **新的步骤**：遍历指针数组，并安全地转换为我们的 FDialogLine 类型
	for (FTableRowBase* RowBase : RowBaseArray)
	{
		// 将通用的 FTableRowBase* 转换为 FDialogLine*
		if (FDialogLine* DialogLine = static_cast<FDialogLine*>(RowBase))
		{
			// 将数据复制到 StoryLines 数组中（值复制）
			StoryLines.Add(*DialogLine);
		}
	}

	// 检查转换后 StoryLines 的数量
	if (StoryLines.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast any rows to FDialogLine. Check your DataTable structure."));
		return;
	}

	CurrentDialogIndex = 0;
	UE_LOG(LogTemp, Log, TEXT("VN Dialog system initialized. Total lines: %d"), StoryLines.Num());
    
	// 此时可以通知 UI 系统显示第一个场景或第一行对话
}

void AVNGameMode::PlaySoundForLine(const FDialogLine& DialogLine)
{
	UE_LOG(LogTemp, Log, TEXT("PlaySoundForLine called"));

	// --- BGM切换逻辑 ---
	// 注意：CurrentBGMTrack 现在是成员变量，每次 PIE 重启时会自动重置

	// 调试日志：显示 BGM 状态
	if (DialogLine.BGM.IsNull())
	{
		UE_LOG(LogTemp, Log, TEXT("BGM is NULL for this dialogue line"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("BGM configured: %s"), *DialogLine.BGM.ToString());
		UE_LOG(LogTemp, Log, TEXT("Current BGM track: %s"), CurrentBGMTrack.IsNull() ? TEXT("NULL") : *CurrentBGMTrack.ToString());
	}

	if(!DialogLine.BGM.IsNull() && DialogLine.BGM != CurrentBGMTrack)
	{
		CurrentBGMTrack = DialogLine.BGM;
		UE_LOG(LogTemp, Log, TEXT("Switching to new BGM: %s"), *CurrentBGMTrack.ToString());

		if(CurrentBgmComponent && CurrentBgmComponent->IsPlaying())
		{
			CurrentBgmComponent->Stop();
			UE_LOG(LogTemp, Log, TEXT("Stopped previous BGM"));
		}

		//使用SpawnSound2D播放声音，他会自动处理加载和播放
		USoundCue* LoadedBGM = DialogLine.BGM.LoadSynchronous();
		if (LoadedBGM)
		{
			CurrentBgmComponent = UGameplayStatics::SpawnSound2D(this, LoadedBGM);
			if (CurrentBgmComponent)
			{
				UE_LOG(LogTemp, Log, TEXT("BGM spawned and playing"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn BGM AudioComponent"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load BGM asset"));
		}
	}

	// --- 特殊音效（SFX）播放逻辑 ---
	if(!DialogLine.SFX.IsNull())
	{
		UE_LOG(LogTemp, Log, TEXT("Playing SFX: %s"), *DialogLine.SFX.ToString());
		USoundBase* LoadedSFX = DialogLine.SFX.LoadSynchronous();
		if (LoadedSFX)
		{
			UGameplayStatics::PlaySound2D(this, LoadedSFX);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load SFX asset"));
		}
	}
}

bool AVNGameMode::GetNextDialogLine(FDialogLine& OutDialogLine)
{
	if (CurrentDialogIndex < StoryLines.Num())
	{
		// 将数据复制到传入的引用参数中
		OutDialogLine = StoryLines[CurrentDialogIndex];
		CurrentDialogIndex++;

		// 【新增】将这一行添加到历史记录中
		AddToHistory(OutDialogLine);

		UE_LOG(LogTemp, Log, TEXT("Dialog Line %d: [%s]: %s"),
			   CurrentDialogIndex,
			   *OutDialogLine.CharacterName,
			   *OutDialogLine.DialogueText.ToString());

		// 处理音频
		PlaySoundForLine(OutDialogLine);

		// 处理背景切换：如果此对话行配置了背景图片，通知 UIManager 切换
		if (!OutDialogLine.BackgroundImage.IsNull())
		{
			if (UGameInstance* GameInstance = GetGameInstance())
			{
				if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
				{
					UIManager->SetBackground(OutDialogLine.BackgroundImage);
					UE_LOG(LogTemp, Log, TEXT("Switching background to: %s"), *OutDialogLine.BackgroundImage.ToString());
				}
			}
		}

		// 成功获取数据，返回 true
		return true;
	}

	// 对话结束，返回 false
	UE_LOG(LogTemp, Log, TEXT("End of Story Flow."));
	return false;
}

void AVNGameMode::LoadDialogueData()
{
	// 检查 DataTable 是否设置
	if (!DialogDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DialogDataTable is NULL. Please set it in BP_VNGameMode."));
		return;
	}

	// 从 DataTable 加载所有数据行
	StoryLines.Empty();
	TArray<FTableRowBase*> RowBaseArray;
	FString ContextString = TEXT("FDialogLine Context");
	DialogDataTable->GetAllRows(ContextString, RowBaseArray);

	if (RowBaseArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("DialogDataTable is empty."));
		return;
	}

	// 转换为 FDialogLine
	for (FTableRowBase* RowBase : RowBaseArray)
	{
		if (FDialogLine* DialogLine = static_cast<FDialogLine*>(RowBase))
		{
			StoryLines.Add(*DialogLine);
		}
	}

	//CurrentDialogIndex = 0;
	UE_LOG(LogTemp, Log, TEXT("VNGameMode: Dialogue data loaded. Total lines: %d"), StoryLines.Num());
}

void AVNGameMode::StartNewGame()
{
	UE_LOG(LogTemp, Log, TEXT("VNGameMode: Starting new game..."));

	bGameStarted = true;

	if(CurrentBgmComponent && CurrentBgmComponent->IsPlaying())
	{
		CurrentBgmComponent->Stop();
	}
	//相关指针重置为null
	CurrentBgmComponent = nullptr;
	CurrentBGMTrack = nullptr;
	
	//【新增】清空先前的对话历史记录
	ClearHistory();
	
	// 1. 加载对话数据
	LoadDialogueData();

	// 2. 获取 PlayerController
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameMode: Failed to get PlayerController"));
		return;
	}

	// 3. 初始化对话 UI
	if (AVNPlayerController* VNController = Cast<AVNPlayerController>(PC))
	{
		VNController->InitializeUI();
		UE_LOG(LogTemp, Log, TEXT("VNGameMode: Dialogue UI initialized"));
	}

	// 4. 显示第一句对话
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
		{
			if (UVNDialogueWidget* DialogueWidget = UIManager->GetDialogueWidget())
			{
				FDialogLine FirstLine;
				if (GetNextDialogLine(FirstLine))
				{
					DialogueWidget->DisplayDialogueLine(FirstLine);
					UE_LOG(LogTemp, Log, TEXT("VNGameMode: First dialogue line displayed"));
				}
			}
		}
	}
}

void AVNGameMode::ReturnToMainMenu()
{
	UE_LOG(LogTemp, Log, TEXT("VNGameMode: Returning to main menu..."));

	bGameStarted = false;
	CurrentDialogIndex = 0;
	StoryLines.Empty();

	// TODO: 隐藏对话框,显示主菜单
	// 当主菜单实现后,在这里调用 UIManager->ShowMainMenu()
	// --- [ 1. 停止游戏内 BGM ] ---
	if (CurrentBgmComponent && CurrentBgmComponent->IsPlaying())
	{
		CurrentBgmComponent->Stop();
	}
	CurrentBgmComponent = nullptr;
	CurrentBGMTrack = nullptr; 

	// --- [ 2. 处理 UI 切换 ] ---
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameMode::ReturnToMainMenu - Failed to get PlayerController"));
		return;
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
		{
			// 1. 停用(隐藏)对话框 (调用我们刚创建的新函数)
			UIManager->HideDialogue();
			
			// 2. 显示主菜单
			UIManager->ShowMainMenu(PC);
			
			// 3. 重新播放主菜单BGM (这部分逻辑之前只在BeginPlay里)
			if (!MainMenuBGM.IsNull())
			{
				CurrentBGMTrack = MainMenuBGM;
				USoundCue* LoadedBGM = MainMenuBGM.LoadSynchronous();
				if (LoadedBGM)
				{
					CurrentBgmComponent = UGameplayStatics::SpawnSound2D(this, LoadedBGM);
					if (CurrentBgmComponent)
					{
						UE_LOG(LogTemp, Log, TEXT("VNGameMode: Playing Main Menu BGM after return."));
					}
				}
			}
		}
	}
}

// -----------------------------------------------------------------
// --- [新增] 存档/读档功能实现
// -----------------------------------------------------------------

void AVNGameMode::SaveGame(const FString& SlotName)
{
    if (!bGameStarted)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot save game: Game not started."));
        return;
    }

    // 1. 创建或获取存档对象
    UVNSaveGame* SaveGameObject = Cast<UVNSaveGame>(UGameplayStatics::CreateSaveGameObject(UVNSaveGame::StaticClass()));
    if (!SaveGameObject)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create SaveGame object."));
        return;
    }

    // 2. 填充数据
    // [重要] GetNextDialogLine 已经将 CurrentDialogIndex++
    // 所以我们保存的是 (CurrentDialogIndex - 1)，即当前屏幕上显示的行
    SaveGameObject->SavedDialogIndex = FMath::Max(0, CurrentDialogIndex - 1);
    SaveGameObject->SavedBGMTrack = CurrentBGMTrack;
    //SaveGameObject->SavedBackgroundImage = PersistentBackgroundImage;
    SaveGameObject->bSavedGameStarted = bGameStarted;
    SaveGameObject->Timestamp = FDateTime::Now();
    SaveGameObject->SlotName = SlotName;

    // 3. 异步保存到磁盘
    UGameplayStatics::AsyncSaveGameToSlot(SaveGameObject, SlotName, 0);
    UE_LOG(LogTemp, Log, TEXT("Game saved to slot: %s (Index: %d)"), *SlotName, SaveGameObject->SavedDialogIndex);
}

bool AVNGameMode::LoadGame(const FString& SlotName, FDialogLine& OutLoadedLine)
{
    // 1. 检查存档是否存在
    if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("Load failed: SaveGame slot %s does not exist."), *SlotName);
        OutLoadedLine = FDialogLine();
        return false;
    }

    // 2. 加载存档对象
    UVNSaveGame* SaveGameObject = Cast<UVNSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
    if (!SaveGameObject)
    {
        UE_LOG(LogTemp, Error, TEXT("Load failed: Could not cast SaveGame object."));
        OutLoadedLine = FDialogLine();
        return false;
    }
	
    // 3. 恢复GameMode的状态
    bGameStarted = SaveGameObject->bSavedGameStarted;
    CurrentDialogIndex = SaveGameObject->SavedDialogIndex;
    CurrentBGMTrack = SaveGameObject->SavedBGMTrack;
    //PersistentBackgroundImage = SaveGameObject->SavedBackgroundImage;

    // 4. 确保对话数据已加载
    LoadDialogueData();

    // 5. 检查索引是否有效
    if (!StoryLines.IsValidIndex(CurrentDialogIndex))
    {
        UE_LOG(LogTemp, Error, TEXT("Load failed: Saved index %d is invalid."), CurrentDialogIndex);
        ReturnToMainMenu(); // 存档数据损坏，返回主菜单
        return false;
    }
    else
    {
    	UE_LOG(LogTemp, Log, TEXT("Load Success: Saved index %d is valid."), CurrentDialogIndex);
    }
    
    // --- 6. [重要] 恢复游戏世界状态 ---
    
    //TODO: 6a. 停止主菜单BGM并播放游戏BGM
    //PlayBGM(CurrentBGMTrack);

    //TODO: 6b. 恢复背景图
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
        {
            // 确保对话框可见
            if (AVNPlayerController* VNController = Cast<AVNPlayerController>(GetWorld()->GetFirstPlayerController()))
            {
                VNController->InitializeUI(); 
            }
            // 设置背景
            //UIManager->SetBackground(PersistentBackgroundImage);
        }
    }

    // 7. 获取加载的行的数据，并返回给UI
    OutLoadedLine = StoryLines[CurrentDialogIndex];
    
    // 8. [关键] 播放 *这一行* 的一次性音效和语音
    // (因为 ProcessLineState 还没有被调用, 我们只恢复持久状态)
    if (!OutLoadedLine.SFX.IsNull())
    {
        UGameplayStatics::PlaySound2D(this, OutLoadedLine.SFX.LoadSynchronous());
    }
    if (!OutLoadedLine.VoiceLine.IsNull())
    {
        UGameplayStatics::PlaySound2D(this, OutLoadedLine.VoiceLine.LoadSynchronous());
    }

    // 9. [关键] 推进索引，为 *下一次* 玩家点击做准备
    //CurrentDialogIndex++;

    UE_LOG(LogTemp, Log, TEXT("Game loaded from slot: %s. Resuming at index %d. Next index will be %d."), *SlotName, CurrentDialogIndex - 1, CurrentDialogIndex);

	//10.更新当前UI显示的对话
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
		{
			if (UVNDialogueWidget* DialogueWidget = UIManager->GetDialogueWidget())
			{
				FDialogLine outLine;
				if (GetNextDialogLine(outLine))
				{
					DialogueWidget->DisplayDialogueLine(outLine);
					UE_LOG(LogTemp, Log, TEXT("VNGameMode: Current dialogue line displayed"));
				}
			}
		}
	}


	return true;
}