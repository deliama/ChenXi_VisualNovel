// Fill out your copyright notice in the Description page of Project Settings.


#include "VNHistoryWidget.h"

#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UVNHistoryWidget::OnDisplayHistory(const TArray<FDialogLine>& HistoryLines)
{
	HistoryScrollBox->ClearChildren();
	FString historyText = "";
	for (const FDialogLine& Line : HistoryLines)
	{
		if(!Line.CharacterName.IsEmpty())
			historyText += Line.CharacterName+": ";  //TODO:之后可以考虑换颜色
		historyText += Line.DialogueText.ToString();
		historyText += "\n";
	}
	TextHistory->SetText(FText::FromString(historyText));
	TextHistory->SetVisibility(ESlateVisibility::Visible);
}
