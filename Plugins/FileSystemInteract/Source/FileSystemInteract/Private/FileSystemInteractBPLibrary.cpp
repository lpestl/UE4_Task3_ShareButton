// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "FileSystemInteractPrivatePCH.h"
#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapperModule.h"
#include "FileManagerGeneric.h"
#include "FileSystemInteractBPLibrary.h"

UFileSystemInteractBPLibrary::UFileSystemInteractBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float UFileSystemInteractBPLibrary::FileSystemInteractSampleFunction(float Param)
{
	return -1;
}

FString UFileSystemInteractBPLibrary::GetSaveDirPath() {
	return FPaths::GameSavedDir();
}

FString UFileSystemInteractBPLibrary::GetScreenshotDirPath() {
	FString path = FPaths::GameSavedDir();
#if WITH_EDITOR
	path.Append(TEXT("Screenshots/Windows/"));
#else
	path.Append(TEXT("Screenshots/WindowsNoEditor/"));
#endif
	return path;
}

TArray<FString> UFileSystemInteractBPLibrary::GetFilesInFolder(FString Directory) {
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

UTexture2D* UFileSystemInteractBPLibrary::LoadTexture2D_FromFile(const FString& FullFilePath, int typeFormatEnum, bool& IsValid, int32& Width, int32& Height) {
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