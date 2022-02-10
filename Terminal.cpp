#include "Terminal.h"

Terminal::Terminal() {
  TJBox_ObjectRef customPropertiesRef =
      JBox_GetMotherboardObjectRef("/custom_properties");
  fTextOutRef = JBox_MakePropertyRef(customPropertiesRef, "text_out_buffer");
}

void Terminal::SendProperties() {
  if (!fDirty)
    return;

  // Copy string to outbuffer
  TJBox_UInt64 textIndex = fOutIndex;
  TJBox_SizeT currentLength = 0;

  while (fTextBuffer[textIndex] && currentLength < kOutBufferLength) {
    fOutBuffer[currentLength] = fTextBuffer[textIndex];

    currentLength++;
    textIndex = (textIndex + 1) % kTextBufferLength;
  }

  // Set Remaining to zero
  /*while(currentLength < kOutBufferLength)
      fOutBuffer[currentLength++] = 0;
  */

  JBox_SetRTStringData(fTextOutRef, currentLength, fOutBuffer);
}

void Terminal::ScrollDown() {
  fDirty = true;

  while (fTextBuffer[fOutIndex] != '\n') {
    if (fTextBuffer[fOutIndex] == 0 || fOutIndex == fBufferIndex)
      return;

    fOutIndex = (fOutIndex + 1) % kTextBufferLength;
  }

  // Start at first character after newline
  fOutIndex++;
}

void Terminal::Putch(const char c) {
  fDirty = true;

  fTextBuffer[fBufferIndex] = c;
  fBufferIndex = (fBufferIndex + 1) % kTextBufferLength;

  // Handle New Line
  if (c == '\n') {
    fTerminalY++;
    fTerminalX = 0;
  } else {
    fTerminalX++;
  }

  // Automatic Wrapping
  if (fTerminalX >= kTerminalWidth)
    Putch('\n');

  // Automatic Scrolling
  if (fTerminalY >= kTerminalHeight) {
    fTerminalY--;

    ScrollDown();
  }
}

void Terminal::Puts(const char *s) {
  while (*s)
    Putch(*(s++));

  // Null Terminate
  fTextBuffer[fBufferIndex] = 0; 
}