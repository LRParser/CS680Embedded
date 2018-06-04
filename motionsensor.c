#include <u.h>
#include <libc.h>

void writeByte(int fd, char reg, char val)
{
  char* mycmds = malloc(2*sizeof(char));
  mycmds[0] = reg;
  mycmds[1] = val;
  write(fd,mycmds,2);
}

void sendReadRequest(int fd, char reg)
{
   char* mydata = malloc(1*sizeof(char));
  mydata[0] = reg; // control byte 
  write(fd,mydata,1);
}

int readWord(int fd, char reg)
{
  int val;
  char buf[1];
  val = 0;
  seek(fd,reg,0);
  read(fd,buf,1);
  val = buf[0];
  val = val << 8;
  print("Read high bits %x, in decimal: %d\n",buf[0],val);
  seek(fd,reg + 1,0);
  read(fd,buf,1);
  val = val + buf[0];
  print("Read low bits %x, in decimal: %d\n",buf[0],val);
  if(val >= 0x8000)
  {
    val = -(65536 - val);
  }
  print("Return %d\n",val);
  return val;
}


void
main()
{
  int fd;
  int i;
  int val;
  float initialValX;
  float initialValY;
  float initialValZ;
  float deviationFromInitial;
  float accelX;
  float accelY;
  float accelZ;
  int bindret = bind("#J68","/dev",MAFTER);
  print("bind ret: %d\n",bindret);
  fd = open("/dev/i2c.68.data",ORDWR);
  print("%d\n",fd);
  writeByte(fd,0x6B,0x00); // wake up the sensor
  print("Waking, reading in 2 seconds\n");
  sleep(2000);
  sendReadRequest(fd,0x3b);
  
  i = 0;
  val = 0;
  initialValX = 0;
  initialValY = 0;
  initialValZ = 0;
  deviationFromInitial = 0;
  while(i < 10)
  {
  float deviationX;
  float deviationY;
  float deviationZ;

  accelX = readWord(fd,0x3b) / 16384.0;
  accelY = readWord(fd, 0x3d) / 16384.0;
  accelZ = readWord(fd, 0x3f) / 16384.0;

  if(initialValX == 0)
  {
    initialValX = accelX;
  }
  if(initialValY == 0)
  {
    initialValY = accelY;
  }
  if(initialValZ == 0)
  {
    initialValZ = accelZ;
  }

  deviationX = initialValX - accelX;
  if(deviationX < 0)
  {
    deviationX = -1 * deviationX;
  }

  deviationY = initialValY - accelY;
  if(deviationY < 0)
  {
    deviationY = -1 * deviationY;
  }

  deviationZ = initialValZ - accelZ;
  if(deviationZ < 0)
  {
    deviationZ = -1 * deviationZ;
  }

  deviationFromInitial = deviationX + deviationY + deviationZ;

   print("accelX: %f, accelY: %f, accelZ: %f\n",accelX,accelY,accelZ);
   print("Deviation from initial: %f\n",deviationFromInitial);
  

  sleep(1000);
  i = i + 1;
  }
   exits(0);
}
