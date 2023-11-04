MS1 work:
    Alex: main, csfimfuncs 
    Dylan: Makefile

MS2 work:
    Alex: cache misses logic (stores and loads and fifo/lru), created 3 structs outline, logic in main 
    Dylan: cache hits logic (stores and loads), logic for timestamps and cache counter

MS3 work:
    Alex: best cache explanation
    Dylan: refactoring code, added map optimization

All experiments run on Gcc.trace and with a 512KB cache: 
Cache results for 1024 32 16 (set associative 512 KB):
    W.a and w.b and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 4,988,872
    W.a and w.b and fifo 
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 4,989,272
    W.a and w.t and lru 
        Total loads: 318197
        Total stores: 197486
        Load hits: 314798
        Load misses: 3399
        Store hits: 188250
        Store misses: 9236
        Total cycles: 25,117,398
    W.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 24,548,855
    N.w.a and w.t and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 312207
        Load misses: 5990
        Store hits: 165216
        Store misses: 32270
        Total cycles: 22,456,807
    N.w.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 312207
        Load misses: 5990
        Store hits: 165216
        Store misses: 32270
        Total cycles: 22,456,807
    Conclusions:
        write allocate write back had best time cycles by far and had best or tied for best hit rates. fifo and lru performed similarly. 

Cache results for 256 128 16 (set associative #2 512 KB):
    W.a and w.b and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 4,988,872
    W.a and w.b and fifo 
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 4,988,872
    W.a and w.t and lru 
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 24,548,855
    W.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 24,548,855
    N.w.a and w.t and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 312207
        Load misses: 5990
        Store hits: 165216
        Store misses: 32270
        Total cycles: 22,456,807
    N.w.a and w.t and fifo  
        Total loads: 318197
        Total stores: 197486
        Load hits: 312207
        Load misses: 5990
        Store hits: 165216
        Store misses: 32270
        Total cycles: 22,456,807
    Conclusions:
        Set associative cache with more blocks/set has the same or less time cycles as experiment 1. Lru and fifo almost identical results. 

Cache Results for 32768 1 16 (direct mapped 512 KB):
    W.a and w.b and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 315816
        Load misses: 2381
        Store hits: 188593
        Store misses: 8893
        Total cycles: 5,124,809
    W.a and w.b and fifo        
        Total loads: 318197
        Total stores: 197486
        Load hits: 315816
        Load misses: 2381
        Store hits: 188593
        Store misses: 8893
        Total cycles: 5,124,809
    W.a and w.t and lru 
        Total loads: 318197
        Total stores: 197486
        Load hits: 315816
        Load misses: 2381
        Store hits: 188593
        Store misses: 8893
        Total cycles: 24,574,016
    W.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 315816
        Load misses: 2381
        Store hits: 188593
        Store misses: 8893
        Total cycles: 24,574,016
    N.w.a and w.t and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 312176
        Load misses: 6021
        Store hits: 165174
        Store misses: 32312
        Total cycles: 22,469,176
    N.w.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 312176
        Load misses: 6021
        Store hits: 165174
        Store misses: 32312
        Total cycles: 22,469,176
    Conclusions: 
        Lru and fifo identical for direct mapped, makes sense as only one eviction choice in direct mapped cache. W.a and w.b was by far the best in terms of time cycles and hit rates

Cache results for 1 32768 16 (fully associative 512 KB):
    W.a and w.b and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 4,988,872
    W.a and w.b and fifo 
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 4,988,872
    W.a and w.t and lru 
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 24,548,855
    W.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 315855
        Load misses: 2342
        Store hits: 188617
        Store misses: 8869
        Total cycles: 24,548,855
    N.w.a and w.t and lru   
        Total loads: 318197
        Total stores: 197486
        Load hits: 312207
        Load misses: 5990
        Store hits: 165216
        Store misses: 32270
        Total cycles: 22,456,807
    N.w.a and w.t and fifo      
        Total loads: 318197
        Total stores: 197486
        Load hits: 312207
        Load misses: 5990
        Store hits: 165216
        Store misses: 32270
        Total cycles: 22,456,807
    Conclusions:
        W.a and w.b was the best in terms of both time cycles and hit rates. lru and fifo were identical.

Experiment for block size: 256 8 256 write allocate write back (512 KB)
lru
    Total loads: 318197
    Total stores: 197486
    Load hits: 317720
    Load misses: 477
    Store hits: 196760
    Store misses: 726
    Total cycles: 8,437,680
Fifo
    Total loads: 318197
    Total stores: 197486
    Load hits: 317714
    Load misses: 483
    Store hits: 196760
    Store misses: 726
    Total cycles: 8,488,874
Conclusion:
    Total cycles increases almost doubled although misses went way down. Bigger block size seems to be less efficient in terms of total cycles with same amount of sets and cache size. 

Overall conclusion and explanation of best cache:

I performed 5 experiments, all using gcc.trace and 512KB size cache so that I could isolate cache performance factors with constant data operations and cache size. I kept the cache size constant by ensuring that when I updated the number of sets or blocks/sets,
sets*(blocks/set)*block_size stayed at 524,288 bytes (aka 512KB) as changing cache size would create an unwanted change in performance. The purpose of my experiments was to determine what the best cache configuration is between direct mapped, fully associative, and set associative and how block size and blocks/set 
(for set associaitive) affect performance. I judged each cache's performance by writing down the results of running every combination (write and eviction policies) of each cache configuration (d.m, f.a, or s.a) and looking at which cache had the highest hit rate 
with the least time cycles. I decided to perform multiple experiments to determine many different optimal cache factors because the best cache configuration is not just dependent on whether it is direct mapped or associative but also how costly and effective its write and 
eviction policies are, as well as how much data it is storing with how much space it has to store that data. Lastly, I actually performed the experiments by putting in all the parameters as command line arguments for my program, recording the output hits/misses/cycles, and then repeating
for every valid combination of factors. 

From the above experiments, I found that direct mapped caches performed worst, averaged over all configurations, in terms of hit rate and time cycles. For all configurations, full associative and set associative
were almost identical in terms of cycles and hit rates. However, fully associative caches aren't practical to use in real life as they're hard and costly to build at scale, so set associative is best choice and 
over all experiments, write allocate and write back performed best in terms of hit rates and low time cycles. In my experiments, lru and fifo performed very similarly, so either option seems reasonable for best cache. However, 
I only used gcc.trace and the optimal eviction policy depends on the data you're working with (if you're reading a lot of the same data over and over again, you would want to use lru whereas if you have scattered data 
with no seeming pattern you should use fifo). I also decided to run a second set associative cache experiment to see how changing the blocks/set while keeping cache size the same would affect performance. 
I found that with more blocks/set, average time cycles decreased slightly, which makes sense as more blocks/set might've lessened the chance for a miss even though we had fewer sets in that experiment to keep cache size the same. 
I also wanted to test how block size affects the cache, and I found that as you increase block size while keeping sets and cache size the same, misses decreased but time cycles almost doubled as the miss penalty was so much larger, 
so I concluded that a smaller block size is better than a large one.

Thus, the best cache is a set associative cache that employs write-allocate and write-back, uses an eviction policy that matches testing data (lru if repititive data, fifo if random), and has a larger rather than smaller amount of 
blocks/set (up to a certain point due to practicality), and doesn't use too big of a block size as increasing block size increases miss penalty (too costly for time cycles). This best cache makes sense intuitively as set associative caches 
provide a higher hit rate due to having multiple sets to index into, and multiple slots within those sets to reduce the chance for a miss. Set associative caches are also easier to scale and build than fully associative caches. 
Lastly, it makes sense that a smaller block size would result in a more optimal cache (in terms of time cycles) as memory operations are more costly with larger data sizes.  


