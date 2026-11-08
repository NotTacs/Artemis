#pragma once
#include "CoreMinimal.h"

class FFortAssets
{
public:
	void**                                        VTable;                                            // 0x0000(0x0008)
	uint8                                         Pad_8[0x8];                                        // 0x0008(0x0008)
	TMap<FSoftObjectPath, int32>                  ServerAndClientConstructorReferencesToCountMap;    // 0x0010(0x0050)
	TMap<FSoftObjectPath, int32>                  ClientOnlyConstructorReferencesToCountMap;         // 0x0060(0x0050)
	TSet<const UObject*>                          AlwaysLoadedAssets;                                // 0x00B0(0x0050)
	TSet<const UObject*>                          LoadedAssets;                                      // 0x0100(0x0050)
	TMap<UObject*, FName>                         GCPreventionPool;                                  // 0x0150(0x0050)
	CRITICAL_SECTION                              LoadedAssetsCritical;                              // 0x01A0(0x0028)
	bool                                          bFinishedStartupWindowForAlwaysLoadedAssets;       // 0x01C8(0x0001)
	uint8                                         Pad_1C9[0x7];                                      // 0x01C9(0x0007)
	double                                        LastSyncLoadReportStackTime;                       // 0x01D0(0x0008)
	int32                                         AthenaLoadIssueAnalyticReportCountThisSession;     // 0x01D8(0x0004)
	int32                                         CampaignLoadIssueAnalyticReportCountThisSession;   // 0x01DC(0x0004)

public:
	FFortAssets() = delete;
	FFortAssets(const FFortAssets&) = delete;
	FFortAssets& operator=(const FFortAssets&) = delete;

	static FFortAssets* Get();
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& StringReference, bool bLogLoadTime);
	void AddLoadedAsset(const UObject* Asset);

	template<typename T>
	static T* GetAsset(const TSoftObjectPtr<T>& AssetPointer, bool bLogLoadTime = true, bool bKeepInMemory = true)
	{
		if (AssetPointer.ObjectID.AssetPathName.IsNone())
		{
			return nullptr;
		}

		UObject* Asset = AssetPointer.WeakPtr.Get();
		if (!Asset || !Asset->IsA(T::StaticClass()))
		{
			Asset = SynchronousLoadAsset(reinterpret_cast<const FSoftObjectPath&>(AssetPointer.ObjectID), bLogLoadTime);
			if (Asset && !Asset->IsA(T::StaticClass()))
			{
				Asset = nullptr;
			}
		}

		if (bKeepInMemory && Asset)
		{
			Get()->AddLoadedAsset(Asset);
		}

		return static_cast<T*>(Asset);
	}

	template<typename T>
	static TSubclassOf<T> GetSubclassOf(const TSoftClassPtr<T>& AssetSubclassOf, bool bLogLoadTime = true)
	{
		if (AssetSubclassOf.ObjectID.AssetPathName.IsNone())
		{
			return nullptr;
		}

		UObject* Asset = AssetSubclassOf.WeakPtr.Get();
		if (!IsValidSubclass<T>(Asset))
		{
			Asset = SynchronousLoadAsset(reinterpret_cast<const FSoftObjectPath&>(AssetSubclassOf.ObjectID), bLogLoadTime);
			if (!IsValidSubclass<T>(Asset))
			{
				Asset = nullptr;
			}
		}

		if (Asset)
		{
			Get()->AddLoadedAsset(Asset);
		}

		return static_cast<UClass*>(Asset);
	}

	template<typename T>
	static bool IsValidSubclass(const UObject* Asset)
	{
		if (!Asset || !Asset->IsA(UClass::StaticClass()))
		{
			return false;
		}

		if constexpr (std::is_same_v<T, UClass>)
		{
			return true;
		}
		else
		{
			return static_cast<const UClass*>(Asset)->IsSubclassOf(T::StaticClass());
		}
	}

	template<typename T>
	static void GetAssetArray(const TArray<TSoftObjectPtr<T>>& AssetPointerList, TArray<T*>& OutArray, bool bLogLoadTime = true)
	{
		for (const TSoftObjectPtr<T>& AssetPointer : AssetPointerList)
		{
			if (T* Asset = GetAsset(AssetPointer, bLogLoadTime, true))
			{
				OutArray.Add(Asset);
			}
		}
	}
};

static_assert(sizeof(FFortAssets) == 0x1E0, "FFortAssets has the wrong size");
static_assert(offsetof(FFortAssets, LoadedAssets) == 0x100, "FFortAssets::LoadedAssets is at the wrong offset");
static_assert(offsetof(FFortAssets, LoadedAssetsCritical) == 0x1A0, "FFortAssets::LoadedAssetsCritical is at the wrong offset");
