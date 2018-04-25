#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"

static long myledread(Chan*, void*, long, vlong);
static long myledwrite(Chan*, void*, long, vlong);
static int state = 0;

static void processstate(void*)
{
    if(state == 1)
        {
              gpiosel(27, Output);
                  gpioout(27, 1);
                    }
      else if(state == 2)
          {
                gpiosel(27, Output);
                    gpioout(27, 0);
                      }
        else if(state == 3)
            {
                  gpiosel(27, Output);
                      while(1)
                            {
                                    print("Blink Turn on\n");
                                          gpioout(27,1);
                                                tsleep(&up->sleep, return0, 0, 1000);
                                                      print("Blink Turn off\n");
                                                            gpioout(27,0);
                                                                  tsleep(&up->sleep, return0, 0, 1000);
                                                                      }
                        }
}

static long myledread(Chan*, void *buf, long n, vlong)
{
    char lbuf[20];
      char *e;
        e = lbuf + sizeof(lbuf);
          print("State %d\n",state);
            seprint(lbuf,e,"State: %d\n",state);
              return readstr(0,buf,n,lbuf);
}


static long myledwrite(Chan*, void *a, long n, vlong)
{
    if(strncmp(a,"start",5) == 0) {
          print("Got start command\n");
              kproc("myled", processstate, nil);
                }
      else if(strncmp(a,"stop",4) == 0) {
            print("Got stop command\n");
              }
        else if(strncmp(a,"on",2) == 0) {
              print("Got on command\n");
                  state = 1;
                    }
          else if(strncmp(a,"off",3) == 0) {
                print("Got off command\n");
                   state = 2;
                     }
            else if(strncmp(a,"blink",5) == 0) {
                  print("Got blink command\n");
                      state = 3;
                        }
              print("Exit myledblink function\n");
                return n;
}

void myledlink(void)
{
    addarchfile("myled",0666,myledread, myledwrite);
}
