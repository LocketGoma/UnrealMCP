// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModelContextProtocolToolSearch.h"

#include "Dom/JsonObject.h"
#include "ModelContextProtocol.h"
#include "ModelContextProtocolToolResults.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace UE::ModelContextProtocol::Private
{
	TSharedPtr<FJsonObject> MakeToolsetNameSchema(const FString& Description)
	{
		TSharedPtr<FJsonObject> Schema = MakeShared<FJsonObject>();
		Schema->SetStringField(TEXT("type"), TEXT("object"));

		TSharedPtr<FJsonObject> Properties = MakeShared<FJsonObject>();
		TSharedPtr<FJsonObject> ToolsetNameProp = MakeShared<FJsonObject>();
		ToolsetNameProp->SetStringField(TEXT("type"), TEXT("string"));
		ToolsetNameProp->SetStringField(TEXT("description"), Description);
		Properties->SetObjectField(TEXT("toolset_name"), ToolsetNameProp);
		Schema->SetObjectField(TEXT("properties"), Properties);

		TArray<TSharedPtr<FJsonValue>> RequiredArray;
		RequiredArray.Add(MakeShared<FJsonValueString>(TEXT("toolset_name")));
		Schema->SetArrayField(TEXT("required"), RequiredArray);

		return Schema;
	}
}

// -- FListToolsetsTool --

FListToolsetsTool::FListToolsetsTool(FListToolsetsDelegate InDelegate)
	: ListDelegate(MoveTemp(InDelegate))
{
}

TSharedPtr<FJsonObject> FListToolsetsTool::GetInputJsonSchema() const
{
	TSharedPtr<FJsonObject> Schema = MakeShared<FJsonObject>();
	Schema->SetStringField(TEXT("type"), TEXT("object"));
	Schema->SetObjectField(TEXT("properties"), MakeShared<FJsonObject>());
	return Schema;
}

FModelContextProtocolToolResult FListToolsetsTool::Run(const TSharedPtr<FJsonObject>& Params)
{
	FString Result = ListDelegate();
	return UE::ModelContextProtocol::MakeTextResult(Result);
}

// -- FDescribeToolsetTool --

FDescribeToolsetTool::FDescribeToolsetTool(FDescribeToolsetDelegate InDelegate)
	: DescribeDelegate(MoveTemp(InDelegate))
{
}

TSharedPtr<FJsonObject> FDescribeToolsetTool::GetInputJsonSchema() const
{
	TSharedPtr<FJsonObject> Schema = UE::ModelContextProtocol::Private::MakeToolsetNameSchema(TEXT("Name of the toolset to describe. Use list_toolsets to see available names."));
	TSharedPtr<FJsonObject> ToolNameProp = MakeShared<FJsonObject>();
	ToolNameProp->SetStringField(TEXT("type"), TEXT("string"));
	ToolNameProp->SetNumberField(TEXT("minLength"), 1);
	ToolNameProp->SetStringField(TEXT("description"), TEXT("Optional exact tool name, short or fully qualified. Omit to return all tools."));
	Schema->GetObjectField(TEXT("properties"))->SetObjectField(TEXT("tool_name"), ToolNameProp);
	TSharedPtr<FJsonObject> SummaryProp = MakeShared<FJsonObject>();
	SummaryProp->SetStringField(TEXT("type"), TEXT("boolean"));
	SummaryProp->SetBoolField(TEXT("default"), true);
	SummaryProp->SetStringField(TEXT("description"), TEXT("Defaults to true: return names and descriptions of at most 160 characters, without input/output schemas. Set false to retrieve complete schemas."));
	Schema->GetObjectField(TEXT("properties"))->SetObjectField(TEXT("summary_only"), SummaryProp);
	return Schema;
}

FModelContextProtocolToolResult FDescribeToolsetTool::Run(const TSharedPtr<FJsonObject>& Params)
{
	if (!Params.IsValid())
	{
		return UE::ModelContextProtocol::MakeErrorResult(TEXT("Missing parameters."));
	}

	FString ToolsetName;
	if (!Params->TryGetStringField(TEXT("toolset_name"), ToolsetName) || ToolsetName.IsEmpty())
	{
		return UE::ModelContextProtocol::MakeErrorResult(TEXT("Missing required parameter: toolset_name"));
	}

	FString ToolName;
	const bool bFilterTool = Params->HasField(TEXT("tool_name"));
	if (bFilterTool && (!Params->HasTypedField<EJson::String>(TEXT("tool_name")) ||
		!Params->TryGetStringField(TEXT("tool_name"), ToolName) || ToolName.IsEmpty()))
	{
		return UE::ModelContextProtocol::MakeErrorResult(TEXT("tool_name must be a non-empty string when provided."));
	}
	bool bSummaryOnly = true;
	if (Params->HasField(TEXT("summary_only")) &&
		(!Params->HasTypedField<EJson::Boolean>(TEXT("summary_only")) ||
		 !Params->TryGetBoolField(TEXT("summary_only"), bSummaryOnly)))
	{
		return UE::ModelContextProtocol::MakeErrorResult(TEXT("summary_only must be a boolean when provided."));
	}

	TValueOrError<FString, FString> Result = DescribeDelegate(ToolsetName);
	if (Result.HasError())
	{
		return UE::ModelContextProtocol::MakeErrorResult(Result.GetError());
	}
	if (!bFilterTool && !bSummaryOnly)
	{
		return UE::ModelContextProtocol::MakeTextResult(Result.GetValue());
	}

	// Start from the registry's enabled tools. Detailed responses preserve complete
	// schemas and shared definitions; summaries deliberately omit schema payloads.
	TSharedPtr<FJsonObject> Schema;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Result.GetValue());
	const TArray<TSharedPtr<FJsonValue>>* Tools = nullptr;
	if (!FJsonSerializer::Deserialize(Reader, Schema) || !Schema.IsValid() ||
		!Schema->TryGetArrayField(TEXT("tools"), Tools))
	{
		return UE::ModelContextProtocol::MakeErrorResult(TEXT("No tool schema available for this toolset."));
	}

	const FString QualifiedName = ToolsetName + TEXT(".") + ToolName;
	TArray<TSharedPtr<FJsonValue>> SelectedTools;
	for (const TSharedPtr<FJsonValue>& Entry : *Tools)
	{
		const TSharedPtr<FJsonObject>* Tool = nullptr;
		FString Name;
		if (Entry.IsValid() && Entry->TryGetObject(Tool) && (*Tool)->TryGetStringField(TEXT("name"), Name) &&
			(!bFilterTool || Name.Equals(ToolName, ESearchCase::CaseSensitive) || Name.Equals(QualifiedName, ESearchCase::CaseSensitive)))
		{
			SelectedTools.Add(Entry);
			if (bFilterTool) break;
		}
	}
	if (bFilterTool && SelectedTools.IsEmpty())
	{
		return UE::ModelContextProtocol::MakeErrorResult(FString::Printf(
			TEXT("Tool '%s' not found or not enabled in toolset '%s'."), *ToolName, *ToolsetName));
	}

	if (bSummaryOnly)
	{
		const auto ShortDescription = [](const FString& Description)
		{
			TArray<FString> Words;
			Description.ParseIntoArrayWS(Words);
			const FString Line = FString::Join(Words, TEXT(" "));
			return Line.Len() > 160 ? Line.Left(157) + TEXT("...") : Line;
		};
		TSharedPtr<FJsonObject> Summary = MakeShared<FJsonObject>();
		Summary->SetStringField(TEXT("name"), ToolsetName);
		FString Value;
		if (Schema->TryGetStringField(TEXT("version"), Value))
		{
			Summary->SetStringField(TEXT("version"), Value);
		}
		if (Schema->TryGetStringField(TEXT("description"), Value))
		{
			Summary->SetStringField(TEXT("description"), ShortDescription(Value));
		}
		Summary->SetBoolField(TEXT("summaryOnly"), true);
		for (TSharedPtr<FJsonValue>& Entry : SelectedTools)
		{
			const TSharedPtr<FJsonObject> Tool = Entry->AsObject();
			TSharedRef<FJsonObject> Brief = MakeShared<FJsonObject>();
			Brief->SetStringField(TEXT("name"), Tool->GetStringField(TEXT("name")));
			FString Description;
			Tool->TryGetStringField(TEXT("description"), Description);
			Brief->SetStringField(TEXT("description"), ShortDescription(Description));
			Entry = MakeShared<FJsonValueObject>(Brief);
		}
		Schema = Summary;
	}
	Schema->SetArrayField(TEXT("tools"), SelectedTools);
	FString Json;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
		TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Json);
	if (!FJsonSerializer::Serialize(Schema.ToSharedRef(), Writer))
	{
		return UE::ModelContextProtocol::MakeErrorResult(TEXT("Failed to serialize tool description."));
	}
	return UE::ModelContextProtocol::MakeTextResult(Json);
}

// -- FCallTool --

FCallTool::FCallTool(FCallToolDelegate InDelegate)
	: CallDelegate(MoveTemp(InDelegate))
{
}

TSharedPtr<FJsonObject> FCallTool::GetInputJsonSchema() const
{
	TSharedPtr<FJsonObject> Schema = MakeShared<FJsonObject>();
	Schema->SetStringField(TEXT("type"), TEXT("object"));

	TSharedPtr<FJsonObject> Properties = MakeShared<FJsonObject>();

	TSharedPtr<FJsonObject> ToolsetNameProp = MakeShared<FJsonObject>();
	ToolsetNameProp->SetStringField(TEXT("type"), TEXT("string"));
	ToolsetNameProp->SetStringField(TEXT("description"), TEXT("Optional. Name of the toolset containing the tool. Omit to call a top-level MCP tool. Use list_toolsets to discover toolset names."));
	Properties->SetObjectField(TEXT("toolset_name"), ToolsetNameProp);

	TSharedPtr<FJsonObject> ToolNameProp = MakeShared<FJsonObject>();
	ToolNameProp->SetStringField(TEXT("type"), TEXT("string"));
	ToolNameProp->SetStringField(TEXT("description"), TEXT("Name of the tool to call, without toolset prefix. Use describe_toolset to discover tool names and input schemas."));
	Properties->SetObjectField(TEXT("tool_name"), ToolNameProp);

	TSharedPtr<FJsonObject> ArgumentsProp = MakeShared<FJsonObject>();
	ArgumentsProp->SetStringField(TEXT("type"), TEXT("object"));
	ArgumentsProp->SetStringField(TEXT("description"), TEXT("Arguments to pass to the tool. Must match the tool's input schema. Defaults to an empty object."));
	Properties->SetObjectField(TEXT("arguments"), ArgumentsProp);

	Schema->SetObjectField(TEXT("properties"), Properties);

	TArray<TSharedPtr<FJsonValue>> RequiredArray;
	RequiredArray.Add(MakeShared<FJsonValueString>(TEXT("tool_name")));
	Schema->SetArrayField(TEXT("required"), RequiredArray);

	return Schema;
}

void FCallTool::RunAsync(const FModelContextProtocolToolRequestId& RequestId, const TSharedPtr<FJsonObject>& Params, const FResultCallback& OnComplete)
{
	if (!Params.IsValid())
	{
		OnComplete(UE::ModelContextProtocol::MakeErrorResult(TEXT("Missing parameters.")));
		return;
	}

	FString ToolName;
	if (!Params->TryGetStringField(TEXT("tool_name"), ToolName) || ToolName.IsEmpty())
	{
		OnComplete(UE::ModelContextProtocol::MakeErrorResult(TEXT("Missing required parameter: tool_name")));
		return;
	}

	FString ToolsetName;
	Params->TryGetStringField(TEXT("toolset_name"), ToolsetName);

	const TSharedPtr<FJsonObject>* ArgumentsObject = nullptr;
	Params->TryGetObjectField(TEXT("arguments"), ArgumentsObject);
	TSharedPtr<FJsonObject> Arguments = (ArgumentsObject && ArgumentsObject->IsValid()) ? *ArgumentsObject : MakeShared<FJsonObject>();

	CallDelegate(ToolsetName, ToolName, Arguments, RequestId, OnComplete);
}
