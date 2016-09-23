libtcod is a free, fast, portable and uncomplicated API for roguelike developers providing an advanced true color console, input, and lots of other utilities frequently used in roguelikes.

1.6: [![Build status](https://ci.appveyor.com/api/projects/status/6jh07hq205iy0mlh/branch/default?svg=true)](https://ci.appveyor.com/project/rmtew/libtcod/branch/default)

1.5: COMING.

## What can it do? ##

If you want to get a quick overview of the features that libtcod provides, check out the [Features](https://bitbucket.org/libtcod/libtcod/wiki/Features) page.

If you want to get a quick overview of games which have used libtcod, check out the [Projects](http://roguecentral.org/doryen/projects-2/) page.

libtcod is also well known for it's [easy to follow tutorial](http://www.roguebasin.com/index.php?title=Complete_Roguelike_Tutorial,_using_python%2Blibtcod), which many people have used to get started developing a roguelike.  Follow the relevant links the tutorial gives, and it will point you to the right locations.

### How do I get set up? ###

#### Using the provided binaries ####

Currently, only Windows binaries are available from the Bitbucket [download section](https://bitbucket.org/libtcod/libtcod/downloads) for this project.  If you are programming in C or C++, various sample projects are provided which can be used as examples.  For those who wish to program in Python which is the only scripting language support that has been contributed so far, the basics are described in [part 1](http://www.roguebasin.com/index.php?title=Complete_Roguelike_Tutorial,_using_python%2Blibtcod,_part_1#Setting_it_up) of the tutorial.

#### Compiling from source ####

You have two options, both of which are located under the `build/` subdirectory.

For Windows users, either running the top-level 'build.bat' script, or opening the solution directly within Visual Studio.

For all other platforms, you need to make use of the `autotools` support.  This is a standard approach used for decades, and if you are not familiar with it, you can Bing (or Google) the topic and learn more. For example, to build a production library :

    cd build/autotools
    autoreconf -i # if building from hg
    ./configure CFLAGS='-O2'
    make

### SDL vs SDL2 ###

**If you are using 1.5, then SDL should be your choice -- unless the tutorial you are doing refers to SDL2.**

[SDL2](http://hg.libsdl.org/SDL) is the latest version of SDL.  SDL itself, is no longer actively developed.

While libtcod still supports SDL, it is no longer actively developed.  The current development efforts of the SDL project are focused on SDL2.  And similarly, libtcod development will be focused on the SDL2 code path.

For now, SDL should be considered the most stable version to use, and SDL2 experimental.  At this time, SDL2 key handling is flawed and favours tablet and mobile devices with no touch screen or numeric keypad.

The SDL2 revision e12c38730512 is the only one that is supported by this project at this time.  If you are using any other revision, try the supported revision before reporting problems.  This is to ensure that we are all dealing with the same SDL2 bugs, otherwise your problems may not be reproducible when reported.