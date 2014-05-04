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

#include "common/progress.h"

bool g_precise_progress = false;

void
display_progress(progress_c total_progress) {
  auto progress = g_precise_progress ? (boost::format("%1%/%2% %3$7.5f") % total_progress.done() % total_progress.total() % total_progress.pct())
                                     : (boost::format("%1$1.0f") % total_progress.pct());
  mxinfo(boost::format(Y("Progress: %1%%%%2%")) % progress.str() % "\r");
}
