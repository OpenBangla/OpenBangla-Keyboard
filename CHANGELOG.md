# Change Log
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
