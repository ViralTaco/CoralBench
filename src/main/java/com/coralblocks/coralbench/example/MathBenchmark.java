/* 
 * Copyright 2024 (c) CoralBlocks - http://www.coralblocks.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language
 * governing permissions and limitations under the License.
 */
package com.coralblocks.coralbench.example;

import com.coralblocks.coralbench.Bench;

public class MathBenchmark {
	
    private final static long doSomething(int load, int i) {
        
        long x = 0;
         
        for(int j = 0; j < load; j++) {
            long pow = (i % 8) * (i % 16);
            if (i % 2 == 0) {
                x += pow;
            } else {
                x -= pow;
            }
        }
         
        return x;
    }
    
	public static void main(String[] args) {
		
		final int warmupIterations = 1_000_000;
		final int measurementIterations = 9_000_000;
		final int totalIterations = measurementIterations + warmupIterations;
		final int load = 10000;
		
		Bench bench = new Bench(warmupIterations);
		
		long x = 0;
		
		for(int i = 0; i < totalIterations; i++) {
			bench.mark();
			x += doSomething(load, i);
			bench.measure();
		}
		
		System.out.println("Value computed: " + x);
		
		bench.printResults();
	}
}