#pragma once

#include "Jukebox.h"

class Terminal {
private:
  static const TJBox_SizeT kTextBufferLength = 2048;
  static const TJBox_SizeT kOutBufferLength = 1024;

  static const TJBox_UInt64 kTerminalWidth = 50;
  static const TJBox_UInt64 kTerminalHeight = 20;

  static_assert(kOutBufferLength >= kTerminalWidth * kTerminalHeight);

  TJBox_PropertyRef fTextOutRef;
  TJBox_UInt64 fBufferIndex = 0;
  TJBox_UInt64 fOutIndex = 0;
  TJBox_Bool fDirty = false;

  TJBox_UInt64 fTerminalY = 0;
  TJBox_UInt64 fTerminalX = 0;

  // Potential Optimization, use two mom buffers to skip outbuffer entirely
  TJBox_UInt8 fTextBuffer[kTextBufferLength];
  TJBox_UInt8 fOutBuffer[kOutBufferLength];

public:
  Terminal();
  void Putch(const char c);
  void Puts(const char *s);
  void SendProperties();
  void ScrollDown();
};
