#include <ArduinoUnit.h>
#include <BaseAnalogRead.h>
#include <BufferedAnalogRead.h>

TestSuite suite;

class TestBufferedAnalogRead: public BufferedAnalogRead
{
  public:
    int* getBuffer() { return _Buffer; }
    byte Count() { return BufferedAnalogRead::Count(); }
    byte Head() { return BufferedAnalogRead::Head(); }

    void ClearCounters()
    {
      ResetCount = 0;
      FlushBufferCount = 0;
      NewValueCount = 0;
      NewValue = 0;
      NewValueIndex = 0;
      ReplaceOldValue = 0;
      ReplaceNewValue = 0;
      ReplaceIndex = 0;
      ReplaceCount = 0;
    }

    // This method is triggered when the object should be reset.
    // Descendents should override this if they maintain
    // additional information.
    int ResetCount;
    void Event_Reset() 
    {
      ResetCount++;
      NewValueCount = 0;
      NewValue = 0;
      NewValueIndex = 0;
      ReplaceOldValue = 0;
      ReplaceNewValue = 0;
      ReplaceIndex = 0;
      ReplaceCount = 0;
    }

    // This method is triggered when buffers need to be cleared.
    // Descendents should override this if they maintain a buffer.
    int FlushBufferCount;
    void Event_FlushBuffers() {FlushBufferCount++;}

    // This method is triggered when a new value is added to the buffer
    // and the buffer is still filling.
    int NewValue;
    int NewValueCount;
    int NewValueIndex;

    void Event_NewValue(int aValue, byte aIndex) 
    {
      NewValue = aValue;
      NewValueIndex = aIndex;
      NewValueCount++;
    }
    
    // This method is triggered when a new value is added to the buffer
    // and the buffer is at capacity.
    int ReplaceOldValue;
    int ReplaceNewValue;
    int ReplaceIndex;
    int ReplaceCount;
    void Event_ReplaceValue(int aOldValue, int aNewValue, byte aIndex) 
    {
      ReplaceOldValue = aOldValue;
      ReplaceNewValue = aNewValue;
      ReplaceIndex = aIndex;
      ReplaceCount++;
    }
 
};

TestBufferedAnalogRead tester;

const byte NumInvalidBufferSizes = 14;
byte InvalidBufferSizes[NumInvalidBufferSizes] = {0, 1, 3, 7, 9, 15, 17, 31, 33, 63, 65, 127, 129, 255};
test(bad_buffer_size)
{
    bool result = tester.setBufferSize(DEFAULT_BUFFER_SIZE);
    assertTrue(result);
    
  for(int i=0; i< NumInvalidBufferSizes; i++)
  {
    byte bad = InvalidBufferSizes[i];
    result = tester.setBufferSize(bad);
    assertTrue(!result);
    assertEquals(DEFAULT_BUFFER_SIZE, tester.BufferSize());
  }
}

test(clear)
{
  tester.setBufferSize(MAX_BUFFER_SIZE);
  int* lBuffer = tester.getBuffer();
  // Fill with data
  for(int i=0; i<1024; i++)
  {
    int val = tester.Read(i+1);
  }
  
  tester.ClearCounters();

  // Test
  tester.Clear();
  assertEquals(1, tester.ResetCount);
  assertEquals(1, tester.FlushBufferCount);
  assertEquals(MAX_BUFFER_SIZE, tester.BufferSize());
  assertEquals(0, tester.Head());
  assertEquals(0, tester.Count());
  for(int i=0; i<MAX_BUFFER_SIZE; i++)
  {
    assertEquals(0, lBuffer[i]);
  }
}

test(count)
{
  tester.Clear();
  for(int i=0; i<1023; i++)
  {
    tester.Read();
    if(i < tester.BufferSize())
    {
      assertEquals(i+1, tester.Count());
    }
    else
    {
      assertEquals(tester.BufferSize(), tester.Count());
    }
  }
}

const byte NumValidBufferSizes = 7;
byte ValidBufferSizes[NumValidBufferSizes] = {2, 4, 8, 16, 32, 64, 128};
test(good_buffer_size)
{
  for(int i=0; i< NumValidBufferSizes; i++)
  {
    bool result = tester.setBufferSize(ValidBufferSizes[i]);
    assertTrue(result);
    assertEquals(ValidBufferSizes[i], tester.BufferSize());
  }
}

test(is_enabled)
{
  tester.Clear();
  tester.IsEnabled = false;
  for(int i=0; i<1024; i++)
    tester.Read(i);
  tester.IsEnabled = true;

  assertEquals(0, tester.Count());
  assertEquals(0, tester.Head());
}

test(read)
{
  tester.setBufferSize(DEFAULT_BUFFER_SIZE);
  int* lBuffer = tester.getBuffer();
  
  for(int i=0; i<1024; i++)
  {
    int val = tester.Read(i);
    int idx = tester.Head();
    
    assertEquals(i, val);
    assertEquals(i, tester.Value);
    assertEquals(i % DEFAULT_BUFFER_SIZE, idx);
    assertEquals(i, lBuffer[idx]);
  }
}

test(value_events)
{
  tester.Clear();
  tester.ClearCounters();
  for(int i=0; i<1024; i++)
  {
    tester.Read(i);
    
    int count = i+1;
    if(count <= tester.BufferSize())
    {
      assertEquals(i, tester.NewValue);
      assertEquals(i % tester.BufferSize(), tester.NewValueIndex);
      assertEquals(count, tester.NewValueCount);
      assertEquals(0, tester.ReplaceOldValue);
      assertEquals(0, tester.ReplaceNewValue);
      assertEquals(0, tester.ReplaceIndex);
      assertEquals(0, tester.ReplaceCount);
    }
    else
    {
      assertEquals(tester.BufferSize()-1, tester.NewValue);
      assertEquals(tester.BufferSize()-1, tester.NewValueIndex);
      assertEquals(tester.BufferSize(), tester.NewValueCount);
      assertEquals(i - tester.BufferSize(), tester.ReplaceOldValue);
      assertEquals(i, tester.ReplaceNewValue);
      assertEquals(i % tester.BufferSize(), tester.ReplaceIndex);
      assertEquals(count-tester.BufferSize(), tester.ReplaceCount);
    }
  }
}

void setup()
{
}

void loop()
{
  suite.run();
}

