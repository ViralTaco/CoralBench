package com.coralblocks.coralbench.bench;

import com.coralblocks.coralbench.Bench;
import com.coralblocks.coralbench.util.LongMap;
import com.coralblocks.coralbench.util.Random;
import com.coralblocks.coralbench.util.ThreadPinning;

public class LongMapBench {
	
	private static final long THE_SEED = 123456789L;
	
	public static void main(String[] args) {
		
		final int iterations = Integer.parseInt(args[0]);
		
		final int passes = Integer.parseInt(args[1]);
		
		final int coreId = Integer.parseInt(args[2]);
		
		if (coreId >= 0) ThreadPinning.pinCurrentThread(coreId);
		
		final String filler = new String("FILLER");
		
		LongMap<String> map = new LongMap<String>(8388608); // 2 ^ 23
		
		Random rand = new Random(THE_SEED);
		
		Bench bench = new Bench();
		
		System.out.println("\nBenchmarking put operation...\n");
		
		for(int pass = 0; pass < passes; pass++) {

			map.clear(); // we will re-insert everything...
			bench.reset();
			rand.reset();
			
			for(int i = 0; i < iterations; i++) {
				
				long key = rand.nextLong(); // this is deterministic (pseudo-random)
				
				bench.mark();
				map.put(key, filler);
				bench.measure();
			}
			
			System.out.println("Final size of map: " + map.size());
			System.out.println();
			bench.printResults();
		}
		
		System.out.println("Benchmarking get operation...\n");
		
		for(int pass = 0; pass < passes; pass++) {
			
			bench.reset();
			rand.reset();
			
			String gotten = null;
		
			for(int i = 0; i < iterations; i++) {
				
				long key = rand.nextLong(); // this is deterministic (pseudo-random)
				
				bench.mark();
				gotten = map.get(key);
				bench.measure();
			}
			
			System.out.println("Last object gotten: " + gotten);
			System.out.println();
			bench.printResults();
		}
		
		System.out.println("Benchmarking remove operation...\n");
		
		map.clear();
		
		for(int pass = 0; pass < passes; pass++) {
			
			rand.reset();
			for(int i = 0; i < iterations; i++) map.put(rand.nextLong(), filler);
			
			bench.reset();
			rand.reset();
			
			String removed = null;
		
			for(int i = 0; i < iterations; i++) {
				
				long key = rand.nextLong(); // this is deterministic (pseudo-random)
				
				bench.mark();
				removed = map.remove(key);
				bench.measure();
			}
			
			System.out.println("Last object removed: " + removed);
			System.out.println("Final size of map: " + map.size());
			System.out.println();
			bench.printResults();
		}
	}
}