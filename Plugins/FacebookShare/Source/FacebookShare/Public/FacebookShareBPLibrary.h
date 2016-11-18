// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Core/Public/Misc/Base64.h"
#include <Windows.h>
//#include <curl/curl.h>
#include "FacebookShareBPLibrary.generated.h"

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
class UFacebookShareBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "FacebookShare sample test testing"), Category = "FacebookShareTesting")
	static float FacebookShareSampleFunction(float Param);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Uri For Login", Keywords = "FacebookShare login get url plugin test testing"), Category = "FacebookShareTesting")
	static FString GetUrlForLogin(FString app_id, FString redirect_uri, FString state, FString response_type, FString scope);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Access Token", Keywords = "FacebookShare parse response get access token url plugin test testing"), Category = "FacebookShareTesting")
	static bool GetAccessToken(FString response_uri, FString &access_token_out);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Upload Photo On Facebook", Keywords = "FacebookShare upload photo plugin test testing"), Category = "FacebookShareTesting")
	static void CrutchWithCurl(FString token, FString filePath, FString caption);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Uri For HTTPRequest All Photo", Keywords = "FacebookShare get all uri http photo plugin test testing"), Category = "FacebookShareTesting")
	static FString GetUriForHttpAllPhoto(FString token);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Parse Json Response Get All Photo", Keywords = "FacebookShare get all uri Response json photo plugin test testing"), Category = "FacebookShareTesting")
	static void ParseResponseGetAllPhoto(FString responseJson, TArray<FString> &all_id_photos_out, TArray<FString> &url_photos);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Uri For Feed", Keywords = "FacebookShare feed get url photo plugin test testing"), Category = "FacebookShareTesting")
	static FString GetUrlForFeed(FString app_id, FString redirect_uri, FString picture, FString caption);
};
