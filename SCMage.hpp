#include "mage.h"
#include "SC_PlugIn.h"

struct SCMage : public Unit {
    MAGE::Mage* mage;
    bool mageIsBusy;
};