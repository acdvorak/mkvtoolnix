/*
   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   class definition and output handling for progress information

   Written by Andrew C. Dvorak <andy@andydvorak.net>.
*/

#ifndef MTX_PROGRESS_H
#define MTX_PROGRESS_H

#include "common/common_pch.h"
#include "common/math.h"

class progress_c;

void display_progress(progress_c const &current_progress, bool is_100percent = false);
void display_progress_complete();

class progress_c {
private:
  int64_t m_done;
  int64_t m_total;
  int64_t m_remaining;
  double  m_pct;          // [ 0.0, 100.0 ]
  bool    m_is_complete;  // is_100percent
  bool    m_is_initialized;

public:
  enum format_e {
    simple = 0,
    precise,
  };

  static format_e ms_format;

  // Uninitialized
  progress_c()
    : m_done{}
    , m_total{}
    , m_remaining{}
    , m_pct{}
    , m_is_complete{}
    , m_is_initialized{false}
  {
  }

  // In progress
  progress_c(int64_t num_done, int64_t num_total, bool initialized = true)
    : m_done{num_done}
    , m_total{num_total}
    , m_remaining{num_total - num_done}
    , m_pct{num_total != 0 ? 100.0 * num_done / num_total : 0.0}
    , m_is_complete{num_total > 0 && num_done == num_total}
    , m_is_initialized{initialized}
  {
    if (m_done > m_total)
      mxerror(boost::format(Y("progress_c: Programming error: done > total (%1% > %2%). Please file a bug report.\n")) % m_done % m_total);
  }

  // Complete
  explicit progress_c(int64_t num_total)
    : m_done{num_total}
    , m_total{num_total}
    , m_remaining{0}
    , m_pct{num_total != 0 ? 100.0 : 0.0}
    , m_is_complete{num_total > 0}
    , m_is_initialized{true}
  {
    if (m_done > m_total)
      mxerror(boost::format(Y("progress_c: Programming error: done > total (%1% > %2%). Please file a bug report.\n")) % m_done % m_total);
  }

  int64_t done() const {
    return m_done;
  }

  int64_t total() const {
    return m_total;
  }

  int64_t remaining() const {
    return m_remaining;
  }

  double pct() const {
    return m_pct;
  }

  bool is_complete() const {
    return m_is_complete;
  }

  bool is_initialized() const {
    return m_is_initialized;
  }

  // arithmetic
  progress_c operator +(progress_c const &other) {
    return progress_c{m_done            + other.m_done,
                      m_total           + other.m_total,
                      m_is_initialized || other.m_is_initialized};
  }

  progress_c operator +=(progress_c const &other) {
    m_done           += other.m_done;
    m_total          += other.m_total;
    m_is_initialized |= other.m_is_initialized;
    return *this;
  }

  // comparison
  bool operator ==(progress_c const &other) const {
    return m_is_initialized == other.m_is_initialized
        && m_is_complete    == other.m_is_complete
        && m_done           == other.m_done
        && m_total          == other.m_total
        && m_done           == other.m_done;
  }

  bool operator !=(progress_c const &other) const {
    return !operator==(other);
  }

  // construction
  static progress_c uninitialized() {
    return progress_c{};
  }

  static progress_c zero() {
    return progress_c{0};
  }

  static progress_c zero(int64_t total) {
    return progress_c{0, total};
  }

  static progress_c i(int64_t done, int64_t total) {
    return progress_c{done, total};
  }

  static progress_c complete(int64_t total) {
    return progress_c{total};
  }

  static progress_c complete(progress_c const &other) {
    return progress_c{other.m_total, other.m_total, other.m_is_initialized};
  }

  static progress_c scale(int64_t file_size, int64_t units_read, int64_t units_per_file) {
    double  scale      = 1.0 * file_size / units_per_file;
    int64_t bytes_done = irnd(units_read * scale);
    return progress_c{bytes_done, file_size};
  }
};

#endif  // MTX_PROGRESS_H
