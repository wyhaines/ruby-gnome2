/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/**********************************************************************

  rbgobject.h -

  $Author: sakai $
  $Date: 2002/10/13 06:41:58 $

  Copyright (C) 2002  Masahiro Sakai

**********************************************************************/

#ifndef __RBGOBJECT_H__
#define __RBGOBJECT_H__

#include <glib-object.h>
#include "ruby.h"
#include "rbglib.h"
#include "rbgutil.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define G_INITIALIZE(obj, cobj)\
 (rbgobj_initialize_object(obj, (gpointer)cobj))

#define G_DEF_CLASS(gtype, name, module)\
 (rbgobj_define_class(gtype, name, module, 0, 0))
#define G_DEF_CLASS2(gtype, name, module, mark, free)\
 (rbgobj_define_class(gtype, name, module, mark, free))

#define G_DEF_INTERFACE(gtype, name, module)\
 (rbgobj_define_class(gtype, name, module, 0, 0))
#define G_DEF_INTERFACE2(gtype, name, module, mark, free)\
 (rbgobj_define_class(gtype, name, module, mark, free))

#define G_RELATIVE(obj, rel) (rbgobj_add_relative(obj, rel))
#define G_RELATIVE2(obj, rel, id, hash_key)\
 (rbgobj_add_relative_removable(obj, rel, id, hash_key))
#define G_REMOVE_RELATIVE(obj, id, hash_key)\
 (rbgobj_remove_relative(obj, id, hash_key))

#define CLASS2CINFO(klass) (rbgobj_lookup_class(klass))
#define GTYPE2CINFO(gtype) (rbgobj_lookup_class_by_gtype(gtype))
#define RVAL2CINFO(obj)    (rbgobj_lookup_class(CLASS_OF(obj)))
#define GTYPE2CLASS(gtype) (GTYPE2CINFO(gtype)->klass)
#define CLASS2GTYPE(klass) (rbgobj_lookup_class(klass)->gtype)
#define RVAL2GTYPE(obj)    (CLASS2GTYPE(CLASS_OF(obj)))

#define RVAL2GOBJ(obj)  (rbgobj_instance_from_ruby_object(obj))
#define GOBJ2RVAL(gobj) (rbgobj_ruby_object_from_instance(gobj))
#define GVAL2RVAL(v)    (rbgobj_gvalue_to_rvalue(v))

#define RVAL2BOXED(obj, gtype)  (rbgobj_boxed_get(obj, gtype))
#define BOXED2RVAL(cobj, gtype) (rbgobj_make_boxed(cobj, gtype))

typedef struct {
    VALUE klass;
    GType gtype;
    void (*mark)(gpointer);
    void (*free)(gpointer);
} RGObjClassInfo;

/* rbgobject.c */
extern ID id_relatives;

extern void rbgobj_initialize_object(VALUE obj, gpointer cobj);
extern gpointer rbgobj_instance_from_ruby_object(VALUE obj);
extern VALUE rbgobj_ruby_object_from_instance(gpointer instance);

extern void rbgobj_add_relative(VALUE obj, VALUE relative);
extern void rbgobj_add_relative_removable(VALUE obj, VALUE relative,
                                          ID obj_ivar_id, VALUE hash_key);
extern void rbgobj_remove_relative(VALUE obj, ID obj_ivar_id, VALUE hash_key);

extern GObject* rbgobj_gobject_new(GType type, VALUE params_hash);
extern VALUE rbgobj_create_object(VALUE klass);

extern void rbgobj_define_property_accessors(VALUE klass);
extern void rbgobj_define_signal_constants(VALUE klass);


/* rbgobj_type.c */
extern const RGObjClassInfo* rbgobj_lookup_class(VALUE klass);
extern const RGObjClassInfo* rbgobj_lookup_class_by_gtype(GType gtype);
extern VALUE rbgobj_define_class(GType gtype, gchar* name, VALUE module,
                                 void* mark, void* free); 
extern VALUE rbgobj_cType;
extern VALUE rbgobj_gtype_new(GType gtype);
extern GType rbgobj_gtype_get(VALUE obj);


/* rbgobj_closure.c */
extern GClosure* g_rclosure_new(VALUE callback_proc, VALUE extra_args);

/* rbgobj_value.c */
extern VALUE rbgobj_gvalue_to_rvalue(const GValue* value);
extern void rbgobj_rvalue_to_gvalue(VALUE val, GValue* result);

typedef void (*RValueToGValueFunc)(VALUE from, GValue* to);
typedef VALUE (*GValueToRValueFunc)(const GValue* from);
extern void rbgobj_register_r2g_func(GType gtype, RValueToGValueFunc func);
extern void rbgobj_register_g2r_func(GType gtype, GValueToRValueFunc func);

/* rbgobj_valuetypes.c */
extern VALUE rbgobj_ptr_new(GType type, gpointer ptr);
extern gpointer rbgobj_ptr2cptr(VALUE ptr);

#define RBGOBJ_TYPE_RUBY_VALUE (rbgobj_ruby_value_get_type())
extern GType rbgobj_ruby_value_get_type();
extern VALUE g_value_get_ruby_value(const GValue* value);
extern void g_value_set_ruby_value(GValue* value, VALUE ruby);

/* rbgobj_object.c */

extern void rbgobj_register_property_setter(GType gtype, const char* prop_name, RValueToGValueFunc func);
extern void rbgobj_register_property_getter(GType gtype, const char* prop_name, GValueToRValueFunc func);

/* rbgobj_boxed.c */
extern VALUE rbgobj_boxed_create(VALUE klass);
extern gpointer rbgobj_boxed_get(VALUE obj, GType gtype);
extern VALUE rbgobj_make_boxed(gpointer data, GType gtype);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RBGOBJECT_H__ */
