/*
 * Copyright (C) 2003, 2004 Laurent Sansonetti <lrz@gnome.org>
 * Copyright (C) 2007 Ruby-GNOME2 Project Team
 *
 * This file is part of Ruby/GStreamer.
 *
 * Ruby/GStreamer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Ruby/GStreamer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Ruby/GStreamer; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#include "rbgst.h"

#define SELF(obj) (RVAL2GST_STRUCT(obj))

static VALUE
initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE name, fields;
    GstStructure *structure;

    rb_scan_args(argc, argv, "11", &name, &fields);

    structure = gst_structure_empty_new(RVAL2CSTR(name));
    if (!NIL_P(fields)) {
        /* set fields */
    }

    G_INITIALIZE(self, structure);

    return Qnil;
}

static VALUE
get_name(VALUE self)
{
    return CSTR2RVAL(gst_structure_get_name(SELF(self)));
}

static VALUE
set_name(VALUE self, VALUE name)
{
    gst_structure_set_name(SELF(self), RVAL2CSTR(name));
    return Qnil;
}

static VALUE
has_name_p(VALUE self, VALUE name)
{
    return CBOOL2RVAL(gst_structure_has_name(SELF(self), RVAL2CSTR(name)));
}

static VALUE
get_value(VALUE self, VALUE name)
{
    return GVAL2RVAL(gst_structure_get_value(SELF(self), RVAL2CSTR(name)));
}

static VALUE
set_value(VALUE self, VALUE name, VALUE rb_value)
{
    GValue value = {0};

    rbgobj_initialize_gvalue(&value, rb_value);
    gst_structure_set_value(SELF(self), RVAL2CSTR(name), &value);
    g_value_unset(&value);

    return Qnil;
}

static VALUE
remove_fields(int argc, VALUE *argv, VALUE self)
{
    int i;
    VALUE name, rest;
    GstStructure *structure;

    rb_scan_args(argc, argv, "1*", &name, &rest);

    structure = SELF(self);
    for (i = 0; i < argc; i++) {
        name = argv[i];

        gst_structure_remove_field(structure, RVAL2CSTR(name));
    }

    return Qnil;
}

static VALUE
remove_all_fields(VALUE self)
{
    gst_structure_remove_all_fields(SELF(self));
    return Qnil;
}

typedef struct _EachCallbackInfo
{
    int state;
    VALUE block;
} EachCallbackInfo;

typedef struct _EachCallbackData
{
    VALUE block;
    VALUE key;
    VALUE value;
} EachCallbackData;

typedef struct _RValueToGValueData
{
    VALUE rvalue;
    GValue *gvalue;
} RValueToGValueData;

static VALUE
invoke_foreach_proc(VALUE user_data)
{
    EachCallbackData *data = (EachCallbackData *)user_data;

    return rb_funcall(data->block,
                      rb_intern("call"),
                      2,
                      data->key,
                      data->value);
}

static gboolean
foreach_cb(GQuark field_id, const GValue *value, gpointer user_data)
{
    EachCallbackInfo *info = user_data;
    EachCallbackData data;

    data.block = info->block;
    data.key = rb_str_new2(g_quark_to_string(field_id));
    data.value = GVAL2RVAL(value);
    rb_protect(invoke_foreach_proc, (VALUE)(&data), &(info->state));

    return info->state == 0;
}

static VALUE
foreach(VALUE self)
{
    EachCallbackInfo info;

    info.state = 0;
    info.block = rb_block_proc();

    if (!gst_structure_foreach(SELF(self), foreach_cb, &info))
        rb_jump_tag(info.state);

    return Qnil;
}

static VALUE
invoke_rvalue_to_gvalue(VALUE user_data)
{
    RValueToGValueData *data = (RValueToGValueData *)user_data;

    rbgobj_rvalue_to_gvalue(data->rvalue, data->gvalue);

    return Qnil;
}

static gboolean
map_in_place_cb(GQuark field_id, GValue *value, gpointer user_data)
{
    EachCallbackInfo *info = user_data;
    EachCallbackData data;
    VALUE result;

    data.block = info->block;
    data.key = rb_str_new2(g_quark_to_string(field_id));
    data.value = GVAL2RVAL(value);
    result = rb_protect(invoke_foreach_proc, (VALUE)(&data), &(info->state));
    if (info->state == 0) {
        RValueToGValueData convert_data;
        convert_data.rvalue = result;
        convert_data.gvalue = value;
        rb_protect(invoke_rvalue_to_gvalue,
                   (VALUE)(&convert_data), &(info->state));
    }

    return info->state == 0;
}

static VALUE
map_in_place(VALUE self)
{
    EachCallbackInfo info;

    info.state = 0;
    info.block = rb_block_proc();

    if (!gst_structure_map_in_place(SELF(self), map_in_place_cb, &info))
        rb_jump_tag(info.state);

    return Qnil;
}

void
Init_gst_structure(void)
{
    VALUE rb_cGstStructure;

    rb_cGstStructure = G_DEF_CLASS(GST_TYPE_STRUCTURE, "Structure", mGst);

    rb_include_module(rb_cGstStructure, rb_mEnumerable);

    rb_define_method(rb_cGstStructure, "initialize", initialize, -1);

    rb_define_method(rb_cGstStructure, "name", get_name, 0);
    rb_define_method(rb_cGstStructure, "set_name", set_name, 1);
    rb_define_method(rb_cGstStructure, "has_name?", has_name_p, 1);
    rb_define_alias(rb_cGstStructure, "have_name?", "has_name?");

    rb_define_method(rb_cGstStructure, "[]", get_value, 1);
    rb_define_method(rb_cGstStructure, "[]=", set_value, 2);

    rb_define_method(rb_cGstStructure, "remove", remove_fields, -1);
    rb_define_alias(rb_cGstStructure, "delete", "remove");
    rb_define_method(rb_cGstStructure, "remove_all", remove_all_fields, 0);
    rb_define_alias(rb_cGstStructure, "clear", "remove_all");

    rb_define_method(rb_cGstStructure, "each", foreach, 0);

    rb_define_method(rb_cGstStructure, "map!", map_in_place, 0);
    rb_define_alias(rb_cGstStructure, "collect!", "map!");

    G_DEF_SETTERS(rb_cGstStructure);
}
