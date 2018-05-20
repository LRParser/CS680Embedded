#include "u.h"
#include "../port/lib.h"
#include "../port/error.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"

static long myledread(Chan*, void*, long, vlong);
static long myledwrite(Chan*, void*, long, vlong);

static int serverstate = 0;
static int state = 0;
static int blinkstate = 0;

static void processstate(void*)
{
      while(serverstate)
            {
                    int readval = 0;
                          readval = gpioin(22);
                                if (readval == 1)
                                        {
                                                  state++;
                                                          state = state % 3;
                                                                }
                                    if(state == 0)
                                          {
                                                  gpioout(27,0);
                                                      }
                                        else if(state == 1)
                                              {
                                                      gpioout(27,1);
                                                          }
                                            else if(state == 2)
                                                  {
                                                          if(blinkstate == 0)
                                                                  {
                                                                            print("Blink Turn on\n");
                                                                                    gpioout(27,1);
                                                                                            blinkstate = 1;
                                                                                                  }
                                                                else 
                                                                        {
                                                                                  print("Blink Turn off\n");
                                                                                          gpioout(27,0);
                                                                                                  blinkstate = 0;
                                                                                                        }
                                                                      print("Sleep in processstate\n");
                                                                            tsleep(&up->sleep, return0, 0, 1000);
                                                                                }
                                             }
}

static long myledread(Chan*, void *buf, long n, vlong)
{
    char lbuf[20];
      char *e;
        e = lbuf + sizeof(lbuf);
          if (serverstate == 0)
              {
                    if(state == 0)
                          {
                                    seprint(lbuf,e,"LED Off, Scan Off");
                                        }
                        else if(state == 1)
                              {
                                        seprint(lbuf,e,"LED On, Scan Off");
                                            }
                            else if(state == 2)
                                  {
                                           seprint(lbuf,e,"LED Blink, Scan Off");
                                               }
                              }
            else if(serverstate == 1)
                {
                      if(state == 0)
                            {
                                    seprint(lbuf,e,"LED Off, Scan On");
                                        }
                          else if(state == 1)
                                {
                                         seprint(lbuf,e,"LED On, Scan On");
                                             }
                              else if(state == 2)
                                     {
                                               seprint(lbuf,e,"LED Blink, Scan On");
                                                    }
                                }
              
              return readstr(0,buf,n,lbuf);
}


static long myledwrite(Chan*, void *a, long n, vlong)
{

    // We always want 27 out, 22 in
    //   gpiosel(27, Output);
    //     gpiosel(22, Input);
    //
    //       if(strncmp(a,"start",5) == 0) {
    //           print("Got start command\n");
    //               serverstate = 1;
    //                   kproc("myled", processstate, nil);
    //                     }
    //                       else if(strncmp(a,"stop",4) == 0) {
    //                           // Stop the scanning and turn off the LED.
    //                               print("Got stop command\n");
    //                                   state = 2;
    //                                       serverstate = 0;
    //                                           gpioout(27,0);
    //                                             }
    //                                               else if(strncmp(a,"on",2) == 0) {
    //                                                   print("Got on command\n");
    //                                                       state = 1;
    //                                                           gpioout(27,1);
    //                                                             }
    //                                                               else if(strncmp(a,"off",3) == 0) {
    //                                                                   print("Got off command\n");
    //                                                                      state = 2;
    //                                                                         gpioout(27,0);
    //                                                                           }
    //                                                                             else if(strncmp(a,"blink",5) == 0) {
    //                                                                                 print("Got blink command\n");
    //                                                                                     state = 3;
    //                                                                                         kproc("myled", processstate, nil);
    //                                                                                           }
    //                                                                                             print("Exit myledblink function\n");
    //                                                                                               return n;
    //                                                                                               }
    //
    //                                                                                               void myledlink(void)
    //                                                                                               {
    //                                                                                                 addarchfile("myled",0666,myledread, myledwrite);
    //                                                                                                 }
