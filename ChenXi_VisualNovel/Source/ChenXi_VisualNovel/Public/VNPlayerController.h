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

	// 初始化对话框 UI（供 GameMode 调用）
	void InitializeUI();

protected:
	// // Called to bind functionality to input
	// virtual void SetupInputComponent() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override; // <-- 这个声明是合法的

	
	/** 切换自动模式 */
	UFUNCTION(BlueprintCallable, Category="VN|Input")
	void ToggleAutoMode();

	/** 切换快进模式 */
	UFUNCTION(BlueprintCallable, Category="VN|Input")
	void ToggleFastForwardMode();

	/** 当打字机完成时被调用 (用于启动自动前进计时器) */
	UFUNCTION() // UFUNCTION() 宏是绑定委托所必需的
	void OnTypewriterFinished();
	
	/** 计时器回调函数, 真正执行前进的地方 */
	void TriggerAutoAdvance();

	/** 计时器回调函数，真正执行快进的地方 */
	void TriggerFastForward();

	/**
	 * 检查自动模式是否已开启 (BlueprintPure)
	 * 蓝图将调用此函数来检查 bIsAutoMode 的值，以便决定显示哪张图片。
	 * BlueprintPure (纯函数) 会显示为绿色节点，没有执行引脚。
	 */
	UFUNCTION(BlueprintPure, Category = "VN|Input")
	bool IsAutoModeEnabled() const { return bIsAutoMode; }

	/**
	 *	检查快进模式是否已开启（Blueprint）
	 * @return 快进模式是否已开启
	 */
	UFUNCTION(BlueprintPure, Category = "VN|Input")
	bool IsFastForwardModeEnabled() const { return bIsFastForwardMode; }

private:
	// // 玩家输入处理函数：推进对话 (绑定到鼠标左键/确认键)
	// void AdvanceDialogue();

	// **新增：** 用于 VN 输入的输入映射上下文 (IMC)
	UPROPERTY(EditDefaultsOnly, Category = "VN|Input")
	class UInputMappingContext* DefaultInputMapping;
	// **新增：** 用于推进对话的输入动作 (IA)
	UPROPERTY(EditDefaultsOnly, Category = "VN|Input")
	class UInputAction* AdvanceDialogueAction;

	// 绑定到 IA 的函数，处理对话推进
	void AdvanceDialogue(const FInputActionValue& Value);

	/** 用于切换自动模式的输入动作 (在蓝图中设置) */
	UPROPERTY(EditDefaultsOnly, Category = "VN|Input")
	class UInputAction* ToggleAutoModeAction;

	/** 切换快进模式 */
	UPROPERTY(EditDefaultsOnly, Category="VN|Input")
	class UInputAction* ToggleFastForwardAction;

	/** 自动模式开启时, 打字机结束后等待多久再前进 (秒) */
	UPROPERTY(EditDefaultsOnly, Category = "VN|Input")
	float AutoModeDelay = 0.5f;

	/** 快进模式时，每句话之前等待时间（秒） */
	UPROPERTY(EditDefaultsOnly, Category = "VN|Input")
	float FastForwardDelay = 0.05f;

	/** 追踪自动模式是否开启 */
	bool bIsAutoMode = false;

	/** 追踪快进模式是否开启 */
	bool bIsFastForwardMode = false;

	/** 自动前进的计时器句柄 */
	FTimerHandle AutoAdvanceTimerHandle;

	/** 快进模式的计时器句柄 */
	FTimerHandle FastForwardTimerHandle;

	

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