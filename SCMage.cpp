#include <string>
#include "SCMage.hpp"
#include <pthread.h>
#include "alice.hpp"

static InterfaceTable *ft;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Plugin wrapper

static void SCMage_next(SCMage* unit, int inNumSamples);
static void SCMage_Ctor(SCMage* unit);
static void SCMage_addVoice(SCMage* unit, sc_msg_iter* msg);
void* SCMage_genThread(void * argv);

void SCMage_Ctor(SCMage* unit) {
    void* mem = RTAlloc(unit->mWorld, sizeof(MAGE::Mage));
    unit->mage = new(mem) MAGE::Mage();

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

void SCMage_Dtor(SCMage* unit) {
    pthread_cancel(unit->thread);
    pthread_join(unit->thread, NULL);
    unit->mage->~Mage();
    RTFree(unit->mWorld, unit->mage);
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

PluginLoad(SCMageUGens) {
    ft = inTable;
    DefineDtorUnit(SCMage);
}
