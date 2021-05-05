/*
  fetch.c

  Astronomy Picture of the Day (APOD) viewer client
  for Ataris using #Fujinet.

  This module fetches an APOD photo (or sample image)
  from the APOD server via the FujiNet "N:" device,
  using the HTTP transport.

  By Bill Kendrick <bill@newbreedsoftware.com>
  2021-03-27 - 2021-05-04
*/

#include <stdio.h>
#include <atari.h>
#include "fetch.h"
#include "menu.h"
#include "nsio.h"
#include "rgb.h"
#include "screen_helpers.h"


/* The base URL for the web app */
char * baseurl = "N:HTTP://billsgames.com/fujinet/apod/index.php";

/* Space to store the composed URL */
char url[255];


/**
 * Fetch the image
 *
 * @param byte choice -- which viewing mode we're using, and hence which image
 *   conversion to do / fetch (see "menus.h")
 * @param byte sample -- which static sample image to fetch, if doing so
 *   (versus downloading a particular APOD photo)
 * @param byte pick_yr, pick_mo, pick_day -- the date (year 0 = 2000),
 *   month, and day we want to fetch from the APOD site
 *   (if pick_day is 0, we're grabbing a sample)
 */
void fetch_image(unsigned char choice, char sample, int size, unsigned char pick_yr, unsigned pick_mo, unsigned pick_day) {
  unsigned short data_len, data_read;
  int i;

  if (pick_day == 0) {
    snprintf(url, sizeof(url), "%s?mode=%s&sample=%d", baseurl, modes[choice], sample);
  } else {
    snprintf(url, sizeof(url), "%s?mode=%s&sample=%d&date=%02d%02d%02d", baseurl, modes[choice], sample, pick_yr, pick_mo, pick_day);
  }

  nopen(1 /* unit 1 */, url, 4 /* read */);
  /* FIXME: Check for error */

  if (size == 7680) {
    /* Single screen image to load */
    nread(1, scr_mem1, (unsigned short) size);
  } else {
    /* Multiple screen images to load... */

    OS.color4 = rgb_table[0];
    wait_for_vblank();

    for(data_read = 0; data_read < 7680; data_read += data_len)
    {
      nstatus(1);
      data_len=(OS.dvstat[1] << 8) + OS.dvstat[0];
      if (data_len == 0) break;
      if (data_read + data_len > 7680) data_len = 7680 - data_read;
      nread(1, scr_mem1 + data_read, data_len);
    }

    OS.sdmctl = 0;
    wait_for_vblank();
    OS.sdlst = dlist2;
    OS.color4 = rgb_table[1];
    wait_for_vblank();
    OS.sdmctl = DMACTL_PLAYFIELD_NORMAL | DMACTL_DMA_FETCH;
    wait_for_vblank();

    for(data_read = 0; data_read < 7680; data_read += data_len)
    {
      nstatus(1);
      data_len=(OS.dvstat[1] << 8) + OS.dvstat[0];
      if (data_len == 0) break;
      if (data_read + data_len > 7680) data_len = 7680 - data_read;
      nread(1, scr_mem2 + data_read, data_len);
    }

    OS.sdmctl = 0;
    wait_for_vblank();
    OS.sdlst = dlist3;
    OS.color4 = rgb_table[2];
    wait_for_vblank();
    OS.sdmctl = DMACTL_PLAYFIELD_NORMAL | DMACTL_DMA_FETCH;
    wait_for_vblank();

    for(data_read = 0; data_read < 7680; data_read += data_len)
    {
      nstatus(1);
      data_len=(OS.dvstat[1] << 8) + OS.dvstat[0];
      if (data_len == 0) break;
      if (data_read + data_len > 7680) data_len = 7680 - data_read;
      nread(1, scr_mem3 + data_read, data_len);
    }

    OS.sdlst = dlist1;

    OS.color4 = 0;
  }

  if (choice == CHOICE_MEDRES_COLOR) {
    nread(1, (char *) 712, 1);
    nread(1, (char *) 708, 3);
  }

  nread(1, txt_mem, 256);
  for (i = 0; i < 256; i++) {
    if (txt_mem[i] < 32) {
      txt_mem[i] += 64;
    } else if (txt_mem[i] < 96) {
      txt_mem[i] -= 32;
    }
  }

  nclose(1 /* unit 1 */);
}
