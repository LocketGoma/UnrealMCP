// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Misc/DateTime.h"
#include "Misc/Optional.h"

namespace UE::FileUtilities
{
struct FMCPZipFileMetaData
{
	TOptional<FDateTime> Timestamp;
};
}
