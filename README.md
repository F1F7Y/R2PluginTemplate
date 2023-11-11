# R2PluginTemplate
A template repository for R2Northstar v3 plugins written in C++.
Based on [Emmas DiscordRPC rewrite](https://github.com/R2Northstar/NorthstarDiscordRPC/tree/rewrite)

Check [plugin.cpp](./src/plugin.cpp) for examples.

## Windows
### Setting up the build system
- Install [msys2](https://www.msys2.org/) and follow installation steps on their website
- Open the MingW shell
- Run:
  ```sh
  pacman -S {make,gcc,cmake,base-devel}
  pacman -S mingw-w64-x86_64-{spdlog,fmt,cmake,make}
  ```

### Compiling
- Run:
  ```sh
  cmake . -G "MinGW Makefiles"
  cmake --build .
  ```
