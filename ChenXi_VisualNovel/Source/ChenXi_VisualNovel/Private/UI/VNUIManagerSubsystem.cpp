// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#include "UI/VNUIManagerSubsystem.h"
#include "UI/VNPrimaryGameLayout.h"
#include "UI/VNBackgroundWidget.h"
#include "VNDialogueWidget.h"
#include "CommonActivatableWidget.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VNUIManagerSubsystem)

// 定义对话层 GameplayTag
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_Layer_Dialogue, "UI.Layer.Dialogue");
// 定义菜单层 GameplayTag
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_Layer_Menu, "UI.Layer.Menu");

UVNUIManagerSubsystem::UVNUIManagerSubsystem()
{
}

void UVNUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Initialized"));
}

void UVNUIManagerSubsystem::Deinitialize()
{
	// Clean up the dialogue widget
	if (DialogueWidget)
	{
		DialogueWidget->RemoveFromParent();
		DialogueWidget = nullptr;
	}

	// Clean up the background widget
	if (BackgroundWidget)
	{
		BackgroundWidget->RemoveFromParent();
		BackgroundWidget = nullptr;
	}

	// Clean up the root layout
	if (RootLayout)
	{
		RootLayout->RemoveFromParent();
		RootLayout = nullptr;
	}

	Super::Deinitialize();

	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Deinitialized"));
}

bool UVNUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Don't create for dedicated servers
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// 如果有蓝图派生类，优先使用蓝图派生类
		// C++ 基类（Abstract）不应该被实例化
		if (GetClass()->HasAnyClassFlags(CLASS_Abstract))
		{
			// 如果这是抽象基类，不实例化
			return false;
		}

		// Only create if this is the most-derived class
		return ChildClasses.Num() == 0;
	}

	return false;
}

void UVNUIManagerSubsystem::CreateRootLayoutIfNeeded(APlayerController* PlayerController)
{
	// Already created
	if (RootLayout)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: Root layout already exists"));
		return;
	}

	// No player controller
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: PlayerController is null"));
		return;
	}

	// No layout class configured
	if (RootLayoutClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: No RootLayoutClass configured. Please create BP_VNUIManagerSubsystem blueprint and set Root Layout Class in Class Defaults."));
		return;
	}

	// Load the layout class
	TSubclassOf<UVNPrimaryGameLayout> LoadedLayoutClass = RootLayoutClass.LoadSynchronous();
	if (!LoadedLayoutClass)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to load RootLayoutClass"));
		return;
	}

	// Create the layout widget
	RootLayout = CreateWidget<UVNPrimaryGameLayout>(PlayerController, LoadedLayoutClass);
	if (!RootLayout)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to create root layout widget"));
		return;
	}

	// Add to viewport at highest Z-order
	RootLayout->AddToViewport(0);

	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Root layout created and added to viewport"));
}

void UVNUIManagerSubsystem::SetBackground(TSoftObjectPtr<UTexture2D> NewBackground)
{
	// 确保根布局已创建
	if (!RootLayout)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::SetBackground: Root layout not created yet"));
		return;
	}

	// 确保背景 Widget 已创建
	CreateBackgroundWidgetIfNeeded();

	if (!BackgroundWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::SetBackground: Failed to create background widget"));
		return;
	}

	// 调用背景 Widget 的切换方法
	BackgroundWidget->SetBackground(NewBackground);
}

void UVNUIManagerSubsystem::CreateBackgroundWidgetIfNeeded()
{
	// 已经创建
	if (BackgroundWidget)
	{
		return;
	}

	// 没有配置背景 Widget 类
	if (BackgroundWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: No BackgroundWidgetClass configured. Please create BP_VNUIManagerSubsystem blueprint and set Background Widget Class in Class Defaults."));
		return;
	}

	// 加载背景 Widget 类
	TSubclassOf<UVNBackgroundWidget> LoadedBackgroundClass = BackgroundWidgetClass.LoadSynchronous();
	if (!LoadedBackgroundClass)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to load BackgroundWidgetClass"));
		return;
	}

	// 获取玩家控制器（用于创建 Widget）
	if (!RootLayout)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Root layout not created yet"));
		return;
	}

	APlayerController* PC = RootLayout->GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to get owning player controller"));
		return;
	}

	// 创建背景 Widget
	BackgroundWidget = CreateWidget<UVNBackgroundWidget>(PC, LoadedBackgroundClass);
	if (!BackgroundWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to create background widget"));
		return;
	}

	// 将背景 Widget 添加到 Background Named Slot 层
	static const FGameplayTag TAG_UI_Layer_Background = FGameplayTag::RequestGameplayTag(FName("UI.Layer.Background"));
	RootLayout->AddNativeWidgetToLayer(TAG_UI_Layer_Background, BackgroundWidget);

	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Background widget created and added to Background layer"));
}

void UVNUIManagerSubsystem::ShowDialogue(APlayerController* PlayerController)
{
	// 对话框已经创建，直接返回
	if (DialogueWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: Dialogue widget already exists"));
		return;
	}

	// 确保根布局已创建
	if (!RootLayout)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::ShowDialogue: Root layout not created yet"));
		return;
	}

	// 确保传入了有效的 PlayerController
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::ShowDialogue: PlayerController is null"));
		return;
	}

	// 没有配置对话框 Widget 类
	if (DialogueWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: No DialogueWidgetClass configured. Please set Dialogue Widget Class in BP_VNUIManagerSubsystem Class Defaults."));
		return;
	}

	// 加载对话框 Widget 类
	TSubclassOf<UVNDialogueWidget> LoadedDialogueClass = DialogueWidgetClass.LoadSynchronous();
	if (!LoadedDialogueClass)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to load DialogueWidgetClass"));
		return;
	}

	// 创建对话框 Widget
	DialogueWidget = CreateWidget<UVNDialogueWidget>(PlayerController, LoadedDialogueClass);
	if (!DialogueWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to create dialogue widget"));
		return;
	}

	// 将对话框推入 DialogueStack 层（使用 CommonUI Stack 管理）
	RootLayout->PushWidgetToLayerStack(TAG_UI_Layer_Dialogue, DialogueWidget);

	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Dialogue widget created and pushed to Dialogue layer"));
}

void UVNUIManagerSubsystem::ShowMainMenu(APlayerController* PlayerController)
{
	// 确保根布局已创建
	if (!RootLayout)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::ShowMainMenu: Root layout not created yet"));
		return;
	}

	// 确保传入了有效的 PlayerController
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::ShowMainMenu: PlayerController is null"));
		return;
	}

	// 没有配置主菜单 Widget 类
	if (MainMenuWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: No MainMenuWidgetClass configured. Please set Main Menu Widget Class in BP_VNUIManagerSubsystem Class Defaults."));
		return;
	}

	// 加载主菜单 Widget 类
	TSubclassOf<UCommonActivatableWidget> LoadedMenuClass = MainMenuWidgetClass.LoadSynchronous();
	if (!LoadedMenuClass)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to load MainMenuWidgetClass"));
		return;
	}

	// 创建主菜单 Widget
	UCommonActivatableWidget* MainMenuWidget = CreateWidget<UCommonActivatableWidget>(PlayerController, LoadedMenuClass);
	if (!MainMenuWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to create main menu widget"));
		return;
	}

	// 将主菜单推入 Menu Stack 层
	RootLayout->PushWidgetToLayerStack(TAG_UI_Layer_Menu, MainMenuWidget);

	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Main menu created and pushed to Menu layer"));
}
