#include <u.h>
#include <libc.h>
#define TIMING_LOOP_480_MICROS i=0; j=0; for(;i<125000;i++) { j++; }
#define TIMING_LOOP_60_MICROS i=0; j=0; for(;i<15000;i++) { j++; }
#define WRITE_ONE fprint(fd,"function 4 pulse"); TIMING_LOOP_60_MICROS
#define WRITE_ZERO fprint(fd,"function 4 out"); TIMING_LOOP_60_MICROS fprint(fd,"function 4 in");
void
main()
{
  int fd;
  int num_iter = 30000;
  vlong nsec1;
  vlong nsec2;
  int pin4;

  vlong nsec3;
  vlong nsec4;
  int i;
  int j;
  char buf[17];

  i = 0;
  j = 0;
  pin4 = -1;

  nsec1 = nsec();
  TIMING_LOOP_60_MICROS
  nsec2 = nsec();
  print("Elapsed micros: %d\n",(nsec2-nsec1)/1000);

  nsec1 = nsec();
  TIMING_LOOP_480_MICROS
  nsec2 = nsec();
  print("Elapsed micros: %d\n",(nsec2-nsec1)/1000);

  bind("#G","/dev",MAFTER);
  fd = open("/dev/gpio",ORDWR);
  fprint(fd,"function 4 out");

  // STEP 1: We send a reset pulse and expect a presence pulse back
  nsec3 = nsec();
  // First pull low
  fprint(fd,"set 4 0");
  // wait for minimum of 480us
  TIMING_LOOP_480_MICROS
  // now let go of bus...
  fprint(fd,"function 4 in");
  // Now, for between 60 and 300 us, the sensor pulls low (presence pulse)
  TIMING_LOOP_60_MICROS
  // We now read and expect low
  read(fd, buf, 16);
  buf[16] = 0;
  vlong gvals = strtoull(buf, nil, 16);
  int pin4 = gvals & (1 << 4);
  nsec4 = nsec();


  // STEP 2: Issue a ROM command (skip ROM, in this case 0xCC)
  // In binary, 0xCC is 11001100
  WRITE_ONE
  WRITE_ONE
  WRITE_ZERO
  WRITE_ZERO
  WRITE_ONE
  WRITE_ONE
  WRITE_ZERO
  WRITE_ZERO

  // Step 3:



  print("Elapsed now is: %d\n",(nsec4-nsec3)/1000);

  print("Read from bus a: %d\n",pin4);


  int mode = 0;

  while(0)
  {
    read(fd, buf, 16);
    buf[16] = 0;
    vlong gvals = strtoull(buf, nil, 16);
    int pin4 = gvals & (1 << 4);
    print("%d",pin4);
    print("\n");
    sleep(1000);
   }
   exits(0);
} 