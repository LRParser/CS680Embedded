#include <u.h>
#include <libc.h>
void
main()
{
  int fd;
  bind("#G","/dev/gpio",MAFTER);
  fd = open("/dev/gpio",ORDWR);
  fprint(fd,"function 27 out");
  fprint(fd,"function 22 in");
  char buf[17];

  int mode = 0;

  while(1)
  {
    read(fd,buf,16);
    buf[16] = 0;
    vlong gvals = strtoull(buf, nil, 16);
    int pin22 = gvals & (1 << 22);
    if (pin22 != 0)
    {
      mode = mode + 1;
      mode = mode % 3;
    }

    if(mode == 0)
    {
      fprint(fd,"set 27 0");
    }
    else if(mode == 1)
    {
      fprint(fd,"set 27 1");
    }
    else {
      fprint(fd, "set 27 1");
      sleep(300);
      fprint(fd, "set 27 0");
      sleep(300);
    }
  }
  exits(0);
}
