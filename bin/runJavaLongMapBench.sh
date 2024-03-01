#!/bin/bash

ITERATIONS=${1:-100000}
PASSES=${1:-6}
CORE_ID=${1:--1}

CMD="java -verbose:gc -Xms2g -Xmx4g -XX:NewSize=512m -XX:MaxNewSize=1024m -cp target/coralbench-all.jar com.coralblocks.coralbench.bench.LongMapBench $ITERATIONS $PASSES $CORE_ID"

echo $CMD

$CMD


