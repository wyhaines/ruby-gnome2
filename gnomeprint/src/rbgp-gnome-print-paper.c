/*
 * Copyright (C) 2004 Kouhei Sutou <kou@cozmixng.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "rbgp.h"
#include <libgnomeprint/gnome-print-paper.h>

static GnomePrintPaper*
paper_copy(const GnomePrintPaper *paper) {
  GnomePrintPaper* new_paper;
  g_return_val_if_fail (paper != NULL, NULL);
  new_paper = g_new(GnomePrintPaper, sizeof(GnomePrintPaper));
  *new_paper = *paper;
  return new_paper;
}

GType
gnome_print_paper_get_type(void)
{
  static GType our_type = 0;
  if (our_type == 0) {
    our_type = g_boxed_type_register_static("GnomePrintPaper",
                                            (GBoxedCopyFunc)paper_copy,
                                            (GBoxedFreeFunc)g_free);
  }
  return our_type;
}


#define _SELF(o) ((GnomePrintPaper*)RVAL2BOXED(o, GNOME_TYPE_PRINT_PRINT_PAPER))
#define GPP2RVAL(o) (BOXED2RVAL(o, GNOME_TYPE_PRINT_PRINT_PAPER))
#define CONST_GPP2RVAL(o) (BOXED2RVAL((GnomePrintPaper*)o, GNOME_TYPE_PRINT_PRINT_PAPER))


#define ATTR_INT(name)                          \
static VALUE                                    \
gp_paper_int_ ## name(VALUE self)               \
{                                               \
    return INT2NUM(_SELF(self)->name);          \
}

#define ATTR_STRING(name)                       \
static VALUE                                    \
gp_paper_string_ ## name(VALUE self)            \
{                                               \
    return CSTR2RVAL(_SELF(self)->name);        \
}

#define ATTR_DOUBLE(name)                       \
static VALUE                                    \
gp_paper_double_ ## name(VALUE self)            \
{                                               \
    return rb_float_new(_SELF(self)->name);     \
}

#define DEFINE_READER(gpt, type, name)                                        \
rb_define_method(gpt, G_STRINGIFY(name), gp_paper_ ## type ## _ ## name, 0);


ATTR_INT(version);
ATTR_STRING(name);
ATTR_DOUBLE(width);
ATTR_DOUBLE(height);


static VALUE
gp_paper_get_default(VALUE self)
{
  return CONST_GPP2RVAL(gnome_print_paper_get_default());
}

#define RETURN_GPP_OR_NIL(paper)                \
if (paper) {                                    \
  return CONST_GPP2RVAL(paper);                 \
} else {                                        \
  return Qnil;                                  \
}

static VALUE
gp_paper_get_by_name(VALUE self, VALUE name)
{
  const GnomePrintPaper *paper;
  paper = gnome_print_paper_get_by_name(RVAL2CSTR(name));
  RETURN_GPP_OR_NIL(paper);
}

static VALUE
gp_paper_get_by_size(VALUE self, VALUE width, VALUE height)
{
  const GnomePrintPaper *paper;
  paper = gnome_print_paper_get_by_size(NUM2DBL(width),
                                        NUM2DBL(height));
  RETURN_GPP_OR_NIL(paper);
}

static VALUE
gp_paper_get_closest_by_size(VALUE self, VALUE width, VALUE height, VALUE mustfit)
{
  const GnomePrintPaper *paper;
  paper = gnome_print_paper_get_closest_by_size(NUM2DBL(width),
                                                NUM2DBL(height),
                                                RVAL2CBOOL(mustfit));
  RETURN_GPP_OR_NIL(paper);
}

#define GET_ARG_RAISE                                                 \
rb_raise(rb_eArgError,                                                \
         "(name), (width, height) or (width, height, must_fit) ")

static VALUE
gp_paper_get_generic(int argc, VALUE *argv, VALUE self)
{
  if (argc == 0) {
    return gp_paper_get_default(self);
  } else {
    VALUE name, width, height, mustfit;
    rb_scan_args(argc, argv, "13", &name, &height, &mustfit);
    
    if (TYPE(name) == T_STRING) {
      if (argc > 1)
        GET_ARG_RAISE;
      return gp_paper_get_by_name(self, name);
    } else {
      width = name;
      if (NIL_P(mustfit)) {
        return gp_paper_get_by_size(self, width, height);
      } else {
        return gp_paper_get_closest_by_size(self, width, height, mustfit);
      }
    }
  }
}



static VALUE
gp_paper_get_list(VALUE self)
{
  VALUE array = rb_ary_new();
  GList *list = gnome_print_paper_get_list();
  GList *elem;

  elem = list;
  while (elem) {
    GnomePrintPaper *paper = (GnomePrintPaper *)elem->data;
    rb_ary_push(array, GPP2RVAL(paper));
    elem = elem->next;
  }
  gnome_print_paper_free_list(list);
  
  return array;
}



void
Init_gnome_print_paper(VALUE mGnome)
{
  VALUE cPaper = G_DEF_CLASS(GNOME_TYPE_PRINT_PRINT_PAPER,
                             "PrintPaper", mGnome);

  DEFINE_READER(cPaper, int, version);
  DEFINE_READER(cPaper, string, name);
  DEFINE_READER(cPaper, double, width);
  DEFINE_READER(cPaper, double, height);
  
  rb_define_module_function(cPaper, "default", gp_paper_get_default, 0);
  rb_define_module_function(cPaper, "get", gp_paper_get_generic, -1);
  rb_define_module_function(cPaper, "list", gp_paper_get_list, 0);
}
