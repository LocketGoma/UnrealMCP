// Copyright Epic Games, Inc. All Rights Reserved.

#include "FileUtilities/MCPZipArchiveReader.h"

#if WITH_EDITOR

#include "Containers/StringConv.h"
#include "Logging/LogMacros.h"
#include "Misc/OutputDevice.h"
#include "Misc/Paths.h"
#include "libzip/zip.h"

DEFINE_LOG_CATEGORY_STATIC(LogMCPZipArchive, Log, All);

namespace
{
void ReportMCPZipError(FOutputDevice* ErrorHandler, const FString& Message)
{
	if (ErrorHandler)
	{
		ErrorHandler->Log(LogMCPZipArchive.GetCategoryName(), ELogVerbosity::Warning, Message);
	}
	else
	{
		UE_LOG(LogMCPZipArchive, Warning, TEXT("%s"), *Message);
	}
}
}

FMCPZipArchiveReader::FMCPZipArchiveReader(
	IFileHandle* InFileHandle, const FString& InArchivePath, FOutputDevice* ErrorHandler)
	: FZipArchiveReader(InFileHandle, ErrorHandler)
{
	if (FZipArchiveReader::IsValid())
	{
		bMetadataLoaded = LoadMetadata(InArchivePath, ErrorHandler);
	}
}

bool FMCPZipArchiveReader::LoadMetadata(const FString& ArchivePath, FOutputDevice* ErrorHandler)
{
	// FZipArchiveReader's ZIP handle is private. Open the directory separately,
	// cache only metadata, and leave decompression to the engine implementation.
	const FString AbsolutePath = FPaths::ConvertRelativePathToFull(ArchivePath);
	zip_error_t Error;
	zip_error_init(&Error);
#if PLATFORM_WINDOWS
	zip_source_t* Source = zip_source_win32w_create(*AbsolutePath, 0, 0, &Error);
#else
	FTCHARToUTF8 Utf8Path(*AbsolutePath);
	zip_source_t* Source = zip_source_file_create(Utf8Path.Get(), 0, 0, &Error);
#endif
	zip_t* Archive = Source ? zip_open_from_source(Source, ZIP_RDONLY, &Error) : nullptr;
	if (!Archive)
	{
		ReportMCPZipError(ErrorHandler, FString::Printf(
			TEXT("Could not open ZIP metadata for %s: %hs"), *AbsolutePath, zip_error_strerror(&Error)));
		if (Source)
		{
			zip_source_free(Source);
		}
		zip_error_fini(&Error);
		return false;
	}
	zip_error_fini(&Error);

	const zip_int64_t EntryCount = zip_get_num_entries(Archive, 0);
	bool bSuccess = EntryCount >= 0 && EntryCount <= MAX_int32;
	for (zip_int64_t Index = 0; bSuccess && Index < EntryCount; ++Index)
	{
		zip_stat_t Stat;
		zip_stat_init(&Stat);
		if (zip_stat_index(Archive, Index, 0, &Stat) != 0 || !(Stat.valid & ZIP_STAT_NAME) || !Stat.name)
		{
			bSuccess = false;
			break;
		}

		UE::FileUtilities::FMCPZipFileMetaData Metadata;
		if (Stat.valid & ZIP_STAT_MTIME)
		{
			Metadata.Timestamp = FDateTime::FromUnixTimestamp(Stat.mtime);
		}
		// Match the UE 5.7 reader's filename conversion and duplicate-entry behavior.
		FileMetadata.Add(FString(ANSI_TO_TCHAR(Stat.name)), MoveTemp(Metadata));
	}
	zip_discard(Archive);
	if (!bSuccess)
	{
		FileMetadata.Reset();
		ReportMCPZipError(ErrorHandler, TEXT("Could not read the ZIP metadata directory."));
	}
	return bSuccess;
}

bool FMCPZipArchiveReader::TryReadFile(
	FStringView FileName, TArray<uint8>& OutData, FOutputDevice* ErrorHandler,
	UE::FileUtilities::FMCPZipFileMetaData* OutMetadata) const
{
	if (OutMetadata)
	{
		OutMetadata->Timestamp.Reset();
		if (!bMetadataLoaded)
		{
			OutData.Reset();
			ReportMCPZipError(ErrorHandler, TEXT("ZIP metadata is unavailable."));
			return false;
		}
	}

	if (!FZipArchiveReader::TryReadFile(FileName, OutData, ErrorHandler))
	{
		return false;
	}

	if (OutMetadata)
	{
		const UE::FileUtilities::FMCPZipFileMetaData* Metadata =
			FileMetadata.FindByHash(GetTypeHash(FileName), FileName);
		if (!Metadata)
		{
			OutData.Reset();
			ReportMCPZipError(ErrorHandler, TEXT("The ZIP entry has no matching metadata record."));
			return false;
		}
		*OutMetadata = *Metadata;
	}
	return true;
}

#endif
