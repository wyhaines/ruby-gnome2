/*****************************************************************************
 *
 * rbgconf-entry.c: GConf::Entry class.
 *
 * Copyright (C) 2002,2003 Nikolai :: lone-star :: Weibull <lone-star@home.se>.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * $Author: ggc $
 *
 * $Date: 2007/07/13 16:07:28 $
 *
 *****************************************************************************/

/* Includes ******************************************************************/

#include "rbgconf.h"
#include "rbgconf-entry.h"

/* Defines *******************************************************************/

#define _SELF(s)	((GConfEntry *)RVAL2BOXED(s, GCONF_TYPE_ENTRY))

/* Type Definitions **********************************************************/

/* Function Declarations *****************************************************/

/* Global Variables **********************************************************/

/* Function Implementations **************************************************/

static GConfEntry *
rbgconf_entry_copy(entry)
	const GConfEntry *entry;
{
	/*
	 * XXX: i think this actually suffices in this case, hopefully GConf
	 * will agree =). as i see it gconf_client_get_entry() gives us a entry
	 * we can mess about with, and creating a 'copy' in this way should
	 * probably be ok.
	 *
	 * UPDATE: it works without copying. but is that perhaps by pure
	 * chance?
	 */
	GConfEntry *new_entry;
	GConfValue *value;

	g_return_val_if_fail(entry != NULL, NULL);

	value = gconf_entry_get_value(entry);
	if (value != NULL) {
		value = gconf_value_copy(value);
	}
	new_entry = gconf_entry_new_nocopy(
			g_strdup(gconf_entry_get_key(entry)),
			value);
	
	return new_entry;
	/*
	 * XXX: this is the old behavior, may be responsible for causing the
	 * following:
	 * (gconfclock_client.rb:2381): GConf-CRITICAL **: file gconf-value.c:
	 * line 1495
	 * (gconf_entry_unref): assertion `REAL_ENTRY (entry)->refcount > 0'
	 * failed
	 *
	return entry;
	*/
}

GType
gconf_entry_get_type(void)
{
	static GType our_type = 0;

	if (our_type == 0) {
		our_type = g_boxed_type_register_static("GConfEntry",
				(GBoxedCopyFunc)rbgconf_entry_copy,
				(GBoxedFreeFunc)gconf_entry_free);
	}

	return our_type;
}

static VALUE
entry_get_key(self)
	VALUE self;
{
	return CSTR2RVAL(gconf_entry_get_key(_SELF(self)));
}

static VALUE
entry_get_value(self)
	VALUE self;
{
	GConfValue *value = gconf_entry_get_value(_SELF(self));
	if (value != NULL) {
		return GCVAL2RVAL(gconf_value_copy(value));
	} else {
		return Qnil;
	}
}

static VALUE
entry_get_is_default(self)
	VALUE self;
{
	return CBOOL2RVAL(gconf_entry_get_is_default(_SELF(self)));
}

static VALUE
entry_get_schema_name(self)
	VALUE self;
{
	return CSTR2RVAL(gconf_entry_get_schema_name(_SELF(self)));
}

static VALUE
entry_set_is_default(self, is_default)
	VALUE self, is_default;
{
	gconf_entry_set_is_default(_SELF(self), RVAL2CBOOL(is_default));
	return self;
}

static VALUE
entry_set_schema_name(self, name)
	VALUE self, name;
{
	gconf_entry_set_schema_name(_SELF(self), RVAL2CSTR(name));
	return self;
}

static VALUE
entry_set_value(self, val)
	VALUE self, val;
{
	gconf_entry_set_value_nocopy(_SELF(self), RVAL2GCVAL(val));
	return self;
}

void
Init_gconf_entry(m_gconf)
	VALUE m_gconf;
{
	VALUE gconf_entry;

	gconf_entry = G_DEF_CLASS(GCONF_TYPE_ENTRY, "Entry", m_gconf);

	rb_define_method(gconf_entry, "key", entry_get_key, 0);
	rb_define_method(gconf_entry, "value", entry_get_value, 0);
	rb_define_method(gconf_entry, "default?", entry_get_is_default, 0);
	rb_define_method(gconf_entry, "schema_name", entry_get_schema_name, 0);
	rb_define_method(gconf_entry, "set_default", entry_set_is_default,
			 1);
	rb_define_method(gconf_entry, "set_schema_name", entry_set_schema_name,
			 1);
	rb_define_method(gconf_entry, "set_value", entry_set_value, 1);

	G_DEF_SETTERS(gconf_entry);
}

/* vim: set sts=0 sw=8 ts=8: *************************************************/
