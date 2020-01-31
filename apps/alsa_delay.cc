// ----------------------------------------------------------------------------
//
//  Copyright (C) 2006-2018 Fons Adriaensen <fons@linuxaudio.org>
//    
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <zita-alsa-pcmi.h>
#include "pxthread.h"
#include "mtdm.h"


class Audiothr;

static Audiothr  *audio = 0;


class Audiothr : public Pxthread
{
public:

    Audiothr (const char *playdev, const char *captdev,
	      int fsamp, int frsize, int nfrags, int inp, int out);
    virtual ~Audiothr (void);

    void stop (void) { _stop = true; }
    bool running (void) const { return !_stop; }
    MTDM *mtdm (void) const { return (MTDM *) _mtdm; }

private:

    virtual void thr_main (void);

    Alsa_pcmi  *_pcmi;
    MTDM       *_mtdm;
    bool        _stop;
    int         _frsize;
    float      *_ipbuf;
    float      *_opbuf;
    int         _inp;
    int         _out;
};


Audiothr::Audiothr (const char *playdev, const char *captdev,
		    int fsamp, int frsize, int nfrags, int inp, int out) :
    _pcmi (0),
    _mtdm (0),
    _stop (true),
    _frsize (frsize),
    _ipbuf (0),
    _opbuf (0),
    _inp (inp),
    _out (out)
{
    _mtdm  = new MTDM (fsamp);
    _ipbuf = new float [frsize];
    _opbuf = new float [frsize];

    // Create and initialise the audio device.
    _pcmi = new Alsa_pcmi (playdev, captdev, 0, fsamp, frsize, nfrags, 0);
    if (_pcmi->state ()) 
    {
	fprintf (stderr, "Can't open ALSA device\n");
        delete _pcmi;
        exit (0);
    }
    _pcmi->printinfo ();
    _stop = false;

    // Run a real-time thread to process audio.
    if (thr_start (SCHED_FIFO, -20, 0x20000))
    {
 	fprintf (stderr, "Can't run in RT mode, trying normal scheduling.\n");
        if (thr_start (SCHED_OTHER, 0, 0x20000))
        {
 	    fprintf (stderr, "Can't start audio thread.\n");
	    exit (1);
	}
    }
}


Audiothr::~Audiothr (void)
{
    delete[] _ipbuf;
    delete[] _opbuf;
    delete _pcmi;
    delete _mtdm;
}


void Audiothr::thr_main (void)
{
    int i, k;

    // Start the audio device.
    _pcmi->pcm_start ();

    // Main loop.
    while (!_stop)
    {
	k = _pcmi->pcm_wait ();  
	if (k < _frsize)
	{
	    // Normally we shouldn't do this in a real-time context.
	    fprintf (stderr, "Error: pcm_wait returned %d.", k);
	}
        while (k >= _frsize)
       	{
	    // Read input signal.
            _pcmi->capt_init (_frsize);
            _pcmi->capt_chan (_inp, _ipbuf, _frsize);
            _pcmi->capt_done (_frsize);

	    // Process.
	    _mtdm->process (_frsize, _ipbuf, _opbuf);

	    // Write output signal and clear all other outputs.
            _pcmi->play_init (_frsize);
            _pcmi->play_chan (_out, _opbuf, _frsize);              
	    for (i = 0; i < _pcmi->nplay (); i++)
	    {
		if (i != _out) _pcmi->clear_chan (i, _frsize);
	    }
            _pcmi->play_done (_frsize);

            k -= _frsize;
	}
    }

    // Stop the audio device.
    _pcmi->pcm_stop ();
}


static void sigint_handler (int)
{
    signal (SIGINT, SIG_IGN);
    audio->stop ();
}


int main (int ac, char *av [])
{
    int       fsamp, frsize, nfrags, inp, out;
    MTDM     *mtdm;
    float     t;

    if (ac < 8)
    {
	fprintf (stderr, "alsa-delay <playdev> <captdev> <fsamp> <frsize> <nfrags> <input> <output>\n");
	fprintf (stderr, "Input and output numbers start at 1.\n");
        return 1;
    }

    fsamp = atoi (av [3]);
    frsize = atoi (av [4]);
    nfrags = atoi (av [5]);
    inp = atoi (av [6]) - 1;
    out = atoi (av [7]) - 1;
    
    audio = new Audiothr (av [1], av [2], fsamp, frsize, nfrags, inp, out);
    mtdm = audio->mtdm ();
    t = 1000.0f / fsamp;

    signal (SIGINT, sigint_handler);

    while (audio->running ())
    {
	usleep (250000);

	if (mtdm->resolve () < 0) printf ("Signal below threshold...\n");
	else 
	{
	    if (mtdm->err () > 0.3) 
	    {
		mtdm->invert ();
		mtdm->resolve ();
	    }
	    printf ("%10.3lf frames %10.3lf ms", mtdm->del (), mtdm->del () * t);
	    if (mtdm->err () > 0.2) printf (" ??");
            if (mtdm->inv ()) printf (" Inv");
	    printf ("\n");
	}
    }

    usleep (250000);
    delete audio;
    return 0;
}


// --------------------------------------------------------------------------------

