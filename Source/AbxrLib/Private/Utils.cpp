#include "Utils.h"
#include "Misc/Base64.h"
#include <openssl/sha.h>

FString Utils::ComputeSHA256(const FString& Input)
{
	// Convert FString to UTF-8
	FTCHARToUTF8 Converter(*Input);
	const uint8* Data = reinterpret_cast<const uint8*>(Converter.Get());
	int32 Length = Converter.Length();

	// SHA-256: 32 bytes
	uint8 Hash[SHA256_DIGEST_LENGTH];
	SHA256(Data, Length, Hash);

	// Encode to Base64
	return FBase64::Encode(Hash, SHA256_DIGEST_LENGTH);
}
