/*
  mkvmerge -- utility for splicing together matroska files
      from component media subtypes

  common.cpp

  Written by Moritz Bunkus <moritz@bunkus.org>

  Distributed under the GPL
  see the file COPYING for details
  or visit http://www.gnu.org/copyleft/gpl.html
*/

/*!
    \file
    \version \$Id: common.cpp,v 1.18 2003/05/09 06:08:53 mosu Exp $
    \brief helper functions, common variables
    \author Moritz Bunkus         <moritz @ bunkus.org>
*/

#include <errno.h>
#include <iconv.h>
#include <langinfo.h>
#include <locale.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

int verbose = 1;

void _die(const char *s, const char *file, int line) {
  fprintf(stderr, "die @ %s/%d : %s\n", file, line, s);
  exit(1);
}

void _trace(const char *func, const char *file, int line) {
  fprintf(stdout, "trace: %s:%s (%d)\n", file, func, line);
}

/*
 * Endianess stuff
 */

uint16_t get_uint16(const void *buf) {
  uint16_t ret;
  unsigned char *tmp;

  tmp = (unsigned char *) buf;

  ret = tmp[1] & 0xff;
  ret = (ret << 8) + (tmp[0] & 0xff);

  return ret;
}

uint32_t get_uint32(const void *buf) {
  uint32_t ret;
  unsigned char *tmp;

  tmp = (unsigned char *) buf;

  ret = tmp[3] & 0xff;
  ret = (ret << 8) + (tmp[2] & 0xff);
  ret = (ret << 8) + (tmp[1] & 0xff);
  ret = (ret << 8) + (tmp[0] & 0xff);

  return ret;
}

/*
 * Character map conversion stuff
 */

typedef struct {
  iconv_t ict_from_utf8, ict_to_utf8;
  char *charset;
} mkv_conv_t;

static mkv_conv_t *mkv_convs = NULL;
static int num_mkv_convs = 0;
int cc_local_utf8 = -1;

int add_mkv_conv(const char *charset, iconv_t ict_from, iconv_t ict_to) {
  mkv_conv_t *c;
  int i;

  for (i = 0; i < num_mkv_convs; i++)
    if (!strcmp(mkv_convs[i].charset, charset))
      return i;

  mkv_convs = (mkv_conv_t *)saferealloc(mkv_convs, (num_mkv_convs + 1) *
                                        sizeof(mkv_conv_t));
  c = &mkv_convs[num_mkv_convs];
  c->charset = safestrdup(charset);
  c->ict_from_utf8 = ict_from;
  c->ict_to_utf8 = ict_to;
  num_mkv_convs++;

  return num_mkv_convs - 1;
}

int utf8_init(char *charset) {
  char *lc_charset;
  iconv_t ict_from_utf8, ict_to_utf8;
  int i;

  if ((charset == NULL) || (*charset == 0)) {
    setlocale(LC_CTYPE, "");
    lc_charset = nl_langinfo(CODESET);
    if (!strcmp(lc_charset, "UTF8") || !strcmp(lc_charset, "UTF-8"))
      return -1;
  } else
    lc_charset = charset;

  for (i = 0; i < num_mkv_convs; i++)
    if (!strcmp(mkv_convs[i].charset, lc_charset))
      return i;

  ict_to_utf8 = iconv_open("UTF-8", lc_charset);
  if (ict_to_utf8 == (iconv_t)(-1))
    fprintf(stdout, "Warning: Could not initialize the iconv library for "
            "the conversion from %s to UFT-8. "
            "Some strings will not be converted to UTF-8 and the resulting "
            "Matroska file might not comply with the Matroska specs ("
            "error: %d, %s).\n", lc_charset, errno, strerror(errno));

  ict_from_utf8 = iconv_open(lc_charset, "UTF-8");
  if (ict_from_utf8 == (iconv_t)(-1))
    fprintf(stdout, "Warning: Could not initialize the iconv library for "
            "the conversion from UFT-8 to %s. "
            "Some strings cannot be converted from UTF-8 and might be "
            "displayed incorrectly (error: %d, %s).\n", lc_charset, errno,
            strerror(errno));

  return add_mkv_conv(lc_charset, ict_from_utf8, ict_to_utf8);
}

void utf8_done() {
  int i;

  for (i = 0; i < num_mkv_convs; i++)
    safefree(mkv_convs[i].charset);
  if (mkv_convs != NULL)
    safefree(mkv_convs);
}

static char *convert_charset(iconv_t ict, char *src) {
  char *dst, *psrc, *pdst;
  size_t lsrc, ldst;
  int len;

  if (ict == (iconv_t)(-1))
    return safestrdup(src);

  len = strlen(src) * 4;
  dst = (char *)safemalloc(len + 1);
  memset(dst, 0, len + 1);

  iconv(ict, NULL, 0, NULL, 0);      // Reset the iconv state.
  lsrc = len / 4;
  ldst = len;
  psrc = src;
  pdst = dst;
#ifdef __CYGWIN__
  iconv(ict, (const char **)&psrc, &lsrc, &pdst, &ldst);
#else
  iconv(ict, &psrc, &lsrc, &pdst, &ldst);
#endif

  return dst;
}

char *to_utf8(int handle, char *local) {
  char *copy;

  if (handle == -1) {
    copy = safestrdup(local);
    return copy;
  }

  if (handle >= num_mkv_convs)
    die("Invalid conversion handle.");

  return convert_charset(mkv_convs[handle].ict_to_utf8, local);
}

char *from_utf8(int handle, char *utf8) {
  char *copy;

  if (handle == -1) {
    copy = safestrdup(utf8);
    return copy;
  }

  if (handle >= num_mkv_convs)
    die("Invalid conversion handle.");

  return convert_charset(mkv_convs[handle].ict_from_utf8, utf8);
}

/*
 * Random unique uint32_t numbers
 */

static uint32_t *ru_numbers = NULL;
static int num_ru_numbers = 0;

int is_unique_uint32(uint32_t number) {
  int i;

  for (i = 0; i < num_ru_numbers; i++)
    if (ru_numbers[i] == number)
      return 0;

  return 1;
}

void add_unique_uint32(uint32_t number) {
  ru_numbers = (uint32_t *)saferealloc(ru_numbers, (num_ru_numbers + 1) *
                                       sizeof(uint32_t));

  ru_numbers[num_ru_numbers] = number;
  num_ru_numbers++;
}

uint32_t create_unique_uint32() {
  uint32_t rnumber, half;

  do {
    half = (uint32_t)(65535.0 * rand() / RAND_MAX);
    rnumber = half;
    half = (uint32_t)(65535.0 * rand() / RAND_MAX);
    rnumber |= (half << 16);
  } while ((rnumber == 0) || !is_unique_uint32(rnumber));
  add_unique_uint32(rnumber);

  return rnumber;
}

/*
 * Miscellaneous stuff
 */

char *_safestrdup(const char *s, const char *file, int line) {
  char *copy;

  if (s == NULL)
    return NULL;

  copy = strdup(s);
  if (copy == NULL) {
    fprintf(stderr, "die @ %s/%d : in safestrdup: strdup == NULL\n", file,
            line);
    exit(1);
  }

  return copy;
}

unsigned char *_safestrdup(const unsigned char *s, const char *file,
                           int line) {
  char *copy;

  if (s == NULL)
    return NULL;

  copy = strdup((const char *)s);
  if (copy == NULL) {
    fprintf(stderr, "die @ %s/%d : in safestrdup: strdup == NULL\n", file,
            line);
    exit(1);
  }

  return (unsigned char *)copy;
}

void *_safememdup(const void *s, size_t size, const char *file, int line) {
  void *copy;

  if (s == NULL)
    return NULL;

  copy = malloc(size);
  if (copy == NULL) {
    fprintf(stderr, "die @ %s/%d : in safememdup: malloc == NULL\n", file,
            line);
    exit(1);
  }
  memcpy(copy, s, size);

  return copy;
}

void *_safemalloc(size_t size, const char *file, int line) {
  void *mem;

  mem = malloc(size);
  if (mem == NULL) {
    fprintf(stderr, "die @ %s/%d : in safemalloc: malloc == NULL\n", file,
            line);
    exit(1);
  }

  return mem;
}

void *_saferealloc(void *mem, size_t size, const char *file, int line) {
  mem = realloc(mem, size);
  if (mem == NULL) {
    fprintf(stderr, "die @ %s/%d : in safemalloc: realloc == NULL\n", file,
            line);
    exit(1);
  }

  return mem;
}
