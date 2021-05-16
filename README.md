# Tappy
A Terraria server written in C++.

[![Discord](https://img.shields.io/discord/435913149296803870?style=flat-square)](https://discord.gg/9Bwbm77)

## Binaries
No binaries exist yet. You must build it yourself.

## Building
Please excuse the horrid CMake. There is much work to do.

You need `cmake`, and `ninja`. This project also depends
on _"Lagom"_ from SerenityOS. Build it like this:

```bash
cd serenity
mkdir BuildLagom
cd BuildLagom
cmake -G Ninja ../Meta/Lagom
ninja
```

Then, you can build Tappy itself.

```bash
mkdir Build
cd Build
cmake -G Ninja ..
ninja
```
