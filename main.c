#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    Display *display;
    Window root;
    int opcode, event, error;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Failed to open display\n");
        exit(1);
    }

    if (!XQueryExtension(display, "XInputExtension", &opcode, &event, &error)) {
        fprintf(stderr, "X Input extension is not supported\n");
        exit(1);
    }

    int major = 2, minor = 0;
    if (XIQueryVersion(display, &major, &minor) == BadRequest) {
        fprintf(stderr, "Failed to requiest XInput2 v2.0\n");
        exit(1);
    }

    root = DefaultRootWindow(display);

    XIEventMask evmask;
    unsigned char mask[(XI_LASTEVENT + 7)/8] = { 0 };
    evmask.deviceid = XIAllMasterDevices;
    evmask.mask_len = sizeof(mask);
    evmask.mask = mask;
    XISetMask(mask, XI_RawButtonPress);
    XISetMask(mask, XI_RawButtonRelease);

    XISelectEvents(display, root, &evmask, 1);
    XFlush(display);

    while (1) {
        XEvent ev;
        XGenericEventCookie *cookie = &ev.xcookie;
        XNextEvent(display, &ev);

        if (cookie->type == GenericEvent && cookie->extension == opcode &&
            XGetEventData(display, cookie)) {
            XIRawEvent *re = (XIRawEvent*)cookie->data;
            if (cookie->evtype == XI_RawButtonPress && re->detail == 2) {
                printf("Whell click\n");
            }
            XFreeEventData(display, cookie);
        }
    }

    XCloseDisplay(display);
    return 0;
}