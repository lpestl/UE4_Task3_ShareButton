// Fill out your copyright notice in the Description page of Project Settings.

#include "Task_3.h"
#include "FileSystemInteract.h"
#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapperModule.h"
//#include "DiffFilesCommandlet.generated.h"
#include "FileManagerGeneric.h"


// Sets default values
AFileSystemInteract::AFileSystemInteract()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	savedGameDir = FPaths::GameSavedDir();
#if WITH_EDITOR
	screenshotDir = "Screenshots/Windows/";
#else
	screenshotDir = "Screenshots/WindowsNoEditor/";
#endif
}

// Called when the game starts or when spawned
void AFileSystemInteract::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFileSystemInteract::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

TArray<FString> AFileSystemInteract::GetFilesInFolder(FString Directory)
{
	TArray<FString> output;
	output.Empty();
	if (FPaths::DirectoryExists(Directory))
	{
		FString path = Directory + "*.png";
		//IFileManager::Get().fi
		FFileManagerGeneric::Get().FindFiles(output, *path, true, false);
		for (int i = 0; i < output.Num(); i++)
		{
			output[i] = Directory + output[i];
			//print(output[i]);
		}
	}
	return output;
}

FString AFileSystemInteract::GetPathSavesDir() {
	return FPaths::GameSavedDir();
}

void AFileSystemInteract::PickeUpShot(FString path) {

}

/*static EImageFormat::Type AFileSystemInteract::GetJoyImageFormat(EFileTypesCode ftype) {
	return EImageFormat::PNG;
}*/

UTexture2D* AFileSystemInteract::Victory_LoadTexture2D_FromFile(const FString& FullFilePath, int typeFormatEnum, bool& IsValid, int32& Width, int32& Height)
{
	IsValid = false;
	UTexture2D* LoadedT2D = NULL;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	
	EImageFormat::Type type;
	switch (typeFormatEnum)
	{
		case 1: type = EImageFormat::Type::JPEG; break;
		case 0: type = EImageFormat::Type::PNG; break;
		case 3: type = EImageFormat::Type::BMP; break;
		case 4: type = EImageFormat::Type::ICO; break;
		case 5: type = EImageFormat::Type::EXR; break;
		case 6: type = EImageFormat::Type::ICNS; break;
	}
	IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(type);

	//Load From File
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath))
	{
		return NULL;
	}


	//Create T2D!
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		const TArray<uint8>* UncompressedBGRA = NULL;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			//Valid?
			if (!LoadedT2D)
			{
				return NULL;
			}

			//Out!
			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			//Copy!
			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			//Update!
			LoadedT2D->UpdateResource();
		}
	}

	// Success!
	IsValid = true;
	return LoadedT2D;
}