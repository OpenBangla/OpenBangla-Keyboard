# Change Log
## 2.0.0
* Complete re-implementation of input method functionalities in Rust. ([#107][107]) It is [~1.2x faster in searching][3] dictionary suggestions.
* Dictionary suggestion is now available in Fixed Method Layouts(Provat, National etc.) ([#61][61], [#6][6])
* Phonetic suggestion now includes the typed English word as the last item in the suggestion list. ([#121][121], [#124][124])
* UI Refresh & Polish
    * Uses Material Design Icons. ([#165][165])
    * Merges the About button and it's menu into the Icon button which is also used to move the TopBar. This shrinks the TopBar a little. ([#165][165])
    * Redesign the Layout Viewer Dialog. ([#115][115], [#145][145])
    * Redesign the About Dialog. ([#116][116], [#155][155])
    * Redesign the Settings Dialog. This also merges all the available settings from the settings menu. ([#159][159])
    * Polish the Auto Correct entries Dialog. ([#164][164])
* Introduces a new Fixed Keyboard layout format. ([#143][143])
    * Now all of the keyboard layout we distribute are 58% smaller altogether.
    * Properly supports [other system keyboard layouts][36] instead of just the Qwerty layout.
    * Supports AltGr & ShiftAltGr layer of numerical keys.
    * Automatic conversion of old layout formats.
* Now AltGr layer can be typed with the Right Alt key in Fixed Keyboard methods. ([#152][152])
* **Bug Fix**: Handle the BackSlash key in Fixed Keyboard methods. ([#98][98])
* **Bug Fix**: Crash when using the Arrow keys in phonetic method. ([#123][123])
* **Bug Fix**: Fix writing of `Hasant` + `য` and য-ফলা(`্য`) in Fixed Keyboard methods. ([#117][117], [#163][163])
* Follows XDG Directory Specification for storing user specific data. ([#16][16]) Automatically [migrates user specific files][143] into the new place.
* Changes binary names to be more UNIX like. ([#103][103])

[107]: https://github.com/OpenBangla/OpenBangla-Keyboard/pull/107
[3]: https://github.com/OpenBangla/riti/pull/3
[61]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/61
[6]: https://github.com/OpenBangla/riti/pull/6
[121]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/121
[124]: https://github.com/OpenBangla/OpenBangla-Keyboard/pull/124
[16]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/16
[165]: https://github.com/OpenBangla/OpenBangla-Keyboard/pull/165
[116]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/116
[155]: https://github.com/OpenBangla/OpenBangla-Keyboard/pull/155
[159]: https://github.com/OpenBangla/OpenBangla-Keyboard/pull/159
[164]: https://github.com/OpenBangla/OpenBangla-Keyboard/pull/164
[115]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/115
[145]: https://github.com/OpenBangla/OpenBangla-Keyboard/pull/145
[143]: https://github.com/OpenBangla/OpenBangla-Keyboard/pull/143
[36]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/36
[98]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/98
[123]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/123
[152]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/152
[117]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/117
[163]: https://github.com/OpenBangla/OpenBangla-Keyboard/issues/163#issuecomment-689956532
[103]: https://github.com/OpenBangla/OpenBangla-Keyboard/pull/103
## 1.5.1
* Fixes a dictionary loading issue which was affecting in some scenarios. ([#73](https://github.com/OpenBangla/OpenBangla-Keyboard/issues/73))
* Polished some UI dialogs. ([#66](https://github.com/OpenBangla/OpenBangla-Keyboard/pull/66))
## 1.5.0
* AutoCorrect item is shown as the first suggestion item if available when writing using Avro Phonetic method. ([#42](https://github.com/OpenBangla/OpenBangla-Keyboard/pull/42))
* Performance update for Avro Phonetic suggestion making code. ([#42](https://github.com/OpenBangla/OpenBangla-Keyboard/pull/42))
* Be less restrictive when saving users selection. ([#41](https://github.com/OpenBangla/OpenBangla-Keyboard/pull/41)) 
* Now it is possible to write using AltGr and ShiftAltGr layer when using Fixed layout methods. Fixes bug [#40](https://github.com/OpenBangla/OpenBangla-Keyboard/issues/40).
* Auto-update checking is now configurable from settings dialog. There is now also a menu item in the TopBar for instant update checking. ([#32](https://github.com/OpenBangla/OpenBangla-Keyboard/issues/32))
* Fixed a crash. ([#52](https://github.com/OpenBangla/OpenBangla-Keyboard/issues/52))
## 1.4.0
* Fixes a serious bug [#26](https://github.com/OpenBangla/OpenBangla-Keyboard/issues/26).
* OpenBangla Keyboard will check for updates automatically and notify the user if an update is available.
## 1.3.0
* OpenBangla Keyboard now features a Phonetic AutoCorrect Preference Dialog [#20](https://github.com/OpenBangla/OpenBangla-Keyboard/pull/20). User now can view/edit/add AutoCorrect entries.
* Better Topbar integration with X11 windowing system and reduce the size of TopBar [#22](https://github.com/OpenBangla/OpenBangla-Keyboard/pull/22).
* Let users turn off candidate box [#11](https://github.com/OpenBangla/OpenBangla-Keyboard/issues/11)
* Build system improvement. There is no need to pass `CMAKE_INSTALL_PREFIX` when building.
* Now supports RPM packaging.
* Fixed issue [#17](https://github.com/OpenBangla/OpenBangla-Keyboard/issues/17). OpenBangla Keyboard was failing to store Layout setting when using Qt 5.10 version.
* Improved Settings dialog UI.
* Rewritten Avro Layout conversion code. OpenBangla Keyboard now handles errors more gracefully and reports them to the user.

## 1.2.0
* Fixed a memory leak.
* Fixed a long standing bug [#8](https://github.com/OpenBangla/OpenBangla-Keyboard/issues/8).
## 1.1.0
* Improve Phonetic support. Resolves issue [#2](https://github.com/OpenBangla/OpenBangla-Keyboard/issues/2). Now Phonetic support is mostly complete!
* A new button in TopBar for easily moving it.
* User can select candidate by clicking.
* These new layouts were added: **Munir Optima (uni)**, **Avro Easy**, **National (Jatiya)** and **Probhat**. Thanks to [OmicronLab](https://www.omicronlab.com/) because these layouts were also imported from [Avro Keyboard](https://www.omicronlab.com/avro-keyboard.html)!

## 1.0.0
First release! All things are new!

**Changes after 0.1.0 beta:**
* New feature: Tab to switch between the matches in candidate window
* While typing numbers in phonetic method, pressing : should make it : not ঃ
* Old Reph feature in fixed typing method
* User can select the orientation of candidate window in Settings
* Icons for OpenBangla Keyboard by [@saaiful](https://github.com/saaiful)

**Known issues:**
* [Implement suffix phonetic suggestion](https://github.com/OpenBangla/OpenBangla-Keyboard/issues/2)

## 0.1.0
Beta release
