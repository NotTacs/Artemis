#include "pch.h"
#include "../Public/FortAssets.h"

FFortAssets* FFortAssets::Get()
{
	static FFortAssets* (*GetOriginal)() = decltype(GetOriginal)(InSDKUtils::GetImageBase() + 0x1299fd8);
	return GetOriginal();
}

UObject* FFortAssets::SynchronousLoadAsset(const FSoftObjectPath& StringReference, bool bLogLoadTime)
{
	static UObject* (*SynchronousLoadAssetOriginal)(const FSoftObjectPath*, bool) = decltype(SynchronousLoadAssetOriginal)(InSDKUtils::GetImageBase() + 0xb67d3c);
	return SynchronousLoadAssetOriginal(&StringReference, bLogLoadTime);
}

void FFortAssets::AddLoadedAsset(const UObject* Asset)
{
	static void (*AddLoadedAssetOriginal)(FFortAssets*, const UObject*) = decltype(AddLoadedAssetOriginal)(InSDKUtils::GetImageBase() + 0x1299d10);
	AddLoadedAssetOriginal(this, Asset);
}
