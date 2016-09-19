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

    unit->mage->addEngine("slt", "/home/nathan/git/mage/data/configFiles/cmu-artic/slt.conf");

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
