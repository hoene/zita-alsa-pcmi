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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <zita-alsa-pcmi.h>
#include "pxthread.h"


// --------------------------------------------------------------------------------


static char  playdev [256];
static char  captdev [256];
static int   fsamp;
static int   frsize;
static int   nfrags;


class Audiothr : public Pxthread
{
private:
 
    virtual void thr_main (void);
};


void Audiothr::thr_main (void)
{
    Alsa_pcmi  *D;
    float      *buf;
    int        i, k;

    // Interleaved buffer for channels 1 and 2.
    buf = new float [frsize * 2];

    // Create and initialise the audio device.
    D = new Alsa_pcmi (playdev, captdev, 0, fsamp, frsize, nfrags, 0);
    if (D->state ()) 
    {
	fprintf (stderr, "Can't open ALSA device\n");
        delete D;
        exit (1);
    }
    if ((D->ncapt () < 2) || (D->nplay () < 2))
    {
	fprintf (stderr, "Expected a stereo device.\n");
        delete D;
        exit (1);
    }
    D->printinfo ();

    // Start the audio device.
    D->pcm_start ();

    // Main loop
    while (true)
    {
	k = D->pcm_wait ();  
	if (k < frsize)
	{
	    // Normally we shouldn't do this in a real-time context.
	    fprintf (stderr, "Error: pcm_wait returned %d.\n", k);
	}
        while (k >= frsize)
       	{
	    // Copy the first 2 inputs to the first two outputs.
	    // Clear all other outputs.
            D->capt_init (frsize);
            D->capt_chan (0, buf + 0, frsize, 2);
            D->capt_chan (1, buf + 1, frsize, 2);              
            D->capt_done (frsize);

            D->play_init (frsize);
            D->play_chan (0, buf + 0, frsize, 2);
            D->play_chan (1, buf + 1, frsize, 2);              
	    for (i = 2; i < D->nplay (); i++) D->clear_chan (i, frsize);
            D->play_done (frsize);

            k -= frsize;
	}
    }

    // Stop the audio device.
    D->pcm_stop ();

    delete D;
    delete[] buf;
}


// --------------------------------------------------------------------------------


int main (int ac, char *av [])
{
    Audiothr A;

    if (ac < 6)
    {
	fprintf (stderr, "alsa-loopback <playdev> <captdev> <fsamp> <frsize> <nfrags>\n");
        return 1;
    }

    strcpy (playdev, av [1]);
    strcpy (captdev, av [2]);
    fsamp  = atoi (av [3]);
    frsize = atoi (av [4]);
    nfrags = atoi (av [5]);

    // Run a real-time thread to handle audio.
    if (A.thr_start (SCHED_FIFO, -50, 0x20000))
    {
 	fprintf (stderr, "Can't run in RT mode, trying normal scheduling.\n");
        if (A.thr_start (SCHED_OTHER, 0, 0x20000))
        {
 	    fprintf (stderr, "Can't start audio thread.\n");
	    return 1;
	}
    }

    sleep (1000000);

    return 0;
}


// --------------------------------------------------------------------------------
