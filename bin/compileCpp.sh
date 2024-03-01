#!/bin/bash

g++ -std=c++17 src/main/c/CoralBlocks/CoralBench/Util/MathUtils.cpp src/main/c/TestMathUtils.cpp -o src/main/c/TestMathUtils

g++ -std=c++17 src/main/c/CoralBlocks/CoralBench/Util/MathUtils.cpp src/main/c/TestLongMap.cpp -o src/main/c/TestLongMap

g++ -std=c++17 src/main/c/CoralBlocks/CoralBench/Bench.cpp src/main/c/TestBench.cpp -o src/main/c/TestBench

g++ -std=c++17 src/main/c/CoralBlocks/CoralBench/Util/Random.cpp src/main/c/TestRandom.cpp -o src/main/c/TestRandom

g++ -std=c++17 src/main/c/CoralBlocks/CoralBench/Util/ThreadPinning.cpp src/main/c/TestThreadPinning.cpp -o src/main/c/TestThreadPinning

g++ -O3 -std=c++17 src/main/c/CoralBlocks/CoralBench/Util/MathUtils.cpp src/main/c/CoralBlocks/CoralBench/Bench/LongMapGetBench.cpp -o src/main/c/LongMapGetBench

g++ -O3 -std=c++17 src/main/c/CoralBlocks/CoralBench/Util/MathUtils.cpp src/main/c/CoralBlocks/CoralBench/Bench/LongMapRemoveBench.cpp -o src/main/c/LongMapRemoveBench

g++ -O3 -std=c++17 src/main/c/CoralBlocks/CoralBench/Util/MathUtils.cpp src/main/c/CoralBlocks/CoralBench/Bench/LongMapPutBench.cpp -o src/main/c/LongMapPutBench


