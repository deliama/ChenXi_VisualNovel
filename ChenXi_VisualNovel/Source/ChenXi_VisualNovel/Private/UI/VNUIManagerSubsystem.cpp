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
	UE_LOG(LogTemp, Warning, TEXT("<<<<< UVNUIManagerSubsystem::Initialize HAS BEEN CALLED for class: %s >>>>>"), *GetClass()->GetName());
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

	// 不要为专用服务器创建
	if (CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		return false;
	}

	// 【核心修复】
	// 我们必须移除所有对 "CLASS_Abstract" 的检查。
	// 引擎会自动处理抽象类，我们不需要（也不应该）在这里检查它。
	// 我们唯一要做的就是防止它在服务器上运行。
	
	return true;
	

	// // Don't create for dedicated servers
	// if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	// {
	// 	TArray<UClass*> ChildClasses;
	// 	GetDerivedClasses(GetClass(), ChildClasses, false);
	//
	// 	if (GetClass()->HasAnyClassFlags(CLASS_Abstract))
	// 	{
	// 		return false;
	// 	}
	// 	
	// 	return ChildClasses.Num() == 0;
	// }
	//
	// return false;
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

	// --- [ 修正点 1 ] ---
	// No layout class configured (使用 !RootLayoutClass 检查 TSubclassOf)
	if (!RootLayoutClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: No RootLayoutClass configured. Please create BP_VNUIManagerSubsystem blueprint and set Root Layout Class in Class Defaults."));
		return;
	}

	// --- [ 修正点 2 ] ---
	// TSubclassOf 可以直接传给 CreateWidget，不再需要 LoadSynchronous
	RootLayout = CreateWidget<UVNPrimaryGameLayout>(PlayerController, RootLayoutClass);
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

	// --- [ 修正点 3 ] ---
	// (使用 !BackgroundWidgetClass 检查 TSubclassOf，并移除 LoadSynchronous)
	if (!BackgroundWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: No BackgroundWidgetClass configured. Please create BP_VNUIManagerSubsystem blueprint and set Background Widget Class in Class Defaults."));
		return;
	}

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

	// (直接使用 BackgroundWidgetClass)
	BackgroundWidget = CreateWidget<UVNBackgroundWidget>(PC, BackgroundWidgetClass);
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
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: Dialogue widget already exists"));
		return;
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

	// --- [ 修正点 4 ] ---
	// (使用 !DialogueWidgetClass 检查 TSubclassOf，并移除 LoadSynchronous)
	if (!DialogueWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: No DialogueWidgetClass configured. Please set Dialogue Widget Class in BP_VNUIManagerSubsystem Class Defaults."));
		return;
	}

	// (直接使用 DialogueWidgetClass)
	DialogueWidget = CreateWidget<UVNDialogueWidget>(PlayerController, DialogueWidgetClass);
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

	// --- [ 修正点 5 ] ---
	// (使用 !MainMenuWidgetClass 检查 TSubclassOf，并移除 LoadSynchronous)
	if (!MainMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNUIManagerSubsystem: No MainMenuWidgetClass configured. Please set Main Menu Widget Class in BP_VNUIManagerSubsystem Class Defaults."));
		return;
	}

	// (直接使用 MainMenuWidgetClass)
	UCommonActivatableWidget* MainMenuWidget = CreateWidget<UCommonActivatableWidget>(PlayerController, MainMenuWidgetClass);
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
		// DeactivateWidget 会自动将其从 CommonUI 堆栈中移除
		DialogueWidget->DeactivateWidget();
		
		// 清除我们的 TObjectPtr 引用，这样下次 ShowDialogue 就会重新创建它
		DialogueWidget = nullptr; 
		
		UE_LOG(LogTemp, Log, TEXT("VNUIManagerSubsystem: Dialogue widget deactivated and cleared."));
	}
}