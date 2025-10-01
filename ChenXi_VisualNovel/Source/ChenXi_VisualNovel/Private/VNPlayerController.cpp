// VNPlayerController.cpp

#include "VNPlayerController.h"
#include "InputCoreTypes.h" // 包含 FKey 等输入相关定义
#include "VNDialogueWidget.h"
#include "VNGameMode.h"      // 引入 GameMode 头文件，以便调用其函数
#include "EnhancedInputSubsystems.h" // 增强输入子系统
#include "EnhancedInputComponent.h"  // 增强输入组件


void AVNPlayerController::BeginPlay()
{
	// 必须调用基类实现
	Super::BeginPlay();
    
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
		if (AdvanceDialogueAction) // <--- 符号无法解析的报错通常发生在这里
		{
			// 绑定 Triggered 事件到 AdvanceDialogue 函数
			EnhancedInputComponent->BindAction(
				AdvanceDialogueAction, 
				ETriggerEvent::Triggered, 
				this, 
				&AVNPlayerController::AdvanceDialogue
			);
			UE_LOG(LogTemp, Log, TEXT("Input Action 'AdvanceDialogue' bound successfully."));
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

// 新增函数：初始化 UI
void AVNPlayerController::InitializeUI()
{
	// 确保只创建一次
	if (!DialogueWidgetInstance && DialogueWidgetClass)
	{
		// 1. 创建 Widget 实例
		DialogueWidgetInstance = CreateWidget<UVNDialogueWidget>(this, DialogueWidgetClass);
        
		if (DialogueWidgetInstance)
		{
			// 2. 将 Widget 添加到视口 (Viewport)
			DialogueWidgetInstance->AddToViewport();
            
			// 3. 此时可以设置输入模式为 UI Only，防止角色移动等
			// FInputModeUIOnly InputMode;
			// SetInputMode(InputMode);
            
			UE_LOG(LogTemp, Log, TEXT("Dialog Widget Initialized and added to Viewport."));
		}
	}
}

// 确保函数签名完全匹配 .h 文件中的声明
void AVNPlayerController::AdvanceDialogue(const FInputActionValue& Value)
{
	// *** 确保这是你在 C++ 文件中唯一的 AdvanceDialogue 定义 ***

	// 仅保留日志用于测试 Enhanced Input
	UE_LOG(LogTemp, Warning, TEXT("!!! MOUSE CLICK DETECTED VIA ENHANCED INPUT !!!"));

	// 1. 确保 UI 已经实例化并显示。
	if (!DialogueWidgetInstance)
	{
		InitializeUI();
	}
    
	// 检查 GameMode 和 Widget 实例是否都存在
	AVNGameMode* VNGameMode = Cast<AVNGameMode>(GetWorld()->GetAuthGameMode());
    
	if (VNGameMode && DialogueWidgetInstance)
	{
		FDialogLine CurrentLineData;

		// 2. 调用 GameMode 的函数获取下一行数据
		if (VNGameMode->GetNextDialogLine(CurrentLineData))
		{
			// 3. 将获取到的数据传递给 UI Widget
			DialogueWidgetInstance->DisplayDialogueLine(CurrentLineData);
            
			UE_LOG(LogTemp, Log, TEXT("Controller passed data to UI: [%s]"), *CurrentLineData.CharacterName);
		}
		else
		{
			// 对话结束
			UE_LOG(LogTemp, Warning, TEXT("Dialogue has finished."));
		}
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