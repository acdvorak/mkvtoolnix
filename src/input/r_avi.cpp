
/*
  mkvmerge -- utility for splicing together matroska files
      from component media subtypes

  r_avi.h

  Written by Moritz Bunkus <moritz@bunkus.org>

  Distributed under the GPL
  see the file COPYING for details
  or visit http://www.gnu.org/copyleft/gpl.html
*/

/*!
    \file
    \version $Id$
    \brief AVI demultiplexer module
    \author Moritz Bunkus <moritz@bunkus.org>
*/

// {{{ includes

#include "os.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
// This one goes out to haali ;)
#include <sys/types.h>

extern "C" {
#include <avilib.h>
}

#include "mkvmerge.h"
#include "aac_common.h"
#include "common.h"
#include "error.h"
#include "mpeg4_common.h"
#include "r_avi.h"
#include "p_aac.h"
#include "p_ac3.h"
#include "p_mp3.h"
#include "p_pcm.h"
#include "p_video.h"

#define PFX "avi_reader: "
#define MIN_CHUNK_SIZE 128000

// }}}

// {{{ FUNCTION avi_reader_c::probe_file

int
avi_reader_c::probe_file(mm_io_c *mm_io,
                         int64_t size) {
  unsigned char data[12];

  if (size < 12)
    return 0;
  try {
    mm_io->setFilePointer(0, seek_beginning);
    if (mm_io->read(data, 12) != 12)
      return 0;
    mm_io->setFilePointer(0, seek_beginning);
  } catch (exception &ex) {
    return 0;
  }
  if(strncasecmp((char *)data, "RIFF", 4) ||
     strncasecmp((char *)data+8, "AVI ", 4))
    return 0;
  return 1;
}

// }}}

// {{{ C'TOR

avi_reader_c::avi_reader_c(track_info_c *nti)
  throw (error_c):
  generic_reader_c(nti) {
  long fsize, i;
  int64_t size, bps;
  vector<avi_demuxer_t>::iterator demuxer;

  try {
    io = new mm_io_c(ti->fname, MODE_READ);
    size = io->get_size();
    if (!avi_reader_c::probe_file(io, size))
      throw error_c(PFX "Source is not a valid AVI file.");
  } catch (exception &ex) {
    throw error_c(PFX "Could not read the source file.");
  }

  if (verbose)
    mxinfo("Using AVI demultiplexer for %s. Opening file. This "
           "may take some time depending on the file's size.\n", ti->fname);

  fsize = 0;
  rederive_keyframes = 0;
  vptzr = -1;
  vheaders_set = false;

  frames = 0;
  delete io;
  if ((avi = AVI_open_input_file(ti->fname, 1)) == NULL) {
    const char *msg = PFX "Could not initialize AVI source. Reason: ";
    char *s, *error;
    error = AVI_strerror();
    s = (char *)safemalloc(strlen(msg) + strlen(error) + 1);
    mxprints(s, "%s%s", msg, error);
    throw error_c(s, true);
  }

  fps = AVI_frame_rate(avi);
  maxframes = AVI_video_frames(avi);
  for (i = 0; i < maxframes; i++)
    if (AVI_frame_size(avi, i) > fsize)
      fsize = AVI_frame_size(avi, i);
  max_frame_size = fsize;

  create_packetizers();

  foreach(demuxer, ademuxers) {
    bps = demuxer->samples_per_second * demuxer->channels *
      demuxer->bits_per_sample / 8;
    if (bps > fsize)
      fsize = bps;
  }

  if (video_fps < 0)
    video_fps = fps;
  chunk_size = max_frame_size < MIN_CHUNK_SIZE ? MIN_CHUNK_SIZE :
    max_frame_size;
  chunk = (unsigned char *)safemalloc(chunk_size);
  act_wchar = 0;
  old_key = 0;
  old_chunk = NULL;
  video_done = 0;
  dropped_frames = 0;
}

// }}}

// {{{ D'TOR

avi_reader_c::~avi_reader_c() {
  if (avi != NULL)
    AVI_close(avi);

  safefree(chunk);
  safefree(old_chunk);
  ti->private_data = NULL;

  mxverb(2, "avi_reader_c: Dropped video frames: %d\n", dropped_frames);
}

// }}}

void
avi_reader_c::create_packetizer(int64_t tid) {
  char *codec;

  if ((tid == 0) && demuxing_requested('v', 0) && (vptzr == -1)) {
    codec = AVI_video_compressor(avi);
    if (!strcasecmp(codec, "DIV3") ||
        !strcasecmp(codec, "AP41") || // Angel Potion
        !strcasecmp(codec, "MPG3") ||
        !strcasecmp(codec, "MP43"))
      is_divx = RAVI_DIVX3;
    else if (!strcasecmp(codec, "MP42") ||
             !strcasecmp(codec, "DIV2") ||
             !strcasecmp(codec, "DIVX") ||
             !strcasecmp(codec, "XVID") ||
             !strcasecmp(codec, "DX50"))
      is_divx = RAVI_MPEG4;
    else
      is_divx = 0;

    ti->private_data = (unsigned char *)avi->bitmap_info_header;
    if (ti->private_data != NULL)
      ti->private_size = get_uint32(&avi->bitmap_info_header->bi_size);
    ti->id = 0;                 // ID for the video track.
    vptzr = add_packetizer(new video_packetizer_c(this, NULL,
                                                  AVI_frame_rate(avi),
                                                  AVI_video_width(avi),
                                                  AVI_video_height(avi),
                                                  false, ti));
    if (verbose)
      mxinfo("+-> Using video output module for video track ID 0.\n");
  }
  if (tid == 0)
    return;

  if ((tid <= AVI_audio_tracks(avi)) && demuxing_requested('a', tid))
    add_audio_demuxer(tid - 1);
}

void
avi_reader_c::create_packetizers() {
  uint32_t i;

  for (i = 0; i < ti->track_order->size(); i++)
    create_packetizer((*ti->track_order)[i]);

  create_packetizer(0);

  for (i = 0; i < AVI_audio_tracks(avi); i++)
    create_packetizer(i + 1);
}

// {{{ FUNCTION avi_reader_c::add_audio_demuxer

void
avi_reader_c::add_audio_demuxer(int aid) {
  generic_packetizer_c *packetizer;
  vector<avi_demuxer_t>::iterator it;
  avi_demuxer_t demuxer;
  alWAVEFORMATEX *wfe;
  uint32_t audio_format;

  foreach(it, ademuxers)
    if (it->aid == aid) // Demuxer already added?
      return;

  memset(&demuxer, 0, sizeof(avi_demuxer_t));
  demuxer.aid = aid;
  demuxer.ptzr = -1;
  ti->id = aid + 1;             // ID for this audio track.

  wfe = avi->wave_format_ex[aid];
  AVI_set_audio_track(avi, aid);
  if (AVI_read_audio_chunk(avi, NULL) < 0) {
    mxwarn("Could not find an index for audio track %d (avilib error message: "
           "%s). Skipping track.\n", aid + 1, AVI_strerror());
    return;
  }
  audio_format = AVI_audio_format(avi);
  demuxer.samples_per_second = AVI_audio_rate(avi);
  demuxer.channels = AVI_audio_channels(avi);
  demuxer.bits_per_sample = AVI_audio_bits(avi);
  ti->avi_block_align = get_uint16(&wfe->n_block_align);
  ti->avi_avg_bytes_per_sec = get_uint32(&wfe->n_avg_bytes_per_sec);
  ti->avi_samples_per_chunk = get_uint32(&avi->stream_headers[aid].dw_scale);

  if (get_uint16(&wfe->cb_size) > 0) {
    ti->private_data = (unsigned char *)(wfe + 1);
    ti->private_size = get_uint16(&wfe->cb_size);
  } else {
    ti->private_data = NULL;
    ti->private_size = 0;
  }
  ti->avi_samples_per_sec = demuxer.samples_per_second;

  switch(audio_format) {
    case 0x0001: // raw PCM audio
      if (verbose)
        mxinfo("+-> Using the PCM output module for audio track ID %d.\n",
               aid + 1);
      packetizer = new pcm_packetizer_c(this, demuxer.samples_per_second,
                                        demuxer.channels,
                                        demuxer.bits_per_sample, ti);
      break;
    case 0x0055: // MP3
      if (verbose)
        mxinfo("+-> Using the MPEG audio output module for audio track ID "
               "%d.\n", aid + 1);
      packetizer = new mp3_packetizer_c(this, demuxer.samples_per_second,
                                        demuxer.channels, ti);
      break;
    case 0x2000: // AC3
      if (verbose)
        mxinfo("+-> Using the AC3 output module for audio track ID %d.\n",
               aid + 1);
      packetizer = new ac3_packetizer_c(this, demuxer.samples_per_second,
                                        demuxer.channels, 0, ti);
      break;
    case 0x00ff: { // AAC
      int profile, channels, sample_rate, output_sample_rate;
      bool is_sbr;

      if ((ti->private_size != 2) && (ti->private_size != 5))
        mxerror("The AAC track %d does not contain valid headers. The extra "
                "header size is %d bytes, expected were 2 or 5 bytes.\n",
                aid + 1, ti->private_size);
      if (!parse_aac_data(ti->private_data, ti->private_size, profile,
                          channels, sample_rate, output_sample_rate,
                          is_sbr))
        mxerror("The AAC track %d does not contain valid headers. Could not "
                "parse the AAC information.\n", aid + 1);
      if (is_sbr)
        profile = AAC_PROFILE_SBR;
      demuxer.samples_per_second = sample_rate;
      demuxer.channels = channels;
      if (verbose)
        mxinfo("+-> Using the AAC output module for audio track ID %d.\n",
               aid + 1);
      packetizer = new aac_packetizer_c(this, AAC_ID_MPEG4, profile,
                                        demuxer.samples_per_second,
                                        demuxer.channels, ti, false, true);
      if (is_sbr)
        packetizer->set_audio_output_sampling_freq(output_sample_rate);
      break;
    }
    default:
      mxerror("Unknown/unsupported audio format 0x%04x for audio track ID "
              "%d.\n", audio_format, aid + 1);
  }
  demuxer.ptzr = add_packetizer(packetizer);

  ademuxers.push_back(demuxer);
}

// }}}

// {{{ FUNCTION avi_reader_c::is_keyframe

int
avi_reader_c::is_keyframe(unsigned char *data,
                          long size,
                          int suggestion) {
  int i;

  if (!rederive_keyframes)
    return suggestion;

  switch (is_divx) {
    case RAVI_DIVX3:
      i = *((int *)data);
      return ((i & 0x40000000) ? 0 : 1);
    case RAVI_MPEG4:
      for (i = 0; i < size - 5; i++) {
        if ((data[i] == 0x00) && (data[i + 1] == 0x00) &&
            (data[i + 2] == 0x01)) {
          if ((data[i + 3] == 0) || (data[i + 3] == 0xb0))
            return 1;
          if (data[i + 3] == 0xb6) {
            if ((data[i + 4] & 0xc0) == 0x00)
              return 1;
            else
              return 0;
          }
          i += 2;
        }
      }

      return suggestion;
    default:
      return suggestion;
  }
}

// }}}

// {{{ FUNCTION avi_reader_c::read

int
avi_reader_c::read(generic_packetizer_c *ptzr) {
  vector<avi_demuxer_t>::iterator demuxer;
  int key, last_frame, frames_read, size;
  long nread;
  bool need_more_data, done;
  int64_t duration;

  key = 0;
  if ((vptzr != -1) && !video_done && (PTZR(vptzr) == ptzr)) {
    debug_enter("avi_reader_c::read (video)");

    need_more_data = false;
    last_frame = 0;
    done = false;

    // Make sure we have a frame to work with.
    if (old_chunk == NULL) {
      debug_enter("AVI_read_frame");
      nread = AVI_read_frame(avi, (char *)chunk, &key);
      debug_leave("AVI_read_frame");
      if (nread < 0) {
        frames = maxframes + 1;
        done = true;
      } else {
//         key = is_keyframe(chunk, nread, key);
        old_chunk = chunk;
        chunk = (unsigned char *)safemalloc(chunk_size);
        old_key = key;
        old_nread = nread;
        frames++;
      }
    }
    if (!done) {
      frames_read = 1;
      // Check whether we have identical frames
      while (!done && (frames <= (maxframes - 1))) {
        debug_enter("AVI_read_frame");
        nread = AVI_read_frame(avi, (char *)chunk, &key);
        debug_leave("AVI_read_frame");
        if (nread < 0) {
          memory_c mem(old_chunk, old_nread, true);
          PTZR(vptzr)->process(mem, -1, -1,
                               old_key ? VFT_IFRAME : VFT_PFRAMEAUTOMATIC);
          old_chunk = NULL;
          mxwarn(PFX "Reading frame number %d resulted in an error. "
                 "Aborting this track.\n", frames);
          frames = maxframes + 1;
          break;
        }
//         key = is_keyframe(chunk, nread, key);
        if (frames == (maxframes - 1)) {
          last_frame = 1;
          done = true;
        }
        if (nread == 0) {
          frames_read++;
          dropped_frames++;
        }
        else if (nread > 0)
          done = true;
        frames++;
      }
      duration = (int64_t)(1000000000.0 * frames_read / fps);
      if (nread > 0) {
        memory_c mem(old_chunk, old_nread, true);
        PTZR(vptzr)->process(mem, -1, duration,
                             old_key ? VFT_IFRAME : VFT_PFRAMEAUTOMATIC);
        old_chunk = NULL;
        if (! last_frame) {
          if (old_chunk != NULL)
            safefree(old_chunk);
          old_chunk = chunk;
          chunk = (unsigned char *)safemalloc(chunk_size);
          old_key = key;
          old_nread = nread;
        } else if (nread > 0) {
          memory_c mem(chunk, nread, false);
          PTZR(vptzr)->process(mem, -1, duration,
                               key ? VFT_IFRAME : VFT_PFRAMEAUTOMATIC);
        }
      }
    }

    if (last_frame) {
      frames = maxframes + 1;
      video_done = 1;
    } else if (frames != (maxframes + 1))
      need_more_data = true;

    debug_leave("avi_reader_c::read (video)");

    if (need_more_data)
      return EMOREDATA;
    else {
      PTZR(vptzr)->flush();
      return 0;
    }
  }

  foreach(demuxer, ademuxers) {
    unsigned char *audio_chunk;

    if (PTZR(demuxer->ptzr) != ptzr)
      continue;

    debug_enter("avi_reader_c::read (audio)");

    need_more_data = false;

    AVI_set_audio_track(avi, demuxer->aid);
    size = AVI_read_audio_chunk(avi, NULL);
    if (size > 0) {
      audio_chunk = (unsigned char *)safemalloc(size);

      debug_enter("AVI_read_audio");
      nread = AVI_read_audio_chunk(avi, (char *)audio_chunk);
      debug_leave("AVI_read_audio");

      if (nread > 0) {
        size = AVI_read_audio_chunk(avi, NULL);
        if (size > 0)
          need_more_data = true;
        memory_c mem(audio_chunk, nread, true);
        PTZR(demuxer->ptzr)->add_avi_block_size(nread);
        PTZR(demuxer->ptzr)->process(mem);
      } else
        safefree(audio_chunk);
    }

    debug_leave("avi_reader_c::read (audio)");

    if (need_more_data)
      return EMOREDATA;
    else {
      PTZR(demuxer->ptzr)->flush();
      return 0;
    }
  }

  return 0;
}

// }}}

// {{{ FUNCTIONS avi_reader_c::display_priority/_progess

int
avi_reader_c::display_priority() {
  if (vptzr != -1)
    return DISPLAYPRIORITY_HIGH;
  else
    return DISPLAYPRIORITY_LOW;
}

static char wchar[] = "-\\|/-\\|/-";

void
avi_reader_c::display_progress(bool final) {
  int myframes;

  if (vptzr != -1) {
    myframes = frames;
    if (frames == (maxframes + 1))
      myframes--;
    if (final)
      mxinfo("progress: %d/%d frames (100%%)\r", maxframes, maxframes);
    else
      mxinfo("progress: %d/%d frames (%d%%)\r", myframes, maxframes,
             myframes * 100 / maxframes);
  } else {
    mxinfo("Working... %c\r", wchar[act_wchar]);
    act_wchar++;
    if (act_wchar == strlen(wchar))
      act_wchar = 0;
  }
}

// }}}

// {{{ FUNCTION avi_reader_c::set_headers

void
avi_reader_c::set_headers() {
  uint32_t i;
  vector<avi_demuxer_t>::iterator d;

  vheaders_set = false;
  foreach(d, ademuxers)
    d->headers_set = false;

  for (i = 0; i < ti->track_order->size(); i++) {
    if ((*ti->track_order)[i] == 0) {
      if ((vptzr != -1) && !vheaders_set) {
        PTZR(vptzr)->set_headers();
        vheaders_set = true;
      }
      continue;
    }
    foreach(d, ademuxers)
      if ((d->aid + 1) == (*ti->track_order)[i])
        break;
    if ((d != ademuxers.end()) && (d->ptzr != -1) && !d->headers_set) {
      PTZR(d->ptzr)->set_headers();
      d->headers_set = true;
    }
  }

  if ((vptzr != -1) && !vheaders_set) {
    PTZR(vptzr)->set_headers();
    vheaders_set = true;
  }

  foreach(d, ademuxers)
    if (!d->headers_set) {
      PTZR(d->ptzr)->set_headers();
      d->headers_set = true;
    }
}

// }}}

// {{{ FUNCTION avi_reader_c::identify

void
avi_reader_c::identify() {
  int i;
  const char *type;
  uint32_t par_num, par_den;
  bool extended_info_shown;
  
  mxinfo("File '%s': container: AVI\n", ti->fname);
  extended_info_shown = false;
  type = AVI_video_compressor(avi);
  if (!strncasecmp(type, "MP42", 4) ||
      !strncasecmp(type, "DIV2", 4) ||
      !strncasecmp(type, "DIVX", 4) ||
      !strncasecmp(type, "XVID", 4) ||
      !strncasecmp(type, "DX50", 4)) {
    unsigned char *buffer;
    uint32_t width, height, disp_width, disp_height;
    float aspect_ratio;
    int size, key;
    string extended_info;

    size = AVI_frame_size(avi, 0);
    if (size > 0) {
      buffer = (unsigned char *)safemalloc(size);
      AVI_read_frame(avi, (char *)buffer, &key);
      if (mpeg4_extract_par(buffer, size, par_num, par_den)) {
        width = AVI_video_width(avi);
        height = AVI_video_height(avi);
        aspect_ratio = (float)width / (float)height * (float)par_num /
          (float)par_den;
        if (aspect_ratio > ((float)width / (float)height)) {
          disp_width = irnd(height * aspect_ratio);
          disp_height = height;
        } else {
          disp_width = width;
          disp_height = irnd(width / aspect_ratio);
        }
        if (identify_verbose)
          extended_info = mxsprintf(" [display_dimensions:%ux%u ]",
                                    disp_width, disp_height);
        mxinfo("Track ID 0: video (%s)%s\n", type, extended_info.c_str());
        extended_info_shown = true;
      }
      safefree(buffer);
    }
  }
  if (!extended_info_shown)
    mxinfo("Track ID 0: video (%s)\n", AVI_video_compressor(avi));
  for (i = 0; i < AVI_audio_tracks(avi); i++) {
    AVI_set_audio_track(avi, i);
    switch (AVI_audio_format(avi)) {
      case 0x0001:
        type = "PCM";
        break;
      case 0x0055:
        type = "MP3";
        break;
      case 0x2000:
        type = "AC3";
        break;
      case 0x00ff:
        type = "AAC";
        break;
      default:
        type = "unknown";
    }
    mxinfo("Track ID %d: audio (%s)\n", i + 1, type);
  }
}

// }}}
