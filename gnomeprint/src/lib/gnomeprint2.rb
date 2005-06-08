require "glib2"
require "libart2"
require "pango"
require "gnomeprint2.so"

module Gnome
  module Print
    LOG_DOMAIN = "GnomePrint"
  end
  
  module PrintCupsPlugin
    LOG_DOMAIN = "GnomePrintCupsPlugin"
  end
  
  module PrintPAPIPlugin
    LOG_DOMAIN = "GnomePrintPAPIPlugin"
  end
  
  module PrintLpdPlugin
    LOG_DOMAIN = "GnomePrintLpdPlugin"
  end

  class PrintContext
    def image(pixbuf)
      save do
        scale(pixbuf.width, pixbuf.height)
        args = [pixbuf.pixels, pixbuf.width, pixbuf.height, pixbuf.rowstride]
        if pixbuf.has_alpha?
          rgba_image(*args)
        else
          rgb_image(*args)
        end
      end
    end
  end
end

GLib::Log.set_log_domain(Gnome::Print::LOG_DOMAIN)
GLib::Log.set_log_domain(Gnome::PrintCupsPlugin::LOG_DOMAIN)
GLib::Log.set_log_domain(Gnome::PrintPAPIPlugin::LOG_DOMAIN)
GLib::Log.set_log_domain(Gnome::PrintLpdPlugin::LOG_DOMAIN)
