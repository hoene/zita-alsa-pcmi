# ----------------------------------------------------------------------------
#
#  Copyright (C) 2003-2011 Fons Adriaensen <fons@linuxaudio.org>
#    
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# ----------------------------------------------------------------------------


PREFIX = /usr/local
SUFFIX := $(shell uname -m | sed -e 's/^unknown/$//' -e 's/^i.86/$//' -e 's/^x86_64/$/64/')
LIBDIR = lib$(SUFFIX)
VERSION = "0.1.0"
LDFLAGS += -L$(PREFIX)/$(LIBDIR)
CPPFLAGS += -DVERSION=\"$(VERSION)\"
CXXFLAGS += -O2 -Wall -MMD -MP


all:	alsa_loopback alsa_delay


ALSA_LOOPBACK_O =	alsa_loopback.o pxthread.o
alsa_loopback:	LDLIBS += -lzita-alsa-pcmi -lasound -lpthread -lrt
alsa_loopback:	$(ALSA_LOOPBACK_O)
	g++ $(LDFLAGS) -o $@ $(ALSA_LOOPBACK_O) $(LDLIBS)
$(ALSA_LOOPBACK_O):
-include $(_ALSA_LOOPBACK_O:%.o=%.d)


ALSA_DELAY_O =	alsa_delay.o mtdm.o pxthread.o
alsa_delay:	LDLIBS += -lzita-alsa-pcmi -lasound -lpthread -lrt
alsa_delay:	$(ALSA_DELAY_O)
	g++ $(LDFLAGS) -o $@ $(ALSA_DELAY_O) $(LDLIBS)
$(ALSA_DELAY_O):
-include $(ALSA_DELAY_O:%.o=%.d)


install:	all
	install -Dm 755 alsa_loopback  $(DESTDIR)$(PREFIX)/bin/alsa_loopback
	install -Dm 755 alsa_delay     $(DESTDIR)$(PREFIX)/bin/alsa_delay


uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/alsa_loopback
	rm -f $(DESTDIR)$(PREFIX)/bin/alsa_delay


clean:
	/bin/rm -f *~ *.o *.a *.d *.so alsa_loopback alsa_delay

