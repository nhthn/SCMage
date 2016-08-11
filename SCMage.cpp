#include <string>
#include "SC_PlugIn.h"
#include "mage.h"

static InterfaceTable *ft;

struct SCMage : public Unit {
    MAGE::Mage *mage;
    bool mageIsBusy;
};

// Data shared between RT and NRT threads.
struct CmdData {
    SCMage* unit;
};

bool SCMageAsyncInit_run(World* world, CmdData* cmdData) {
    SCMage* unit = cmdData->unit;
    unit->mage->addEngine("slt", "/home/nathan/git/mage/data/configFiles/cmu-artic/slt.conf");
    return true;
}

void SCMageAsyncInit_cleanup(World* world, void* cmdData) {
    RTFree(world, cmdData);
}

static int counter = 0; // so that I can distinguish the posts when the post window scrolls...

bool SCMageAsyncMain_run(World* world, CmdData* cmdData) {
    SCMage* unit = cmdData->unit;
    counter++;
    printf("SCMageAsyncMain_run start %d\n", counter);
    unit->mageIsBusy = true;
    unit->mage->run();
    unit->mageIsBusy = false;
    printf("SCMageAsyncMain_run end %d\n", counter);
    return true;
}

void SCMageAsyncMain_cleanup(World* world, void* cmdData) {
    RTFree(world, cmdData);
}

static void SCMage_next(SCMage* unit, int inNumSamples);
static void SCMage_Ctor(SCMage* unit);
static void SCMage_addVoice(SCMage* unit, sc_msg_iter* msg);

void SCMage_Ctor(SCMage* unit) {
    unit->mage = new MAGE::Mage();

    // extremely silly
    // I don't have any facilities for loading label files at the moment, using this for testing
    unit->mage->pushLabel(MAGE::Label("x^x-pau+ae=l@x_x/A:0_0_0/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:1+1+2/D:0_0/E:x+x@x+x&x+x#x+x/F:content_2/G:0_0/H:x=x^1=10|0/I:19=12/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("x^pau-ae+l=ax@1_2/A:0_0_0/B:1-1-2@1-2&1-19#1-10$1-5!0-2;0-8|ae/C:0+0+2/D:0_0/E:content+2@1+12&1+6#0+2/F:aux_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("pau^ae-l+ax=s@2_1/A:0_0_0/B:1-1-2@1-2&1-19#1-10$1-5!0-2;0-8|ae/C:0+0+2/D:0_0/E:content+2@1+12&1+6#0+2/F:aux_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ae^l-ax+s=w@1_2/A:1_1_2/B:0-0-2@2-1&2-18#1-10$1-5!1-1;1-7|ax/C:1+0+3/D:0_0/E:content+2@1+12&1+6#0+2/F:aux_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("l^ax-s+w=aa@2_1/A:1_1_2/B:0-0-2@2-1&2-18#1-10$1-5!1-1;1-7|ax/C:1+0+3/D:0_0/E:content+2@1+12&1+6#0+2/F:aux_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ax^s-w+aa=z@1_3/A:0_0_2/B:1-0-3@1-1&3-17#1-9$1-5!2-2;2-6|aa/C:0+0+3/D:content_2/E:aux+1@2+11&2+6#1+1/F:content_3/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^w-aa+z=b@2_2/A:0_0_2/B:1-0-3@1-1&3-17#1-9$1-5!2-2;2-6|aa/C:0+0+3/D:content_2/E:aux+1@2+11&2+6#1+1/F:content_3/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("w^aa-z+b=ih@3_1/A:0_0_2/B:1-0-3@1-1&3-17#1-9$1-5!2-2;2-6|aa/C:0+0+3/D:content_2/E:aux+1@2+11&2+6#1+1/F:content_3/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("aa^z-b+ih=g@1_3/A:1_0_3/B:0-0-3@1-3&4-16#2-9$1-5!1-1;3-5|ih/C:1+0+2/D:aux_1/E:content+3@3+10&2+5#2+2/F:to_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("z^b-ih+g=ih@2_2/A:1_0_3/B:0-0-3@1-3&4-16#2-9$1-5!1-1;3-5|ih/C:1+0+2/D:aux_1/E:content+3@3+10&2+5#2+2/F:to_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("b^ih-g+ih=n@3_1/A:1_0_3/B:0-0-3@1-3&4-16#2-9$1-5!1-1;3-5|ih/C:1+0+2/D:aux_1/E:content+3@3+10&2+5#2+2/F:to_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^g-ih+n=ih@1_2/A:0_0_3/B:1-0-2@2-2&5-15#2-8$1-5!2-3;4-4|ih/C:0+0+2/D:aux_1/E:content+3@3+10&2+5#2+2/F:to_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("g^ih-n+ih=ng@2_1/A:0_0_3/B:1-0-2@2-2&5-15#2-8$1-5!2-3;4-4|ih/C:0+0+2/D:aux_1/E:content+3@3+10&2+5#2+2/F:to_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^n-ih+ng=t@1_2/A:1_0_2/B:0-0-2@3-1&6-14#3-8$1-5!1-2;5-3|ih/C:0+0+2/D:aux_1/E:content+3@3+10&2+5#2+2/F:to_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^ih-ng+t=ax@2_1/A:1_0_2/B:0-0-2@3-1&6-14#3-8$1-5!1-2;5-3|ih/C:0+0+2/D:aux_1/E:content+3@3+10&2+5#2+2/F:to_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^ng-t+ax=g@1_2/A:0_0_2/B:0-0-2@1-1&7-13#3-8$1-5!2-1;6-2|ax/C:1+0+3/D:content_3/E:to+1@4+9&3+5#1+1/F:content_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ng^t-ax+g=eh@2_1/A:0_0_2/B:0-0-2@1-1&7-13#3-8$1-5!2-1;6-2|ax/C:1+0+3/D:content_3/E:to+1@4+9&3+5#1+1/F:content_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^ax-g+eh=t@1_3/A:0_0_2/B:1-0-3@1-1&8-12#3-7$1-5!3-1;7-1|eh/C:1+1+3/D:to_1/E:content+1@5+8&3+4#2+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ax^g-eh+t=v@2_2/A:0_0_2/B:1-0-3@1-1&8-12#3-7$1-5!3-1;7-1|eh/C:1+1+3/D:to_1/E:content+1@5+8&3+4#2+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("g^eh-t+v=eh@3_1/A:0_0_2/B:1-0-3@1-1&8-12#3-7$1-5!3-1;7-1|eh/C:1+1+3/D:to_1/E:content+1@5+8&3+4#2+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("eh^t-v+eh=r@1_3/A:1_0_3/B:1-1-3@1-2&9-11#4-6$1-4!1-2;8-5|eh/C:0+0+1/D:content_1/E:content+2@6+7&4+3#1+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^v-eh+r=iy@2_2/A:1_0_3/B:1-1-3@1-2&9-11#4-6$1-4!1-2;8-5|eh/C:0+0+1/D:content_1/E:content+2@6+7&4+3#1+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("v^eh-r+iy=t@3_1/A:1_0_3/B:1-1-3@1-2&9-11#4-6$1-4!1-2;8-5|eh/C:0+0+1/D:content_1/E:content+2@6+7&4+3#1+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("eh^r-iy+t=ay@1_1/A:1_1_3/B:0-0-1@2-1&10-10#5-6$2-4!1-1;1-4|iy/C:1+0+2/D:content_1/E:content+2@6+7&4+3#1+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("r^iy-t+ay=er@1_2/A:0_0_1/B:1-0-2@1-2&11-9#5-5$2-4!2-2;2-3|ay/C:0+0+2/D:content_2/E:content+2@7+6&5+2#1+2/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("iy^t-ay+er=d@2_1/A:0_0_1/B:1-0-2@1-2&11-9#5-5$2-4!2-2;2-3|ay/C:0+0+2/D:content_2/E:content+2@7+6&5+2#1+2/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^ay-er+d=ah@1_2/A:1_0_2/B:0-0-2@2-1&12-8#6-5$2-4!1-1;3-2|er/C:1+0+2/D:content_2/E:content+2@7+6&5+2#1+2/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ay^er-d+ah=v@2_1/A:1_0_2/B:0-0-2@2-1&12-8#6-5$2-4!1-1;3-2|er/C:1+0+2/D:content_2/E:content+2@7+6&5+2#1+2/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("er^d-ah+v=s@1_2/A:0_0_2/B:1-0-2@1-1&13-7#6-4$2-4!2-1;4-1|ah/C:1+1+3/D:content_2/E:in+1@8+5&6+2#1+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("d^ah-v+s=ih@2_1/A:0_0_2/B:1-0-2@1-1&13-7#6-4$2-4!2-1;4-1|ah/C:1+1+3/D:content_2/E:in+1@8+5&6+2#1+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ah^v-s+ih=t@1_3/A:1_0_2/B:1-1-3@1-2&14-6#7-3$2-3!1-2;5-4|ih/C:0+0+2/D:in_1/E:content+2@9+4&6+1#2+3/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("v^s-ih+t=ih@2_2/A:1_0_2/B:1-1-3@1-2&14-6#7-3$2-3!1-2;5-4|ih/C:0+0+2/D:in_1/E:content+2@9+4&6+1#2+3/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^ih-t+ih=ng@3_1/A:1_0_2/B:1-1-3@1-2&14-6#7-3$2-3!1-2;5-4|ih/C:0+0+2/D:in_1/E:content+2@9+4&6+1#2+3/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^t-ih+ng=b@1_2/A:1_1_3/B:0-0-2@2-1&15-5#8-3$3-3!1-1;1-3|ih/C:1+0+2/D:in_1/E:content+2@9+4&6+1#2+3/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^ih-ng+b=ay@2_1/A:1_1_3/B:0-0-2@2-1&15-5#8-3$3-3!1-1;1-3|ih/C:1+0+2/D:in_1/E:content+2@9+4&6+1#2+3/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^ng-b+ay=hh@1_2/A:0_0_2/B:1-0-2@1-1&16-4#8-2$3-3!2-2;2-2|ay/C:0+0+2/D:content_2/E:in+1@10+3&7+1#1+2/F:pps_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ng^b-ay+hh=er@2_1/A:0_0_2/B:1-0-2@1-1&16-4#8-2$3-3!2-2;2-2|ay/C:0+0+2/D:content_2/E:in+1@10+3&7+1#1+2/F:pps_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("b^ay-hh+er=s@1_2/A:1_0_2/B:0-0-2@1-1&17-3#9-2$3-3!1-1;3-1|er/C:1+1+3/D:in_1/E:pps+1@11+2&7+1#2+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ay^hh-er+s=ih@2_1/A:1_0_2/B:0-0-2@1-1&17-3#9-2$3-3!1-1;3-1|er/C:1+1+3/D:in_1/E:pps+1@11+2&7+1#2+1/F:content_2/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("hh^er-s+ih=s@1_3/A:0_0_2/B:1-1-3@1-2&18-2#9-1$3-2!2-0;4-1|ih/C:0+1+2/D:pps_1/E:content+2@12+1&7+0#3+0/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("er^s-ih+s=t@2_2/A:0_0_2/B:1-1-3@1-2&18-2#9-1$3-2!2-0;4-1|ih/C:0+1+2/D:pps_1/E:content+2@12+1&7+0#3+0/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^ih-s+t=er@3_1/A:0_0_2/B:1-1-3@1-2&18-2#9-1$3-2!2-0;4-1|ih/C:0+1+2/D:pps_1/E:content+2@12+1&7+0#3+0/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^s-t+er=pau@1_2/A:1_1_3/B:0-1-2@2-1&19-1#10-1$4-1!1-0;1-0|er/C:1+0+2/D:pps_1/E:content+2@12+1&7+0#3+0/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^t-er+pau=aa@2_1/A:1_1_3/B:0-1-2@2-1&19-1#10-1$4-1!1-0;1-0|er/C:1+0+2/D:pps_1/E:content+2@12+1&7+0#3+0/F:in_1/G:0_0/H:19=12^1=10|L-H%/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^er-pau+aa=n@x_x/A:0_1_2/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:1+0+2/D:content_2/E:x+x@x+x&x+x#x+x/F:in_1/G:19_12/H:x=x^1=10|0/I:3=3/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("er^pau-aa+n=dh@1_2/A:0_1_2/B:1-0-2@1-1&1-3#1-2$1-2!0-2;0-2|aa/C:0+0+2/D:content_2/E:in+1@1+3&1+1#0+2/F:det_1/G:19_12/H:3=3^2=9|L-L%/I:8=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("pau^aa-n+dh=ax@2_1/A:0_1_2/B:1-0-2@1-1&1-3#1-2$1-2!0-2;0-2|aa/C:0+0+2/D:content_2/E:in+1@1+3&1+1#0+2/F:det_1/G:19_12/H:3=3^2=9|L-L%/I:8=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("aa^n-dh+ax=b@1_2/A:1_0_2/B:0-0-2@1-1&2-2#1-2$1-2!1-1;0-1|ax/C:1+1+4/D:in_1/E:det+1@2+2&1+1#0+1/F:content_1/G:19_12/H:3=3^2=9|L-L%/I:8=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^dh-ax+b=ae@2_1/A:1_0_2/B:0-0-2@1-1&2-2#1-2$1-2!1-1;0-1|ax/C:1+1+4/D:in_1/E:det+1@2+2&1+1#0+1/F:content_1/G:19_12/H:3=3^2=9|L-L%/I:8=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("dh^ax-b+ae=ng@1_4/A:0_0_2/B:1-1-4@1-1&3-1#1-1$1-1!2-0;0-0|ae/C:1+0+3/D:det_1/E:content+1@3+1&1+0#0+0/F:cc_1/G:19_12/H:3=3^2=9|L-L%/I:8=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ax^b-ae+ng=k@2_3/A:0_0_2/B:1-1-4@1-1&3-1#1-1$1-1!2-0;0-0|ae/C:1+0+3/D:det_1/E:content+1@3+1&1+0#0+0/F:cc_1/G:19_12/H:3=3^2=9|L-L%/I:8=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("b^ae-ng+k=pau@3_2/A:0_0_2/B:1-1-4@1-1&3-1#1-1$1-1!2-0;0-0|ae/C:1+0+3/D:det_1/E:content+1@3+1&1+0#0+0/F:cc_1/G:19_12/H:3=3^2=9|L-L%/I:8=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ae^ng-k+pau=ae@4_1/A:0_0_2/B:1-1-4@1-1&3-1#1-1$1-1!2-0;0-0|ae/C:1+0+3/D:det_1/E:content+1@3+1&1+0#0+0/F:cc_1/G:19_12/H:3=3^2=9|L-L%/I:8=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ng^k-pau+ae=n@x_x/A:1_1_4/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:1+0+3/D:content_1/E:x+x@x+x&x+x#x+x/F:cc_1/G:3_3/H:x=x^1=10|0/I:8=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("k^pau-ae+n=d@1_3/A:1_1_4/B:1-0-3@1-1&1-8#1-5$1-4!0-1;0-2|ae/C:1+0+2/D:content_1/E:cc+1@1+6&1+3#0+2/F:in_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("pau^ae-n+d=ah@2_2/A:1_1_4/B:1-0-3@1-1&1-8#1-5$1-4!0-1;0-2|ae/C:1+0+2/D:content_1/E:cc+1@1+6&1+3#0+2/F:in_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ae^n-d+ah=v@3_1/A:1_1_4/B:1-0-3@1-1&1-8#1-5$1-4!0-1;0-2|ae/C:1+0+2/D:content_1/E:cc+1@1+6&1+3#0+2/F:in_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^d-ah+v=hh@1_2/A:1_0_3/B:1-0-2@1-1&2-7#1-4$1-4!1-1;0-1|ah/C:1+1+3/D:cc_1/E:in+1@2+5&1+3#0+1/F:content_2/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("d^ah-v+hh=ae@2_1/A:1_0_3/B:1-0-2@1-1&2-7#1-4$1-4!1-1;0-1|ah/C:1+1+3/D:cc_1/E:in+1@2+5&1+3#0+1/F:content_2/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ah^v-hh+ae=v@1_3/A:1_0_2/B:1-1-3@1-2&3-6#2-3$1-3!1-2;0-2|ae/C:0+0+2/D:in_1/E:content+2@3+4&1+2#0+1/F:content_2/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("v^hh-ae+v=ih@2_2/A:1_0_2/B:1-1-3@1-2&3-6#2-3$1-3!1-2;0-2|ae/C:0+0+2/D:in_1/E:content+2@3+4&1+2#0+1/F:content_2/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("hh^ae-v+ih=ng@3_1/A:1_0_2/B:1-1-3@1-2&3-6#2-3$1-3!1-2;0-2|ae/C:0+0+2/D:in_1/E:content+2@3+4&1+2#0+1/F:content_2/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ae^v-ih+ng=n@1_2/A:1_1_3/B:0-0-2@2-1&4-5#3-3$2-3!1-1;1-1|ih/C:1+1+3/D:in_1/E:content+2@3+4&1+2#0+1/F:content_2/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("v^ih-ng+n=ah@2_1/A:1_1_3/B:0-0-2@2-1&4-5#3-3$2-3!1-1;1-1|ih/C:1+1+3/D:in_1/E:content+2@3+4&1+2#0+1/F:content_2/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^ng-n+ah=th@1_3/A:0_0_2/B:1-1-3@1-2&5-4#3-2$2-2!2-3;2-3|ah/C:0+0+2/D:content_2/E:content+2@4+3&2+1#1+2/F:to_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ng^n-ah+th=ih@2_2/A:0_0_2/B:1-1-3@1-2&5-4#3-2$2-2!2-3;2-3|ah/C:0+0+2/D:content_2/E:content+2@4+3&2+1#1+2/F:to_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^ah-th+ih=ng@3_1/A:0_0_2/B:1-1-3@1-2&5-4#3-2$2-2!2-3;2-3|ah/C:0+0+2/D:content_2/E:content+2@4+3&2+1#1+2/F:to_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ah^th-ih+ng=t@1_2/A:1_1_3/B:0-0-2@2-1&6-3#4-2$3-2!1-2;1-2|ih/C:0+0+2/D:content_2/E:content+2@4+3&2+1#1+2/F:to_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("th^ih-ng+t=ax@2_1/A:1_1_3/B:0-0-2@2-1&6-3#4-2$3-2!1-2;1-2|ih/C:0+0+2/D:content_2/E:content+2@4+3&2+1#1+2/F:to_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^ng-t+ax=d@1_2/A:0_0_2/B:0-0-2@1-1&7-2#4-2$3-2!2-1;2-1|ax/C:1+1+2/D:content_2/E:to+1@5+2&3+1#1+1/F:content_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ng^t-ax+d=uw@2_1/A:0_0_2/B:0-0-2@1-1&7-2#4-2$3-2!2-1;2-1|ax/C:1+1+2/D:content_2/E:to+1@5+2&3+1#1+1/F:content_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^ax-d+uw=pau@1_2/A:0_0_2/B:1-1-2@1-1&8-1#4-1$3-1!3-0;3-0|uw/C:1+0+4/D:to_1/E:content+1@6+1&3+0#2+0/F:content_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ax^d-uw+pau=w@2_1/A:0_0_2/B:1-1-2@1-1&8-1#4-1$3-1!3-0;3-0|uw/C:1+0+4/D:to_1/E:content+1@6+1&3+0#2+0/F:content_1/G:3_3/H:8=6^3=8|L-L%/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("d^uw-pau+w=ah@x_x/A:1_1_2/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:1+0+4/D:content_1/E:x+x@x+x&x+x#x+x/F:content_1/G:8_6/H:x=x^1=10|0/I:6=6/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("uw^pau-w+ah=n@1_4/A:1_1_2/B:1-0-4@1-1&1-6#1-6$1-3!0-1;0-2|ah/C:1+0+2/D:content_1/E:content+1@1+6&1+3#0+2/F:cc_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("pau^w-ah+n=s@2_3/A:1_1_2/B:1-0-4@1-1&1-6#1-6$1-3!0-1;0-2|ah/C:1+0+2/D:content_1/E:content+1@1+6&1+3#0+2/F:cc_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("w^ah-n+s=ao@3_2/A:1_1_2/B:1-0-4@1-1&1-6#1-6$1-3!0-1;0-2|ah/C:1+0+2/D:content_1/E:content+1@1+6&1+3#0+2/F:cc_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ah^n-s+ao=r@4_1/A:1_1_2/B:1-0-4@1-1&1-6#1-6$1-3!0-1;0-2|ah/C:1+0+2/D:content_1/E:content+1@1+6&1+3#0+2/F:cc_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^s-ao+r=t@1_2/A:1_0_4/B:1-0-2@1-1&2-5#1-5$1-3!1-1;0-1|ao/C:1+1+4/D:content_1/E:cc+1@2+5&2+3#1+1/F:content_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^ao-r+t=w@2_1/A:1_0_4/B:1-0-2@1-1&2-5#1-5$1-3!1-1;0-1|ao/C:1+1+4/D:content_1/E:cc+1@2+5&2+3#1+1/F:content_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ao^r-t+w=ay@1_4/A:1_0_2/B:1-1-4@1-1&3-4#2-4$1-2!1-1;0-3|ay/C:1+0+2/D:cc_1/E:content+1@3+4&2+2#2+1/F:content_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("r^t-w+ay=s@2_3/A:1_0_2/B:1-1-4@1-1&3-4#2-4$1-2!1-1;0-3|ay/C:1+0+2/D:cc_1/E:content+1@3+4&2+2#2+1/F:content_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^w-ay+s=sh@3_2/A:1_0_2/B:1-1-4@1-1&3-4#2-4$1-2!1-1;0-3|ay/C:1+0+2/D:cc_1/E:content+1@3+4&2+2#2+1/F:content_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("w^ay-s+sh=iy@4_1/A:1_0_2/B:1-1-4@1-1&3-4#2-4$1-2!1-1;0-3|ay/C:1+0+2/D:cc_1/E:content+1@3+4&2+2#2+1/F:content_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ay^s-sh+iy=hh@1_2/A:1_1_4/B:1-0-2@1-1&4-3#3-3$2-2!1-1;1-2|iy/C:1+0+3/D:content_1/E:content+1@4+3&3+1#1+2/F:aux_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^sh-iy+hh=ae@2_1/A:1_1_4/B:1-0-2@1-1&4-3#3-3$2-2!1-1;1-2|iy/C:1+0+3/D:content_1/E:content+1@4+3&3+1#1+2/F:aux_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("sh^iy-hh+ae=d@1_3/A:1_0_2/B:1-0-3@1-1&5-2#4-2$2-2!1-1;2-1|ae/C:1+1+4/D:content_1/E:aux+1@5+2&4+1#1+1/F:content_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("iy^hh-ae+d=p@2_2/A:1_0_2/B:1-0-3@1-1&5-2#4-2$2-2!1-1;2-1|ae/C:1+1+4/D:content_1/E:aux+1@5+2&4+1#1+1/F:content_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("hh^ae-d+p=iy@3_1/A:1_0_2/B:1-0-3@1-1&5-2#4-2$2-2!1-1;2-1|ae/C:1+1+4/D:content_1/E:aux+1@5+2&4+1#1+1/F:content_1/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ae^d-p+iy=p@1_4/A:1_0_3/B:1-1-4@1-1&6-1#5-1$2-1!1-0;3-0|iy/C:0+0+3/D:aux_1/E:content+1@6+1&4+0#2+0/F:in_2/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("d^p-iy+p=t@2_3/A:1_0_3/B:1-1-4@1-1&6-1#5-1$2-1!1-0;3-0|iy/C:0+0+3/D:aux_1/E:content+1@6+1&4+0#2+0/F:in_2/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("p^iy-p+t=pau@3_2/A:1_0_3/B:1-1-4@1-1&6-1#5-1$2-1!1-0;3-0|iy/C:0+0+3/D:aux_1/E:content+1@6+1&4+0#2+0/F:in_2/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("iy^p-t+pau=ih@4_1/A:1_0_3/B:1-1-4@1-1&6-1#5-1$2-1!1-0;3-0|iy/C:0+0+3/D:aux_1/E:content+1@6+1&4+0#2+0/F:in_2/G:8_6/H:6=6^4=7|L-L%/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("p^t-pau+ih=n@x_x/A:1_1_4/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:0+0+3/D:content_1/E:x+x@x+x&x+x#x+x/F:in_2/G:6_6/H:x=x^1=10|0/I:10=7/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^pau-ih+n=t@1_3/A:1_1_4/B:0-0-3@1-2&1-10#1-6$1-4!0-1;0-3|ih/C:1+0+1/D:content_1/E:in+2@1+7&1+3#0+2/F:det_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("pau^ih-n+t=uw@2_2/A:1_1_4/B:0-0-3@1-2&1-10#1-6$1-4!0-1;0-3|ih/C:1+0+1/D:content_1/E:in+2@1+7&1+3#0+2/F:det_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^n-t+uw=dh@3_1/A:1_1_4/B:0-0-3@1-2&1-10#1-6$1-4!0-1;0-3|ih/C:1+0+1/D:content_1/E:in+2@1+7&1+3#0+2/F:det_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^t-uw+dh=ax@1_1/A:0_0_3/B:1-0-1@2-1&2-9#1-5$1-4!0-2;0-2|uw/C:0+0+2/D:content_1/E:in+2@1+7&1+3#0+2/F:det_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^uw-dh+ax=b@1_2/A:1_0_1/B:0-0-2@1-1&3-8#2-5$1-4!1-1;0-1|ax/C:1+1+3/D:in_2/E:det+1@2+6&1+3#0+1/F:content_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("uw^dh-ax+b=uh@2_1/A:1_0_1/B:0-0-2@1-1&3-8#2-5$1-4!1-1;0-1|ax/C:1+1+3/D:in_2/E:det+1@2+6&1+3#0+1/F:content_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("dh^ax-b+uh=k@1_3/A:0_0_2/B:1-1-3@1-1&4-7#2-4$1-3!2-2;0-2|uh/C:0+0+2/D:det_1/E:content+1@3+5&1+2#0+2/F:pps_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ax^b-uh+k=hh@2_2/A:0_0_2/B:1-1-3@1-1&4-7#2-4$1-3!2-2;0-2|uh/C:0+0+2/D:det_1/E:content+1@3+5&1+2#0+2/F:pps_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("b^uh-k+hh=er@3_1/A:0_0_2/B:1-1-3@1-1&4-7#2-4$1-3!2-2;0-2|uh/C:0+0+2/D:det_1/E:content+1@3+5&1+2#0+2/F:pps_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("uh^k-hh+er=s@1_2/A:1_1_3/B:0-0-2@1-1&5-6#3-4$2-3!1-1;1-1|er/C:1+1+3/D:content_1/E:pps+1@4+4&2+2#1+1/F:content_2/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("k^hh-er+s=ih@2_1/A:1_1_3/B:0-0-2@1-1&5-6#3-4$2-3!1-1;1-1|er/C:1+1+3/D:content_1/E:pps+1@4+4&2+2#1+1/F:content_2/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("hh^er-s+ih=s@1_3/A:0_0_2/B:1-1-3@1-2&6-5#3-3$2-2!2-2;2-4|ih/C:0+0+2/D:pps_1/E:content+2@5+3&2+1#2+2/F:aux_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("er^s-ih+s=t@2_2/A:0_0_2/B:1-1-3@1-2&6-5#3-3$2-2!2-2;2-4|ih/C:0+0+2/D:pps_1/E:content+2@5+3&2+1#2+2/F:aux_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^ih-s+t=er@3_1/A:0_0_2/B:1-1-3@1-2&6-5#3-3$2-2!2-2;2-4|ih/C:0+0+2/D:pps_1/E:content+2@5+3&2+1#2+2/F:aux_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^s-t+er=w@1_2/A:1_1_3/B:0-0-2@2-1&7-4#4-3$3-2!1-1;1-3|er/C:1+0+3/D:pps_1/E:content+2@5+3&2+1#2+2/F:aux_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^t-er+w=aa@2_1/A:1_1_3/B:0-0-2@2-1&7-4#4-3$3-2!1-1;1-3|er/C:1+0+3/D:pps_1/E:content+2@5+3&2+1#2+2/F:aux_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^er-w+aa=z@1_3/A:0_0_2/B:1-0-3@1-1&8-3#4-2$3-2!2-1;2-2|aa/C:1+0+3/D:content_2/E:aux+1@6+2&3+1#1+1/F:content_2/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("er^w-aa+z=r@2_2/A:0_0_2/B:1-0-3@1-1&8-3#4-2$3-2!2-1;2-2|aa/C:1+0+3/D:content_2/E:aux+1@6+2&3+1#1+1/F:content_2/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("w^aa-z+r=iy@3_1/A:0_0_2/B:1-0-3@1-1&8-3#4-2$3-2!2-1;2-2|aa/C:1+0+3/D:content_2/E:aux+1@6+2&3+1#1+1/F:content_2/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("aa^z-r+iy=d@1_3/A:1_0_3/B:1-0-3@1-2&9-2#5-1$3-2!1-0;3-1|iy/C:0+1+2/D:aux_1/E:content+2@7+1&3+0#2+0/F:cc_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("z^r-iy+d=ih@2_2/A:1_0_3/B:1-0-3@1-2&9-2#5-1$3-2!1-0;3-1|iy/C:0+1+2/D:aux_1/E:content+2@7+1&3+0#2+0/F:cc_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("r^iy-d+ih=ng@3_1/A:1_0_3/B:1-0-3@1-2&9-2#5-1$3-2!1-0;3-1|iy/C:0+1+2/D:aux_1/E:content+2@7+1&3+0#2+0/F:cc_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("iy^d-ih+ng=pau@1_2/A:1_0_3/B:0-1-2@2-1&10-1#6-1$3-1!1-0;4-0|ih/C:1+0+3/D:aux_1/E:content+2@7+1&3+0#2+0/F:cc_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("d^ih-ng+pau=b@2_1/A:1_0_3/B:0-1-2@2-1&10-1#6-1$3-1!1-0;4-0|ih/C:1+0+3/D:aux_1/E:content+2@7+1&3+0#2+0/F:cc_1/G:6_6/H:10=7^5=6|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^ng-pau+b=ah@x_x/A:0_1_2/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:1+0+3/D:content_2/E:x+x@x+x&x+x#x+x/F:cc_1/G:10_7/H:x=x^1=10|0/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ng^pau-b+ah=t@1_3/A:0_1_2/B:1-0-3@1-1&1-6#1-5$1-3!0-1;0-1|ah/C:1+1+2/D:content_2/E:cc+1@1+5&1+2#0+1/F:content_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("pau^b-ah+t=ih@2_2/A:0_1_2/B:1-0-3@1-1&1-6#1-5$1-3!0-1;0-1|ah/C:1+1+2/D:content_2/E:cc+1@1+5&1+2#0+1/F:content_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("b^ah-t+ih=t@3_1/A:0_1_2/B:1-0-3@1-1&1-6#1-5$1-3!0-1;0-1|ah/C:1+1+2/D:content_2/E:cc+1@1+5&1+2#0+1/F:content_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ah^t-ih+t=hh@1_2/A:1_0_3/B:1-1-2@1-1&2-5#1-4$1-2!1-1;0-4|ih/C:1+0+3/D:cc_1/E:content+1@2+4&1+1#0+3/F:aux_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^ih-t+hh=ae@2_1/A:1_0_3/B:1-1-2@1-1&2-5#1-4$1-2!1-1;0-4|ih/C:1+0+3/D:cc_1/E:content+1@2+4&1+1#0+3/F:aux_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^t-hh+ae=d@1_3/A:1_1_2/B:1-0-3@1-1&3-4#2-3$2-2!1-1;1-3|ae/C:1+0+2/D:content_1/E:aux+1@3+3&2+1#1+2/F:det_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^hh-ae+d=n@2_2/A:1_1_2/B:1-0-3@1-1&3-4#2-3$2-2!1-1;1-3|ae/C:1+0+2/D:content_1/E:aux+1@3+3&2+1#1+2/F:det_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("hh^ae-d+n=ow@3_1/A:1_1_2/B:1-0-3@1-1&3-4#2-3$2-2!1-1;1-3|ae/C:1+0+2/D:content_1/E:aux+1@3+3&2+1#1+2/F:det_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ae^d-n+ow=p@1_2/A:1_0_3/B:1-0-2@1-1&4-3#3-2$2-2!1-1;2-2|ow/C:1+0+3/D:aux_1/E:det+1@4+2&2+1#2+1/F:content_2/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("d^n-ow+p=ih@2_1/A:1_0_3/B:1-0-2@1-1&4-3#3-2$2-2!1-1;2-2|ow/C:1+0+3/D:aux_1/E:det+1@4+2&2+1#2+1/F:content_2/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^ow-p+ih=k@1_3/A:1_0_2/B:1-0-3@1-2&5-2#4-1$2-2!1-0;3-1|ih/C:0+1+3/D:det_1/E:content+2@5+1&2+0#3+0/F:cc_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ow^p-ih+k=ch@2_2/A:1_0_2/B:1-0-3@1-2&5-2#4-1$2-2!1-0;3-1|ih/C:0+1+3/D:det_1/E:content+2@5+1&2+0#3+0/F:cc_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("p^ih-k+ch=er@3_1/A:1_0_2/B:1-0-3@1-2&5-2#4-1$2-2!1-0;3-1|ih/C:0+1+3/D:det_1/E:content+2@5+1&2+0#3+0/F:cc_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^k-ch+er=z@1_3/A:1_0_3/B:0-1-3@2-1&6-1#5-1$2-1!1-0;4-0|er/C:1+0+2/D:det_1/E:content+2@5+1&2+0#3+0/F:cc_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("k^ch-er+z=pau@2_2/A:1_0_3/B:0-1-3@2-1&6-1#5-1$2-1!1-0;4-0|er/C:1+0+2/D:det_1/E:content+2@5+1&2+0#3+0/F:cc_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ch^er-z+pau=ao@3_1/A:1_0_3/B:0-1-3@2-1&6-1#5-1$2-1!1-0;4-0|er/C:1+0+2/D:det_1/E:content+2@5+1&2+0#3+0/F:cc_1/G:10_7/H:6=5^6=5|L-L%/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("er^z-pau+ao=r@x_x/A:0_1_3/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:1+0+2/D:content_2/E:x+x@x+x&x+x#x+x/F:cc_1/G:6_5/H:x=x^1=10|0/I:7=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("z^pau-ao+r=k@1_2/A:0_1_3/B:1-0-2@1-1&1-7#1-4$1-3!0-1;0-1|ao/C:1+1+4/D:content_2/E:cc+1@1+4&1+2#0+1/F:content_4/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("pau^ao-r+k=aa@2_1/A:0_1_3/B:1-0-2@1-1&1-7#1-4$1-3!0-1;0-1|ao/C:1+1+4/D:content_2/E:cc+1@1+4&1+2#0+1/F:content_4/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ao^r-k+aa=n@1_4/A:1_0_2/B:1-1-4@1-4&2-6#1-3$1-2!1-2;0-5|aa/C:0+0+2/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("r^k-aa+n=v@2_3/A:1_0_2/B:1-1-4@1-4&2-6#1-3$1-2!1-2;0-5|aa/C:0+0+2/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("k^aa-n+v=er@3_2/A:1_0_2/B:1-1-4@1-4&2-6#1-3$1-2!1-2;0-5|aa/C:0+0+2/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("aa^n-v+er=s@4_1/A:1_0_2/B:1-1-4@1-4&2-6#1-3$1-2!1-2;0-5|aa/C:0+0+2/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^v-er+s=ey@1_2/A:1_1_4/B:0-0-2@2-3&3-5#2-3$2-2!1-1;1-4|er/C:1+0+2/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("v^er-s+ey=sh@2_1/A:1_1_4/B:0-0-2@2-3&3-5#2-3$2-2!1-1;1-4|er/C:1+0+2/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("er^s-ey+sh=ax@1_2/A:0_0_2/B:1-0-2@3-2&4-4#2-2$2-2!2-3;2-3|ey/C:0+0+3/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^ey-sh+ax=n@2_1/A:0_0_2/B:1-0-2@3-2&4-4#2-2$2-2!2-3;2-3|ey/C:0+0+3/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ey^sh-ax+n=z@1_3/A:1_0_2/B:0-0-3@4-1&5-3#3-2$2-2!1-2;3-2|ax/C:0+0+2/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("sh^ax-n+z=ih@2_2/A:1_0_2/B:0-0-3@4-1&5-3#3-2$2-2!1-2;3-2|ax/C:0+0+2/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ax^n-z+ih=n@3_1/A:1_0_2/B:0-0-3@4-1&5-3#3-2$2-2!1-2;3-2|ax/C:0+0+2/D:cc_1/E:content+4@2+3&1+1#0+2/F:in_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^z-ih+n=ih@1_2/A:0_0_3/B:0-0-2@1-1&6-2#3-2$2-2!2-1;4-1|ih/C:1+1+2/D:content_4/E:in+1@3+2&2+1#1+1/F:content_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("z^ih-n+ih=t@2_1/A:0_0_3/B:0-0-2@1-1&6-2#3-2$2-2!2-1;4-1|ih/C:1+1+2/D:content_4/E:in+1@3+2&2+1#1+1/F:content_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^n-ih+t=pau@1_2/A:0_0_2/B:1-1-2@1-1&7-1#3-1$2-1!3-0;5-0|ih/C:1+0+3/D:in_1/E:content+1@4+1&2+0#2+0/F:cc_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^ih-t+pau=ae@2_1/A:0_0_2/B:1-1-2@1-1&7-1#3-1$2-1!3-0;5-0|ih/C:1+0+3/D:in_1/E:content+1@4+1&2+0#2+0/F:cc_1/G:6_5/H:7=4^7=4|L-L%/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^t-pau+ae=n@x_x/A:1_1_2/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:1+0+3/D:content_1/E:x+x@x+x&x+x#x+x/F:cc_1/G:7_4/H:x=x^1=10|0/I:5=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^pau-ae+n=d@1_3/A:1_1_2/B:1-0-3@1-1&1-5#1-3$1-2!0-1;0-4|ae/C:1+0+3/D:content_1/E:cc+1@1+5&1+1#0+4/F:wp_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("pau^ae-n+d=w@2_2/A:1_1_2/B:1-0-3@1-1&1-5#1-3$1-2!0-1;0-4|ae/C:1+0+3/D:content_1/E:cc+1@1+5&1+1#0+4/F:wp_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ae^n-d+w=ah@3_1/A:1_1_2/B:1-0-3@1-1&1-5#1-3$1-2!0-1;0-4|ae/C:1+0+3/D:content_1/E:cc+1@1+5&1+1#0+4/F:wp_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^d-w+ah=t@1_3/A:1_0_3/B:1-0-3@1-1&2-4#1-2$1-2!1-1;0-3|ah/C:1+0+2/D:cc_1/E:wp+1@2+4&1+1#0+3/F:aux_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("d^w-ah+t=ih@2_2/A:1_0_3/B:1-0-3@1-1&2-4#1-2$1-2!1-1;0-3|ah/C:1+0+2/D:cc_1/E:wp+1@2+4&1+1#0+3/F:aux_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("w^ah-t+ih=z@3_1/A:1_0_3/B:1-0-3@1-1&2-4#1-2$1-2!1-1;0-3|ah/C:1+0+2/D:cc_1/E:wp+1@2+4&1+1#0+3/F:aux_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ah^t-ih+z=dh@1_2/A:1_0_3/B:1-0-2@1-1&3-3#2-1$1-2!1-0;0-2|ih/C:0+0+2/D:wp_1/E:aux+1@3+3&1+1#0+2/F:det_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^ih-z+dh=ax@2_1/A:1_0_3/B:1-0-2@1-1&3-3#2-1$1-2!1-0;0-2|ih/C:0+0+2/D:wp_1/E:aux+1@3+3&1+1#0+2/F:det_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^z-dh+ax=y@1_2/A:1_0_2/B:0-0-2@1-1&4-2#3-1$1-2!1-0;0-1|ax/C:0+1+3/D:aux_1/E:det+1@4+2&1+1#0+1/F:content_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("z^dh-ax+y=uw@2_1/A:1_0_2/B:0-0-2@1-1&4-2#3-1$1-2!1-0;0-1|ax/C:0+1+3/D:aux_1/E:det+1@4+2&1+1#0+1/F:content_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("dh^ax-y+uw=s@1_3/A:0_0_2/B:0-1-3@1-1&5-1#3-1$1-1!2-0;0-0|uw/C:1+0+2/D:det_1/E:content+1@5+1&1+0#0+0/F:in_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ax^y-uw+s=pau@2_2/A:0_0_2/B:0-1-3@1-1&5-1#3-1$1-1!2-0;0-0|uw/C:1+0+2/D:det_1/E:content+1@5+1&1+0#0+0/F:in_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("y^uw-s+pau=ah@3_1/A:0_0_2/B:0-1-3@1-1&5-1#3-1$1-1!2-0;0-0|uw/C:1+0+2/D:det_1/E:content+1@5+1&1+0#0+0/F:in_1/G:7_4/H:5=5^8=3|L-L%/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("uw^s-pau+ah=v@x_x/A:0_1_3/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:1+0+2/D:content_1/E:x+x@x+x&x+x#x+x/F:in_1/G:5_5/H:x=x^1=10|0/I:6=5/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^pau-ah+v=ax@1_2/A:0_1_3/B:1-0-2@1-1&1-6#1-4$1-4!0-2;0-2|ah/C:0+0+1/D:content_1/E:in+1@1+5&1+3#0+2/F:det_1/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("pau^ah-v+ax=b@2_1/A:0_1_3/B:1-0-2@1-1&1-6#1-4$1-4!0-2;0-2|ah/C:0+0+1/D:content_1/E:in+1@1+5&1+3#0+2/F:det_1/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ah^v-ax+b=uh@1_1/A:1_0_2/B:0-0-1@1-1&2-5#1-4$1-4!1-1;0-1|ax/C:1+1+3/D:in_1/E:det+1@2+4&1+3#0+1/F:content_1/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("v^ax-b+uh=k@1_3/A:0_0_1/B:1-1-3@1-1&3-4#1-3$1-3!2-1;0-2|uh/C:1+0+3/D:det_1/E:content+1@3+3&1+2#0+1/F:content_1/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ax^b-uh+k=th@2_2/A:0_0_1/B:1-1-3@1-1&3-4#1-3$1-3!2-1;0-2|uh/C:1+0+3/D:det_1/E:content+1@3+3&1+2#0+1/F:content_1/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("b^uh-k+th=ao@3_1/A:0_0_1/B:1-1-3@1-1&3-4#1-3$1-3!2-1;0-2|uh/C:1+0+3/D:det_1/E:content+1@3+3&1+2#0+1/F:content_1/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("uh^k-th+ao=t@1_3/A:1_1_3/B:1-0-3@1-1&4-3#2-2$2-3!1-1;1-1|ao/C:1+1+2/D:content_1/E:content+1@4+2&2+1#1+1/F:content_2/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("k^th-ao+t=ae@2_2/A:1_1_3/B:1-0-3@1-1&4-3#2-2$2-3!1-1;1-1|ao/C:1+1+2/D:content_1/E:content+1@4+2&2+1#1+1/F:content_2/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("th^ao-t+ae=l@3_1/A:1_1_3/B:1-0-3@1-1&4-3#2-2$2-3!1-1;1-1|ao/C:1+1+2/D:content_1/E:content+1@4+2&2+1#1+1/F:content_2/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ao^t-ae+l=ax@1_2/A:1_0_3/B:1-1-2@1-2&5-2#3-1$2-2!1-0;2-1|ae/C:0+1+2/D:content_1/E:content+2@5+1&3+0#1+0/F:in_2/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^ae-l+ax=s@2_1/A:1_0_3/B:1-1-2@1-2&5-2#3-1$2-2!1-0;2-1|ae/C:0+1+2/D:content_1/E:content+2@5+1&3+0#1+0/F:in_2/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ae^l-ax+s=pau@1_2/A:1_1_2/B:0-1-2@2-1&6-1#4-1$3-1!1-0;1-0|ax/C:0+0+3/D:content_1/E:content+2@5+1&3+0#1+0/F:in_2/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("l^ax-s+pau=w@2_1/A:1_1_2/B:0-1-2@2-1&6-1#4-1$3-1!1-0;1-0|ax/C:0+0+3/D:content_1/E:content+2@5+1&3+0#1+0/F:in_2/G:5_5/H:6=5^9=2|L-L%/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ax^s-pau+w=ih@x_x/A:0_1_2/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:0+0+3/D:content_2/E:x+x@x+x&x+x#x+x/F:in_2/G:6_5/H:x=x^1=10|0/I:9=4/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^pau-w+ih=th@1_3/A:0_1_2/B:0-0-3@1-2&1-9#1-6$1-5!0-1;0-2|ih/C:1+0+2/D:content_2/E:in+2@1+4&1+2#0+1/F:content_2/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("pau^w-ih+th=aw@2_2/A:0_1_2/B:0-0-3@1-2&1-9#1-6$1-5!0-1;0-2|ih/C:1+0+2/D:content_2/E:in+2@1+4&1+2#0+1/F:content_2/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("w^ih-th+aw=t@3_1/A:0_1_2/B:0-0-3@1-2&1-9#1-6$1-5!0-1;0-2|ih/C:1+0+2/D:content_2/E:in+2@1+4&1+2#0+1/F:content_2/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^th-aw+t=p@1_2/A:0_0_3/B:1-0-2@2-1&2-8#1-5$1-5!0-1;0-1|aw/C:1+1+3/D:content_2/E:in+2@1+4&1+2#0+1/F:content_2/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("th^aw-t+p=ih@2_1/A:0_0_3/B:1-0-2@2-1&2-8#1-5$1-5!0-1;0-1|aw/C:1+1+3/D:content_2/E:in+2@1+4&1+2#0+1/F:content_2/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("aw^t-p+ih=k@1_3/A:1_0_2/B:1-1-3@1-2&3-7#2-4$1-4!1-2;0-3|ih/C:0+0+3/D:in_2/E:content+2@2+3&1+1#0+2/F:cc_1/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("t^p-ih+k=ch@2_2/A:1_0_2/B:1-1-3@1-2&3-7#2-4$1-4!1-2;0-3|ih/C:0+0+3/D:in_2/E:content+2@2+3&1+1#0+2/F:cc_1/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("p^ih-k+ch=er@3_1/A:1_0_2/B:1-1-3@1-2&3-7#2-4$1-4!1-2;0-3|ih/C:0+0+3/D:in_2/E:content+2@2+3&1+1#0+2/F:cc_1/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ih^k-ch+er=z@1_3/A:1_1_3/B:0-0-3@2-1&4-6#3-4$2-4!1-1;1-2|er/C:1+0+2/D:in_2/E:content+2@2+3&1+1#0+2/F:cc_1/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("k^ch-er+z=ao@2_2/A:1_1_3/B:0-0-3@2-1&4-6#3-4$2-4!1-1;1-2|er/C:1+0+2/D:in_2/E:content+2@2+3&1+1#0+2/F:cc_1/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ch^er-z+ao=r@3_1/A:1_1_3/B:0-0-3@2-1&4-6#3-4$2-4!1-1;1-2|er/C:1+0+2/D:in_2/E:content+2@2+3&1+1#0+2/F:cc_1/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("er^z-ao+r=k@1_2/A:0_0_3/B:1-0-2@1-1&5-5#3-3$2-4!2-1;2-1|ao/C:1+1+4/D:content_2/E:cc+1@3+2&2+1#1+1/F:content_4/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("z^ao-r+k=aa@2_1/A:0_0_3/B:1-0-2@1-1&5-5#3-3$2-4!2-1;2-1|ao/C:1+1+4/D:content_2/E:cc+1@3+2&2+1#1+1/F:content_4/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ao^r-k+aa=n@1_4/A:1_0_2/B:1-1-4@1-4&6-4#4-2$2-3!1-2;3-2|aa/C:0+0+2/D:cc_1/E:content+4@4+1&2+0#2+0/F:0_0/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("r^k-aa+n=v@2_3/A:1_0_2/B:1-1-4@1-4&6-4#4-2$2-3!1-2;3-2|aa/C:0+0+2/D:cc_1/E:content+4@4+1&2+0#2+0/F:0_0/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("k^aa-n+v=er@3_2/A:1_0_2/B:1-1-4@1-4&6-4#4-2$2-3!1-2;3-2|aa/C:0+0+2/D:cc_1/E:content+4@4+1&2+0#2+0/F:0_0/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("aa^n-v+er=s@4_1/A:1_0_2/B:1-1-4@1-4&6-4#4-2$2-3!1-2;3-2|aa/C:0+0+2/D:cc_1/E:content+4@4+1&2+0#2+0/F:0_0/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("n^v-er+s=ey@1_2/A:1_1_4/B:0-0-2@2-3&7-3#5-2$3-3!1-1;1-1|er/C:1+1+2/D:cc_1/E:content+4@4+1&2+0#2+0/F:0_0/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("v^er-s+ey=sh@2_1/A:1_1_4/B:0-0-2@2-3&7-3#5-2$3-3!1-1;1-1|er/C:1+1+2/D:cc_1/E:content+4@4+1&2+0#2+0/F:0_0/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("er^s-ey+sh=ax@1_2/A:0_0_2/B:1-1-2@3-2&8-2#5-1$3-2!2-0;2-1|ey/C:0+1+2/D:cc_1/E:content+4@4+1&2+0#2+0/F:0_0/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("s^ey-sh+ax=n@2_1/A:0_0_2/B:1-1-2@3-2&8-2#5-1$3-2!2-0;2-1|ey/C:0+1+2/D:cc_1/E:content+4@4+1&2+0#2+0/F:0_0/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ey^sh-ax+n=pau@1_2/A:1_1_2/B:0-1-2@4-1&9-1#6-1$4-1!1-0;1-0|ax/C:0+0+0/D:cc_1/E:content+4@4+1&2+0#2+0/F:0_0/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("sh^ax-n+pau=x@2_1/A:1_1_2/B:0-1-2@4-1&9-1#6-1$4-1!1-0;1-0|ax/C:0+0+0/D:cc_1/E:content+4@4+1&2+0#2+0/F:0_0/G:6_5/H:9=4^10=1|H-H%/I:0=0/J:79+57-10"));
    unit->mage->pushLabel(MAGE::Label("ax^n-pau+x=x@x_x/A:0_1_2/B:x-x-x@x-x&x-x#x-x$x-x!x-x;x-x|x/C:0+0+0/D:content_4/E:x+x@x+x&x+x#x+x/F:0_0/G:9_4/H:x=x^1=10|0/I:0=0/J:79+57-10"));

    CmdData* cmdData = (CmdData*)RTAlloc(unit->mWorld, sizeof(CmdData));
    cmdData->unit = unit;

    DoAsynchronousCommand(
        unit->mWorld, // world
        0, // reply address (not used)
        "", // command name (not used)
        (void*)cmdData, // command data, cast to void*
        (AsyncStageFn)SCMageAsyncInit_run, // stage 2, NRT
        NULL, // stage 3, RT (not used)
        NULL, // stage 4, NRT (not used)
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
            (AsyncStageFn)SCMageAsyncMain_run, // stage 2, NRT
            NULL, // stage 3, RT (not used)
            NULL, // stage 4, NRT (not used)
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
    printf("SCMage_Dtor %d\n", counter);
    //delete unit->mage;
}

void SCMage_addVoice(Unit* unit, sc_msg_iter* args) {
    printf("wahhh\n");
}

PluginLoad(SCMageUGens) {
    ft = inTable;
    DefineDtorUnit(SCMage);
    DefineUnitCmd("SCMage", "addVoice", SCMage_addVoice);
}
