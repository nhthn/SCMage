#include <string>
#include "SCMage.hpp"
#include <pthread.h>
#include "alice.hpp"

static InterfaceTable *ft;

static void SCMage_next(SCMage* unit, int inNumSamples);
static void SCMage_Ctor(SCMage* unit);
static void* SCMage_genThread(void* argv);

void SCMage_Ctor(SCMage* unit) {
    void* mage_memory = RTAlloc(unit->mWorld, sizeof(MAGE::Mage));
    unit->mage = new(mage_memory) MAGE::Mage();

    SCMage_alice(unit);

    unit->freqValue = 0.0f;
    unit->freqAction = MAGE::noaction;

    pthread_create(&(unit->thread), NULL, SCMage_genThread, (void*)unit);

    SETCALC(SCMage_next);
    OUT0(0) = 0.0f;
}

void SCMage_next(SCMage* unit, int inNumSamples) {
    float freqValue = IN0(1);
    float freqAction = IN0(2);
    float timeScale = IN0(3);
    float *out = OUT(0);

    unit->freqValue = freqValue;
    unit->freqAction = freqAction;

    if (unit->mage->ready()) {
        unit->mage->setSpeed(timeScale * MAGE::defaultFrameRate, MAGE::overwrite);
    }
    for (int i = 0; i < inNumSamples; i++) {
        unit->mage->updateSamples();
        out[i] = unit->mage->popSamples();
    }
}


const std::string prefix = "/home/nathan/git/mage/data/voices/";

void SCMage_addVoice(SCMage* unit, std::string voiceName) {
    int mage_argc = 47;
    const std::string mage_argv_strings[mage_argc] = {
        "-s", "48000", // TODO: match server's sample rate
        "-p", "240",
        "-a", "0.55",
        "-td", prefix + voiceName + "/tree-dur.inf",
        "-tm", prefix + voiceName + "/tree-mgc.inf",
        "-tf", prefix + voiceName + "/tree-lf0.inf",
        "-tl", prefix + voiceName + "/tree-lpf.inf",
        "-md", prefix + voiceName + "/dur.pdf",
        "-mm", prefix + voiceName + "/mgc.pdf",
        "-mf", prefix + voiceName + "/lf0.pdf",
        "-ml", prefix + voiceName + "/lpf.pdf",
        "-dm", prefix + voiceName + "/mgc.win1",
        "-dm", prefix + voiceName + "/mgc.win2",
        "-dm", prefix + voiceName + "/mgc.win3",
        "-df", prefix + voiceName + "/lf0.win1",
        "-df", prefix + voiceName + "/lf0.win2",
        "-df", prefix + voiceName + "/lf0.win3",
        "-dl", prefix + voiceName + "/lpf.win1",
        "-em", prefix + voiceName + "/tree-gv-mgc.inf",
        "-ef", prefix + voiceName + "/tree-gv-lf0.inf",
        "-cm", prefix + voiceName + "/gv-mgc.pdf",
        "-cf", prefix + voiceName + "/gv-lf0.pdf",
        "-k", prefix + voiceName + "/gv-switch.inf",
        // This argument (referred to as labfn in the MAGE source) is not used.
        // But if I leave it out, I get a segfault. (?????)
        ""
    };

    // Convert from std::strings to C-style strings as required by addEngine
    char** mage_argv = new char*[MAGE::maxNumOfArguments];
    for (int i = 0; i < mage_argc; i++) {
        mage_argv[i] = new char[MAGE::maxStrLen];
        strcpy(mage_argv[i], mage_argv_strings[i].c_str());
    }

    unit->mage->addEngine(voiceName, mage_argc, mage_argv);
}

void* SCMage_genThread(void* argv) {
    SCMage* unit = (SCMage*)argv;

    SCMage_addVoice(unit, "slt");

    while (1) {
        pthread_testcancel();
        if (unit->mage->ready()) {
            unit->mage->run();
            unit->mage->setPitch(unit->freqValue, unit->freqAction);
            usleep(100);
        } else {
            usleep(100);
        }
    }
    return NULL;
}

void SCMage_Dtor(SCMage* unit) {
    pthread_cancel(unit->thread);
    pthread_join(unit->thread, NULL);
    unit->mage->~Mage();
    RTFree(unit->mWorld, unit->mage);
}



PluginLoad(SCMageUGens) {
    ft = inTable;
    DefineDtorUnit(SCMage);
}
