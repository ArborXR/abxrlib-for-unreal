#include "QR/AbxrQrScanCommon.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJavaEnv.h"
#endif

namespace
{
	bool IsAllDigits(const FString& Value)
	{
		if (Value.IsEmpty()) return false;
		for (const TCHAR C : Value)
		{
			if (!FChar::IsDigit(C)) return false;
		}
		return true;
	}
}

bool AbxrQrScanCommon::TryExtractPinFromQrPayload(const FString& ScanResult, FString& OutPin)
{
	OutPin.Reset();
	FString Trimmed = ScanResult.TrimStartAndEnd();
	if (Trimmed.IsEmpty()) return false;

	if (Trimmed.StartsWith(TEXT("ABXR:"), ESearchCase::IgnoreCase))
	{
		const FString Candidate = Trimmed.Mid(5).TrimStartAndEnd();
		if (IsAllDigits(Candidate))
		{
			OutPin = Candidate;
			return true;
		}
	}

	if (Trimmed.Len() == 6 && IsAllDigits(Trimmed))
	{
		OutPin = Trimmed;
		return true;
	}

	return false;
}

bool AbxrQrScanCommon::IsLikelyPicoDevice()
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env) return false;

	jclass BuildClass = FAndroidApplication::FindJavaClass("android.os.Build");
	if (!BuildClass) return false;

	const auto ReadField = [Env, BuildClass](const char* FieldName) -> FString
	{
		jfieldID Field = Env->GetStaticFieldID(BuildClass, FieldName, "Ljava/lang/String;");
		if (!Field) return FString();
		jstring Value = static_cast<jstring>(Env->GetStaticObjectField(BuildClass, Field));
		if (!Value) return FString();
		const FString Out = FJavaHelper::FStringFromLocalRef(Env, Value);
		return Out;
	};

	const FString Combined =
		ReadField("MANUFACTURER") + TEXT(" ") +
		ReadField("MODEL") + TEXT(" ") +
		ReadField("DEVICE") + TEXT(" ") +
		ReadField("PRODUCT");

	return Combined.Contains(TEXT("pico"), ESearchCase::IgnoreCase);
#else
	return false;
#endif
}
