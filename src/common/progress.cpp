/** \brief class definition and output handling for progress information

   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   \file

   \author Written by Andrew C. Dvorak <acdvorak@gmail.com>.
*/

#include <boost/algorithm/string/join.hpp>

#include "common/common_pch.h"

#include "common/fs_sys_helpers.h"
#include "common/progress.h"

progress_c::format_e progress_c::ms_format = progress_c::simple;

static progress_detail_c s_previous_detail;
static int64_t           s_previous_detail_time = 0;

static inline bool
should_display_progress(progress_detail_c const &current_detail, int64_t current_time) {
  return (!s_previous_detail.job.is_initialized())
      || ( current_detail.job.is_complete()     &&  !s_previous_detail.job.is_complete())
      || ((current_detail != s_previous_detail) && ((current_time - s_previous_detail_time) >= 500));
}

static void
display_progress_output(progress_detail_c const &detail) {
  auto job    = detail.job;
  auto format = (progress_c::precise == progress_c::ms_format)
                  ? (boost::format("%1%/%2% %3$7.5f") % job.done() % job.total() % job.pct())
                  : (boost::format("%1$1.0f") % job.pct());
  mxinfo(boost::format(Y("Progress: %1%%%%2%")) % format.str() % "\r");
}

void
display_progress(progress_detail_c const &detail, bool is_100percent) {
  static auto s_no_progress = debugging_option_c{"no_progress"};

  if (s_no_progress)
    return;

  if (is_100percent) {
    auto detail_complete = progress_detail_c{detail.file_index, detail.num_files, progress_c::complete(detail.file), progress_c::complete(detail.job)};
    display_progress_output(detail_complete);
    return;
  }

  int64_t current_time = get_current_time_millis();

  if (!should_display_progress(detail, current_time))
    return;

  display_progress_output(detail);

  s_previous_detail      = detail;
  s_previous_detail_time = current_time;
}

void
display_progress_complete() {
  display_progress(s_previous_detail, true);
}
