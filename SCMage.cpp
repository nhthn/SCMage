#include <string>
#include "SCMage.hpp"
#include <pthread.h>
#include "alice.hpp"

static InterfaceTable *ft;

static void SCMage_next(SCMage* unit, int inNumSamples);
static void SCMage_Ctor(SCMage* unit);
static void* SCMage_genThread(void* argv);

const std::string prefix = "/home/nathan/git/mage/data/voices/";

void SCMage_Ctor(SCMage* unit) {
    void* mage_memory = RTAlloc(unit->mWorld, sizeof(MAGE::Mage));
    unit->mage = new(mage_memory) MAGE::Mage();

    std::string voice = "slt";

    int argc = 47;
    const std::string argv_strings[argc] = {
        "-s", "48000", // TODO: match server's sample rate
        "-p", "240",
        "-a", "0.55",
        "-td", prefix + voice + "/tree-dur.inf",
        "-tm", prefix + voice + "/tree-mgc.inf",
        "-tf", prefix + voice + "/tree-lf0.inf",
        "-tl", prefix + voice + "/tree-lpf.inf",
        "-md", prefix + voice + "/dur.pdf",
        "-mm", prefix + voice + "/mgc.pdf",
        "-mf", prefix + voice + "/lf0.pdf",
        "-ml", prefix + voice + "/lpf.pdf",
        "-dm", prefix + voice + "/mgc.win1",
        "-dm", prefix + voice + "/mgc.win2",
        "-dm", prefix + voice + "/mgc.win3",
        "-df", prefix + voice + "/lf0.win1",
        "-df", prefix + voice + "/lf0.win2",
        "-df", prefix + voice + "/lf0.win3",
        "-dl", prefix + voice + "/lpf.win1",
        "-em", prefix + voice + "/tree-gv-mgc.inf",
        "-ef", prefix + voice + "/tree-gv-lf0.inf",
        "-cm", prefix + voice + "/gv-mgc.pdf",
        "-cf", prefix + voice + "/gv-lf0.pdf",
        "-k", prefix + voice + "/gv-switch.inf",
        // This argument (referred to as labfn in the MAGE source) is not used.
        // But if I leave it out, I get a segfault. (?????)
        ""
    };

    char** argv = new char*[MAGE::maxNumOfArguments];
    for (int i = 0; i < argc; i++) {
        argv[i] = new char[MAGE::maxStrLen];
        strcpy(argv[i], argv_strings[i].c_str());
    }

    unit->mage->addEngine(voice, argc, argv);

    SCMage_alice(unit);
    pthread_create(&(unit->thread), NULL, SCMage_genThread, (void*)unit);

    SETCALC(SCMage_next);
    OUT0(0) = 0.0f;
}

void SCMage_next(SCMage* unit, int inNumSamples) {
    float *out = OUT(0);
    for (int i = 0; i < inNumSamples; i++) {
        unit->mage->updateSamples();
        out[i] = unit->mage->popSamples();
    }
}

void* SCMage_genThread(void* argv) {
    SCMage* unit = (SCMage*)argv;
    while (1) {
        pthread_testcancel();
        if (unit->mage->ready()) {
            unit->mage->run();
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
