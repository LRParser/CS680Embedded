#include <u.h>
#include <libc.h>

#define WRITE_ONE fprint(fd,"function 4 pulse"); delaymicros(90);
#define WRITE_ZERO fprint(fd,"function 4 out"); delaymicros(90); fprint(fd,"function 4 in"); delaymicros(1);
#define WRITE_SKIP_ROM WRITE_ZERO WRITE_ZERO WRITE_ONE WRITE_ONE WRITE_ZERO WRITE_ZERO WRITE_ONE WRITE_ONE
#define WRITE_CONVERT_T   WRITE_ZERO WRITE_ZERO WRITE_ONE WRITE_ZERO WRITE_ZERO WRITE_ZERO WRITE_ONE WRITE_ZERO
#define WRITE_READ_SCRATCHPAD WRITE_ZERO WRITE_ONE WRITE_ONE WRITE_ONE WRITE_ONE WRITE_ONE WRITE_ZERO WRITE_ONE

void delaymicros(int micros)
{
  int elapsedmicros;
  vlong start;

  elapsedmicros = 0;
  start = nsec();
  while(1)
  {
    vlong now = nsec();
    int elapsedmicros = (now - start) / 1000;
    if (elapsedmicros >= micros) {
      //print("Elapsed %d with input of %d\n",elapsedmicros,micros);
      break;
    }
  }
}

void doInit(int fd)
{
  vlong nsec1;
  vlong nsec2;
  int pdval;
  int pinval;
  char buf[17];
  vlong gvals;
  int pres_us_elapsed;
  pdval = -1;
  pinval = -1;
  pres_us_elapsed = 0;
// First pull low
  fprint(fd,"function 4 out");
  fprint(fd,"set 4 0");
  // wait for minimum of 480us
  delaymicros(480);
  // now let go of bus...
  fprint(fd,"function 4 in");
  nsec1 = nsec();
  delaymicros(1);
  // The line should go high between 15 and 60 us
  read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); pdval = (gvals & (1 << 4)) == 0 ? 0 : 1;
  while(pdval != 1)
  {
    read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); pdval = (gvals & (1 << 4)) == 0 ? 0 : 1;
  }
  //print("Read pdval 1\n");
  // Now, we expact that for between 60 and 300 us, the sensor pulls low (presence pulse)
  while(pdval != 0)
  {
    read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); pdval = (gvals & (1 << 4)) == 0 ? 0 : 1;
  }
  //print("Read pdval 0\n");
  // We wait until the line goes high again (presence detect is over)
  while(pinval != 1)
  {
    read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); pinval = (gvals & (1 << 4)) == 0 ? 0 : 1;
  }
  nsec2 = nsec();
  pres_us_elapsed = (nsec2-nsec1) / 1000;
  delaymicros(480 - pres_us_elapsed);
  //print("Had to delay another %d micros\n",480-pres_us_elapsed);

}

void readtemp(void)
{
  int fd;
  vlong nsec1;
  vlong nsec2;
  int pdval;
  int pres_us_elapsed;
  int pinval;
  int convval;
  int conv_us_elapsed;
  int templsb0;
  int templsb1;
  int templsb2;
  int templsb3;
  int templsb4;
  int templsb5;
  int templsb6;
  int templsb7;

  int tempmsb0;
  int tempmsb1;
  int tempmsb2;
  int tempmsb3;
  int tempmsb4;
  int tempmsb5;
  int tempmsb6;
  int tempmsb7;
  int total_sum;

  int lsbval;
  int msbval;
  int tempunits;
  float celsiustemp;
  long fulltemp1;
  long fulltemp2;

  vlong gvals;

  vlong nsec3;
  vlong nsec4;
  int i;
  int j;
  int k;
  int l;
  char buf[17];

  i = 0;
  j = 0;
  k = 0;
  l = 0;
  lsbval = 0;
  msbval = 0;
  tempunits = 0;
  pdval = -1;
  pinval = -1;
  convval = -1;
  conv_us_elapsed = 0;
  pres_us_elapsed = -1;

  bind("#G","/dev",MAFTER);
  fd = open("/dev/gpio",ORDWR);
  
  // STEP 1: We send a reset pulse and expect a presence pulse back
  doInit(fd);
  
  //delaymicros(100);

  // STEP 2: Issue a ROM command (skip ROM, in this case 0xCC)
  // In binary, 0xCC is 11001100, LSB first this is 00110011
 WRITE_SKIP_ROM
  // Step 3: Issue a function command (convert T, in this case 0x44)
  // In binary, 0x44 is 01000100, LSB first this is 00100010
  WRITE_CONVERT_T

  // For default 12-bit resolution, we might need to wait up to 750ms

  delaymicros(750*1000);

  // Step 3.5: Now we need to do another reset
  doInit(fd);

  // Step 4: Issue another ROM command (skip ROM)
  WRITE_SKIP_ROM

  // Step 5: Issue a function command (read scratchpad, in this case 0xBE)
  // In binary, 0xBE is 10111110, LSB first this is 01111101
  WRITE_READ_SCRATCHPAD

  fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); templsb0 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
  fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); templsb1 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); templsb2 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); templsb3 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); templsb4 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); templsb5 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); templsb6 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); templsb7 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);

  fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); tempmsb0 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
  fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); tempmsb1 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); tempmsb2 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); tempmsb3 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); tempmsb4 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); tempmsb5 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); tempmsb6 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);
   fprint(fd,"function 4 pulse"); delaymicros(2); read(fd, buf, 16); buf[16] = 0; gvals = strtoull(buf, nil, 16); tempmsb7 = (gvals & (1 << 4)) == 0 ? 0 : 1; delaymicros(60);

  print("Temp read is: %d%d%d%d%d%d%d%d%d%d%d%d\n",tempmsb3,tempmsb2,tempmsb1,tempmsb0,templsb7,templsb6,templsb5,templsb4,templsb3,templsb2,templsb1,templsb0);

  lsbval = (templsb0 << 0) | (templsb1 << 1) | (templsb2 << 2) | (templsb3 << 3) | (templsb4 << 4) | (templsb5 << 5) | (templsb6 << 6) | (templsb7 << 7);
  msbval = (tempmsb0 == 0 ? 0 : 256) + (tempmsb1 == 0 ? 0 : 512) + (tempmsb2 == 0 ? 0 : 1024) + (tempmsb3 == 0 ? 0 : 2048);
  celsiustemp = (msbval + lsbval) * .0625;
  print("Temp in C: %f\n",celsiustemp);
} 

void main()
{
  print("Initial temp\n");
  readtemp();
  print("Now, place your finger on the sensor for 10 seconds; we will now read the temperature again\n");
  sleep(10000);
  readtemp();
}