/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgdkcursor.c -

  $Author: mutoh $
  $Date: 2003/08/29 19:14:53 $

  Copyright (C) 2001-2003 Masao Mutoh
************************************************/

#include "global.h"

static VALUE
gdkcursor_initialize(argc, argv, self)
    int argc;
    VALUE *argv;
    VALUE self;
{
    GdkCursor* cursor;
    VALUE source_or_type, mask, fg, bg, x, y;

    rb_scan_args(argc, argv, "15", &source_or_type, &mask, &fg, &bg, &x, &y);
    if (argc == 1){
        cursor = gdk_cursor_new(RVAL2GENUM(source_or_type, GDK_TYPE_CURSOR_TYPE));
    } else {
        cursor = gdk_cursor_new_from_pixmap(GDK_PIXMAP(RVAL2GOBJ(source_or_type)), 
                                            NIL_P(mask)?NULL:GDK_PIXMAP(RVAL2GOBJ(mask)), 
                                            NIL_P(fg)?NULL:(GdkColor*)RVAL2BOXED(fg, GDK_TYPE_COLOR), 
                                            NIL_P(bg)?NULL:(GdkColor*)RVAL2BOXED(bg, GDK_TYPE_COLOR), 
                                            NUM2INT(x), NUM2INT(y));
    }
    G_INITIALIZE(self, cursor);
       
    return Qnil;
}

static VALUE
gdkcursor_is_pixmap(self)
    VALUE self;
{
    return (((GdkCursor*)RVAL2BOXED(self, GDK_TYPE_COLOR))->type == 
            GDK_CURSOR_IS_PIXMAP) ? Qtrue : Qfalse;
}

void
Init_gtk_gdk_cursor()
{
    VALUE gdkCursor = G_DEF_CLASS(GDK_TYPE_CURSOR, "Cursor", mGdk);

    rb_define_method(gdkCursor, "initialize", gdkcursor_initialize, -1);
    rb_define_method(gdkCursor, "pixmap?", gdkcursor_is_pixmap, 0);

    G_DEF_CLASS(GDK_TYPE_CURSOR_TYPE, "Type", gdkCursor);
    G_DEF_CONSTANTS(gdkCursor, GDK_TYPE_CURSOR_TYPE, "GDK_");
}
