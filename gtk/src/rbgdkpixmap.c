/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgdkpixmap.c -

  $Author: mutoh $
  $Date: 2003/03/23 13:05:25 $

  Copyright (C) 2002,2003 Ruby-GNOME2 Project Team
  Copyright (C) 1998-2000 Yukihiro Matsumoto,
                          Daisuke Kanda,
                          Hiroshi Igarashi
************************************************/

#include "global.h"
#ifdef HAVE_XREADBITMAPFILEDATA
#include <X11/Xlib.h>
#endif /* HAVE_XREADBITMAPFILEDATA */

#define RVAL2COLOR(c) (NIL_P(c)?NULL:(GdkColor*)RVAL2BOXED(c, GDK_TYPE_COLOR))

static VALUE
gdkpmap_initialize(self, win, w, h, depth)
    VALUE self, win, w, h, depth;
{
	G_INITIALIZE(self, gdk_pixmap_new(GDK_WINDOW(RVAL2GOBJ(win)), 
                                          NUM2INT(w), NUM2INT(h), 
                                          NUM2INT(depth)));
	return Qnil;
}

static VALUE
gdkpmap_create_from_data(argc, argv, self)
    int argc;
    VALUE *argv;
    VALUE self;
{
    VALUE win, data, w, h, depth, fg, bg, ret;

    rb_scan_args(argc, argv, "43", &win, &data, &w, &h, &depth, &fg, &bg);

    Check_Type(data, T_STRING);
    if (NIL_P(depth)){
        ret = GOBJ2RVAL(gdk_bitmap_create_from_data(NIL_P(win) ? NULL : GDK_WINDOW(RVAL2GOBJ(win)),
                                                    RVAL2CSTR(data), NUM2INT(w), NUM2INT(h)));
    } else {
        ret = GOBJ2RVAL(gdk_pixmap_create_from_data(NIL_P(win) ? NULL : GDK_WINDOW(RVAL2GOBJ(win)),
                                                    RVAL2CSTR(data), NUM2INT(w), NUM2INT(h), 
                                                    NUM2INT(depth), RVAL2COLOR(fg), RVAL2COLOR(bg)));
    }
    return ret;
}

static VALUE
gdkpmap_create_from_xpm(self, win, color, fname)
    VALUE self, win, color, fname;
{
    GdkPixmap *new;
    GdkBitmap *mask;

    new = gdk_pixmap_create_from_xpm(GDK_WINDOW(RVAL2GOBJ(win)), &mask,
                          RVAL2COLOR(color), RVAL2CSTR(fname));
    if (!new) {
		rb_raise(rb_eArgError, "Pixmap not created from %s", RVAL2CSTR(fname));
    }
    return rb_assoc_new(GOBJ2RVAL(new),GOBJ2RVAL(mask));
}

static VALUE
gdkpmap_create_from_xpm_d(self, win, tcolor, data)
    VALUE self, win, tcolor, data;
{
    GdkPixmap *new;
    GdkBitmap *mask;
    int i;
    gchar **buf;

    Check_Type(data, T_ARRAY);
    buf = ALLOCA_N(char*, RARRAY(data)->len);
    for (i=0; i < RARRAY(data)->len; i++) {
		buf[i] = RVAL2CSTR(RARRAY(data)->ptr[i]);
    }
    new = gdk_pixmap_create_from_xpm_d(GDK_WINDOW(RVAL2GOBJ(win)), 
						   &mask, RVAL2COLOR(tcolor), buf);

    return rb_assoc_new(GOBJ2RVAL(new),GOBJ2RVAL(mask));
}

static VALUE
gdkpmap_colormap_create_from_xpm(self, win, colormap, tcolor, fname)
    VALUE self, win, colormap, tcolor, fname;
{
    GdkPixmap *new;
    GdkBitmap *mask;

    new = gdk_pixmap_colormap_create_from_xpm(NIL_P(win) ? NULL : GDK_WINDOW(RVAL2GOBJ(win)), 
                                              GDK_COLORMAP(RVAL2GOBJ(colormap)),
                                              &mask, RVAL2COLOR(tcolor),
                                              RVAL2CSTR(fname));
    if (!new) {
		rb_raise(rb_eArgError, "Pixmap not created from %s", RVAL2CSTR(fname));
    }
    return rb_assoc_new(GOBJ2RVAL(new),GOBJ2RVAL(mask));
}

static VALUE
gdkpmap_colormap_create_from_xpm_d(self, win, colormap, tcolor, data)
    VALUE self, win, colormap, tcolor, data;
{
    GdkPixmap *new;
    GdkBitmap *mask;
    int i;
    gchar **buf;

    Check_Type(data, T_ARRAY);
    buf = ALLOCA_N(char*, RARRAY(data)->len);
    for (i=0; i<RARRAY(data)->len; i++) {
	buf[i] = RVAL2CSTR(RARRAY(data)->ptr[i]);
    }

    new = gdk_pixmap_colormap_create_from_xpm_d(NIL_P(win) ? NULL : GDK_WINDOW(RVAL2GOBJ(win)),
                                                GDK_COLORMAP(RVAL2GOBJ(colormap)),
                                                &mask, RVAL2COLOR(tcolor), buf);

    return rb_assoc_new(GOBJ2RVAL(new),GOBJ2RVAL(mask));
}

#ifdef HAVE_XREADBITMAPFILEDATA
static VALUE
gdkpmap_create_from_xbm(self, win, fname)
    VALUE self, win, fname;
{
    GdkBitmap *new;
    unsigned char *data;
    unsigned int width, height;
    int x, y;

    Check_Type(fname, T_STRING);
    if (XReadBitmapFileData(RVAL2CSTR(fname), &width, &height, &data, &x, &y))
        rb_raise(rb_eArgError, "Bitmap not created from %s", RVAL2CSTR(fname));

    new = gdk_bitmap_create_from_data(GDK_WINDOW(RVAL2GOBJ(win)), 
                                      data, width, height);
    XFree(data);
    return GOBJ2RVAL(new);
}
#endif /* HAVE_XREADBITMAPFILEDATA */

void
Init_gtk_gdk_pixmap()
{
    VALUE gdkPixmap = G_DEF_CLASS(GDK_TYPE_PIXMAP, "Pixmap", mGdk);

    rb_define_method(gdkPixmap, "initialize", gdkpmap_initialize, 4);
    rb_define_singleton_method(gdkPixmap, "create_from_data", gdkpmap_create_from_data, -1);
    rb_define_singleton_method(gdkPixmap, "create_from_xpm", gdkpmap_create_from_xpm, 3);
    rb_define_singleton_method(gdkPixmap, "create_from_xpm_d", gdkpmap_create_from_xpm_d, 3);
    rb_define_singleton_method(gdkPixmap, "colormap_create_from_xpm", gdkpmap_colormap_create_from_xpm, 4);
    rb_define_singleton_method(gdkPixmap, "colormap_create_from_xpm_d", gdkpmap_colormap_create_from_xpm_d, 4);
#ifdef HAVE_XREADBITMAPFILEDATA
    rb_define_singleton_method(gdkPixmap, "create_from_xbm", gdkpmap_create_from_xbm, 2);
#endif /* HAVE_XREADBITMAPFILEDATA */
}
