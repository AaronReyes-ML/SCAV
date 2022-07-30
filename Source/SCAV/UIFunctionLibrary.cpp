// Fill out your copyright notice in the Description page of Project Settings.


#include "UIFunctionLibrary.h"
#include "Framework/Application/SlateApplication.h"

void UUIFunctionLibrary::RefreshSlateSettings() {

	FSlateApplication& abc = FSlateApplication::Get();
	abc.SetDragTriggerDistance(.5);
}