#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_LENGTH 10

int main(int argc, char *argv[]){
  //XInitThreads();
  Display *mydisplay;
  char **fontNames;
  Window baseWindow, textWindow1, textWindow2, textWindow3;
  XSetWindowAttributes myat;
  XSizeHints wmsize;
  XWMHints wmhints;
  XTextProperty windowName, iconName;
  XEvent baseEvent;
  GC mygc, myGC1, myGC2, myGC3;
  XGCValues myGCvalues;
  KeySym sym;

  //font and label
  XFontStruct *font1, *font2;
  XTextItem myText;
  char *window_name = "Text Echoing";
  char *icon_name = "Ec";
  char *label1 = "TextBox A: ";
  char *label2 = "TextBox B: ";
  char *label3 = "All here: "; //

  int screen_num, done;
  unsigned long mymask;
  int x, i;

  int yWindow1, yWindow2, yWindow3, width; //

  char buffer[BUFFER_LENGTH];

  // 1. open connection to the server
  mydisplay = XOpenDisplay("");

  // 2. create a top-level window
  screen_num = DefaultScreen(mydisplay);
  myat.border_pixel = BlackPixel(mydisplay, screen_num);
  myat.background_pixel = WhitePixel(mydisplay, screen_num);
  //myat.event_mask = KeyPressMask | ButtonPressMask | ExposureMask;
  myat.event_mask = ExposureMask;
  mymask = CWBackPixel | CWBorderPixel | CWEventMask;
  baseWindow = XCreateWindow(mydisplay,
                             RootWindow(mydisplay, screen_num),
                             300, 400, 550, 400, 2,
                             DefaultDepth(mydisplay, screen_num),
                             InputOutput,
                             DefaultVisual(mydisplay, screen_num),
                             mymask, &myat);

  //3. give the Window Manager hints
  wmsize.flags = USPosition | USSize;
  XSetWMNormalHints(mydisplay, baseWindow, &wmsize);
  wmhints.initial_state = NormalState;
  wmhints.flags = StateHint;
  XSetWMHints(mydisplay, baseWindow, &wmhints);
  XStringListToTextProperty(&window_name, 1, &windowName);
  XSetWMName(mydisplay, baseWindow, &windowName);
  XStringListToTextProperty(&icon_name, 1, &iconName);
  XSetWMIconName(mydisplay, baseWindow, &iconName);

  // 4. establish window resources
  XRebindKeysym(mydisplay, XK_Meta_L, NULL, 0, "MetaL", 5);
  //
  myGCvalues.background = WhitePixel(mydisplay, screen_num);
  myGCvalues.foreground = BlackPixel(mydisplay, screen_num);
  mymask = GCForeground | GCBackground;
  mygc = XCreateGC(mydisplay, baseWindow, mymask, &myGCvalues);

  //font1 = XLoadQueryFont(mydisplay,
  //                       "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*");
  font1 = XLoadQueryFont(mydisplay,
                         "-b&h-lucidatypewriter-medium-r-normal-sans-14-100-100-100-m-80-iso10646-1");

  font2 = XLoadQueryFont(mydisplay,
                         "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*");

  myGC1 = XCreateGC(mydisplay, baseWindow, mymask, &myGCvalues); //

  // 5. create all the other windows needed
  mymask = CWBackPixel | CWBorderPixel | CWEventMask;
  textWindow3 = XCreateWindow(
                  mydisplay, baseWindow,
                  140, 170, 300, 180, 2,
                  DefaultDepth(mydisplay, screen_num),
                  InputOutput,
                  DefaultVisual(mydisplay, screen_num),
                  mymask, &myat);

  myat.event_mask = KeyPressMask | ButtonPressMask | ExposureMask;
  textWindow1 = XCreateWindow(mydisplay, baseWindow,
                  140, 50, 200, 20, 2,
                  DefaultDepth(mydisplay, screen_num),
                  InputOutput,
                  DefaultVisual(mydisplay, screen_num),
                  mymask, &myat);
  textWindow2 = XCreateWindow(mydisplay, baseWindow,
                  140, 110, 200, 20, 2,
                  DefaultDepth(mydisplay, screen_num),
                  InputOutput,
                  DefaultVisual(mydisplay, screen_num),
                  mymask, &myat);
  //6. select event for each window
  //7. map the windows
  XMapWindow(mydisplay, baseWindow);
  XMapWindow(mydisplay, textWindow1);
  XMapWindow(mydisplay, textWindow2);
  XMapWindow(mydisplay, textWindow3);

  // detect wm_delete_window
  Atom wm_delete_window =
         XInternAtom(mydisplay, "WM_DELETE_WINDOW", False);

  XSetWMProtocols(mydisplay, baseWindow, &wm_delete_window, 1);

  //8 enter the event loop
  done = 0;
  //
  yWindow1 = yWindow2 = yWindow3 = 0;
  myText.chars = buffer;
  myText.nchars = 1; //

  while (done == 0){
    XNextEvent(mydisplay, &baseEvent);
    switch (baseEvent.type){
      case ClientMessage:
        if ((Atom)baseEvent.xclient.data.l[0] == wm_delete_window)
          done = 1;
        break;
      case Expose:
        //
        XDrawImageString(mydisplay, baseWindow, mygc,
                         75, 65, label1, strlen(label1));
        XDrawImageString(mydisplay, baseWindow, mygc,
                         75, 125, label2, strlen(label2));
        XDrawImageString(mydisplay, baseWindow, mygc,
                         78, 185, label3, strlen(label3));//
        break;
      case ButtonPress:
        //printf("I got a button press\n");
        break;
      case KeyPress:
        printf("key: %d\n", baseEvent.xkey.keycode);
        if (baseEvent.xkey.keycode == 88){
          done = 1;
          break;
        }
        x = XLookupString(&baseEvent.xkey, buffer, BUFFER_LENGTH, &sym, NULL);
        //sym = XKeycodeToKeysym(mydisplay, baseEvent.xkey.keycode, 1);
        sym = XkbKeycodeToKeysym(mydisplay, baseEvent.xkey.keycode, 0, baseEvent.xkey.state&ShiftMask?1:0);
        if (baseEvent.xkey.window == textWindow1){
          myText.font = font1->fid;
          //XDrawText(mydisplay, textWindow1, myGC1, yWindow1, 15, &myText, 1);
          XLockDisplay(mydisplay);
          XDrawString(mydisplay, textWindow1, myGC1, yWindow1, 15, myText.chars, 1);
          XUnlockDisplay(mydisplay);
          width = XTextWidth(font1, buffer, 1);
          yWindow1 += width;
          if (baseEvent.xkey.keycode == 9){
            XClearWindow(mydisplay, textWindow1);
            yWindow1 = 0;
          }
        }
        if (baseEvent.xkey.window == textWindow2){
          myText.font = font2->fid;
          XLockDisplay(mydisplay);
          XDrawString(mydisplay, textWindow2, myGC1, yWindow2, 15, myText.chars, 1);
          //XDrawText(mydisplay, textWindow2, myGC1, yWindow2, 15, &myText, 1);
          XUnlockDisplay(mydisplay);
          width = XTextWidth(font1, buffer, 1);
          yWindow2 += width;
        }
        XLockDisplay(mydisplay);
        //XDrawText(mydisplay, textWindow3, myGC1, yWindow3, 15, &myText, 1);
        XDrawString(mydisplay, textWindow3, myGC1, yWindow3, 15, myText.chars, 1);
        XUnlockDisplay(mydisplay);
        yWindow3 += width;
        break;
    }
  }
  //9. clean up before exiting
  XUnmapWindow(mydisplay, baseWindow);
  XUnmapWindow(mydisplay, textWindow1);
  XUnmapWindow(mydisplay, textWindow2);
  
  return 0;
}

