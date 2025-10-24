// Fill out your copyright notice in the Description page of Project Settings.


#include "UVNSaveGame.h"

UVNSaveGame::UVNSaveGame()
{
	// 初始化默认值
	SavedDialogIndex = 0;
	SavedBGMTrack = nullptr;
	SavedBackgroundImage = nullptr;
	bSavedGameStarted = false;
	SlotName = "DefaultSave";
}
