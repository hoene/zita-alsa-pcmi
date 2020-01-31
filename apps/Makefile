# ----------------------------------------------------------------------------
#
#  Copyright (C) 2006-2018 Fons Adriaensen <fons@linuxaudio.org>
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
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

VERSION = "0.3.2"
CPPFLAGS +=  -MMD -MP -DVERSION=\"$(VERSION)\"
CXXFLAGS += -O2 -Wall


all:	alsa_loopback alsa_delay


ALSA_LOOPBACK_O =	alsa_loopback.o pxthread.o
alsa_loopback:	LDLIBS += -lzita-alsa-pcmi -lasound -lpthread -lrt
alsa_loopback:	$(ALSA_LOOPBACK_O)
	$(CXX) $(LDFLAGS) -o $@ $(ALSA_LOOPBACK_O) $(LDLIBS)
$(ALSA_LOOPBACK_O):
-include $(_ALSA_LOOPBACK_O:%.o=%.d)


ALSA_DELAY_O =	alsa_delay.o mtdm.o pxthread.o
alsa_delay:	LDLIBS += -lzita-alsa-pcmi -lasound -lpthread -lrt
alsa_delay:	$(ALSA_DELAY_O)
	$(CXX) $(LDFLAGS) -o $@ $(ALSA_DELAY_O) $(LDLIBS)
$(ALSA_DELAY_O):
-include $(ALSA_DELAY_O:%.o=%.d)


install:	all
	install -d $(BINDIR)
	install -m 755 alsa_loopback  $(DESTDIR)$(BINDIR)
	install -m 755 alsa_delay     $(DESTDIR)$(BINDIR)


uninstall:
	rm -f $(DESTDIR)$(BINDIR)/alsa_loopback
	rm -f $(DESTDIR)$(BINDIR)/bin/alsa_delay


clean:
	/bin/rm -f *~ *.o *.a *.d *.so alsa_loopback alsa_delay

