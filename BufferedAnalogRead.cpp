/*
 *  BufferedAnalogRead.cpp
 *  Sun Tracker
 *
 *  Created by 治永夢守 on 12/05/27.
 *  Copyright 2012 James Knowles. All rights reserved.
 *
 * This work is licensed under a Creative Commons 
 * Attribution-ShareAlike 3.0 Unported License.
 *
 * https://creativecommons.org/licenses/by-sa/3.0/
 *
 *
 */

#include "BufferedAnalogRead.h"
#include <math.h>


BufferedAnalogRead::BufferedAnalogRead(byte aPinNumber) : BaseAnalogRead(aPinNumber)
{
  Init(DEFAULT_BUFFER_SIZE);
}

void BufferedAnalogRead::AddToBuffer(int aValue)
{
  if(_Count < _BufferSize)
  {
    _Head = _Count++;
    _Buffer[_Head] = aValue;

    Event_NewValue(aValue, _Head);
    return;
  }

  _Head = (_Head+1) % _BufferSize;
  int lOldValue = _Buffer[_Head];
  _Buffer[_Head] = aValue;

  Event_ReplaceValue(lOldValue, aValue, _Head);
}

void BufferedAnalogRead::Init(const byte aBufferSize)
{
  _BufferSize = aBufferSize;

  _Count = 0;
  _Head = 0;

  // Clear buffer
  for(int i=0; i<_BufferSize; i++)
    _Buffer[i] = 0;

  Event_FlushBuffers();
}

int BufferedAnalogRead::Read()
{
  BaseAnalogRead::Read();
  AddToBuffer(Reading);
  return Reading;
}

bool BufferedAnalogRead::setBufferSize(byte aSize)
{
  if(aSize > MAX_BUFFER_SIZE) return false;
  if(aSize < MIN_BUFFER_SIZE) return false;

  // Count the number of bits.
  // Method first published by Peter Wegner in CACM 3 (1960), 322. 
  byte temp = aSize;
  byte count;
  for (count = 0; temp; count++) temp &= temp - 1; 

  if(count != 1) return false;

  Init(aSize);
  return true;
}
