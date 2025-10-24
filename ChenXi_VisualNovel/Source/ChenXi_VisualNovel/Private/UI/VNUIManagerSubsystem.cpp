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
#include "UObject/SoftObjectPath.h" // 需要包含这个头文件来使用 FSoftObjectPath

#include UE_INLINE_GENERATED_CPP_BY_NAME(VNUIManagerSubsystem)

// 定义 GameplayTag
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_Layer_Dialogue, "UI.Layer.Dialogue");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_Layer_Menu, "UI.Layer.Menu");

UVNUIManagerSubsystem::UVNUIManagerSubsystem()
{
}

void UVNUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("<<<<< UVNUIManagerSubsystem::Initialize HAS BEEN CALLED for class: %s >>>>>"), *GetClass()->GetName());
	Super::Initialize(Collection);

	

	const FString RootLayoutPath = TEXT("/Game/_Game/UI/WPB_VNRootLayout.WPB_VNRootLayout_C");
	RootLayoutClass = TSoftClassPtr<UVNPrimaryGameLayout>(FSoftObjectPath(RootLayoutPath));
	

	const FString BackgroundWidgetPath = TEXT("/Game/_Game/UI/WBP_VNBackground.WBP_VNBackground_C");
	BackgroundWidgetClass = TSoftClassPtr<UVNBackgroundWidget>(FSoftObjectPath(BackgroundWidgetPath));
	

	const FString DialogueWidgetPath = TEXT("/Game/_Game/UI/WBP_VNDialogueWidget.WBP_VNDialogueWidget_C");
	DialogueWidgetClass = TSoftClassPtr<UVNDialogueWidget>(FSoftObjectPath(DialogueWidgetPath));
	

	const FString MainMenuWidgetPath = TEXT("/Game/_Game/UI/WBP_MainMenu.WBP_MainMenu_C");
	MainMenuWidgetClass = TSoftClassPtr<UCommonActivatableWidget>(FSoftObjectPath(MainMenuWidgetPath));
	

	// 验证路径设置是否成功 (IsValid 会检查 SoftObjectPath 是否有效)
	if (!RootLayoutClass.IsValid() || !BackgroundWidgetClass.IsValid() || !DialogueWidgetClass.IsValid() || !MainMenuWidgetClass.IsValid())
	{
		 UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: FATAL ERROR - One or more UI class paths hardcoded in C++ Initialize are INVALID! Check paths carefully."));
	}
	

	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Initialized and UI class paths set via C++."));
}

void UVNUIManagerSubsystem::Deinitialize()
{
	
	if (DialogueWidget)
	{
		DialogueWidget->RemoveFromParent();
		DialogueWidget = nullptr;
	}
	if (BackgroundWidget)
	{
		BackgroundWidget->RemoveFromParent();
		BackgroundWidget = nullptr;
	}
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
	// 不要为专用服务器创建
	if (CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		return false;
	}
	
	return true;
}


void UVNUIManagerSubsystem::CreateRootLayoutIfNeeded(APlayerController* PlayerController)
{
	if (RootLayout)
	{
		return;
	}
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: PlayerController is null"));
		return;
	}

	
	if (RootLayoutClass.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: RootLayoutClass was not set in Initialize! Check C++ paths."));
		return;
	}

	
	TSubclassOf<UVNPrimaryGameLayout> LoadedLayoutClass = RootLayoutClass.LoadSynchronous();
	if (!LoadedLayoutClass)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to load RootLayoutClass: %s"), *RootLayoutClass.ToString());
		return;
	}

	
	RootLayout = CreateWidget<UVNPrimaryGameLayout>(PlayerController, LoadedLayoutClass);
	if (!RootLayout)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to create root layout widget"));
		return;
	}

	RootLayout->AddToViewport(0);
	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Root layout created and added to viewport"));
}


void UVNUIManagerSubsystem::SetBackground(TSoftObjectPtr<UTexture2D> NewBackground)
{
    
	if (!RootLayout)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::SetBackground: Root layout not created yet"));
		return;
	}
	CreateBackgroundWidgetIfNeeded();
	if (!BackgroundWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::SetBackground: Failed to create background widget"));
		return;
	}
	BackgroundWidget->SetBackground(NewBackground);
}


void UVNUIManagerSubsystem::CreateBackgroundWidgetIfNeeded()
{
	if (BackgroundWidget)
	{
		return;
	}

	if (BackgroundWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: BackgroundWidgetClass was not set in Initialize! Check C++ paths."));
		return;
	}

	if (!RootLayout)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Root layout not created yet for background"));
		return;
	}
	APlayerController* PC = RootLayout->GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to get owning player controller for background"));
		return;
	}

	// 【添加 LoadSynchronous】
	TSubclassOf<UVNBackgroundWidget> LoadedBackgroundClass = BackgroundWidgetClass.LoadSynchronous();
	if (!LoadedBackgroundClass)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to load BackgroundWidgetClass: %s"), *BackgroundWidgetClass.ToString());
		return;
	}

	// 【使用加载后的类创建 Widget】
	BackgroundWidget = CreateWidget<UVNBackgroundWidget>(PC, LoadedBackgroundClass);
	if (!BackgroundWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to create background widget"));
		return;
	}

	static const FGameplayTag TAG_UI_Layer_Background = FGameplayTag::RequestGameplayTag(FName("UI.Layer.Background"));
	RootLayout->AddNativeWidgetToLayer(TAG_UI_Layer_Background, BackgroundWidget);
	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Background widget created and added to Background layer"));
}


void UVNUIManagerSubsystem::ShowDialogue(APlayerController* PlayerController)
{
	if (DialogueWidget)
	{
		return; // 如果已存在，直接返回
	}
	if (!RootLayout)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::ShowDialogue: Root layout not created yet"));
		return;
	}
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::ShowDialogue: PlayerController is null"));
		return;
	}

	if (DialogueWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: DialogueWidgetClass was not set in Initialize! Check C++ paths."));
		return;
	}

	// 【添加 LoadSynchronous】
	TSubclassOf<UVNDialogueWidget> LoadedDialogueClass = DialogueWidgetClass.LoadSynchronous();
	if (!LoadedDialogueClass)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to load DialogueWidgetClass: %s"), *DialogueWidgetClass.ToString());
		return;
	}

	// 【使用加载后的类创建 Widget】
	DialogueWidget = CreateWidget<UVNDialogueWidget>(PlayerController, LoadedDialogueClass);
	if (!DialogueWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to create dialogue widget"));
		return;
	}

	RootLayout->PushWidgetToLayerStack(TAG_UI_Layer_Dialogue, DialogueWidget);
	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Dialogue widget created and pushed to Dialogue layer"));
}


void UVNUIManagerSubsystem::ShowMainMenu(APlayerController* PlayerController)
{
	if (!RootLayout)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::ShowMainMenu: Root layout not created yet"));
		return;
	}
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem::ShowMainMenu: PlayerController is null"));
		return;
	}

	if (MainMenuWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: MainMenuWidgetClass was not set in Initialize! Check C++ paths."));
		return;
	}

	// 【添加 LoadSynchronous】
	TSubclassOf<UCommonActivatableWidget> LoadedMenuClass = MainMenuWidgetClass.LoadSynchronous();
	if (!LoadedMenuClass)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to load MainMenuWidgetClass: %s"), *MainMenuWidgetClass.ToString());
		return;
	}

	// 【使用加载后的类创建 Widget】
	UCommonActivatableWidget* MainMenuWidget = CreateWidget<UCommonActivatableWidget>(PlayerController, LoadedMenuClass);
	if (!MainMenuWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("VNUIManagerSubsystem: Failed to create main menu widget"));
		return;
	}

	RootLayout->PushWidgetToLayerStack(TAG_UI_Layer_Menu, MainMenuWidget);
	UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Main menu created and pushed to Menu layer"));
}


void UVNUIManagerSubsystem::HideDialogue()
{
    
	if (DialogueWidget)
	{
		DialogueWidget->DeactivateWidget();
		DialogueWidget = nullptr; 
		UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Dialogue widget deactivated and cleared."));
	}
}