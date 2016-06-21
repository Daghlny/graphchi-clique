
#!/bin/bash
mydir=/public/home/lyn
outdir=$mydir/graphchi-clique/result
mem=8192

cd $mydir/graphchi-clique

for((threads=1; threads<=16; threads++))
do
    echo $threads
    perf record -o perf.data/$threads.perf.data ./clique/clique_bfs --file=/home/lyn/data/rmat-500/rmat-500-10k.txt --taskPerIter=200 --membudget_mb=$mem -e edgelist > $outdir/10k-clique-$mem-$threads.output
done

