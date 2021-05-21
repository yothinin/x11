#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>

struct{
  int width;
  int height;
  char *text;
  int text_len;

  /* X Windows related variables. */
  Display *display;
  int screen;
  Window root;
  Window window;
  GC gc;
  XFontStruct *font;
  unsigned long black_pixel;
  unsigned long white_pixel;
} text_box;

static void labelText(char *label, int x, int y){
  int labelLen = strlen(label);
  int direction, ascent, descent;
  XCharStruct overall;

  XTextExtents(text_box.font, label, labelLen,
              &direction, &ascent, &descent, &overall);

  y += ascent+descent;

  XClearWindow(text_box.display, text_box.window);
  XDrawString(text_box.display, text_box.window, text_box.gc,
              x, y, label, labelLen);
}

/* Connect to the display, setup the basic variables. */
static void x_connect(){
  text_box.display = XOpenDisplay(NULL);
  if (!text_box.display){
    fprintf(stderr, "Could not open display.\n");
    exit(1);
  }
  text_box.screen = DefaultScreen(text_box.display);
  text_box.root = RootWindow(text_box.display, text_box.screen);
  text_box.black_pixel = BlackPixel(text_box.display, text_box.screen);
  text_box.white_pixel = WhitePixel(text_box.display, text_box.screen);
}

/* Create the window */
static void create_window(){
  text_box.width = 300;
  text_box.height = 300;
  text_box.window =
    XCreateSimpleWindow(text_box.display, text_box.root,
                        1, 1, text_box.width, text_box.height, 0,
                        text_box.black_pixel, text_box.white_pixel);
  XSelectInput(text_box.display, text_box.window,
               ExposureMask | StructureNotifyMask);
  XMapWindow(text_box.display, text_box.window);
}

/* Setup the GC (Graphics Context). */
static void set_up_gc(){
  text_box.screen = DefaultScreen(text_box.display);
  text_box.gc = XCreateGC(text_box.display, text_box.window, 0, 0);
  XSetBackground(text_box.display, text_box.gc, text_box.white_pixel);
  XSetForeground(text_box.display, text_box.gc, text_box.black_pixel);
}

/* Setup the text font. */
static void set_up_font(){
  const char *fontname = "-adobe-helvetica-medium-r-*-*-24-*-*-*-*-*-*-*";
  text_box.font = XLoadQueryFont(text_box.display, fontname);
  /* If the font could not be loaded, revert to the "fixed" font. */
  if (!text_box.font){
    fprintf(stderr, "Unable to load font %s: using fixed\n", fontname);
    text_box.font = XLoadQueryFont(text_box.display, "fixed");
  }
  XSetFont(text_box.display, text_box.gc, text_box.font->fid);
}

static void draw_screen(){
  int x, y, direction, ascent, descent;
  XCharStruct overall;

  /* Center the text in the middle of the box. */
  XTextExtents(text_box.font, text_box.text, text_box.text_len,
                &direction, &ascent, &descent, &overall);
  x = (text_box.width - overall.width) / 2;
  y = (text_box.height / 2) + (ascent - descent) / 2;
  XClearWindow(text_box.display, text_box.window);
  XDrawString(text_box.display, text_box.window, text_box.gc,
              x, y, text_box.text, text_box.text_len);
}

/* Loop over events. */
static void event_loop(){

  Atom wm_delete_window;
  wm_delete_window =
    XInternAtom(text_box.display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(text_box.display, text_box.window, &wm_delete_window, 1);

  while(1){
    XEvent e;
    XNextEvent(text_box.display, &e);
    switch (e.type){
      case Expose:
        labelText("Hello, Yothin", 0, 0);
        //draw_screen();
        break;
      case ClientMessage: //Delete message from Window Manager.
        if ((Atom)e.xclient.data.l[0] == wm_delete_window)
          exit(0);
        break;
      case ConfigureNotify:;
        // Respond to ConfigureNotify when the window is resized.
        XConfigureEvent xce = e.xconfigure;
        if (xce.width != text_box.width || xce.height != text_box.height){
          text_box.width = xce.width;
          text_box.height = xce.height;
          draw_screen();
        }
        break;
      case KeyPress:
        labelText("Hello, Yothin", 300, 300);
        break;
      default:
        break;
    }
  }
}

int main(int arac, char **argv){
  text_box.text = "Hello World!";
  text_box.text_len = strlen(text_box.text);
  x_connect();
  create_window();
  set_up_gc();
  set_up_font();
  event_loop();
  return 0;
}
