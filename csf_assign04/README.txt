CONTRIBUTIONS

Alex: wrote the code in main
Dylan: wrot the readme report and ran the experiment 

REPORT

Test run with threshold 2097152
real    0m0.405s
user    0m0.393s
sys     0m0.008s

Test run with threshold 1048576
real    0m0.256s
user    0m0.440s
sys     0m0.027s

Test run with threshold 524288
real    0m0.217s
user    0m0.474s
sys     0m0.038s

Test run with threshold 262144
real    0m0.134s
user    0m0.540s
sys     0m0.049s

Test run with threshold 131072
real    0m0.141s
user    0m0.535s
sys     0m0.057s

Test run with threshold 65536
real    0m0.142s
user    0m0.563s
sys     0m0.072s

Test run with threshold 32768
real    0m0.146s
user    0m0.566s
sys     0m0.105s

Test run with threshold 16384
real    0m0.153s
user    0m0.588s
sys     0m0.143s

By parallelizing our program, we were able to sort the array with many child processes running to sort smaller portions of the array. The number of parallel processes
we needed to run was inversely portional to the threshold as when the threshold decreases, we need to run more processes to sort the array more before we reach the limit
at which we are fine with running qsort. Thus, it makes sense that our first 4 tested thresholds have decreasing times since we had more parallel processes working to sort the array
due to smaller thresholds. With more processes running in parallel, our array was able to be sorted faster. However, we see that in the last 4 tests, we started to see diminishing returns with runtimes as times began to increase not decrease.
This result makes sense when you take into account hardware complexity. Our computer only has so many CPU cores to run different processes on, so what likely happened in these
last 4 tests is that the number of parallel processes needed exceeded the amount of processes the computer could handle all at once, so the computer had to start using interval timers to
switch between different processes. By switching processes after a certain time interval and not having all processes running together, we lose the essense of our code running in parallel and we also incur the time penalty 
associated with switching between processes. Thus, in the last 4 tests, our sorting is not completely running in parallel and is slowed down by switching between processes on CPU cores,
so our times for the last 4 tests increased instead of the expected decrease. 
