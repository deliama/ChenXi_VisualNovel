// Copyright Chen Xi Visual Novel Team. All Rights Reserved.

#include "UI/VNMenuFunctionLibrary.h"
#include "UI/VNUIManagerSubsystem.h"
#include "UI/VNPrimaryGameLayout.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "VNGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VNMenuFunctionLibrary)

// 定义菜单层 GameplayTag
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_Layer_Menu, "UI.Layer.Menu");

void UVNMenuFunctionLibrary::OpenMenu(UObject* WorldContextObject, TSubclassOf<UCommonActivatableWidget> MenuClass)
{
	if (!MenuClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNMenuFunctionLibrary::OpenMenu - MenuClass is null"));
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::OpenMenu - Failed to get World"));
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::OpenMenu - Failed to get PlayerController"));
		return;
	}

	// 创建菜单实例
	UCommonActivatableWidget* MenuWidget = CreateWidget<UCommonActivatableWidget>(PC, MenuClass);
	if (!MenuWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::OpenMenu - Failed to create menu widget"));
		return;
	}

	// 推入Menu Stack
	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
		{
			if (UVNPrimaryGameLayout* RootLayout = UIManager->GetRootLayout())
			{
				RootLayout->PushWidgetToLayerStack(TAG_UI_Layer_Menu, MenuWidget);
				UE_LOG(LogTemp, Log, TEXT("VNMenuFunctionLibrary::OpenMenu - Opened menu: %s"), *MenuClass->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::OpenMenu - RootLayout not found"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::OpenMenu - UIManager not found"));
		}
	}
}

void UVNMenuFunctionLibrary::CloseMenu(UCommonActivatableWidget* MenuWidget, bool bWithAnimation, float AnimationDuration)
{
	if (!MenuWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNMenuFunctionLibrary::CloseMenu - MenuWidget is null"));
		return;
	}

	if (bWithAnimation && AnimationDuration > 0.0f)
	{
		// 延迟关闭,等待动画播放
		FTimerHandle TimerHandle;
		MenuWidget->GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[MenuWidget]()
			{
				if (MenuWidget)
				{
					MenuWidget->DeactivateWidget();
					UE_LOG(LogTemp, Log, TEXT("VNMenuFunctionLibrary::CloseMenu - Menu closed after animation: %s"), *MenuWidget->GetName());
				}
			},
			AnimationDuration,
			false
		);
	}
	else
	{
		// 立即关闭
		MenuWidget->DeactivateWidget();
		UE_LOG(LogTemp, Log, TEXT("VNMenuFunctionLibrary::CloseMenu - Menu closed immediately: %s"), *MenuWidget->GetName());
	}
}

void UVNMenuFunctionLibrary::StartNewGame(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::StartNewGame - Failed to get World"));
		return;
	}

	// 1. 关闭所有菜单（清空 Menu Stack）
	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		if (UVNUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UVNUIManagerSubsystem>())
		{
			if (UVNPrimaryGameLayout* RootLayout = UIManager->GetRootLayout())
			{
				// 获取 Menu 层的 Stack 并清空所有 Widget
				if (UCommonActivatableWidgetStack* MenuStack = RootLayout->FindOrCreateLayerStack(TAG_UI_Layer_Menu))
				{
					MenuStack->ClearWidgets();
					UE_LOG(LogTemp, Log, TEXT("VNMenuFunctionLibrary::StartNewGame - Cleared menu stack"));
				}
			}
		}
	}

	// 2. 调用 GameMode 的 StartNewGame
	if (AVNGameMode* GameMode = Cast<AVNGameMode>(World->GetAuthGameMode()))
	{
		GameMode->StartNewGame();
		UE_LOG(LogTemp, Log, TEXT("VNMenuFunctionLibrary::StartNewGame - New game started"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::StartNewGame - Failed to get VNGameMode"));
	}
}

void UVNMenuFunctionLibrary::ReturnToMainMenu(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::ReturnToMainMenu - Failed to get World"));
		return;
	}

	// 调用 GameMode 的 ReturnToMainMenu
	if (AVNGameMode* GameMode = Cast<AVNGameMode>(World->GetAuthGameMode()))
	{
		GameMode->ReturnToMainMenu();
		UE_LOG(LogTemp, Log, TEXT("VNMenuFunctionLibrary::ReturnToMainMenu - Returned to main menu"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::ReturnToMainMenu - Failed to get VNGameMode"));
	}
}

void UVNMenuFunctionLibrary::QuitGame(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::QuitGame - Failed to get World"));
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("VNMenuFunctionLibrary::QuitGame - Failed to get PlayerController"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("VNMenuFunctionLibrary::QuitGame - Quitting game"));
	UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, false);
}
