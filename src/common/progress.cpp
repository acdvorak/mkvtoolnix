/** \brief class definition and output handling for progress information

   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   \file

   \author Written by Andrew C. Dvorak <andy@andydvorak.net>.
*/

#include "common/common_pch.h"

#include "common/fs_sys_helpers.h"
#include "common/progress.h"

progress_format_e g_progress_format = PF_SIMPLE;

static progress_c s_previous_progress;

static void
display_progress_output(progress_c const &current_progress) {
  auto progress = PF_PRECISE == g_progress_format
                    ? (boost::format("%1%/%2% %3$7.5f") % current_progress.done() % current_progress.total() % current_progress.pct())
                    : (boost::format("%1$1.0f") % current_progress.pct());
  mxinfo(boost::format(Y("Progress: %1%%%%2%")) % progress.str() % "\r");
}

void
display_progress(progress_c current_progress, bool is_100percent) {
  static auto s_no_progress               = debugging_option_c{"no_progress"};
  static int64_t s_previous_progress_time = 0;

  if (s_no_progress)
    return;

  if (is_100percent) {
    display_progress_output(progress_c::complete(current_progress));
    return;
  }

  bool is_displayable  = false;
  int64_t current_time = get_current_time_millis();

  if (   (!s_previous_progress.is_initialized())
      || (current_progress.is_complete() && !s_previous_progress.is_complete())
      || ((current_progress != s_previous_progress) && ((current_time - s_previous_progress_time) >= 500)))
    is_displayable = true;

  if (!is_displayable)
    return;

  display_progress_output(current_progress);

  s_previous_progress      = current_progress;
  s_previous_progress_time = current_time;
}

void
display_progress_complete() {
  display_progress(s_previous_progress, true);
}
