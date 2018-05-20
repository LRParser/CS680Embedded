#include <u.h>
#include <libc.h>
void
main()
{
   int fd;
   int fd2;
  char cmdcode = 0x00;
  char oncode = 0xAF;
  char offcode = 0xAE;
  char alloncode = 0xA5;
  char* mycmds = malloc(2*sizeof(char));
  mycmds[0] = cmdcode;
  mycmds[1] = oncode;
  int bindret = bind("#J3c","/dev",MAFTER);
  print("bind ret: %d\n",bindret);
  fd = open("/dev/i2c.3c.data",ORDWR);
  print("%d\n",fd);
  fd2 = open("/dec/i2c.3c.ctl",ORDWR);
  int ret1 = write(fd,mycmds,2);
  while(1)
  {
    mycmds[0] = cmdcode; // cmdcode
    mycmds[1] = offcode;
    write(fd,mycmds,2);
    sleep(6400);
    mycmds[0] = cmdcode; // cmdcode
    mycmds[1] = oncode;
    write(fd,mycmds,2);
    mycmds[0] = cmdcode; // cmdcode
    mycmds[1] = alloncode;
    write(fd,mycmds,2);
    sleep(6400);
   }
    print("Done\n");
   exits(0);
} 
