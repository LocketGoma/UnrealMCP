// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FileUtilities/ZipArchiveReader.h"

#if WITH_EDITOR

#include "Containers/Map.h"
#include "FileUtilities/MCPZipFileMetaData.h"

/** Uses the engine reader for file contents and a separate ZIP directory read for timestamps. */
class FILESANDBOXCORE_API FMCPZipArchiveReader final : public FZipArchiveReader
{
public:
	/** Takes ownership of InFileHandle. InArchivePath must identify the same physical ZIP file. */
	FMCPZipArchiveReader(IFileHandle* InFileHandle, const FString& InArchivePath, FOutputDevice* ErrorHandler = nullptr);

	using FZipArchiveReader::TryReadFile;

	bool TryReadFile(FStringView FileName, TArray<uint8>& OutData, FOutputDevice* ErrorHandler,
		UE::FileUtilities::FMCPZipFileMetaData* OutMetadata) const;

private:
	bool LoadMetadata(const FString& ArchivePath, FOutputDevice* ErrorHandler);

	TMap<FString, UE::FileUtilities::FMCPZipFileMetaData> FileMetadata;
	bool bMetadataLoaded = false;
};

#endif
