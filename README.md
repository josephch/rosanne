# Rosanne Card Game

28 is a strategic trick-taking card game played with a standard 52-card deck.  
Rosanne implements 28 card game as a desktop application using C++ and wxWidgets.
Original source code is available at https://sourceforge.net/projects/rosanne/

## Running appImage

Download appImage file from releases

[![Download for Linux](https://img.shields.io/badge/Download-Linux%20AppImage-blue.svg?style=for-the-badge&logo=linux)](https://github.com/josephch/rosanne/releases/download/v1.0b3-unofficial/Rosanne-x86_64.AppImage)

Add executable permission to the appImage file and double click to run.

## Build from source (Development)

### Requirements

- C++17 or later
- wxWidgets 3.2

### Getting the source
```
git clone https://github.com/josephch/rosanne.git
```
Rosanne depends on SFMT libraries. It is  present also as a submodule, can be fetched via
```
git submodule init
git submodule update
```

### Build using cmake
```
mkdir cmake-build
cd cmake-build
cmake ../
make
```

### Build using Code::Blocks

Build after opening project in Code::Blocks

### Running app

#### Steps before running the built binary

rosanne requires cards.xrs and gui.xrs in the working directory.
```
cd xrs/cards/
unzip cards.zip
wxrc cards.xrc -o cards.xrs
```

```
cd include/images
cp ../../xrs/gui/gui.xrc .
wxrc gui.xrc -o gui.xrs
```

Copy gui.xrs and cards.xrs to working directory once before running rosanne.

## License

This project is licensed under the GPL V2 License - see the LICENSE.md file for details
