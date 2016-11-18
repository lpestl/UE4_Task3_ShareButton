// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "FileSystemInteractBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class UFileSystemInteractBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "FileSystemInteract sample test testing"), Category = "FileSystemInteractTesting")
	static float FileSystemInteractSampleFunction(float Param);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Save Dir Path", Keywords = "FileSystemInteract get save dir path test testing"), Category = "FileSystemInteractTesting")
	static FString GetSaveDirPath();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Screenshot Dir Path", Keywords = "FileSystemInteract get screenshot dir path test testing"), Category = "FileSystemInteractTesting")
	static FString GetScreenshotDirPath();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Files Array In Dir", Keywords = "FileSystemInteract get files array in dir path folder test testing"), Category = "FileSystemInteractTesting")
	static TArray<FString> GetFilesInFolder(FString Directory);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Load Texture 2d From File", Keywords = "FileSystemInteract load texture 2d from file path folder test testing"), Category = "FileSystemInteractTesting")
	static UTexture2D* LoadTexture2D_FromFile(const FString& FullFilePath, int typeFormatEnum/* 0 = PNG, 1 = JPEG, 2 = ..., 3 = BMP, 4 = ICO, 5 = EXR, 6 = ICNS */, bool& IsValid, int32& Width, int32& Height);
};
