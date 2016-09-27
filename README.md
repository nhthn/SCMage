## SCMage - EARLY STAGE OF DEVELOPMENT - DO NOT USE! ##

A SuperCollider plugin wrapping the MAGE/pHTS speech synthesizer.

### Building and installing ###

    $ mkdir build && cd build
    $ cmake -DSC_PATH=/path/to/supercollider/ -DMAGE_PATH=/path/to/mage/ ..
    $ make

Here, `/path/to/supercollider/` is the location of a copy of the SuperCollider *source code*. It must match the version of SuperCollider you have running. If set correctly, there should be a file in that directory at `include/plugin_interface/SC_PlugIn.h`.

`/path/to/mage/`, similarly, is a copy of the MAGE source. MAGE should be built first, and there should be a `libmage.so` in `builds/Release/`.

After building, install the SCMage directory as you would a quark. I prefer to add a symbolic link in my extensions directory, but you can also use `Quarks.gui` and click on "Install a folder."

### What frontend should I use? ###

MAGE is only a text-to-speech backend, converting a stream of labels (phonemes tagged with prosodic and linguistic contextual information) to audio. It's extremely tedious to write labels by hand, so some kind of frontend is needed to supply them. Most MAGE users want to convert text to labels, but if you know MAGE's label format, you can generate labels however you want.

SCMage has two frontends:

- `labelizer.py` is a crude Python script that converts text into labels. It can only labelize text with words found in the CMU dictionary, but it is very easy for you to modify for your own needs.
- Festival provides superior quality, but you have to build and download it. I have only tried it on Linux.

#### Festival ####

You can get Festival via Linux repositories, but I haven't figured out how to get the `text2utts` and `dumpfeats` scripts working correctly out of such installations. Unfortunately, you will have to download and build Festival yourself. If you figure out a way around this, I'm all ears.

I've provided a script called `download-and-build-festival.sh` that does this for you (forked from the script Festival uses for testing). Run it with the root of this repository as your working directory. The total download size is 15 MB, a lot of which is excess because Festival is not very good at modularity. I can assure you that it doesn't require root access and won't interfere with any installed versions of Festival.

The script is not very smart, so if it has problems like a failed download you may have to fix them manually. It's very simple though, so troubleshooting should be straightforward.

