// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Common/CommonTypes.h"

namespace DiscIO
{
class IVolume
{
public:
	IVolume() {}
	virtual ~IVolume() {}

	virtual bool Read(u64 _Offset, u64 _Length, u8* _pBuffer) const = 0;
	virtual bool RAWRead(u64 _Offset, u64 _Length, u8* _pBuffer) const = 0;
	virtual bool GetTitleID(u8*) const { return false; }
	virtual std::unique_ptr<u8[]> GetTMD(u32 *_sz) const
	{
		*_sz = 0;
		return std::unique_ptr<u8[]>();
	}
	virtual std::string GetUniqueID() const = 0;
	virtual std::string GetRevisionSpecificUniqueID() const { return ""; }
	virtual std::string GetMakerID() const = 0;
	virtual int GetRevision() const { return 0; }
	// TODO: eliminate?
	virtual std::string GetName() const;
	virtual std::vector<std::string> GetNames() const = 0;
	virtual u32 GetFSTSize() const = 0;
	virtual std::string GetApploaderDate() const = 0;
	virtual bool SupportsIntegrityCheck() const { return false; }
	virtual bool CheckIntegrity() const { return false; }
	virtual bool IsDiscTwo() const { return false; }

	virtual bool ChangePartition(u64 offset) { return false; }

	// Increment CACHE_REVISION if values are changed (ISOFile.cpp)
	enum ECountry
	{
		COUNTRY_EUROPE = 0,
		COUNTRY_JAPAN,
		COUNTRY_USA,
		COUNTRY_AUSTRALIA,
		COUNTRY_FRANCE,
		COUNTRY_GERMANY,
		COUNTRY_INTERNATIONAL,
		COUNTRY_ITALY,
		COUNTRY_KOREA,
		COUNTRY_NETHERLANDS,
		COUNTRY_RUSSIA,
		COUNTRY_SPAIN,
		COUNTRY_TAIWAN,
		COUNTRY_UNKNOWN,
		NUMBER_OF_COUNTRIES
	};

	virtual ECountry GetCountry() const = 0;
	virtual u64 GetSize() const = 0;

	// Size on disc (compressed size)
	virtual u64 GetRawSize() const = 0;
};

// Generic Switch function for all volumes
IVolume::ECountry CountrySwitch(u8 CountryCode);
u8 GetSysMenuRegion(u16 _TitleVersion);

} // namespace
