#include <string>
#include "SC_PlugIn.h"
#include "mage.h"
#include "alice.h"

static InterfaceTable *ft;

/*
struct SCMage : public Unit {
    MAGE::Mage* mage;
    bool mageIsBusy;
};*/

// Data shared between RT and NRT threads.
struct CmdData {
    SCMage* unit;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAGE initialization NRT thread

bool SCMageAsyncInit_stage2NRT(World* world, CmdData* cmdData) {
    SCMage* unit = cmdData->unit;
    unit->mage->addEngine("slt", "/home/nathan/git/mage/data/configFiles/cmu-artic/slt.conf");
    return true;
}

bool SCMageAsyncInit_stage3RT(World* world, CmdData* cmdData) {
    return true;
}

bool SCMageAsyncInit_stage4NRT(World* world, CmdData* cmdData) {
    return true;
}

void SCMageAsyncInit_cleanup(World* world, void* cmdData) {
    RTFree(world, cmdData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main MAGE loop NRT thread

static int counter = 0; // so that I can distinguish the posts when the post window scrolls...

bool SCMageAsyncMain_stage2NRT(World* world, CmdData* cmdData) {
    SCMage* unit = cmdData->unit;
    counter++;
    printf("SCMageAsyncMain_run start %d\n", counter);
    unit->mageIsBusy = true;
    unit->mage->run();
    unit->mageIsBusy = false;
    printf("SCMageAsyncMain_run end %d\n", counter);
    return true;
}

bool SCMageAsyncMain_stage3RT(World* world, CmdData* cmdData) {
    return true;
}

bool SCMageAsyncMain_stage4NRT(World* world, CmdData* cmdData) {
    return true;
}

void SCMageAsyncMain_cleanup(World* world, void* cmdData) {
    RTFree(world, cmdData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Plugin wrapper

static void SCMage_next(SCMage* unit, int inNumSamples);
static void SCMage_Ctor(SCMage* unit);
static void SCMage_addVoice(SCMage* unit, sc_msg_iter* msg);

void SCMage_Ctor(SCMage* unit) {
    void* mem = RTAlloc(unit->mWorld, sizeof(MAGE::Mage));
    unit->mage = new(mem) MAGE::Mage();

    SCMage_alice(unit);

    CmdData* cmdData = (CmdData*)RTAlloc(unit->mWorld, sizeof(CmdData));
    cmdData->unit = unit;

    DoAsynchronousCommand(
        unit->mWorld, // world
        0, // reply address (not used)
        "", // command name (not used)
        (void*)cmdData, // command data, cast to void*
        (AsyncStageFn)SCMageAsyncInit_stage2NRT, // stage 2, NRT
        (AsyncStageFn)SCMageAsyncInit_stage3RT, // stage 3, RT
        (AsyncStageFn)SCMageAsyncInit_stage4NRT, // stage 4, NRT
        (AsyncFreeFn)SCMageAsyncInit_cleanup, // cleanup function
        0, // completion message size (not used)
        0 // completion message data (not used)
    );

    unit->mageIsBusy = false;

    SETCALC(SCMage_next);
    SCMage_next(unit, 1);
}

void SCMage_next(SCMage* unit, int inNumSamples) {
    float *out = OUT(0);

    // Pretty sure Mage::ready() is a reasonably real-time-safe function.
    if (!unit->mageIsBusy && unit->mage->ready()) {
        CmdData* cmdData = (CmdData*)RTAlloc(unit->mWorld, sizeof(CmdData));
        cmdData->unit = unit;
        DoAsynchronousCommand(
            unit->mWorld, // world
            0, // reply address (not used)
            "", // command name (not used)
            (void*)cmdData, // command data, cast to void*
            (AsyncStageFn)SCMageAsyncMain_stage2NRT, // stage 2, NRT
            (AsyncStageFn)SCMageAsyncMain_stage3RT, // stage 3, RT
            (AsyncStageFn)SCMageAsyncMain_stage4NRT, // stage 4, NRT
            (AsyncFreeFn)SCMageAsyncMain_cleanup, // cleanup function
            0, // completion message size (not used)
            0 // completion message data (not used)
        );
    }

    for (int i = 0; i < inNumSamples; i++) {
        unit->mage->updateSamples();
        out[i] = unit->mage->popSamples();
    }
}

void SCMage_Dtor(SCMage* unit) {
    printf("SCMage_Dtor\n");

    if (unit->mageIsBusy) {
        printf("MAGE is still busy!!! RIP SERVER\n");
    }

    // removing these lines doesn't help
    unit->mage->~Mage();
    RTFree(unit->mWorld, unit->mage);
}

void SCMage_addVoice(Unit* unit, sc_msg_iter* args) {
    printf("wahhh\n");
}

PluginLoad(SCMageUGens) {
    ft = inTable;
    DefineDtorUnit(SCMage);
    DefineUnitCmd("SCMage", "addVoice", SCMage_addVoice);
}
