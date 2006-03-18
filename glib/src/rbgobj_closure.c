/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/**********************************************************************

  rbgobj_closure.c -

  $Author: ktou $
  $Date: 2006/03/18 06:53:05 $

  Copyright (C) 2002,2003  Masahiro Sakai

**********************************************************************/

#include "global.h"

static ID id_call;
static gboolean rclosure_initialized = FALSE;

#define CALLBACK_KEY       rb_str_new2("callback")
#define EXTRA_ARGS_KEY     rb_str_new2("extra_args")

typedef struct _GRClosure GRClosure;

struct _GRClosure
{
    GClosure closure;
    VALUE holder;
    gint count;
    GValToRValSignalFunc g2r_func;
};

static VALUE
rclosure_default_g2r_func(num, values)
    guint num;
    const GValue* values;
{
    int i;
    VALUE args = rb_ary_new2(num);
    for (i = 0; i < num; i++)
        rb_ary_store(args, i, GVAL2RVAL(&values[i]));
    return args;
}

struct marshal_arg
{
    GClosure*       closure;
    GValue*         return_value;
    guint           n_param_values;
    const GValue*   param_values;
    gpointer        invocation_hint;
    gpointer        marshal_data;
};

static int
rclosure_alive_p(GRClosure *rclosure)
{
    return rclosure->count > 0;
}

static VALUE
rclosure_marshal_body(struct marshal_arg* arg)
{
    GRClosure*      rclosure        = (GRClosure *)(arg->closure);
    GValue*         return_value    = arg->return_value;   
    guint           n_param_values  = arg->n_param_values; 
    const GValue*   param_values    = arg->param_values;
    //gpointer        invocation_hint = arg->invocation_hint;
    //gpointer        marshal_data    = arg->marshal_data;

    VALUE ret = Qnil;
    VALUE args;
    GValToRValSignalFunc func;

    if (rclosure->g2r_func){
        func = (GValToRValSignalFunc)rclosure->g2r_func;
    } else { 
        func = (GValToRValSignalFunc)rclosure_default_g2r_func;
    }
    args = (*func)(n_param_values, param_values);

    if (rclosure_alive_p(rclosure)) {
        VALUE callback, extra_args;
        callback = rb_hash_aref(rclosure->holder, CALLBACK_KEY);
        extra_args = rb_hash_aref(rclosure->holder, EXTRA_ARGS_KEY);

        if (!NIL_P(extra_args)) {
            args = rb_ary_concat(args, extra_args);
        }

        ret = rb_apply(callback, id_call, args);
    } else {
        rb_warn("GRClosure invoking callback: already destroyed");
    }

    if (return_value && G_VALUE_TYPE(return_value))
        rbgobj_rvalue_to_gvalue(ret, return_value);

    return Qnil;
}

static void
rclosure_marshal(GClosure*       closure,
                 GValue*         return_value,
                 guint           n_param_values,
                 const GValue*   param_values,
                 gpointer        invocation_hint,
                 gpointer        marshal_data)
{
    struct marshal_arg arg;
    int state;

    if (!rclosure_initialized) {
        g_closure_invalidate(closure);
        return;
    }
 
    arg.closure         = closure;
    arg.return_value    = return_value;
    arg.n_param_values  = n_param_values;
    arg.param_values    = param_values;
    arg.invocation_hint = invocation_hint;
    arg.marshal_data    = marshal_data;

    rb_protect((VALUE (*)_((VALUE)))rclosure_marshal_body, (VALUE)&arg, &state);

    if (state){
        /* FIXME */
#if 0
        char buf[BUFSIZ];
        snprintf(buf, BUFSIZ, 
                 "%s:%d warning: unexpected jump occured in GClosure invocation\n",
                 ruby_sourcefile, ruby_sourceline);
        rb_write_deferr(buf);
#else
        rb_jump_tag(state);
#endif
    }
}


static void
rclosure_invalidate(gpointer data, GClosure* closure)
{
    GRClosure *rclosure = (GRClosure*)closure;

    if (rclosure_alive_p(rclosure)) {
        if (rclosure->count != 1) {
            rb_warn("GRClosure invalidate: strange reference count: %d\n",
                    rclosure->count);
        }
        rclosure->count = 0;
    }
}

GClosure*
g_rclosure_new(VALUE callback_proc, VALUE extra_args, GValToRValSignalFunc g2r_func)
{
    GRClosure* closure;

    closure = (GRClosure*)g_closure_new_simple(sizeof(GRClosure), NULL);

    closure->holder     = rb_hash_new();
    closure->count      = 0;
    closure->g2r_func   = g2r_func;

    rb_hash_aset(closure->holder, CALLBACK_KEY, callback_proc);
    rb_hash_aset(closure->holder, EXTRA_ARGS_KEY, extra_args);

    g_closure_set_marshal((GClosure*)closure, &rclosure_marshal);
    g_closure_add_invalidate_notifier((GClosure*)closure, NULL,
                                      &rclosure_invalidate);

    return (GClosure*)closure;
}

static void
rclosure_weak_notify(gpointer data, GObject* where_the_object_was)
{
    GRClosure *rclosure = data;
    if (rclosure_alive_p(rclosure)) {
        rclosure->count--;
    }
}

void
g_rclosure_attach(GClosure *closure, VALUE object)
{
    GRClosure *rclosure = (GRClosure *)closure;
    gobj_holder *holder;

    G_CHILD_ADD(object, rclosure->holder);

    rclosure->count++;
    Data_Get_Struct(object, gobj_holder, holder);
    g_object_weak_ref(holder->gobj, rclosure_weak_notify, rclosure);
}

static void
rclosure_end_proc(VALUE _)
{
    rclosure_initialized = FALSE;
}

static void
Init_rclosure()
{
    id_call = rb_intern("call");

    rclosure_initialized = TRUE;
    rb_set_end_proc(rclosure_end_proc, Qnil);
}

/**********************************************************************/

static VALUE
closure_initialize(self)
    VALUE self;
{
    GClosure* closure = g_rclosure_new(G_BLOCK_PROC(), Qnil, NULL);
    G_INITIALIZE(self, closure);
    g_closure_sink(closure);
    return self;
}

static VALUE
closure_in_marshal(self)
    VALUE self;
{
    GClosure* closure = RVAL2BOXED(self, G_TYPE_CLOSURE);
    return closure->in_marshal ? Qtrue : Qfalse;
}

static VALUE
closure_is_invalid(self)
    VALUE self;
{
    GClosure* closure = RVAL2BOXED(self, G_TYPE_CLOSURE);
    return closure->is_invalid ? Qtrue : Qfalse;
}

static VALUE
closure_invalidate(self)
    VALUE self;
{
    GClosure* closure = RVAL2BOXED(self, G_TYPE_CLOSURE);
    g_closure_invalidate(closure);
    return self;
}

static void
Init_closure()
{
    VALUE cClosure = G_DEF_CLASS(G_TYPE_CLOSURE, "Closure", mGLib);
    rb_define_method(cClosure, "initialize", closure_initialize, 0);
    rb_define_method(cClosure, "in_marshal?", closure_in_marshal, 0);
    rb_define_method(cClosure, "invalid?", closure_is_invalid, 0);
    rb_define_method(cClosure, "invalidate", closure_invalidate, 0);
}

/**********************************************************************/

void Init_gobject_gclosure()
{
    Init_rclosure();
    Init_closure();
}
