# buildman

Builds OpenBangla Keyboard packages for various distributions.

- A lightweight C/I workflow runs by default on each commit.
- if commit message has the string `pkg+` or `deploy+`, then a deploy workflow runs instead of the default one.
  - if build passes & `pkg+` is present, this workflow will upload the packages as Github Actions artifacts.
  - if build passes & `deploy+` is present, this workflow will deploy packages to repositories, and create a github release.
