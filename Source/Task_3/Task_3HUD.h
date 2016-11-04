// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "Task_3HUD.generated.h"

UCLASS()
class ATask_3HUD : public AHUD
{
	GENERATED_BODY()

public:
	ATask_3HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

