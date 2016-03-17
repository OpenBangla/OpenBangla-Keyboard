/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDir>
#include "im.h"
#include "log.h"
#include "Layout.h"
#include "Settings.h"

int main(int argc, char *argv []) {
  gLayout = new Layout();
  gSettings = new Settings();

  /* TODO: Move this else where           *
   * This creates some folders need by us */
  QDir dir;
  bool b = dir.mkpath(dir.homePath() + "/OpenBangla-Keyboard/");
  b = dir.mkpath(dir.homePath() + "/OpenBangla-Keyboard/Layouts/");
  if(!b) {
    LOG_ERROR("[Main]: Unable to create needed folders, exiting..."); exit(EXIT_FAILURE);
  }

  gLayout->loadLayout(gSettings->getLayoutPath());
  if (argv[1] = "--ibus") {
    im_start(true);
  } else {
    im_start(false);
  }
  return 0;
}
