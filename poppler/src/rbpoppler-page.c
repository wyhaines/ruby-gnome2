/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/**********************************************************************

  rbpoppler-page.c -

  $Author: ktou $
  $Date: 2006/05/29 16:29:14 $

  Copyright (C) 2006 Ruby-GNOME2 Project Team

**********************************************************************/

#include "rbpoppler.h"

#define RVAL2LM(obj) ((PopplerLinkMapping *)RVAL2BOXED(obj, POPPLER_TYPE_LINK_MAPPING))

static VALUE
page_render_to_pixbuf(VALUE self, VALUE src_x, VALUE src_y, VALUE src_width,
                      VALUE src_height, VALUE scale, VALUE rotation,
                      VALUE pixbuf)
{
    poppler_page_render_to_pixbuf(RVAL2GOBJ(self), NUM2INT(src_x),
                                  NUM2INT(src_y), NUM2INT(src_width),
                                  NUM2INT(src_height), NUM2DBL(scale),
                                  NUM2INT(rotation), RVAL2GOBJ(pixbuf));
    return Qnil;
}

#ifdef RB_POPPLER_CAIRO_AVAILABLE
static VALUE
page_render(VALUE self, VALUE cairo)
{
    poppler_page_render(RVAL2GOBJ(self), RVAL2CRCONTEXT(cairo));
    return Qnil;
}
#endif

static VALUE
page_render_to_ps(VALUE self, VALUE ps_file)
{
    poppler_page_render_to_ps(RVAL2GOBJ(self), RVAL2GOBJ(ps_file));
    return Qnil;
}

static VALUE
page_render_generic(int argc, VALUE *argv, VALUE self)
{
    if (argc == 1) {
        if (RTEST(rb_obj_is_kind_of(argv[0], rb_cCairo_Context))) {
#ifdef RB_POPPLER_CAIRO_AVAILABLE
            return page_render(self, argv[0]);
#else
            rb_raise(rb_eArgError, "cairo is not available");
#endif
        } else {
            return page_render_to_ps(self, argv[0]);
        }
    } else if (argc == 7) {
        return page_render_to_pixbuf(self, argv[0], argv[1], argv[2], argv[3],
                                     argv[4], argv[5], argv[6]);
    } else {
        rb_raise(rb_eArgError,
                 "wrong number of arguments (%d for 1 or 7)", argc);
    }
}

static VALUE
page_get_size(VALUE self)
{
    double width, height;
    poppler_page_get_size(RVAL2GOBJ(self), &width, &height);
    return rb_ary_new3(2, rb_float_new(width), rb_float_new(height));
}

static VALUE
page_get_index(VALUE self)
{
    return INT2NUM(poppler_page_get_index(RVAL2GOBJ(self)));
}

static VALUE
page_get_thumbnail(VALUE self)
{
    return GOBJ2RVAL(poppler_page_get_thumbnail(RVAL2GOBJ(self)));
}

static VALUE
page_get_thumbnail_size(VALUE self)
{
    int width, height;

    if (poppler_page_get_thumbnail_size(RVAL2GOBJ(self), &width, &height))
        return rb_ary_new3(2, INT2NUM(width), INT2NUM(height));
    else
        return Qnil;
}

static VALUE
page_find_text(VALUE self, VALUE text)
{
    VALUE result;
    GList *matches;
    matches = poppler_page_find_text(RVAL2GOBJ(self), RVAL2CSTR(text));
    result = GLIST2ARY2(matches, POPPLER_TYPE_RECTANGLE);
    g_list_free(matches);
    return result;
}

static VALUE
page_get_text(VALUE self, VALUE rect)
{
    gchar *text;
    VALUE rb_text;

    text = poppler_page_get_text(RVAL2GOBJ(self), RVAL2RECT(rect));
    rb_text = CSTR2RVAL(text);
    g_free(text);
    return rb_text;
}

static VALUE
page_get_link_mapping(VALUE self)
{
    VALUE result;
    GList *map_list;
    map_list = poppler_page_get_link_mapping(RVAL2GOBJ(self));
    result = GLIST2ARY2(map_list, POPPLER_TYPE_LINK_MAPPING);
    g_list_free(map_list);
    return result;
}

static VALUE
page_get_selection_region(VALUE self, VALUE scale, VALUE selection)
{
    return REGION2RVAL(poppler_page_get_selection_region(RVAL2GOBJ(self),
                                                         NUM2DBL(scale),
                                                         RVAL2RECT(selection)));
}

#ifdef RB_POPPLER_CAIRO_AVAILABLE
static VALUE
page_render_selection(VALUE self, VALUE cairo,
                      VALUE selection, VALUE rb_old_selection,
                      VALUE glyph_color, VALUE background_color)
{
    PopplerRectangle *old_selection = NULL;

    if (!NIL_P(rb_old_selection))
        old_selection = RVAL2RECT(rb_old_selection);
    poppler_page_render_selection(RVAL2GOBJ(self), RVAL2CRCONTEXT(cairo),
                                  RVAL2RECT(selection),
                                  old_selection,
                                  RVAL2COLOR(glyph_color),
                                  RVAL2COLOR(background_color));
    return Qnil;
}
#endif

static VALUE
page_render_selection_to_pixbuf(VALUE self, VALUE scale, VALUE rotation,
                                VALUE pixbuf, VALUE selection,
                                VALUE rb_old_selection,
                                VALUE glyph_color, VALUE background_color)
{
    PopplerRectangle *old_selection = NULL;

    if (!NIL_P(rb_old_selection))
        old_selection = RVAL2RECT(rb_old_selection);
    poppler_page_render_selection_to_pixbuf(RVAL2GOBJ(self), NUM2DBL(scale),
                                            NUM2INT(rotation),
                                            RVAL2GOBJ(pixbuf),
                                            RVAL2RECT(selection),
                                            old_selection,
                                            RVAL2COLOR(glyph_color),
                                            RVAL2COLOR(background_color));
    return Qnil;
}

static VALUE
page_render_selection_generic(int argc, VALUE *argv, VALUE self)
{
    if (argc == 5) {
#ifdef RB_POPPLER_CAIRO_AVAILABLE
        return page_render_selection(self, argv[0], argv[1], argv[2],
                                     argv[3], argv[4]);
#else
        rb_raise(rb_eArgError, "cairo is not available");
#endif
    } else if (argc == 7) {
        return page_render_selection_to_pixbuf(self, argv[0], argv[1],
                                               argv[2], argv[3], argv[4],
                                               argv[5], argv[6]);
    } else {
        rb_raise(rb_eArgError,
                 "wrong number of arguments (%d for 5 or 7)", argc);
    }
}



/* A rectangle on a page, with coordinates in PDF points. */
static VALUE
rectangle_initialize(VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2)
{
    PopplerRectangle rectangle;

    rectangle.x1 = NUM2DBL(x1);
    rectangle.y1 = NUM2DBL(y1);
    rectangle.x2 = NUM2DBL(x2);
    rectangle.y2 = NUM2DBL(y2);

    G_INITIALIZE(self, &rectangle);
    return Qnil;
}

static VALUE
rectangle_get_x1(VALUE self)
{
    return rb_float_new(RVAL2RECT(self)->x1);
}

static VALUE
rectangle_get_y1(VALUE self)
{
    return rb_float_new(RVAL2RECT(self)->y1);
}

static VALUE
rectangle_get_x2(VALUE self)
{
    return rb_float_new(RVAL2RECT(self)->x2);
}

static VALUE
rectangle_get_y2(VALUE self)
{
    return rb_float_new(RVAL2RECT(self)->y2);
}


static VALUE
rectangle_set_x1(VALUE self, VALUE x1)
{
    RVAL2RECT(self)->x1 = NUM2DBL(x1);
    return Qnil;
}

static VALUE
rectangle_set_y1(VALUE self, VALUE y1)
{
    RVAL2RECT(self)->y1 = NUM2DBL(y1);
    return Qnil;
}

static VALUE
rectangle_set_x2(VALUE self, VALUE x2)
{
    RVAL2RECT(self)->x2 = NUM2DBL(x2);
    return Qnil;
}

static VALUE
rectangle_set_y2(VALUE self, VALUE y2)
{
    RVAL2RECT(self)->y2 = NUM2DBL(y2);
    return Qnil;
}

static VALUE
rectangle_to_a(VALUE self)
{
    PopplerRectangle *rectangle = RVAL2RECT(self);
    return rb_ary_new3(4,
                       rb_float_new(rectangle->x1),
                       rb_float_new(rectangle->y1),
                       rb_float_new(rectangle->x2),
                       rb_float_new(rectangle->y2));
}


/* Mapping between areas on the current page and PopplerActions */
static VALUE
link_mapping_get_area(VALUE self)
{
    return RECT2RVAL(&(RVAL2LM(self)->area));
}

static VALUE
link_mapping_get_action(VALUE self)
{
    return ACTION2RVAL(RVAL2LM(self)->action);
}

void
Init_poppler_page(VALUE mPoppler)
{
    VALUE cPage, cRectangle, cLinkMapping;

    cPage = G_DEF_CLASS(POPPLER_TYPE_PAGE, "Page", mPoppler);
    cRectangle = G_DEF_CLASS(POPPLER_TYPE_RECTANGLE, "Rectangle", mPoppler);
    cLinkMapping = G_DEF_CLASS(POPPLER_TYPE_LINK_MAPPING, "LinkMapping",
                               mPoppler);

    rb_define_method(cPage, "render", page_render_generic, -1);
    rb_define_method(cPage, "size", page_get_size, 0);
    rb_define_method(cPage, "index", page_get_index, 0);
    rb_define_method(cPage, "thumbnail", page_get_thumbnail, 0);
    rb_define_method(cPage, "thumbnail_size", page_get_thumbnail_size, 0);
    rb_define_method(cPage, "find_text", page_find_text, 1);
    rb_define_method(cPage, "get_text", page_get_text, 1);
    rb_define_method(cPage, "link_mapping", page_get_link_mapping, 0);
    rb_define_method(cPage, "get_selection_region",
                     page_get_selection_region, 2);
    rb_define_method(cPage, "render_selection",
                     page_render_selection_generic, -1);

    G_DEF_SETTERS(cPage);

/* A rectangle on a page, with coordinates in PDF points. */
    rb_define_method(cRectangle, "initialize", rectangle_initialize, 4);
    rb_define_method(cRectangle, "x1", rectangle_get_x1, 0);
    rb_define_method(cRectangle, "y1", rectangle_get_y1, 0);
    rb_define_method(cRectangle, "x2", rectangle_get_x2, 0);
    rb_define_method(cRectangle, "y2", rectangle_get_y2, 0);
    rb_define_method(cRectangle, "set_x1", rectangle_set_x1, 1);
    rb_define_method(cRectangle, "set_y1", rectangle_set_y1, 1);
    rb_define_method(cRectangle, "set_x2", rectangle_set_x2, 1);
    rb_define_method(cRectangle, "set_y2", rectangle_set_y2, 1);
    rb_define_method(cRectangle, "to_a", rectangle_to_a, 0);

    G_DEF_SETTERS(cRectangle);

/* Mapping between areas on the current page and PopplerActions */
    rb_define_method(cLinkMapping, "area", link_mapping_get_area, 0);
    rb_define_method(cLinkMapping, "action", link_mapping_get_action, 0);

    G_DEF_SETTERS(cLinkMapping);
}