=begin
extconf.rb for Ruby/Libglade2 extention library
=end

PACKAGE_NAME = "libglade2"
PACKAGE_ID   = "libglade-2.0"

TOPDIR = File.expand_path(File.dirname(__FILE__) + '/..')
MKMF_GNOME2_DIR = TOPDIR + '/glib/src/lib'
SRCDIR = TOPDIR + '/libglade'

$LOAD_PATH.unshift MKMF_GNOME2_DIR

require 'mkmf-gnome2'

PKGConfig.have_package(PACKAGE_ID) or exit 1
setup_win32(PACKAGE_NAME)

have_header("libintl.h")

have_func("bindtextdomain")
have_func("bind_textdomain_codeset")

add_depend_package("glib2", "glib/src", TOPDIR)
add_depend_package("gtk2", "gtk/src", TOPDIR)

if /mswin32/ =~ RUBY_PLATFORM
# gladewin32 has been changed?
#  $libs.gsub!(/ xml2.lib /, " libxml2.lib ")
#  $libs.gsub!(/ z.lib /, " libz.lib ")
  #FIXME
  $CFLAGS += " -DHAVE_BINDTEXTDOMAIN -DHAVE_BIND_TEXTDOMAIN_CODESET "
end

make_version_header("LIBGLADE", PACKAGE_ID, ".")

create_makefile(PACKAGE_NAME)  
