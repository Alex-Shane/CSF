MS1 work:
    Alex: main, csfimfuncs 
    Dylan: Makefile

MS2 work:
    Alex: cache misses logic (stores and loads), created 3 structs outline, logic in main 
    Dylan: cache hits logic (stores and loads), logic for timestamps and cache counter

MS3 work:
    Alex: best cache explanation
    Dylan: refactoring code, added map optimization

All experiments run on Gcc.trace: 
Cache results for 256 4 16 (set associative):
    W.a and w.b and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 314798
        Load misses: 3399
        Store hits: 188250
        Store misses: 9236
        Total cycles: 9331848
    W.a and w.b and fifo 
        Total loads: 318197
        Total stores: 197486
        Load hits: 314171
        Load misses: 4026
        Store hits: 188047
        Store misses: 9439
        Total cycles: 9831818
    W.a and w.t and lru 
        Total loads: 318197
        Total stores: 197486
        Load hits: 314798
        Load misses: 3399
        Store hits: 188250
        Store misses: 9236
        Total cycles: 25117398
    W.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 314171
        Load misses: 4026
        Store hits: 188047
        Store misses: 9439
        Total cycles: 25448771
    N.w.a and w.t and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 311613
        Load misses: 6584
        Store hits: 164819
        Store misses: 32667
        Total cycles: 22693813
    N.w.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 311017
        Load misses: 7180
        Store hits: 163705
        Store misses: 33781
        Total cycles: 22931617
    Conclusions:
        Lru less cycles and misses than fifo on any config

Cache results for 256 8 16 (set associative #2):
    W.a and w.b and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 315261
        Load misses: 2936
        Store hits: 188513
        Store misses: 8973
        Total cycles: 8550574
    W.a and w.b and fifo 
        Total loads: 318197
        Total stores: 197486
        Load hits: 314928
        Load misses: 3269
        Store hits: 188417
        Store misses: 9069
        Total cycles: 8817745
    W.a and w.t and lru 
        Total loads: 318197
        Total stores: 197486
        Load hits: 315261
        Load misses: 2936
        Store hits: 188513
        Store misses: 8973
        Total cycles: 24827461
    W.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 314928
        Load misses: 3269
        Store hits: 188417
        Store misses: 9069
        Total cycles: 24998728
    N.w.a and w.t and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 311937
        Load misses: 6260
        Store hits: 165155
        Store misses: 32331
        Total cycles: 22564537
    N.w.a and w.t and fifo  
        Total loads: 318197
        Total stores: 197486
        Load hits: 311649
        Load misses: 6548
        Store hits: 164824
        Store misses: 32662
        Total cycles: 22679449
    Conclusions:
        Lru less cycles and misses than fifo for any config. Set associative cache with more blocks/set has less time cycles 

Cache Results for 256 1 16 (direct mapped):
    W.a and w.b and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 298863
        Load misses: 19334
        Store hits: 185202
        Store misses: 12284
        Total cycles: 20280865
    W.a and w.b and fifo        
        Total loads: 318197
        Total stores: 197486
        Load hits: 298863
        Load misses: 19334
        Store hits: 185202
        Store misses: 12284
        Total cycles: 20280865
    W.a and w.t and lru 
        Total loads: 318197
        Total stores: 197486
        Load hits: 298863
        Load misses: 19334
        Store hits: 185202
        Store misses: 12284
        Total cycles: 32694663
    W.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 298863
        Load misses: 19334
        Store hits: 185202
        Store misses: 12284
        Total cycles: 32694663
    N.w.a and w.t and lru
        Gcc.trace
        Total loads: 318197
        Total stores: 197486
        Load hits: 296251
        Load misses: 21946
        Store hits: 158894
        Store misses: 38592
        Total cycles: 28823251
    N.w.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 296251
        Load misses: 21946
        Store hits: 158894
        Store misses: 38592
        Total cycles: 28823251
    Conclusions: 
        Lru and fifo identical for direct mapped, makes sense as only one eviction choice in direct mapped cache. W.a and w.b was by far the best in terms of time cycles and hit rates

Cache results for 1 256 16 (fully associative):
    W.a and w.b and lru
        Total loads: 318197
        Total stores: 197486
        Load hits: 313123
        Load misses: 5074
        Store hits: 187653
        Store misses: 9833
        Total cycles: 10787576
    W.a and w.b and fifo 
        Total loads: 318197
        Total stores: 197486
        Load hits: 311025
        Load misses: 7172
        Store hits: 186977
        Store misses: 10509
        Total cycles: 12443602
    W.a and w.t and lru 
        Total loads: 318197
        Total stores: 197486
        Load hits: 313123
        Load misses: 5074
        Store hits: 187653
        Store misses: 9833
        Total cycles: 26024523
    W.a and w.t and fifo
        Total loads: 318197
        Total stores: 197486
        Load hits: 311025
        Load misses: 7172
        Store hits: 186977
        Store misses: 10509
        Total cycles: 27132025
    N.w.a and w.t and lru   
        Total loads: 318197
        Total stores: 197486
        Load hits: 309777
        Load misses: 8420
        Store hits: 163612
        Store misses: 33874
        Total cycles: 23426377
    N.w.a and w.t and fifo      
        Total loads: 318197
        Total stores: 197486
        Load hits: 307970
        Load misses: 10227
        Store hits: 161070
        Store misses: 36416
        Total cycles: 24147370
    Conclusions:
        Lru was better than fifo for each config in terms of cycles and misses. W.a and w.b was by far the best in terms of time cycles and misses 

Experiment for block size: 256 4 256 write allocate write back 
lru
    Total loads: 318197
    Total stores: 197486
    Load hits: 317691
    Load misses: 506
    Store hits: 196758
    Store misses: 728
    Total cycles: 9679249
Fifo
    Total loads: 318197
    Total stores: 197486
    Load hits: 317642
    Load misses: 555
    Store hits: 196755
    Store misses: 731
    Total cycles: 10248797
Conclusion:
    Total cycles increases by multiple hundreds of thousands (compared to first experiment) when you increase block size and misses plummeted, so smaller block 
    size is better for caches. 

Overall conclusion and explanation of best cache:

I performed 5 experiments (all using gcc.trace) to determine what the best cache configuration is between direct mapped, fully associative, and set associative and how 
block size and blocks per set (for set associaitive) affect performance. I judged the caches performance by writing down the results of running every combination 
(write and eviction policies) of each cache configuration (d.m, f.a, or s.a) and looking at which combination and configuration had the highest hit rate with the smallest time cycles.

From the above experiments, I found that Direct mapped performed worst over all configurations in terms of hit rate and time cycles. Set associative had the overall smallest time cycle 
(9.3 million) although the average time cycles for set associative and fully associative were similar. However, fully associative caches aren't practical to use in real life as they're 
hard and costly to build, so set associative is best choice and over all experiments, write allocate and write back with lru performed best in terms of hit rates and low time cycles.
However, I only used gcc.trace and the optimal eviction policy depends on the data you're working with (if you're reading a lot of the same data over and over again, you would want to use 
lru whereas if you have scattered data with no seeming pattern you should use fifo). For set associative cache experiments, I found that with more blocks, time cycles decreased (up to a point 
where the cache is still practical to build). For best block size, I found that as you increase block size, time cycles also increase, so smaller blocks size is better.

Thus, the best cache is set associative with a larger rather than smaller amount of blocks/set (up to a certain point), write-allocate, write-back, with lru if data with patterns/repetition or 
fifo if random data and a small block size. This best cache makes sense intuitively as set associative caches provides a higher hit rate due to having multiple sets to index into, and multiple 
slots within those sets to reduce the chance for a miss and set associative caches are easier to scale and build than fully associative caches.


