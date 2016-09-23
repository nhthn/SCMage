#!/bin/sh

UTT2LAB=~/git/SCMage/tools/utt2lab
FESTIVAL=~/git/SCMage/festival/programs/festival
TEXT2UTT=$FESTIVAL/examples/text2utt
DUMPFEATS=$FESTIVAL/examples/dumpfeats
base=`basename $1 .txt`
$TEXT2UTT $1 > /tmp/utterance.utt
$DUMPFEATS -eval $UTT2LAB/extra_feats.scm -relation Segment -feats $UTT2LAB/label.feats -output /tmp/extracted_features.txt /tmp/utterance.utt
gawk -f $UTT2LAB/label-full.awk /tmp/extracted_features.txt > /tmp/with_durations.lab
# gawk -f $UTT2LAB/label-mono.awk tmp > $base-mono.lab
gawk -F" " '{print $3}' /tmp/with_durations.lab > $base.lab
