## SCMage ##

A SuperCollider plugin wrapping the MAGE speech synthesizer. Currently in a very early stage of development, **NOT USABLE!**

### Building and installing ###

    $ mkdir build && cd build
    $ cmake -DSC_PATH=/path/to/supercollider/ -DMAGE_PATH=/path/to/mage/ ..
    $ make

Here, `/path/to/supercollider/` is the location of a copy of the SuperCollider *source code*. It must match the version of SuperCollider you have running. If set correctly, there should be a file in that directory at `include/plugin_interface/SC_PlugIn.h`.

`/path/to/mage/`, similarly, is a copy of the MAGE source. MAGE should be built first, and there should be a `libmage.so` in `builds/Release/`.

After building, install the SCMage directory as you would a quark. I prefer to add a symbolic link in my extensions directory, but you can also use `Quarks.gui` and click on "Install a folder."

### I'm sorry ###

You need to make some edits to the MAGE data files and the SCMage source to get this working.

Open up `/path/to/mage/data/configFiles/slt.conf`. Change all of those relative paths to absolute paths.

Back in SCMage, open `SCMageAsyncInit_run` in `SCMage.cpp`. Change `/home/nathan/git/mage/data/configFiles/cmu-artic/slt.conf` to the absolute location of the `slt.conf` file you just opened.

### Testing ###

Pull out your headphones and try `{ SCMage.ar!2 }.play`.