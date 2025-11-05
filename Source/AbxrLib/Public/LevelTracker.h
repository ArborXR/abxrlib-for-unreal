#pragma once

class LevelTracker
{
public:
	static FString GetCurrentLevel() { return CurrentLevel; }
	static void SetCurrentLevel(const FString& Level) { CurrentLevel = Level; }

private:
	static FString CurrentLevel;
};
