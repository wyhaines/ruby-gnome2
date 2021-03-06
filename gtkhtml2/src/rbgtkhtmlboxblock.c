/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgtkhtmlboxblock.c

  $Author: sakai $
  $Date: 2003/11/17 14:22:57 $

  Copyright (C) 2003 Masao Mutoh
************************************************/

#include "rbgtkhtml.h"

#define _SELF(s) (HTML_BOX_BLOCK(RVAL2GOBJ(s)))

static VALUE
rb_html_box_block_initialize(self)
    VALUE self;
{
    G_INITIALIZE(self, html_box_block_new());
    return Qnil;
}

#ifdef HAVE_HTML_BOX_BLOCK_GET_CONTAINING_WIDTH
/*
gint html_box_block_calculate_float_magic (HtmlBox *self, HtmlRelayout *relayout);
*/
static VALUE
rb_html_box_block_get_containing_width(self)
    VALUE self;
{
    return INT2NUM(html_box_block_get_containing_width(_SELF(self)));
}
#endif

void 
Init_html_box_block(mGtkHtml2)
    VALUE mGtkHtml2;
{
    VALUE boxblock = G_DEF_CLASS(HTML_TYPE_BOX_BLOCK, "HtmlBoxBlock", mGtkHtml2);

    rb_define_method(boxblock, "initialize", rb_html_box_block_initialize, 0);
#ifdef HTML_BOX_BLOCK_GET_CONTAINING_WIDTH
    rb_define_method(boxblock, "containing_width", rb_html_box_block_get_containing_width, 0);
#endif
}

