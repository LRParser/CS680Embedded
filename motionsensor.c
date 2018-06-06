 #include <u.h>
#include <libc.h>

void writeByte(int fd, char reg, char val)
{
  char* mycmds = malloc(2*sizeof(char));
  mycmds[0] = reg;
  mycmds[1] = val;
  write(fd,mycmds,2);
}

/* We read the data from MPU6050 with this function. Referenced the SunFounder impl and modified it */
int readWord(int fd, char reg)
{
  int val;
  char buf[1];
  val = 0;
  seek(fd,reg,0);
  read(fd,buf,1);
  val = buf[0];
  val = val << 8;
  //print("Read high bits %x, in decimal: %d\n",buf[0],val);
  seek(fd,reg + 1,0);
  read(fd,buf,1);
  val = val + buf[0];
  //print("Read low bits %x, in decimal: %d\n",buf[0],val);
  // We need to consider that we read the data back in 2s complement
  if(val >= 0x8000)
  {
    val = -(65536 - val);
  }
  //print("Return %d\n",val);
  return val;
}

/* This is a project integrating a push button, a MPU6050 motion sensor, and an LED.
      Together they make a configurable motion detection system that I find useful. When the
       LED is pressed the reference position is set. When it deviates by a fixed threshold from
      that position, the LED lights up. This allows me, for instance, to connect it to an exercise
      pad to make sure a certain pose is held (for instance, practicing Tae-Kwon Do). I make the
    pose, hold the pad, and make sure I did not move after a kick. Or the kids can pretend to 
    be a statue and lose when the light turns on. Finally it can serve as an alarm with the LED
    lighting up when the kids sneak into my office
*/ 

void
main()
{
  int fdMotion;
  int fdGpio;
  int i;
  int val;
  float initialValX;
  float initialValY;
  float initialValZ;
  float deviationFromInitial;
  float accelX;
  float accelY;
  float accelZ;
  float sensorThreshold;
  char gpioBuf[17];

  // Bind the LED that is connected to GPIO23
  bind("#G","/dev",MAFTER);
  fdGpio = open("/dev/gpio",ORDWR);
  fprint(fdGpio,"function 23 out");
  fprint(fdGpio,"set 23 0");

  // Bind the motion sensor over I2c
  int bindret = bind("#J68","/dev",MAFTER);
  print("bind ret: %d\n",bindret);
  fdMotion = open("/dev/i2c.68.data",ORDWR);
  writeByte(fdMotion,0x6B,0x00); // wake up the sensor

  i = 0;
  val = 0;
  initialValX = 0;
  initialValY = 0;
  initialValZ = 0;
  deviationFromInitial = 0;
  sensorThreshold = 1.0;
  while(i < 1000)
  {
    float deviationX;
    float deviationY;
    float deviationZ;

    // Referenced datasheet and SunFounder impl for default conversion heuristics
    accelX = readWord(fdMotion,0x3b) / 16384.0;
    accelY = readWord(fdMotion, 0x3d) / 16384.0;
    accelZ = readWord(fdMotion, 0x3f) / 16384.0;

    read(fdGpio, gpioBuf, 16);
    gpioBuf[16] = 0;
    vlong gvals = strtoull(gpioBuf, nil, 16);
    int pin22 = gvals & (1 << 22);

    if(initialValX == 0 || pin22 != 0)
    {
      initialValX = accelX;
      initialValY = accelY;
      initialValZ = accelZ;
      print("Set reference x: %f, y: %f, z: %f\n",accelX,accelY,accelZ);
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

     //print("x: %f, y: %f, z: %f\n",accelX,accelY,accelZ);
     //print("Deviation from initial: %f\n",deviationFromInitial);
  
    if(deviationFromInitial > sensorThreshold)
    {
      fprint(fdGpio,"set 23 1");
    }
    else
    {
      fprint(fdGpio,"set 23 0");
    }

    sleep(100);
    i = i + 1;
  }
   fprint(fdGpio,"set 23 0");
   exits(0);
} 
