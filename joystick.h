#include <stdio.h>
#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#define JOYSTICK_DEVNAME "/dev/input/js0"

#define JS_EVENT_BUTTON         0x01    /* button pressed/released */
#define JS_EVENT_AXIS           0x02    /* joystick moved */
#define JS_EVENT_INIT           0x80    /* initial state of device */


struct js_event {
        unsigned int time;      /* event timestamp in milliseconds */
        short value;   		/* value */
        unsigned char type;     /* event type */
        unsigned char number;   /* axis/button number */
};

struct wwvi_js_event {
        int button[11];
        int stick_x;
        int stick_y;
};

extern int open_joystick(char *joystick_device);
extern int read_joystick_event(struct js_event *jse);
extern void set_joystick_y_axis(int axis);
extern void set_joystick_x_axis(int axis);
extern void close_joystick();
extern int get_joystick_status(struct wwvi_js_event *wjse);

#endif
