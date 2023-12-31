# GameCheats

- This repository contains runtime applicable (mostly) cheats for various video games
	- Some of them might be flaky and crash your game - use at your own risk!
- The goal of this repository is mostly to fool around and learn x86 and x64 assembly

<div align="center">
	<img src="Screenshot.jpg" alt="Hitman (2016) with some cheats applied">
	<b><i>Hitman&trade; (2016) with some cheats applied</i></b>
</div>

## Releases

- Check [releases](https://github.com/visuve/GameCheats/releasess) for automatic CI builds
	- The binaries are collected from [actions/artifacts](https://api.github.com/repos/visuve/GameCheats/actions/artifacts), i.e. not built on my machine

## Building

- Requirements
	- Visual Studio >= 2022 with C++ Desktop development workload
		- Remember to include CMake

- See steps [build x86](.github/workflows/msvc.yml#L12-L16=) and [build x64](.github/workflows/msvc.yml#L20-L22=) how to actually build this repo

## Principles

- Minimum 3rd party dependencies
- Minimal build environment setup

## Licensing

- See ![LICENSE.txt](/LICENSE.txt)
- Please contribute if you find this repository interesting
