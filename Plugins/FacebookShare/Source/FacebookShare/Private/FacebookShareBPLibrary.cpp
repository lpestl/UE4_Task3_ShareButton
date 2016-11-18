// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "FacebookSharePrivatePCH.h"
#include "FacebookShareBPLibrary.h"

UFacebookShareBPLibrary::UFacebookShareBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

float UFacebookShareBPLibrary::FacebookShareSampleFunction(float Param)
{
	return -1;
}

FString UFacebookShareBPLibrary::GetUrlForLogin(FString app_id, FString redirect_uri, FString state, FString response_type, FString scope) {
	FString url = TEXT("https://www.facebook.com/v2.8/dialog/oauth?");

	if (app_id.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("[ERROR] App_ID must be filled"));
	}
	else {
		url.Append(TEXT("client_id="));
		url.Append(app_id);
	}

	if (redirect_uri.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("[Warning] redirect_uri must be filled"));
		UE_LOG(LogTemp, Warning, TEXT("[Warning] redirect_uri selected by default \" https://www.facebook.com/connect/login_success.html \""));
		url.Append(TEXT("&redirect_uri="));
		url.Append(TEXT("https://www.facebook.com/connect/login_success.html"));
	}
	else {
		url.Append(TEXT("&redirect_uri="));
		url.Append(redirect_uri);
	}

	if (!state.IsEmpty()) {
		url.Append(TEXT("&state="));
		url.Append(state);
	}

	if (response_type.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("[Warning] response_type selected by default \"token\""));
		url.Append(TEXT("&response_type="));
		url.Append(response_type);
	}
	else {
		if (response_type.Equals("token")) {
			url.Append(TEXT("&response_type="));
			url.Append(response_type);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("[Warning] In the current version of the plugin, response_type only supported type \"token\""));
			UE_LOG(LogTemp, Warning, TEXT("[Warning] response_type selected by default \"token\""));
			url.Append(TEXT("&response_type="));
			url.Append(TEXT("token"));
		}
	}

	if (!scope.IsEmpty()) {
		url.Append(TEXT("&scope="));
		url.Append(scope);
	}

	return url;
}

bool UFacebookShareBPLibrary::GetAccessToken(FString response_uri, FString &access_token_out) {
	bool completed = false;

	/*if (response_uri.Find(TEXT("error_reason=")) &&
		response_uri.Find(TEXT("error=")) &&
		response_uri.Find(TEXT("error_description="))) {
		
		FString error_reason;
		FString error_code;
		FString error_description;		
		
		if (response_uri.Split(TEXT("error_reason="), NULL, &error_reason)) {
			error_reason.Split(TEXT("&"), &error_reason, NULL);
			error = TEXT("Response error: \r\nerror_reason=");
			error.Append(error_reason);
		}

		if (response_uri.Split(TEXT("error="), NULL, &error_code)) {
			error_code.Split(TEXT("&"), &error_code, NULL);
			error.Append(TEXT("\r\nerror="));
			error.Append(error_code);
		}

		if (response_uri.Split(TEXT("error_description="), NULL, &error_description)) {
			error_description.Split(TEXT("&"), &error_description, NULL);
			error.Append(TEXT("\r\nerror_description="));
			error.Append(error_description);
		}

		UE_LOG(LogTemp, Error, TEXT("[ERROR] %s"), *error);
		access_token_out.Empty();
		completed = true;
	}*/

	if (response_uri.Find("access_token")) {
		if (response_uri.Split(TEXT("access_token="), NULL, &access_token_out)) {
			access_token_out.Split(TEXT("&"), &access_token_out, NULL);
			access_token_out.Split(TEXT("_#_"), &access_token_out, NULL);
			completed = true;
		}
	}

	return completed;
}

void UFacebookShareBPLibrary::CrutchWithCurl(FString token, FString filePath, FString caption) {
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*filePath))
	{
		//Output it to the engine
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, TEXT("Could Not Find File"));
		return;
	}
	IFileManager& FileManager = IFileManager::Get();
	FString DiskFilename = FileManager.GetFilenameOnDisk(*filePath);

	FString command = TEXT("curl -v -X POST \\ -F \"access_token=");
	command.Append(token);
	if (!caption.IsEmpty()) {
		command.Append(TEXT("\" \\ -F \"caption="));
		command.Append(caption);
	}
	command.Append(TEXT("\" \\ -F \"source=@"));
	command.Append(filePath);
	command.Append(TEXT("\" \\ \"https://graph.facebook.com/v2.8/me/photos\" >> curl.log"));

	WinExec(TCHAR_TO_ANSI(*command), SW_HIDE);
	//system(TCHAR_TO_ANSI(*command));
	UE_LOG(LogTemp, Warning, TEXT("System command:\r\n%s"), *command);
}

FString UFacebookShareBPLibrary::GetUriForHttpAllPhoto(FString token) {
	//This is the url on which to process the request
	FString uri = TEXT("https://graph.facebook.com/v2.8/");
	uri.Append(TEXT("me/photos?access_token="));
	uri.Append(token);

	uri.Append(TEXT("&type=uploaded&fields=id,source"));

	/*TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AActorGraphApi::OnResponsePhoto);

	Request->SetURL(uri);
	Request->SetVerb("GET");
	Request->ProcessRequest();*/
	return uri;
}

void UFacebookShareBPLibrary::ParseResponseGetAllPhoto(FString responseJson, TArray<FString> &all_id_photos_out, TArray<FString> &url_photos) {
	all_id_photos_out.Empty();
	url_photos.Empty();

	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;
	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(responseJson);

	//Output it to the engine
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, Response->GetContentAsString());
	//UE_LOG(LogTemp, Warning, TEXT("Getting json:\r\n%s"), *Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		//Get the value of the json object by field name
		TArray<TSharedPtr<FJsonValue>> arr = JsonObject->GetArrayField("data");
		for (int i = 0; i < arr.Num(); i++) {
			TSharedPtr<FJsonObject> temp = arr[i]->AsObject();
			all_id_photos_out.Add(temp->GetStringField("id"));
			url_photos.Add(temp->GetStringField("source"));
		}
	}
}

FString UFacebookShareBPLibrary::GetUrlForFeed(FString app_id, FString redirect_uri, FString picture, FString caption) {
	FString uri = TEXT("https://www.facebook.com/dialog/feed?");

	if (app_id.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("[ERROR] App_ID must be filled"));
	}
	else {
		uri.Append(TEXT("app_id="));
		uri.Append(app_id);
	}

	uri.Append(TEXT("&display=page"));

	if (!caption.IsEmpty()) {
		uri.Append(TEXT("&caption="));
		uri.Append(caption.Replace(TEXT(" "), TEXT("%20")));
	}

	if (!picture.IsEmpty()) {
		uri.Append(TEXT("&picture="));
		uri.Append(picture);
	}

	if (redirect_uri.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("[Warning] redirect_uri must be filled"));
	}
	else {
		uri.Append(TEXT("&redirect_uri="));
		uri.Append(redirect_uri);
	}

	return uri;
}