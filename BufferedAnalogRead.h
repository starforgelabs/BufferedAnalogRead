//
//  BufferedAnalogRead.h
//
//  Created by 治永夢守 on 12/06/03.
//  Copyright 2012 James Knowles. All rights reserved.
//
// This work is licensed under a Creative Commons 
// Attribution-ShareAlike 3.0 Unported License.
//
// https://creativecommons.org/licenses/by-sa/3.0/
//
// This code is strictly "as is". Use at your own risk. 
//
//

#ifndef BUFFERED_ANALOG_READ_H
#define BUFFERED_ANALOG_READ_H

#include "Arduino.h"

//
// SUMMARY
//
// This class provides a FIFO buffer for analogue readings.
//
// PURPOSE
//
// This class implements a simple circular buffer on top of analogue reads.
//
// It is designed to provide a generic base to build running calculations,
// such as returning an average reading.
//
// SEE ALSO
//
// BaseAnalogRead
// 
// EXAMPLE
//
//

#include <BaseAnalogRead.h>

// Note that the buffer size is currently stored as a byte.
// This must be a power of two.
const byte MAX_BUFFER_SIZE = 128;
const byte MIN_BUFFER_SIZE = 2;

// The initial buffer size will be 2 to this power.
const byte DEFAULT_BUFFER_SIZE = 32;

class BufferedAnalogRead :public BaseAnalogRead
{
  public:
    BufferedAnalogRead(byte aPinNumber=0);

    // Extend the functionality of Read() to use a buffer.
    int Read();
    int Read(const int aValue) { return BaseAnalogRead::Read(aValue); }

    // Retrieve the buffer size
    byte BufferSize() { return _BufferSize; }

    // Reset the object.
    void Clear() { Init(_BufferSize); }

    // Set the buffer size. To remain efficient,
    // this is restricted to powers of two up to MAX_BUFFER_SIZE.
    //
    // Changing this value will reset all the readings.
    //
    // This returns true if successful, or false if unsuccessful.
    bool setBufferSize(byte aSize);

  protected:
    // Descendents should not change the values in the buffer.
    // This is provided for convenience only. 
    int  _Buffer[MAX_BUFFER_SIZE];

    // Obtain the current number of items in the buffer.
    byte Count() { return _Count; }

    // This method is triggered when buffers need to be cleared.
    // Descendents should override this if they maintain a buffer.
    virtual void Event_FlushBuffers() {}

    // This method is triggered when a new value is added to the buffer
    // and the buffer is still filling.
    virtual void Event_NewValue(int aValue, byte aIndex) {}
    
    // This method is triggered when a new value is added to the buffer
    // and the buffer is at capacity.
    virtual void Event_ReplaceValue(int aOldValue, int aNewValue, byte aIndex) {}
    
    // This method is triggered when the object should be reset.
    // Descendents should override this if they maintain
    // additional information.
    virtual void Event_Reset() {}

    // Get the current index of the head of the circular buffer.
    // Note that this is not guaranteed to be correct when updating 
    // during a read.
    byte Head() { return _Head; }

  private:
    // The current buffer size.
    byte _BufferSize;
    // The current number of items in the buffer. 
    byte _Count;
    // Current index into the circular buffer.
    byte _Head;

    void AddToBuffer(int aValue);
    void FlushBuffer();
    void Init(const byte aBufferPower);
    void Reset();
};

#endif // BUFFERED_ANALOG_READ_H

