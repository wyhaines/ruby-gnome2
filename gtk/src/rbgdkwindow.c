/* -*- c-file-style: "ruby" -*- */
/************************************************

  rbgdkwindow.c -

  $Author: mutoh $
  $Date: 2002/06/22 03:14:53 $

  Copyright (C) 1998-2000 Yukihiro Matsumoto,
                          Daisuke Kanda,
                          Hiroshi Igarashi
************************************************/

#include "global.h"

/*
 * Gdk::Window
 */

VALUE gdkWindow;

static VALUE
gdkwin_get_size(self)
    VALUE self;
{
    int width, height;
    gdk_window_get_size(get_gdkwindow(self), &width, &height);
    return rb_assoc_new(INT2NUM(width), INT2NUM(height));
}

static VALUE
gdkwin_get_pointer(self)
    VALUE self;
{
    int x, y;
    GdkModifierType state;
    gdk_window_get_pointer(get_gdkwindow(self), &x, &y, &state);
    return rb_ary_new3(3, INT2FIX(x), INT2FIX(y), INT2FIX((int)state));

}

static VALUE
gdkwin_pointer_grab(self, owner_events, event_mask, confine_to, cursor, time)
    VALUE self, owner_events, event_mask, confine_to, cursor, time;
{
    gdk_pointer_grab(get_gdkwindow(self),
		     RTEST(owner_events),
		     NUM2INT(event_mask),
		     get_gdkwindow(confine_to),
		     get_gdkcursor(cursor),
		     NUM2INT(time));
    return self;
}

static VALUE
gdkwin_pointer_ungrab(self, time)
    VALUE self, time;
{
    gdk_pointer_ungrab(NUM2INT(time));
    return self;
}

static VALUE
gdkwin_pointer_is_grabbed(self)
    VALUE self;
{
    return (gdk_pointer_is_grabbed() ? Qtrue : Qfalse);
}

static VALUE
gdkwin_keyboard_grab(self, owner_events, time)
    VALUE self, owner_events, time;
{
    gdk_keyboard_grab(get_gdkwindow(self),
		     RTEST(owner_events),
		     NUM2INT(time));
    return self;
}

static VALUE
gdkwin_keyboard_ungrab(self, time)
    VALUE self, time;
{
    gdk_keyboard_ungrab(NUM2INT(time));
    return self;
}

static VALUE
gdkwin_foreign_new(self, anid)
    VALUE self, anid;
{
    GdkWindow *window;
    window = gdk_window_foreign_new(NUM2INT(anid));
    return make_gdkwindow(window);
}

static VALUE
gdkwin_root_window(self)
    VALUE self;
{
    return INT2NUM(GDK_ROOT_WINDOW());
}

static VALUE
gdkwin_clear(self)
    VALUE self;
{
    gdk_window_clear(get_gdkwindow(self));
    return self;
}

static VALUE
gdkwin_clear_area(self, x,y,w,h)
    VALUE self,x,y,w,h;
{
    gdk_window_clear_area(get_gdkwindow(self),
			  NUM2INT(x), NUM2INT(y), NUM2INT(w), NUM2INT(h));
    return self;
}

static VALUE
gdkwin_copy_area(self, gc, x, y, src_window, src_x, src_y, width, height)
	VALUE self, gc, x, y, src_window, src_x, src_y, width, height;
{
	gdk_window_copy_area(get_gdkwindow(self), get_gdkgc(gc),
						 NUM2INT(x), NUM2INT(y), get_gdkwindow(src_window),
						 NUM2INT(src_x), NUM2INT(src_y), NUM2INT(width), NUM2INT(height));
	return self;
}

static VALUE
gdkwin_clear_area_e(self, x,y,w,h)
    VALUE self,x,y,w,h;
{
    gdk_window_clear_area_e(get_gdkwindow(self),
			    NUM2INT(x), NUM2INT(y), NUM2INT(w), NUM2INT(h));
    return self;
}

static VALUE
gdkwin_set_background(self, color)
    VALUE self, color;
{
    gdk_window_set_background(get_gdkwindow(self), get_gdkcolor(color));
    return self;
}

static VALUE
gdkwin_set_back_pixmap(self, pixmap, parent_relative)
    VALUE self, pixmap, parent_relative;
{
    gdk_window_set_back_pixmap(get_gdkwindow(self), get_gdkpixmap(pixmap),
			       RTEST(parent_relative));
    return self;
}

static VALUE
gdkwin_move(self, x,y)
    VALUE self, x,y;
{
    gdk_window_move(get_gdkwindow(self),
		    NUM2INT(x), NUM2INT(y));
    return self;
}

static VALUE
gdkwin_raise(self)
    VALUE self;
{
    gdk_window_raise(get_gdkwindow(self));
    return self;
}

static VALUE
gdkwin_lower(self)
    VALUE self;
{
    gdk_window_lower(get_gdkwindow(self));
    return self;
}

static VALUE
gdkwin_register_dnd(self)
    VALUE self;
{
    gdk_window_register_dnd(get_gdkwindow(self));
    return self;
}

static VALUE
gdkwin_set_override_redirect(self, override_redirect)
    VALUE self, override_redirect;
{
    gdk_window_set_override_redirect(get_gdkwindow(self), 
									 RTEST(override_redirect));
    return self;
}

static VALUE
gdkwin_shape_combine_mask(self, shape_mask, offset_x, offset_y)
    VALUE self, shape_mask, offset_x, offset_y;
{
    gdk_window_shape_combine_mask(get_gdkwindow(self), 
								  get_gdkbitmap(shape_mask), 
								  NUM2INT(offset_x), NUM2INT(offset_y));
    return self;
}

static VALUE
gdkwin_set_child_shapes(self)
    VALUE self;
{
    gdk_window_set_child_shapes(get_gdkwindow(self));
    return self;
}

static VALUE
gdkwin_merge_child_shapes(self)
    VALUE self;
{
    gdk_window_merge_child_shapes(get_gdkwindow(self));
    return self;
}   

static VALUE
gdkwin_set_static_gravities(self, use_static)
    VALUE self, use_static;
{
	return (gdk_window_set_static_gravities(get_gdkwindow(self),
						NUM2INT(use_static))) ? Qtrue : Qfalse;
}

static VALUE
gdkwin_set_hints(self, x, y, min_w, min_h, max_w, max_h, flags)
    VALUE self, x, y, min_w, min_h, max_w, max_h, flags;
{
    gdk_window_set_hints(get_gdkwindow(self), NUM2INT(x), NUM2INT(y),
						 NUM2INT(min_w), NUM2INT(min_h), 
						 NUM2INT(max_w), NUM2INT(max_h),
						 NUM2INT(flags));
    return self;
}

static VALUE
gdkwin_set_title(self, title)
    VALUE self, title;
{
	gdk_window_set_title(get_gdkwindow(self),STR2CSTR(title));
	return self;
}

static VALUE
gdkwin_set_colormap(self, colormap)
    VALUE self, colormap;
{
	gdk_window_set_colormap(get_gdkwindow(self),get_gdkcmap(colormap));
	return self;
}

static VALUE
gdkwin_resize(self, w,h)
    VALUE self, w,h;
{
    gdk_window_resize(get_gdkwindow(self),
		      NUM2INT(w), NUM2INT(h));
    return self;
}

static VALUE
gdkwin_move_resize(self, x,y,w,h)
    VALUE self, x,y,w,h;
{
    gdk_window_move_resize(get_gdkwindow(self),
			   NUM2INT(x), NUM2INT(y), NUM2INT(w), NUM2INT(h));
    return self;
}

static VALUE
gdkwin_set_cursor(self, cursor)
    VALUE self, cursor;
{
    gdk_window_set_cursor(get_gdkwindow(self), get_gdkcursor(cursor));
    return self;
}

static VALUE
gdkwin_set_icon(self, icon, pixmap, mask)
    VALUE self, icon, pixmap, mask;
{
    gdk_window_set_icon(get_gdkwindow(self), get_gdkwindow(icon),
			get_gdkpixmap(pixmap), get_gdkbitmap(mask));
    return self;
}

static VALUE
gdkwin_set_icon_name(self, name)
    VALUE self, name;
{
    gdk_window_set_icon_name(get_gdkwindow(self), STR2CSTR(name));
    return self;
}

static VALUE
gdkwin_set_decorations(self, decor)
    VALUE self, decor;
{
    gdk_window_set_decorations(get_gdkwindow(self), NUM2INT(decor));
    return self;
}

static VALUE
gdkwin_set_functions(self, func)
    VALUE self, func;
{
    gdk_window_set_functions(get_gdkwindow(self), NUM2INT(func));
    return self;
}

static VALUE
gdkwin_get_root_origin(self)
    VALUE self;
{
    int x, y;
    gdk_window_get_root_origin(get_gdkwindow(self), &x, &y);
    return rb_assoc_new(INT2FIX(x), INT2FIX(y));
}

static VALUE
gdkwin_get_events(self)
    VALUE self;
{
    return INT2NUM(gdk_window_get_events(get_gdkwindow(self)));
}

static VALUE
gdkwin_set_events(self, mask)
    VALUE self, mask;
{
    gdk_window_set_events(get_gdkwindow(self), NUM2INT(mask));
    return self;
}

static VALUE
gdkwin_reparent(self, new_parent, x, y)
    VALUE self, new_parent, x, y;
{
    gdk_window_reparent(get_gdkwindow(self), get_gdkwindow(new_parent),
			NUM2INT(x), NUM2INT(y));
    return self;
}

static VALUE
gdkwin_get_geometry(self)
    VALUE self;
{
    gint x, y, w, h, d;
    gdk_window_get_geometry(get_gdkwindow(self), &x, &y, &w, &h, &d);
    return rb_ary_new3(5, INT2NUM(x), INT2NUM(y),
		       INT2NUM(w), INT2NUM(h), INT2NUM(d));
}

static VALUE
gdkwin_get_position(self)
	VALUE self;
{
	gint x, y;
	gdk_window_get_position(get_gdkwindow(self), &x, &y);
	return rb_ary_new3(2, INT2NUM(x), INT2NUM(y));
}

static VALUE
gdkwin_get_visual(self)
	VALUE self;
{
	return make_gdkvisual(gdk_window_get_visual(get_gdkwindow(self)));
}

static VALUE
gdkwin_get_colormap(self)
	VALUE self;
{
	return make_gdkcolormap(gdk_window_get_colormap(get_gdkwindow(self)));
}

static VALUE
gdkwin_get_type(self)
	VALUE self;
{
	return INT2FIX(gdk_window_get_type(get_gdkwindow(self)));
}

static VALUE
gdkwin_get_origin(self)
	VALUE self;
{
	gint x, y;
	gdk_window_get_origin(get_gdkwindow(self), &x, &y);
	return rb_ary_new3(2, INT2NUM(x), INT2NUM(y));
}

static VALUE
gdkwin_get_deskrelative_origin(self)
	VALUE self;
{
	gint x, y;
	gdk_window_get_deskrelative_origin(get_gdkwindow(self), &x, &y);
	return rb_ary_new3(2, INT2NUM(x), INT2NUM(y));
}

static VALUE
gdkwin_get_parent(self)
	VALUE self;
{
	return make_gdkwindow(gdk_window_get_parent(get_gdkwindow(self)));
}

static VALUE
gdkwin_get_toplevel(self)
	VALUE self;
{
	return make_gdkwindow(gdk_window_get_toplevel(get_gdkwindow(self)));
}

static VALUE
gdkwin_get_children(self)
	VALUE self;
{
	GList* list = gdk_window_get_children(get_gdkwindow(self));
	VALUE ary = rb_ary_new();
	while (list) {
		rb_ary_push(ary, make_gdkwindow(list->data));
		list = list->next;
	}
	return ary;
}

static VALUE
gdkwin_set_transient_for(self, leader)
    VALUE self, leader;
{   
    gdk_window_set_transient_for(get_gdkwindow(self), get_gdkwindow(leader));
    return self;
}

static VALUE
gdkwin_set_role(self, role)
    VALUE self, role;
{
    gdk_window_set_role(get_gdkwindow(self), STR2CSTR(role));
    return self;
}

static VALUE
gdkwin_set_group(self, leader)
    VALUE self, leader;
{
    gdk_window_set_group(get_gdkwindow(self), get_gdkwindow(leader));
    return self;
}


static VALUE
gdkwin_get_toplevels(self)
	VALUE self;
{
	GList* list = gdk_window_get_toplevels();
	VALUE ary = rb_ary_new();
	while (list) {
		rb_ary_push(ary, make_gdkwindow(list->data));
		list = list->next;
	}
	return ary;
}

static VALUE
gdkwin_show(self)
    VALUE self;
{
    gdk_window_show(get_gdkwindow(self));
    return Qnil;
}

static VALUE
gdkwin_hide(self)
    VALUE self;
{
    gdk_window_hide(get_gdkwindow(self));
    return Qnil;
}

static VALUE
gdkwin_prop_change(self, property, type, mode, src)
    VALUE self, property, type, mode, src;
{
  int        fmt, len, i;
  Atom       atom;
  void*      dat;
  GdkAtom    compound_text = gdk_atom_intern("COMPOUND_TEXT", FALSE);
  GdkAtom    otype, ntype;

  otype = ntype = get_gdkatom(type);

/* They are not available in Ruby/GTK 2
  if(ntype == GDK_SELECTION_TYPE_ATOM){
    len = RARRAY(src)->len;

    dat = (Atom*)ALLOC_N(Atom, len);

    for(i = 0; i < len; i++){
        ((Atom*)dat)[i] = get_gdkatom(rb_ary_entry(src, i));
    }
    fmt = 32;

  } else if(ntype == GDK_SELECTION_TYPE_BITMAP){
    dat = (void*)&(((GdkPixmapPrivate*)get_gdkbitmap(src))->xwindow);
    fmt = 32;
    len = 1;
  
  } else if(ntype == GDK_SELECTION_TYPE_COLORMAP){
    dat = (void*)&(((GdkColormapPrivate*)get_gdkcolormap(src))->xcolormap);
    fmt = 32;
    len = 1;
  } else if(ntype == GDK_SELECTION_TYPE_INTEGER){
*/
  if(ntype == GDK_SELECTION_TYPE_INTEGER){
    i = NUM2INT(src);
    dat = (void*)&i;
    fmt = 32;
    len = 1;
/*
  } else if(ntype == GDK_SELECTION_TYPE_PIXMAP){
    dat = (void*)&(((GdkPixmapPrivate*)get_gdkpixmap(src))->xwindow);
    fmt = 32;
    len = 1;
  
  } else if(ntype == GDK_SELECTION_TYPE_WINDOW||
	     ntype == GDK_SELECTION_TYPE_DRAWABLE){
    dat = (void*)&(((GdkPixmapPrivate*)get_gdkwindow(src))->xwindow);
    fmt = 32;
    len = 1;
*/
  } else if(ntype == GDK_SELECTION_TYPE_STRING) {
    dat = RSTRING(src)->ptr;
    fmt = 8;
    len = RSTRING(src)->len;

  } else if(ntype == compound_text){
	  gdk_string_to_compound_text(RSTRING(src)->ptr,
				&ntype, &fmt, (guchar**)&dat, &len);

  } else {
	  rb_raise(rb_eArgError, "no supperted type.");
  }

  gdk_property_change(get_gdkwindow(self),
		 get_gdkatom(property), ntype, fmt, NUM2INT(mode), dat, len);

  if(otype == GDK_SELECTION_TYPE_ATOM) {
	  xfree(dat);
  } else if(otype == compound_text) {
	  gdk_free_compound_text(dat);
  }

  return self;
}

static VALUE
gdkwin_prop_get(self, property, type, offset, length, delete)
    VALUE self, property, type, offset, length, delete;
{
  /* for argument processing */
  GdkWindow*    w;
  GdkAtom       p, t;
  gulong        o, l;
  gint          d;
  GdkAtom       rtype;
  gint          rfmt, rlen;
  void*		rdat;

  /* for inner processing */
  int		i;
  VALUE		ret;

  if(gdk_property_get(get_gdkwindow(self), get_gdkatom(property),
       get_gdkatom(type), NUM2INT(offset), NUM2INT(length),
       RTEST(delete), &rtype, &rfmt, &rlen, (guchar**)&rdat) == FALSE){
    return Qnil;
  }

  switch(rfmt){
  case 8:
  default:
    ret = rb_str_new(rdat, rlen);
    break;

  case 16:
    ret = rb_ary_new();

    for( i = 0; i < rlen; i++){
        rb_ary_push(ret, rb_Integer(((unsigned short*)rdat)[i]));
    }
    break;

  case 32:
/*
    ret = rb_ary_new();

    if(rtype != GDK_SELECTION_TYPE_ATOM){
        for(i = 0; i < rlen; i++){
            rb_ary_push(ret, INT2FIX(((unsigned long *)rdat)[i]));
        }
    } else {
        for(i = 0; i < rlen; i++){
            rb_ary_push(ret, make_gdkatom((GdkAtom)(unsigned long *)rdat[i]));
        }
    }
*/
	rb_warning("not implemented yet.");
    break;
  }

  return rb_ary_new3(3, make_gdkatom(rtype), ret, rb_Integer(rlen));
}

static VALUE
gdkwin_prop_delete(self, property)
    VALUE self, property;
{
  gdk_property_delete(get_gdkwindow(self), get_gdkatom(property));
  return self;
}

VALUE gdkWindowAttr;

void
Init_gtk_gdk_window()
{
    /* Gdk::Window */

    gdkWindow = rb_define_class_under(mGdk, "Window", gdkDrawable);

    rb_define_method(gdkWindow, "get_size", gdkwin_get_size, 0);
    rb_define_method(gdkWindow, "get_pointer", gdkwin_get_pointer, 0);
    rb_define_method(gdkWindow, "pointer_grab", gdkwin_pointer_grab, 5);
    rb_define_method(gdkWindow, "pointer_ungrab", gdkwin_pointer_ungrab, 1);
    rb_define_method(gdkWindow, "pointer_is_grabbed?", gdkwin_pointer_is_grabbed, 0);
    rb_define_method(gdkWindow, "keyboard_grab", gdkwin_keyboard_grab, 2);
    rb_define_method(gdkWindow, "keyboard_ungrab", gdkwin_keyboard_ungrab, 1);
    rb_define_singleton_method(gdkWindow, "foreign_new", gdkwin_foreign_new, 1);
    rb_define_singleton_method(gdkWindow, "root_window", gdkwin_root_window, 0);
    rb_define_method(gdkWindow, "clear", gdkwin_clear, 0);
    rb_define_method(gdkWindow, "clear_area", gdkwin_clear_area, 4);
    rb_define_method(gdkWindow, "clear_area_e", gdkwin_clear_area_e, 4);
    rb_define_method(gdkWindow, "copy_area", gdkwin_copy_area, 8);
    rb_define_method(gdkWindow, "set_background", gdkwin_set_background, 1);
    rb_define_method(gdkWindow, "set_back_pixmap", gdkwin_set_back_pixmap, 2);
    rb_define_method(gdkWindow, "move", gdkwin_move, 2);
    rb_define_method(gdkWindow, "raise", gdkwin_raise, 0);
    rb_define_method(gdkWindow, "lower", gdkwin_lower, 0);
    rb_define_method(gdkWindow, "register_dnd", gdkwin_register_dnd, 0);
    rb_define_method(gdkWindow, "override_redirect", gdkwin_set_override_redirect, 1);
    rb_define_method(gdkWindow, "shape_combine_mask", gdkwin_shape_combine_mask, 3);
    rb_define_method(gdkWindow, "set_child_shapes", gdkwin_set_child_shapes, 0);
    rb_define_method(gdkWindow, "merge_child_shapes", gdkwin_merge_child_shapes, 0);
    rb_define_method(gdkWindow, "set_static_gravities", gdkwin_set_static_gravities, 1);
    rb_define_method(gdkWindow, "set_hints", gdkwin_set_hints, 7);
    rb_define_method(gdkWindow, "set_title", gdkwin_set_title, 1);
    rb_define_method(gdkWindow, "set_colormap", gdkwin_set_colormap, 0);
    rb_define_method(gdkWindow, "position", gdkwin_get_position, 0);
    rb_define_method(gdkWindow, "visual", gdkwin_get_visual, 0);
    rb_define_method(gdkWindow, "colormap", gdkwin_get_colormap, 0);
    rb_define_method(gdkWindow, "window_type", gdkwin_get_type, 0);
    rb_define_method(gdkWindow, "origin", gdkwin_get_origin, 0);
    rb_define_method(gdkWindow, "deskrelative_origin", gdkwin_get_deskrelative_origin, 0);
    rb_define_method(gdkWindow, "parent", gdkwin_get_parent, 0);
    rb_define_method(gdkWindow, "toplevel", gdkwin_get_toplevel, 0);
    rb_define_method(gdkWindow, "children", gdkwin_get_children, 0);
    rb_define_method(gdkWindow, "set_transient_for", gdkwin_set_transient_for, 1);
    rb_define_method(gdkWindow, "set_role", gdkwin_set_role, 1);
    rb_define_method(gdkWindow, "set_group", gdkwin_set_group, 1);
    rb_define_method(gdkWindow, "toplevels", gdkwin_get_toplevels, 0);
    rb_define_method(gdkWindow, "resize", gdkwin_resize, 2);
    rb_define_method(gdkWindow, "move_resize", gdkwin_move_resize, 4);
    rb_define_method(gdkWindow, "set_cursor", gdkwin_set_cursor, 1);
    rb_define_method(gdkWindow, "set_icon", gdkwin_set_icon, 3);
    rb_define_method(gdkWindow, "set_icon_name", gdkwin_set_icon_name, 1);
    rb_define_method(gdkWindow, "set_decorations", gdkwin_set_decorations, 1);
    rb_define_method(gdkWindow, "set_functions", gdkwin_set_functions, 1);
    rb_define_method(gdkWindow, "set_override_redirect", gdkwin_set_override_redirect, 1);
    rb_define_method(gdkWindow, "get_root_origin", gdkwin_get_root_origin, 0);
    rb_define_method(gdkWindow, "get_events", gdkwin_get_events, 0);
    rb_define_method(gdkWindow, "set_events", gdkwin_set_events, 1);
    rb_define_method(gdkWindow, "reparent", gdkwin_reparent, 3);
    rb_define_method(gdkWindow, "show", gdkwin_show, 0);
    rb_define_method(gdkWindow, "hide", gdkwin_hide, 0);
    rb_define_method(gdkWindow, "get_geometry", gdkwin_get_geometry, 0);

    rb_define_method(gdkWindow, "property_change", gdkwin_prop_change, 4);
    rb_define_method(gdkWindow, "property_get", gdkwin_prop_get, 5);
    rb_define_method(gdkWindow, "property_delete", gdkwin_prop_delete, 1);

    /* GdkWindowHints */
    rb_define_const(gdkWindow, "HINT_POS", INT2FIX(GDK_HINT_POS));
    rb_define_const(gdkWindow, "HINT_MIN_SIZE", INT2FIX(GDK_HINT_MIN_SIZE));
    rb_define_const(gdkWindow, "HINT_MAX_SIZE", INT2FIX(GDK_HINT_MAX_SIZE));
    rb_define_const(gdkWindow, "HINT_BASE_SIZE", INT2FIX(GDK_HINT_BASE_SIZE));
    rb_define_const(gdkWindow, "HINT_ASPECT", INT2FIX(GDK_HINT_ASPECT));
    rb_define_const(gdkWindow, "HINT_RESIZE_INC", INT2FIX(GDK_HINT_RESIZE_INC));

    /* Gdk::WindowAttr */

    gdkWindowAttr = rb_define_class_under(mGdk, "WindowAttr", rb_cData);
}

/*
 * Gdk::Geometry
 */

VALUE gdkGeometry;

GdkGeometry *
rbgdk_geometry_get(geo)
    VALUE geo;
{
    GdkGeometry *ret;

    Data_Get_Struct(geo, GdkGeometry, ret);
    return ret;
}

VALUE
rbgdk_geometry_make(geo)
    GdkGeometry *geo;
{
    VALUE obj;

    obj = Data_Wrap_Struct(gdkGeometry, 0, g_free, geo);

    return obj;
}

static VALUE
geo_s_new(self)
    VALUE self;
{
    VALUE obj;
    GdkGeometry *geo;

    geo = g_new0(GdkGeometry, 1);
    obj = rbgdk_geometry_make(geo);

    return obj;
}

static VALUE
geo_min_width(self)
    VALUE self;
{
    GdkGeometry *geo;
    
    geo = rbgdk_geometry_get(self);
    return INT2NUM(geo->min_width);
}

static VALUE
geo_min_height(self)
    VALUE self;
{
    GdkGeometry *geo;
    
    geo = rbgdk_geometry_get(self);
    return INT2NUM(geo->min_height);
}

static VALUE
geo_max_width(self)
    VALUE self;
{
    GdkGeometry *geo;
    
    geo = rbgdk_geometry_get(self);
    return INT2NUM(geo->max_width);
}

static VALUE
geo_max_height(self)
    VALUE self;
{
    GdkGeometry *geo;
    
    geo = rbgdk_geometry_get(self);
    return INT2NUM(geo->max_height);
}

static VALUE
geo_base_width(self)
    VALUE self;
{
    GdkGeometry *geo;
    
    geo = rbgdk_geometry_get(self);
    return INT2NUM(geo->base_width);
}

static VALUE
geo_base_height(self)
    VALUE self;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    return INT2NUM(geo->base_height);
}

static VALUE
geo_width_inc(self)
    VALUE self;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    return INT2NUM(geo->width_inc);
}

static VALUE
geo_height_inc(self)
    VALUE self;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    return INT2NUM(geo->height_inc);
}

static VALUE
geo_min_aspect(self)
    VALUE self;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    return rb_float_new(geo->min_aspect);
}

static VALUE
geo_max_aspect(self)
    VALUE self;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    return rb_float_new(geo->max_aspect);
}

static VALUE
geo_set(self, min_width, min_height, max_width,	max_height,
	base_width, base_height, width_inc, height_inc,	min_aspect, max_aspect)
    VALUE self,
    min_width, min_height, max_width, max_height,
    base_width, base_height, width_inc, height_inc,
    min_aspect, max_aspect;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    geo->min_width = NUM2INT(min_width);
    geo->min_height = NUM2INT(min_height);
    geo->max_width = NUM2INT(max_width);
    geo->max_height = NUM2INT(max_height);
    geo->base_width = NUM2INT(base_width);
    geo->base_height = NUM2INT(base_height);
    geo->width_inc = NUM2INT(width_inc);
    geo->height_inc = NUM2INT(height_inc);
    geo->min_aspect = NUM2DBL(min_aspect);
    geo->max_aspect = NUM2DBL(max_aspect);
    return self;
}

static VALUE
geo_set_min_width(self, min_width)
    VALUE self, min_width;
{
    GdkGeometry *geo;
    
    geo = rbgdk_geometry_get(self);
    geo->min_width = NUM2INT(min_width);
    return self;
}

static VALUE
geo_set_min_height(self, min_height)
    VALUE self, min_height;
{
    GdkGeometry *geo;
    
    geo = rbgdk_geometry_get(self);
    geo->min_height = NUM2INT(min_height);
    return self;
}

static VALUE
geo_set_max_width(self, max_width)
    VALUE self, max_width;
{
    GdkGeometry *geo;
    
    geo = rbgdk_geometry_get(self);
    geo->max_width = NUM2INT(max_width);
    return self;
}

static VALUE
geo_set_max_height(self, max_height)
    VALUE self, max_height;
{
    GdkGeometry *geo;
    
    geo = rbgdk_geometry_get(self);
    geo->max_height = NUM2INT(max_height);
    return self;
}

static VALUE
geo_set_base_width(self, base_width)
    VALUE self, base_width;
{
    GdkGeometry *geo;
    
    geo = rbgdk_geometry_get(self);
    geo->base_width = NUM2INT(base_width);
    return self;
}

static VALUE
geo_set_base_height(self, base_height)
    VALUE self, base_height;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    geo->base_height = NUM2INT(base_height);
    return self;
}

static VALUE
geo_set_width_inc(self, width_inc)
    VALUE self, width_inc;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    geo->width_inc = NUM2INT(width_inc);
    return self;
}

static VALUE
geo_set_height_inc(self, height_inc)
    VALUE self, height_inc;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    geo->height_inc = NUM2INT(height_inc);
    return self;
}

static VALUE
geo_set_min_aspect(self, min_aspect)
    VALUE self, min_aspect;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    geo->min_aspect = NUM2DBL(min_aspect);
    return self;
}

static VALUE
geo_set_max_aspect(self, max_aspect)
    VALUE self, max_aspect;
{
    GdkGeometry *geo;

    geo = rbgdk_geometry_get(self);
    geo->max_aspect = NUM2DBL(max_aspect);
    return self;
}


void Init_gtk_gdk_geometry()
{
    gdkGeometry = rb_define_class_under(mGdk, "Geometry", rb_cData);

    rb_define_singleton_method(gdkGeometry, "new", geo_s_new, 0);

    rb_define_method(gdkGeometry, "min_width", geo_min_width, 0);
    rb_define_method(gdkGeometry, "min_height", geo_min_height, 0);
    rb_define_method(gdkGeometry, "max_width", geo_max_width, 0);
    rb_define_method(gdkGeometry, "max_height", geo_max_height, 0);
    rb_define_method(gdkGeometry, "base_width", geo_base_width, 0);
    rb_define_method(gdkGeometry, "base_height", geo_base_height, 0);
    rb_define_method(gdkGeometry, "width_inc", geo_width_inc, 0);
    rb_define_method(gdkGeometry, "height_inc", geo_height_inc, 0);
    rb_define_method(gdkGeometry, "min_aspect", geo_min_aspect, 0);
    rb_define_method(gdkGeometry, "max_aspect", geo_max_aspect, 0);

    /* once ruby gets parameterized arguments feature, setters should
     * be changed. 2000-040-26 yashi */
    rb_define_method(gdkGeometry, "set", geo_set, 10);

    rb_define_method(gdkGeometry, "set_min_width", geo_set_min_width, 1);
    rb_define_method(gdkGeometry, "set_min_height", geo_set_min_height, 1);
    rb_define_method(gdkGeometry, "set_max_width", geo_set_max_width, 1);
    rb_define_method(gdkGeometry, "set_max_height", geo_set_max_height, 1);
    rb_define_method(gdkGeometry, "set_base_width", geo_set_base_width, 1);
    rb_define_method(gdkGeometry, "set_base_height", geo_set_base_height, 1);
    rb_define_method(gdkGeometry, "set_width_inc", geo_set_width_inc, 1);
    rb_define_method(gdkGeometry, "set_height_inc", geo_set_height_inc, 1);
    rb_define_method(gdkGeometry, "set_min_aspect", geo_set_min_aspect, 1);
    rb_define_method(gdkGeometry, "set_max_aspect", geo_set_max_aspect, 1);

}
