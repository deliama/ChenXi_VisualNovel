// VNPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
// 引入 Enhanced Input 核心头文件
#include "InputMappingContext.h"
#include "InputAction.h"

class USoundMix;
class USoundClass;
class USoundBase;

#include "VNPlayerController.generated.h"

/**
 * Visual Novel Player Controller: 负责处理玩家输入，推进对话
 */
UCLASS()
class CHENXI_VISUALNOVEL_API AVNPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AVNPlayerController();

	// 新增：供蓝图调用的音量设置函数
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetBGMVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetSFXVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetUIVolume(float Volume);
    
protected:
	// // Called to bind functionality to input
	// virtual void SetupInputComponent() override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override; // <-- 这个声明是合法的

private:
	// // 玩家输入处理函数：推进对话 (绑定到鼠标左键/确认键)
	// void AdvanceDialogue();
	// 新增函数：在游戏开始时创建并显示 UI
	void InitializeUI();

	// 供蓝图设置的属性：指定要创建的对话框 Widget 蓝图类
	UPROPERTY(EditDefaultsOnly, Category = "VN|UI")
	TSubclassOf<class UVNDialogueWidget> DialogueWidgetClass;

	// 存储创建后的对话框 Widget 实例指针
	UPROPERTY()
	class UVNDialogueWidget* DialogueWidgetInstance;

	// **新增：** 用于 VN 输入的输入映射上下文 (IMC)
	UPROPERTY(EditDefaultsOnly, Category = "VN|Input")
	class UInputMappingContext* DefaultInputMapping;
	// **新增：** 用于推进对话的输入动作 (IA)
	UPROPERTY(EditDefaultsOnly, Category = "VN|Input")
	class UInputAction* AdvanceDialogueAction;

	// 绑定到 IA 的函数，处理对话推进
	void AdvanceDialogue(const FInputActionValue& Value);


	// 新增：用于在蓝图中设置我们的SoundMix和SoundClass资产
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundMix* VolumeControlSoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundClass* BGM_SoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundClass* SFX_SoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundClass* UI_SoundClass;
    
	
};