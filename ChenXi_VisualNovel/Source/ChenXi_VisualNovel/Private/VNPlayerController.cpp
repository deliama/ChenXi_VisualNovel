// VNPlayerController.cpp

#include "VNPlayerController.h"
#include "InputCoreTypes.h" // 包含 FKey 等输入相关定义
#include "VNDialogueWidget.h"
#include "VNGameMode.h"      // 引入 GameMode 头文件，以便调用其函数
#include "EnhancedInputSubsystems.h" // 增强输入子系统
#include "EnhancedInputComponent.h"  // 增强输入组件
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundCue.h"
#include "UI/VNUIManagerSubsystem.h"  // UI 管理器
#include "TimerManager.h"
#include "UI/VNPrimaryGameLayout.h"    // 根布局
#include "NativeGameplayTags.h"        // GameplayTag 支持

void AVNPlayerController::BeginPlay()
{
	// 必须调用基类实现
	Super::BeginPlay();

	//游戏开始时，激活主音量混合器
	if(VolumeControlSoundMix)
	{
		UGameplayStatics::PushSoundMixModifier(this, VolumeControlSoundMix);
	}
    
	// --- 设置 Input Mapping Context (IMC) ---
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultInputMapping)
		{
			Subsystem->AddMappingContext(DefaultInputMapping, 0); 
			UE_LOG(LogTemp, Log, TEXT("Enhanced Input Mapping Context added."));
		}
	}
    
	// --- 绑定 Input Action (IA) ---
	// 确保 InputComponent 存在，并尝试将其转换为 EnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 关键：检查 AdvanceDialogueAction 是否已在蓝图配置中设置
		if (AdvanceDialogueAction)
		{
			// 绑定 Triggered 事件到 AdvanceDialogue  函数
			EnhancedInputComponent->BindAction(
				AdvanceDialogueAction, 
				ETriggerEvent::Started, 
				this, 
				&AVNPlayerController::AdvanceDialogue
			);
			UE_LOG(LogTemp, Log, TEXT("Input Action 'AdvanceDialogue' bound successfully."));
		}
		//绑定自动模式
		if (ToggleAutoModeAction)
		{
			EnhancedInputComponent->BindAction(
				ToggleAutoModeAction,
				ETriggerEvent::Started,
				this,
				&AVNPlayerController::ToggleAutoMode
			);
			UE_LOG(LogTemp, Log, TEXT("Input Action 'ToggleAutoMode' bound successfully."));
		}
		//绑定快进模式
		if (ToggleFastForwardAction)
		{
			EnhancedInputComponent->BindAction(
				ToggleFastForwardAction,
				ETriggerEvent::Started, 
				this, 
				&AVNPlayerController::ToggleFastForwardMode
			);
			UE_LOG(LogTemp, Log, TEXT("Input Action 'ToggleFastForwardMode' bound successfully."));
		}
	}
	
}

AVNPlayerController::AVNPlayerController()
{
	// 启用鼠标光标显示和点击事件，这是 VN 游戏的标准做法
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// 默认设置，确保 UI 在游戏开始时就创建
	bAutoManageActiveCameraTarget = false;
}

// 初始化 UI - 使用 CommonUI Stack 系统
void AVNPlayerController::InitializeUI()
{
	// 通过 UIManager 创建对话框
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
		{
			UIManager->ShowDialogue(this);
			UE_LOG(LogTemp, Log, TEXT("对话框初始化请求已发送到 UIManager"));

			// 获取对话框实例并绑定事件
			if (UVNDialogueWidget* DialogueWidget = UIManager->GetDialogueWidget())
			{
				// 绑定 OnTypewriterFinished 事件
				DialogueWidget->OnTypewriterFinished.AddDynamic(this, &AVNPlayerController::OnTypewriterFinished);
				UE_LOG(LogTemp, Log, TEXT("PlayerController bound to OnTypewriterFinished event."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("无法获取 UIManager，对话框初始化失败"));
		}
	}
}

// 确保函数签名完全匹配 .h 文件中的声明
void AVNPlayerController::AdvanceDialogue(const FInputActionValue& Value)
{
	// *** 确保这是你在 C++ 文件中唯一的 AdvanceDialogue 定义 ***

	// 玩家的任何主动点击 (鼠标、手柄、键盘) 都会打断自动前进
	GetWorld()->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
	// 如果玩家点击时是自动模式, 保持自动模式开启, 但计时器被重置并等待下一次 OnTypewriterFinished

	const bool bIsPlayerInput = Value.Get<bool>();
	if (bIsPlayerInput)
	{
		//玩家的主动点击会打断快进模式
		if (bIsFastForwardMode)
		{
			bIsFastForwardMode = false;
			UE_LOG(LogTemp, Log, TEXT("Fast Forward Mode: DISABLED (Interrupted by player input)"));
		}

		//TODO: 如果想做的话，玩家的输入也可能打断自动模式（以防万一）
	}
	
	// 仅保留日志用于测试 Enhanced Input
	//UE_LOG(LogTemp, Warning, TEXT("!!! MOUSE CLICK DETECTED VIA ENHANCED INPUT !!!"));

	// 1. 获取所有需要的子系统和管理器
	UVNDialogueWidget* DialogueWidgetInstance = nullptr;
	UVNPrimaryGameLayout* RootLayoutInstance = nullptr;
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
		{
			// 一次性获取所有需要的UI控件
			DialogueWidgetInstance = UIManager->GetDialogueWidget();
			RootLayoutInstance = UIManager->GetPrimaryGameLayout();
		}
	}

	// 2. 检查对话框是否已初始化（应该由 GameMode 自动初始化）
	if (!DialogueWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueWidget not initialized! GameMode should have initialized it."));
		return;
	}
	if (!RootLayoutInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AdvanceDialogue: RootLayoutInstance is NULL."));
		return;
	}

	// 3. 如果打字机正在播放，跳过动画
	if(DialogueWidgetInstance->bIsTypewriterActive)
	{
		DialogueWidgetInstance->SkipTypewriter();
	}
	else
	{
		// 4. 推进到下一句对话
		AVNGameMode* VNGameMode = Cast<AVNGameMode>(GetWorld()->GetAuthGameMode());

		if (VNGameMode && DialogueWidgetInstance)
		{
			FDialogLine CurrentLineData;

			// 5. 调用 GameMode 的函数获取下一行数据
			if (VNGameMode->GetNextDialogLine(CurrentLineData))
			{
				/*
				// 6. 将获取到的数据传递给 UI Widget
				DialogueWidgetInstance->DisplayDialogueLine(CurrentLineData);

				UE_LOG(LogTemp, Log, TEXT("Controller passed data to UI: [%s]"), *CurrentLineData.CharacterName);
				*/
				// 6. *** 将数据分发到所有系统 ***

				// 6a. (新) 发送到角色动画系统
				RootLayoutInstance->HandleCharacterCommand(CurrentLineData);

				// 6b. (补充) 发送到背景系统
				if (UGameInstance* GameInstance = GetGameInstance())
				{
					if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
					{
						// 直接命令 UIManager 设置背景，它会自己处理创建逻辑
						UIManager->SetBackground(CurrentLineData.BackgroundImage);
					}
				}
				// (确保背景层已注册到RootLayout, 并且 BackgroundWidgetInstance 已添加)
				// RootLayoutInstance->AddNativeWidgetToLayer(FGameplayTag::RequestGameplayTag(TEXT("UI.Layer.Background")), BackgroundWidgetInstance);
				// ^^^ 这一行通常只需要在UI初始化时做一次，而不是每次推进对话都做

				// 6c. (已有) 发送到对话框系统
				DialogueWidgetInstance->DisplayDialogueLine(CurrentLineData);
				// (确保对话框层已注册到RootLayout, 并且 DialogueWidgetInstance 已添加/推送)
				// RootLayoutInstance->PushWidgetToLayerStack(FGameplayTag::RequestGameplayTag(TEXT("UI.Layer.Dialogue")), DialogueWidgetInstance);
				// ^^^ 这一行通常也只需要在UI初始化时做一次

				// // 6d. (补充) 播放BGM和SFX
				// if (!CurrentLineData.BGM.IsNull()) // 先检查软指针是否有效
				// {
				// 	// [修复] LoadSynchronous() 返回的是 USoundBase*，不是 USoundCue*
				// 	USoundBase* BGMSound = CurrentLineData.BGM.LoadSynchronous(); // 同步加载
    //
				// 	if (BGMSound)
				// 	{
				// 		// [修复] 传入正确的 USoundBase* 指针
				// 		UGameplayStatics::PlaySound2D(this, BGMSound); // 传入加载后的指针
				// 	}
				// }
				// if (!CurrentLineData.SFX.IsNull()) // 先检查软指针是否有效
				// {
				// 	USoundBase* SFXSound = CurrentLineData.SFX.LoadSynchronous(); // 同步加载
				// 	if (SFXSound)
				// 	{
				// 		UGameplayStatics::PlaySound2D(this, SFXSound); // 传入加载后的指针
				// 	}
				// }
				

				UE_LOG(LogTemp, Log, TEXT("Controller passed data to UI: [%s]"), *CurrentLineData.CharacterName);
			}
			else
			{
				// 对话结束
				UE_LOG(LogTemp, Warning, TEXT("Dialogue has finished."));
				bIsFastForwardMode = false;
				bIsAutoMode = false;
				GetWorld()->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
				GetWorld()->GetTimerManager().ClearTimer(FastForwardTimerHandle);
			}
		}
	}
}

void AVNPlayerController::ToggleAutoMode()
{
	bIsAutoMode = !bIsAutoMode;

	if (bIsAutoMode)
	{
		UE_LOG(LogTemp, Log, TEXT("Auto Mode: ENABLED"));

		if (bIsFastForwardMode)
		{
			bIsFastForwardMode = false;
			UE_LOG(LogTemp, Log, TEXT("Fast Forward Mode: DISABLED(Auto Mode Enabled"));

			GetWorld()->GetTimerManager().ClearTimer(FastForwardTimerHandle);
		}
		
		// 立即尝试触发一次, 以防对话已经处于“等待”状态
		OnTypewriterFinished();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Auto Mode: DISABLED"));
		// 关闭自动模式时, 清除所有待处理的计时器
		GetWorld()->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
	}
}

void AVNPlayerController::ToggleFastForwardMode()
{
	bIsFastForwardMode= !bIsFastForwardMode;

	if (bIsFastForwardMode)
	{
		UE_LOG(LogTemp, Log, TEXT("Fast Forward Mode: ENABLED"));

		// [新增] 开启快进时, 必须关闭自动模式
		if (bIsAutoMode)
		{
			bIsAutoMode = false;
			UE_LOG(LogTemp, Log, TEXT("Auto Mode: DISABLED (Fast Forward enabled)"));
			// 确保清除自动计时器
			GetWorld()->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
		}

		GetWorld()->GetTimerManager().SetTimer(
			FastForwardTimerHandle,
			this,
			&AVNPlayerController::TriggerFastForward,
			FastForwardDelay,
			true);
		//立即触发一次
		TriggerFastForward();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Fast Forward Mode: DISABLED"));
		GetWorld()->GetTimerManager().ClearTimer(FastForwardTimerHandle);
	}
}

void AVNPlayerController::OnTypewriterFinished()
{

	//检查自动模式
	// 仅在自动模式开启 且 打字机*不*活跃时 (防止SkipTypewriter触发两次) 才设置计时器
	if (bIsAutoMode)
	{
		UVNDialogueWidget* DialogueWidgetInstance = nullptr;
		if (GetGameInstance())
		{
			if (UVNUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UVNUIManagerSubsystem>())
			{
				DialogueWidgetInstance = UIManager->GetDialogueWidget();
			}
		}

		// 确保对话框存在, 并且打字机确实 *已停止*
		if (DialogueWidgetInstance && !DialogueWidgetInstance->bIsTypewriterActive)
		{
			UE_LOG(LogTemp, Log, TEXT("Auto Mode: Typewriter finished, starting timer for next line..."));
			GetWorld()->GetTimerManager().SetTimer(
				AutoAdvanceTimerHandle,
				this,
				&AVNPlayerController::TriggerAutoAdvance,
				AutoModeDelay, // 使用我们在 .h 中定义的延迟
				false
			);
		}
	}
}

void AVNPlayerController::TriggerAutoAdvance()
{
	// 仅在自动模式仍然开启时才执行
	if (bIsAutoMode)
	{
		UE_LOG(LogTemp, Log, TEXT("Auto Mode: Timer fired, advancing dialogue."));
		// 传递一个空的 FInputActionValue, 因为这不是由直接输入触发的
		AdvanceDialogue(FInputActionValue()); 
	}
}

void AVNPlayerController::TriggerFastForward()
{
	// 仅在快进模式仍然开启时才执行
	// (这是一个安全检查, 尽管计时器在 Toggle 时应该已被清除)
	if (bIsFastForwardMode)
	{
		// 传递一个空的 FInputActionValue, 因为这不是由直接输入触发的
		AdvanceDialogue(FInputActionValue()); 
	}
	else
	{
		// 如果由于某种原因计时器仍在运行但 bIsFastForwarding 已为 false，则停止计时器
		GetWorld()->GetTimerManager().ClearTimer(FastForwardTimerHandle);
	}
}

void AVNPlayerController::SetBGMVolume(float Volume)
{
	if(BGM_SoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(
			this,
			VolumeControlSoundMix,
			BGM_SoundClass,
			Volume,
			1.0f,
			0.0f);
	}
}

void AVNPlayerController::SetSFXVolume(float Volume)
{
	if(SFX_SoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(
			this,
			VolumeControlSoundMix,
			SFX_SoundClass,
			Volume,
			1.0f,
			0.0f);
	}
}

void AVNPlayerController::SetUIVolume(float Volume)
{
	if(UI_SoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(
			this,
			VolumeControlSoundMix,
			UI_SoundClass,
			Volume,
			1.0f,
			0.0f);
	}
}

// void AVNPlayerController::SetupInputComponent()
// {
// 	Super::SetupInputComponent();
//     
// 	// 绑定鼠标左键按下事件到 AdvanceDialogue 函数
// 	// 这里的 "LeftMouseButton" 是一个预定义的输入键名
// 	if (InputComponent)
// 	{
// 		InputComponent->BindAction(
// 			"LeftMouseButton", 
// 			IE_Pressed, 
// 			this, 
// 			&AVNPlayerController::AdvanceDialogue
// 		);
// 	}
//     
// 	// 提醒: 这是一个快速Demo绑定。未来建议使用 Enhanced Input System (EIS) 来绑定 Input Action。
// }

// void AVNPlayerController::AdvanceDialogue()
// {
// 	// 1. 确保 UI 已经实例化并显示。第一次点击时会创建 UI。
// 	if (!DialogueWidgetInstance)
// 	{
// 		InitializeUI();
// 	}
//     
// 	// 检查 GameMode 和 Widget 实例是否都存在
// 	AVNGameMode* VNGameMode = Cast<AVNGameMode>(GetWorld()->GetAuthGameMode());
//     
// 	if (VNGameMode && DialogueWidgetInstance)
// 	{
// 		// 定义一个 FDialogLine 结构体实例，用于接收 GameMode 返回的数据
// 		FDialogLine CurrentLineData;
//
// 		// 调用 GameMode 的函数获取下一行数据
// 		bool bSuccess = VNGameMode->GetNextDialogLine(CurrentLineData);
//         
// 		if (bSuccess)
// 		{
// 			// **核心步骤：** 将获取到的数据传递给 UI Widget
// 			DialogueWidgetInstance->DisplayDialogueLine(CurrentLineData);
//             
// 			UE_LOG(LogTemp, Log, TEXT("Controller passed data to UI: [%s]"), *CurrentLineData.CharacterName);
// 		}
// 		else
// 		{
// 			// 对话结束：此时可以隐藏对话框，或者进入游戏结局/选项菜单
// 			UE_LOG(LogTemp, Warning, TEXT("Dialogue has finished."));
// 			// 示例：DialogWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
// 		}
// 	}
// 	else
// 	{
// 		// 确保 UI 实例化失败时能看到警告
// 		if (!DialogueWidgetInstance)
// 		{
// 			UE_LOG(LogTemp, Error, TEXT("Cannot advance dialogue: DialogWidgetInstance is NULL. Check DialogWidgetClass in BP_VNPlayerController."));
// 		}
// 	}
// }