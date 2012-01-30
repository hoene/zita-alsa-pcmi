# ----------------------------------------------------------------------------
#
#  Copyright (C) 2006-2012 Fons Adriaensen <fons@linuxaudio.org>
#    
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http:#www.gnu.org/licenses/>.
#
# ----------------------------------------------------------------------------


# Modify as required.
#
PREFIX = /usr/local
SUFFIX := $(shell uname -m | sed -e 's/^unknown/$//' -e 's/^i.86/$//' -e 's/^x86_64/$/64/')
LIBDIR = lib$(SUFFIX)


MAJVERS = 0
MINVERS = 1.1
VERSION = $(MAJVERS).$(MINVERS)


CPPFLAGS += -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
CXXFLAGS += -O2 -Wall -I. -fPIC
LDLFAGS +=
LDLIBS +=


ZITA-ALSA-PCMI_SO = libzita-alsa-pcmi.so
ZITA-ALSA-PCMI_MAJ = $(ZITA-ALSA-PCMI_SO).$(MAJVERS)
ZITA-ALSA-PCMI_MIN = $(ZITA-ALSA-PCMI_MAJ).$(MINVERS)
ZITA-ALSA-PCMI_DEP = -lasound
ZITA-ALSA-PCMI_O = zita-alsa-pcmi.o
ZITA-ALSA-PCMI_H = zita-alsa-pcmi.h


$(ZITA-ALSA-PCMI_MIN): $(ZITA-ALSA-PCMI_O)
	g++ -shared $(LDFLAGS) -Wl,-soname,$(ZITA-ALSA-PCMI_MAJ) -o $(ZITA-ALSA-PCMI_MIN) $(ZITA-ALSA-PCMI_O) $(ZITA-ALSA-PCMI_DEP)


install:	$(ZITA-ALSA-PCMI_MIN)
	install -Dm 644 $(ZITA-ALSA-PCMI_H)   $(DESTDIR)$(PREFIX)/include/$(ZITA-ALSA-PCMI_H)
	install -Dm 644 $(ZITA-ALSA-PCMI_MIN) $(DESTDIR)$(PREFIX)/$(LIBDIR)/$(ZITA-ALSA-PCMI_MIN)
	ln -sf $(ZITA-ALSA-PCMI_MIN) $(DESTDIR)$(PREFIX)/$(LIBDIR)/$(ZITA-ALSA-PCMI_SO)
	ldconfig

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/include/zita-alsa-pcmi.h
	rm -rf $(DESTDIR)$(PREFIX)/$(LIBDIR)/libzita-alsa-pcmi*

clean:
	/bin/rm -f *~ *.o *.a *.d *.so.*

