// Copyright 2017 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <gtest/gtest.h>

#include <vector>

#include "Core/ConfigManager.h"
#include "Core/HW/GCMemcard/GCMemcardDirectory.h"
#include "TestBinaryData.h"
#include "Common/File.h"
#include "Common/FileUtil.h"

TEST(GCMemcardDirectory, BASIC_TEST)
{
  SConfig::Init();
  auto test = new GCMemcardDirectory("", 0, MemCard59Mb, false, 0);
  Header h;
  test->Read(0, BLOCK_SIZE, (u8*)(&h));
  EXPECT_TRUE(BE16(h.deviceID) == 0);
  EXPECT_TRUE(BE16(h.SizeMb) == MemCard59Mb);
  EXPECT_TRUE(BE16(h.Encoding) == 0);
  u8 t[4] = {0xDE, 0xAD, 0xBE, 0xEF};
  //test->Write(0xA000, 1, t);
}

TEST(GCMemcardDirectory, BASIC_TEST2)
{
  std::string testFile = "ZZ-GALE-Test2.gci";
  File::Delete(testFile);
  DEntry entry;
  memcpy(entry.Makercode, "ZZ", 4);
  memcpy(entry.Gamecode, "GALE", 4);
  memcpy(entry.Filename, "Test2", 6);

  SConfig::Init();
  auto test = new GCMemcardDirectory("", 0, MemCard59Mb, false, 0);

  BlockAlloc b;
  auto startingBlock = BE16(b.AssignBlocksContiguous(2));

  auto lastBlock = b.GetNextBlock(startingBlock);

  test->Write(0x6000, 0x2000, (u8*)&b);

  entry.FirstBlock[0] = 0;
  entry.FirstBlock[1] = startingBlock;
  entry.BlockCount[0] = 0;
  entry.BlockCount[1] = 2;
  
  Directory d;
  test->Read(0x2000, 0x2000, (u8*)&d);
  d.UpdateCounter = 0x0100;
  d.Replace(entry, 0);

  test->Write(0x2000, 0x2000, (u8*)&d);
  u32 dummy1 = 0xDEADBEEF;
  u32 dummy2 = 0xCAFEBABE;
  
  test->Write(0x2000 * startingBlock, 4, (u8*)&dummy1);
  test->Write(0x2000 * lastBlock, 4, (u8*)&dummy2);

  // Run destructor, flush to file
  delete test;

  File::IOFile writtenData(testFile, "rb");
  u8 output[0x40 + 0x4000];
  writtenData.ReadBytes(output, 0x40 + 0x4000);
  File::Delete(testFile);

  u32 at0x0040 = *(u32*)(&output[0x40]);
  u32 at0x2040 = *(u32*)(&output[0x2040]);
  ASSERT_EQ(0xDEADBEEF, at0x0040);
  ASSERT_EQ(0xCAFEBABE, at0x2040);
  File::Delete(testFile);
}


TEST(GCMemcardDirectory, BASIC_TEST3)
{
  SConfig::Init();
  auto test = new GCMemcardDirectory("", 0, MemCard59Mb, false, 0);

  BlockAlloc b;
  u16 startingBlock = BE16(b.AssignBlocksContiguous(2));
  auto lastBlock = b.GetNextBlock(startingBlock);
  test->Write(0x6000, 0x2000, (u8*)&b);

  DEntry entry;
  memcpy(entry.Makercode, "ZZ", 4);
  memcpy(entry.Gamecode, "GALE", 4);
  memcpy(entry.Filename, "Test3", 6);
  std::string testFile = "ZZ-GALE-Test3.gci";


  entry.FirstBlock[0] = 0;
  entry.FirstBlock[1] = startingBlock;
  entry.BlockCount[0] = 0;
  entry.BlockCount[1] = 2;


  u32 dummy1 = 0xDEADBEEF;
  u32 dummy2 = 0xCAFEBABE;

  test->Write(0x2000 * startingBlock, 4, (u8*)&dummy1);
  test->Write(0x2000 * lastBlock, 4, (u8*)&dummy2);

  // Write directory entry after, testing the Datel AdvanceGamePort 
  Directory d;
  test->Read(0x2000, 0x2000, (u8*)&d);
  d.UpdateCounter = 0x0100;
  d.Replace(entry, 0);
  test->Write(0x2000, 0x2000, (u8*)&d);
  delete test;
  File::IOFile writtenData(testFile, "rb");
  u8 output[0x40 + 0x4000];
  writtenData.ReadBytes(output, 0x40 + 0x4000);
  u32 at0x0040 = *(u32*)&output[0x40];
  u32 at0x2040 = *(u32*)&output[0x2040];
  ASSERT_EQ(0xDEADBEEF, at0x0040);
  ASSERT_EQ(0xCAFEBABE, at0x2040);
  File::Delete(testFile);
}
