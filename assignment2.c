#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"

static long myledread(Chan*, void*, long, vlong);
static long myledwrite(Chan*, void*, long, vlong);

static long myledread(Chan*, void *buf, long n, vlong)
{
    char lbuf[20];
      char *e;
        e = lbuf + sizeof(lbuf);
          seprint(lbuf,e,"myleadread");
            return 0;
}

static long myledwrite(Chan*, void *buf, long n, vlong)
{
  return 0L;
}

void myledlink(void)
{
    addarchfile("myled",0666,myledread, myledwrite);
}
