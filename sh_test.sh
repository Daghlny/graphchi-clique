#!/bin/bash

mydir=$HOME

graphchi_path=$mydir/graphchi-clique
cd $graphchi_path

$graphchi_path/clique/clique_bfs file $mydir/data/rmat-500/rmat-500-5k.txt -e edgelist


