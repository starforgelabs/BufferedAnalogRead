#include <ArduinoUnit.h>
#include <BaseAnalogRead.h>
#include <BufferedAnalogRead.h>

TestSuite suite;

class TestBufferedAnalogRead: public BufferedAnalogRead
{
  public:
    int* getBuffer() { return _Buffer; }
    byte Head() { return BufferedAnalogRead::Head(); }
};

TestBufferedAnalogRead tester;

test(read)
{
  int* lBuffer = tester.getBuffer();
  
  for(int i=0; i<1024; i++)
  {
    tester.Reading = i;
    int val = tester.Read();
    int idx = tester.Head();
    
    assertEquals(i, val);
    assertEquals(i, tester.Reading);
    assertEquals(i % DEFAULT_BUFFER_SIZE, idx);
    assertEquals(i, lBuffer[idx]);
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

void setup()
{
  tester.Enabled = false;
}

void loop()
{
  suite.run();
}
