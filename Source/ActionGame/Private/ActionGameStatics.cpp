// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameStatics.h"

#include "ActionGameTypes.h"

const UItemStaticData* UActionGameStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
	if (IsValid(ItemDataClass))
	{
		return GetDefault<UItemStaticData>(ItemDataClass);
	}
	
	return nullptr;
}
