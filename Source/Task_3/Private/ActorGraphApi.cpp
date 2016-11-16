// Fill out your copyright notice in the Description page of Project Settings.

#include "Task_3.h"
//#include "Public/ActorGraphApi.h"
#include "ActorGraphApi.h"


// Sets default values
AActorGraphApi::AActorGraphApi()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//When the object is constructed, Get the HTTP module
	Http = &FHttpModule::Get();

	responseJson.Empty();
	uploadFilePath.Empty();
	access_token.Empty();
	uploadSession.Empty();
	hUpload.Empty();

	fileSize = 0;
	file_offset = 0;

	idCurrPhoto.Empty();
	urlPhoto.Empty();

	bWhaitUpload = false;
	firstGetIds = true;
}

// Called when the game starts or when spawned
void AActorGraphApi::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorGraphApi::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AActorGraphApi::CreatingUploadSession(FString token, FString file_path) {
	uploadFilePath = FPaths::GameSavedDir();
	uploadFilePath.Append(TEXT("up.png"));

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*uploadFilePath))
	{
		//Output it to the engine
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, TEXT("Could Not Find File"));
		return;
	}
	fileSize = FPlatformFileManager::Get().GetPlatformFile().FileSize(*uploadFilePath);
	//UE_LOG(LogTemp, Warning, TEXT("Size with out HANFLE:\r\n%d"), fileSize);
	access_token = token;

	//This is the url on which to process the request
	FString uri = TEXT("https://graph.facebook.com/v2.8/app/uploads?");

	uri.Append(TEXT("access_token="));
	uri.Append(token);

	uri.Append(TEXT("&file_length="));
	uri.Append(FString::FromInt(fileSize));

	uri.Append(TEXT("&file_type="));
	uri.Append(TEXT("image/png"));

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AActorGraphApi::OnResponseUploadSession);

	Request->SetURL(uri);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));
	Request->ProcessRequest();
}

void AActorGraphApi::OnResponseUploadSession(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;
	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		//Get the value of the json object by field name
		uploadSession = JsonObject->GetStringField("id");

		if (!uploadSession.IsValidIndex(0)) {
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Error] in respone not found Upload Session ID"));
			return;
		}
		//Output it to the engine
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, uploadSession);
		UE_LOG(LogTemp, Warning, TEXT("id upload session:\r\n%s"), *uploadSession);
		UploadingFileData();
	}
}

void AActorGraphApi::UploadingFileData() {
	//This is the url on which to process the request
	FString uri = TEXT("https://graph.facebook.com/v2.8/");
	uri.Append(uploadSession);

	/*uri.Append(TEXT("&file_length="));
	uri.Append(FString::FromInt(fileSize));

	uri.Append(TEXT("&file_type="));
	uri.Append(TEXT("image/png"));*/

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AActorGraphApi::OnResponseDoneUpload);

	Request->SetURL(uri);
	Request->SetVerb("POST");

	Request->SetHeader(TEXT("file_offset"), TEXT("0"));

	FString auth = TEXT("OAuth ");
	auth.Append(access_token);
	Request->AppendToHeader(TEXT("Authorization"), auth);

	Request->AppendToHeader(TEXT("Host"), TEXT("graph.facebook.com"));
	Request->AppendToHeader(TEXT("Connection"), TEXT("close"));

	// get data
	TArray<uint8> UpFileRawData;
	FFileHelper::LoadFileToArray(UpFileRawData, *uploadFilePath);

	FString Boundary = "---------------------------" + FString::FromInt(FDateTime::Now().GetTicks());
	Request->AppendToHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary =" + Boundary));
	// Format the file the way OneSky wants it (in a form, with the file contents being a field called "file")
	FString PrefixBoundry = "\r\n--" + Boundary + "\r\n";
	FString FileHeader = "Content-Disposition: form-data; name=\"file\"; \r\nContent-Type: image/png\r\n\r\n";
	FString SuffixBoundary = "\r\n--" + Boundary + "--\r\n";
	FString ContentsString = PrefixBoundry + FileHeader + FBase64::Encode(UpFileRawData) + SuffixBoundary;

	FTCHARToUTF8 EchoStrUtf8(*ContentsString);
	int32 DestLen = EchoStrUtf8.Length();

	//UE_LOG(LogTemp, Warning, TEXT("Size with HANFLE:\r\n%d"), DestLen);

	Request->SetContentAsString(ContentsString);
	Request->ProcessRequest();
}

void AActorGraphApi::OnResponseDoneUpload(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;
	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		//Get the value of the json object by field name
		hUpload = JsonObject->GetStringField("h");

		if (!hUpload.IsValidIndex(0)) {
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Error] in respone Upload File HANDLE"));
			return;
		}
		//Output it to the engine
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, hUpload);
		UE_LOG(LogTemp, Warning, TEXT("HANDLE uploaded file:\r\n%s"), *hUpload);
		//ExampleFeedUploadPhoto();
	}
}

void AActorGraphApi::GetUploadStatus(FString token, FString file_path) {

}

void AActorGraphApi::OnResponseUploadStatus(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {

}

void AActorGraphApi::ExampleFeedUploadPhoto() {
	//This is the url on which to process the request
	FString uri = TEXT("https://graph.facebook.com/v2.8/me/feed?");
	//uri.Append(myId);
	//uri.Append("/feed?");

	uri.Append(TEXT("access_token="));
	uri.Append(access_token);

	IFileManager& FileManager = IFileManager::Get();
	FString DiskFilename = FileManager.GetFilenameOnDisk(*uploadFilePath);

	uri.Append(TEXT("&thumbnail=@"));
	uri.Append(DiskFilename);

	//uri.Append(TEXT("&link=https://www.google.com/search?q=image"));

	//uri.Append(TEXT("&id="));
	//uri.Append(myId);
	//uri.Append(TEXT("url=https://graph.facebook.com/v2.8/"));
	//uri.Append(uploadSession);

	//uri.Append(TEXT("&caption=Wow!!!"));

	//uri.Append(TEXT("&type="));
	//uri.Append(TEXT("image/png"));

	/*uri.Append(TEXT("&access_token="));
	uri.Append(access_token);*/

	system("curl");

	system("pause");
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AActorGraphApi::OnResponseFeed);

	FString auth = TEXT("OAuth ");
	auth.Append(access_token);
	Request->SetHeader(TEXT("Authorization"), auth);

	Request->AppendToHeader(TEXT("type"), TEXT("image/png"));

	Request->SetURL(uri);
	Request->SetVerb("POST");
	//Request->ProcessRequest();
}

void AActorGraphApi::OnResponseFeed(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;
	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Output it to the engine
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, Response->GetContentAsString());
	UE_LOG(LogTemp, Warning, TEXT("Getting json:\r\n%s"), *Response->GetContentAsString());
}

void AActorGraphApi::CrutchWithCurl(FString token, FString filePath, FString caption) {
	
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*filePath))
	{
		//Output it to the engine
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, TEXT("Could Not Find File"));
		return;
	}
	IFileManager& FileManager = IFileManager::Get();
	FString DiskFilename = FileManager.GetFilenameOnDisk(*filePath);
	//FString command = TEXT("pause");
	FString command = TEXT("curl -v -X POST \\ -F \"access_token=");
	command.Append(token);
	if (!caption.IsEmpty()) {
		command.Append(TEXT("\" \\ -F \"caption="));
		command.Append(caption);
	}	
	command.Append(TEXT("\" \\ -F \"source=@"));
	command.Append(filePath);
	command.Append(TEXT("\" \\ \"https://graph.facebook.com/v2.8/me/photos\" > curl.log"));

	WinExec(TCHAR_TO_ANSI(*command), SW_HIDE);
	//system(TCHAR_TO_ANSI(*command));
	UE_LOG(LogTemp, Warning, TEXT("System command:\r\n%s"), *command);
}

void AActorGraphApi::GetAllPhoto(FString token) {
	//This is the url on which to process the request
	FString uri = TEXT("https://graph.facebook.com/v2.8/");
	uri.Append(TEXT("me/photos?access_token="));
	uri.Append(token);
	access_token = token;

	uri.Append(TEXT("&type=uploaded&fields=id,source"));

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AActorGraphApi::OnResponsePhoto);

	Request->SetURL(uri);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void AActorGraphApi::OnResponsePhoto(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;
	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Output it to the engine
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, Response->GetContentAsString());
	//UE_LOG(LogTemp, Warning, TEXT("Getting json:\r\n%s"), *Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		//Get the value of the json object by field name
		TArray<TSharedPtr<FJsonValue>> arr = JsonObject->GetArrayField("data");
		if (firstGetIds) {
			firstGetIds = false;
			for (int i = 0; i < arr.Num(); i++) {
				TSharedPtr<FJsonObject> temp = arr[i]->AsObject();
				photosId.Add(temp->GetStringField("id"));
				//UE_LOG(LogTemp, Log, TEXT("idPhoto: %s"), *temp->GetStringField("id"));
			}
			bWhaitUpload = true;
		}
		else {
			for (int i = 0; i < arr.Num(); i++) {
				TSharedPtr<FJsonObject> temp = arr[i]->AsObject();
				if (photosId.Find(temp->GetStringField("id")) == INDEX_NONE) {
					idCurrPhoto = temp->GetStringField("id");
					urlPhoto = temp->GetStringField("source");
					UE_LOG(LogTemp, Warning, TEXT("ID New photo: %s"), *idCurrPhoto);
					UE_LOG(LogTemp, Warning, TEXT("URL New photo: %s"), *urlPhoto);
					bWhaitUpload = false;
				}
			}
		}
		
		if (bWhaitUpload) GetAllPhoto(access_token);
	}
}