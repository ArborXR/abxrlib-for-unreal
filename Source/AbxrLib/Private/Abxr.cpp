#include "Abxr.h"
#include "Authentication.h"
#include "EventBatcher.h"
#include "InputDialogWidget.h"
#include "LogBatcher.h"
#include "TelemetryBatcher.h"

TMap<FString, int64> UAbxr::AssessmentStartTimes;
TMap<FString, int64> UAbxr::ObjectiveStartTimes;
TMap<FString, int64> UAbxr::InteractionStartTimes;
TMap<FString, int64> UAbxr::LevelStartTimes;
UObject* UAbxr::World;

void UAbxr::LogDebug(const FString& Text, const TMap<FString, FString>& Meta)
{
    LogBatcher::Add("debug", Text, Meta);
}

void UAbxr::LogInfo(const FString& Text, const TMap<FString, FString>& Meta)
{
    LogBatcher::Add("info", Text, Meta);
}

void UAbxr::LogWarn(const FString& Text, const TMap<FString, FString>& Meta)
{
    LogBatcher::Add("warn", Text, Meta);
}

void UAbxr::LogError(const FString& Text, const TMap<FString, FString>& Meta)
{
    LogBatcher::Add("error", Text, Meta);
}

void UAbxr::LogCritical(const FString& Text, const TMap<FString, FString>& Meta)
{
    LogBatcher::Add("critical", Text, Meta);
}

void UAbxr::Event(const FString& Name, const TMap<FString, FString>& Meta)
{
	EventBatcher::Add(Name, Meta);
}

/**
* Add event information
*
* @param Name      Name of the event
* @param Position  Adds position tracking of the object
* @param Meta      Any additional information
*/
void UAbxr::Event(const FString& Name, const FVector& Position, TMap<FString, FString>& Meta)
{
    Meta.Add(TEXT("position_x"), FString::SanitizeFloat(Position.X));
    Meta.Add(TEXT("position_y"), FString::SanitizeFloat(Position.Y));
    Meta.Add(TEXT("position_z"), FString::SanitizeFloat(Position.Z));
    Event(Name, Meta);
}

/**
* Add telemetry information
*
* @param Name      Name of the telemetry
* @param Meta      Any additional information
*/
void UAbxr::TelemetryEntry(const FString& Name, const TMap<FString, FString>& Meta)
{
    TelemetryBatcher::Add(Name, Meta);
}

void UAbxr::EventAssessmentStart(const FString& AssessmentName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("assessment"));
	Meta.Add(TEXT("verb"), TEXT("started"));
	AssessmentStartTimes.Add(AssessmentName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(AssessmentName, Meta);
}

void UAbxr::EventAssessmentComplete(const FString& AssessmentName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("assessment"));
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("score"), FString::FromInt(Score));
	Meta.Add(TEXT("status"), StaticEnum<EEventStatus>()->GetNameStringByValue(static_cast<int64>(Status)));
	AddDuration(AssessmentStartTimes, AssessmentName, Meta);
	Event(AssessmentName, Meta);
	EventBatcher::Send();
}

void UAbxr::EventObjectiveStart(const FString& ObjectiveName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("objective"));
	Meta.Add(TEXT("verb"), TEXT("started"));
	ObjectiveStartTimes.Add(ObjectiveName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(ObjectiveName, Meta);
}

void UAbxr::EventObjectiveComplete(const FString& ObjectiveName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("objective"));
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("score"), FString::FromInt(Score));
	Meta.Add(TEXT("status"), StaticEnum<EEventStatus>()->GetNameStringByValue(static_cast<int64>(Status)));
	AddDuration(ObjectiveStartTimes, ObjectiveName, Meta);
	Event(ObjectiveName, Meta);
}

void UAbxr::EventInteractionStart(const FString& InteractionName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("interaction"));
	Meta.Add(TEXT("verb"), TEXT("started"));
	InteractionStartTimes.Add(InteractionName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(InteractionName, Meta);
}

void UAbxr::EventInteractionComplete(const FString& InteractionName, EInteractionType InteractionType, const FString& Response, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("interaction"));
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("interaction"), StaticEnum<EInteractionType>()->GetNameStringByValue(static_cast<int64>(InteractionType)));
	if (!Response.IsEmpty()) Meta.Add(TEXT("response"), Response);
	AddDuration(InteractionStartTimes, InteractionName, Meta);
	Event(InteractionName, Meta);
}

void UAbxr::EventLevelStart(const FString& LevelName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("verb"), TEXT("started"));
	Meta.Add(TEXT("id"), LevelName);
	LevelStartTimes.Add(LevelName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(TEXT("level_start"), Meta);
}

void UAbxr::EventLevelComplete(const FString& LevelName, const int Score, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("id"), LevelName);
	Meta.Add(TEXT("score"), FString::FromInt(Score));
	AddDuration(LevelStartTimes, LevelName, Meta);
	Event(LevelName, Meta);
}

void UAbxr::EventCritical(const FString& Label, const TMap<FString, FString>& Meta)
{
	const FString taggedName = TEXT("CRITICAL_ABXR_") + Label;
	Event(taggedName, Meta);
}

void UAbxr::AddDuration(TMap<FString, int64>& StartTimes, const FString& Name, TMap<FString, FString>& Meta)
{
	if (StartTimes.Contains(Name))
	{
		const int64 Duration = FDateTime::UtcNow().ToUnixTimestamp() - StartTimes[Name];
		Meta.Add(TEXT("duration"), FString::Printf(TEXT("%lld"), Duration));
		StartTimes.Remove(Name);
	}
	else
	{
		Meta.Add(TEXT("duration"), TEXT("0"));
	}
}

void UAbxr::PresentKeyboard(const FString& PromptText, const FString& KeyboardType, const FString& EmailDomain)
{
	auto* Dialog = UInputDialogWidget::ShowDialog(World, FText::FromString(PromptText), FText::FromString("Type here..."));
	Dialog->OnAccepted.AddLambda([](const FString& Text)
	{
		UE_LOG(LogTemp, Log, TEXT("User typed: %s"), *Text);
		Authentication::KeyboardAuthenticate(Text);
	});
}
