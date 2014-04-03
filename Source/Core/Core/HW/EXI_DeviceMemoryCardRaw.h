// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#pragma once

#include "Common/Thread.h"
#include "GCMemcard.h"

// Data structure to be passed to the flushing thread.
struct FlushData
{
	bool bExiting;
	std::string filename;
	u8 *memcardContent;
	int memcardSize, memcardIndex;
};

class MemoryCardBase
{
public:
	MemoryCardBase(int _card_index = 0, int sizeMb = MemCard2043Mb) :card_index(_card_index), nintendo_card_id(sizeMb) { ; }
	virtual void Flush(bool exiting = false) = 0;
	virtual s32 Read(u32 address, s32 length, u8* destaddress) = 0;
	virtual s32 Write(u32 destaddress, s32 length, u8* srcaddress) = 0;
	virtual void ClearBlock(u32 address) = 0;
	virtual void ClearAll() = 0;
	virtual void DoState(PointerWrap &p) = 0;
	virtual void joinThread() {};
	u32 GetCardId() { return nintendo_card_id; }
protected:
	int card_index;
	u16 nintendo_card_id;
	u32 memory_card_size;
};

class MemoryCard : public MemoryCardBase
{
public:
	MemoryCard(std::string filename, int _card_index, u16 sizeMb = MemCard2043Mb);
	~MemoryCard() { Flush(true); }
	void Flush(bool exiting = false) override;

	s32 Read(u32 address, s32 length, u8* destaddress) override;
	s32 Write(u32 destaddress, s32 length, u8* srcaddress) override;
	void ClearBlock(u32 address) override;
	void ClearAll() override;
	void DoState(PointerWrap &p) override;
	void joinThread() override;
private:
	u8 *memory_card_content;
	bool m_bDirty;
	std::string m_strFilename;

	FlushData flushData;
	std::thread flushThread;
};
