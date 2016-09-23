#!/bin/sh

DUMPFEATS=~/festival/build/festival/examples/dumpfeats
base=`basename $1 .utt`
$DUMPFEATS -eval extra_feats.scm -relation Segment -feats label.feats -output tmp $1
gawk -f label-full.awk tmp > $base.lab
gawk -f label-mono.awk tmp > $base-mono.lab
awk -F" " '{print $3}' filename.lab > nodur-filename.lab
