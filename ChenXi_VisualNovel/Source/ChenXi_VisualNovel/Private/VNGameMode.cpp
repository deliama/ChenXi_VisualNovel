// VNGameMode.cpp

#include "VNGameMode.h"
#include "VNPlayerController.h" // 需要引入PlayerController，以便在构造函数中设置
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

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
    
	// 在游戏开始时立即加载并准备对话
	StartDialog();
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
	// --- BGM切换逻辑 ---
	static TSoftObjectPtr<USoundCue> CurrentBGMTrack = nullptr;
	if(!DialogLine.BGM.IsNull() && DialogLine.BGM != CurrentBGMTrack)
	{
		CurrentBGMTrack = DialogLine.BGM;

		if(CurrentBgmComponent && CurrentBgmComponent->IsPlaying())
		{
			CurrentBgmComponent->Stop();
		}

		//使用SpawnSound2D播放声音，他会自动处理加载和播放
		CurrentBgmComponent = UGameplayStatics::SpawnSound2D(this,DialogLine.BGM.LoadSynchronous());
		
	}

	// --- 特殊音效（SFX）播放逻辑 ---
	if(!DialogLine.SFX.IsNull())
	{
		UGameplayStatics::PlaySound2D(this,DialogLine.SFX.LoadSynchronous());
	}
}


bool AVNGameMode::GetNextDialogLine(FDialogLine& OutDialogLine)
{
	if (CurrentDialogIndex < StoryLines.Num())
	{
		// 将数据复制到传入的引用参数中
		OutDialogLine = StoryLines[CurrentDialogIndex]; 
		CurrentDialogIndex++;
        
		UE_LOG(LogTemp, Log, TEXT("Dialog Line %d: [%s]: %s"), 
			   CurrentDialogIndex,
			   *OutDialogLine.CharacterName, 
			   *OutDialogLine.DialogueText.ToString());

		PlaySoundForLine(OutDialogLine);
		
		// 成功获取数据，返回 true
		return true;
	}
    
	// 对话结束，返回 false
	UE_LOG(LogTemp, Log, TEXT("End of Story Flow."));
	return false; 
}