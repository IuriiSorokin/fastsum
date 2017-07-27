# Estimates of the algorithm complexities.

Let `N` be number of elements in the original vector. 
#### `sum1`
The algorithm performs the sorting `N-1` times, and the number of items being sorted changes from `N` to `2`. STL guarantees the complexity of sorting ` n*log(n)`. The sorting algorithm is not aware of the fact that from the second iteration on the vector is already almost sorted (only the first element is not on it's place), and the sorting could be accomplished with `O(n)` comparisons and one move of a memory block (also `O(n)`). It looks like the actual complexity of the sorting is indeed `n*log(n)` (see benchmarks below). 
The number of operations, that the summing algorithm will perform, is roughly proportional to:
`Integral( n*log(n) ) dn ` from `1` to `N`,  which after neglecting the `N^2` term and constants, gives the complexity of: 
`N^2 * log(N)`.

#### `sum2`
The algorithm performs `N-1` partial sortings. The number of elements below the median is always `2`. The number of elements above (and including) the median changes from `N-2` to `0`. The guranteed complexity of the soring is 
`n_above * log( n_below )`.
Hence, the complexity of the summing algorithm is 
`Integral( n_above * log( const )  ) ~ N^2`

#### `sum3`
There are two time-consuming actions: the construction of the multiset, and the acutal summing. The complexity of constructing the multiset is `N*log(N)`. This estimate considers only arranging the elements in the underlying data structure (RB-tree), and does not take the memory allocations into account. 
In the summing part the most time-consuming operations are insertion and erasure of the elements. For both the complexity is `log(n)` (where `n` is the size of the multiset). The complexity of the summing part is therefore also `N*log(N)`, with, presumably, different constants.

# Measurement of the actual exectuion time.
The actual exection time has been measured as a function of the sample size, and compared with the above estimates for the complexity. The complexity estimates for `sum1` and `sum2` are consistent with the measurements, whereas the `sum3` execution time rose slightly faster than `N*log(N)`. 

The test data was generated using `std::exponential_distribution` with `lamda = 0.01`. Exponential distribution is one of the worst cases for the summing algorithms, as when many small items are added to a large intermediate sum the rounding error accumulates rapidly.

The results are in the tables below. 
The first column is the number of items in the sample.
The second is the average best time: for each number of items the algorithm was executed 7 times, and the 5 best (smallest) exectuion times were selected, out of which the average was calcuated.
The thrid column is the mean relative error for all 7 executions. The relative error is defined as `abs(correct_sum - evaluated_sum) / correct_sum`.
In the fourth column the average best time is compared with the estimated complexity for the given algorithm (the estimated complexity is compatible with the measurement if the ratio stays constant with the increase of the sample size).

The progrmam was compiled with g++ 6.2.1 with "-O3", and executed on Intel i7-4790 @ 3.60GHz. The execution time is specified in microseconds. 

As mentioned above, for `sum1` and `sum2` the complexity estimates are compatible with the measurements:

```
=================== Complexity of sum1 =======================
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n^2*log(n))
     100    2.280000e+01    2.541793e-17    4.950957e-04
     320    2.488000e+02    1.615234e-17    4.212123e-04
    1000    2.643000e+03    2.050718e-17    3.826134e-04
    3200    3.079740e+04    0.000000e+00    3.726420e-04
   10000    3.337598e+05    3.317702e-17    3.623751e-04
   32000    3.949698e+06    8.303712e-17    3.718253e-04
  100000    4.279730e+07    2.658199e-17    3.717326e-04
  320000    4.758187e+08    4.984892e-17    3.665698e-04
```
```  
  =================== Complexity of sum2 =======================
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n^2)
     100    1.900000e+01    4.263644e-17    1.900000e-03
     320    1.570000e+02    1.702978e-17    1.533203e-03
    1000    1.484400e+03    6.129007e-17    1.484400e-03
    3200    1.769920e+04    5.202459e-17    1.728438e-03
   10000    1.951104e+05    1.681397e-17    1.951104e-03
   32000    2.062521e+06    4.165074e-17    2.014181e-03
  100000    2.028747e+07    5.305061e-17    2.028747e-03
  320000    2.073556e+08    3.322726e-17    2.024957e-03
```

For `sum3` the execution time rises slightly faster:
```
=================== Complexity of sum3 =======================
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n*log(n))
     100    1.680000e+01    0.000000e+00    3.648074e-02
     320    6.020000e+01    6.493705e-17    3.261348e-02
    1000    1.990000e+02    2.189106e-17    2.880820e-02
    3200    6.806000e+02    2.656637e-17    2.635237e-02
   10000    2.416000e+03    5.020681e-17    2.623139e-02
   32000    9.026000e+03    6.212339e-17    2.719070e-02
  100000    3.340020e+04    8.002949e-17    2.901105e-02
  320000    1.747816e+05    3.321851e-17    4.308845e-02
 1000000    7.919784e+05    4.260454e-17    5.732531e-02
 3200000    3.247844e+06    5.320052e-17    6.775981e-02
10000000    1.246619e+07    5.107194e-17    7.734285e-02
```

The `sum3` algorithm was then split into two parts: construction of the multiset and the actual summing. Each part was profiled sepearately. The construction-only part does not perform the summing (returns the smallest element in the multiset), so its relative error should be ignored. 

It turned out, it is the construction time that rises faster than `N*log(N)`. Most likely this is caused by memory allocation. To check this one can provide a custom constant-time allocator, that would reserve the memory from the OS in advance. However, this goes already too far beyond the scope of the subject.
```
=================== Complexity of sum3_construct_only ========
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n*log(n))
     100    6.600000e+00    9.998975e-01    1.433172e-02
     320    2.680000e+01    9.999935e-01    1.451896e-02
    1000    9.100000e+01    9.999989e-01    1.317360e-02
    3200    3.296000e+02    9.999999e-01    1.276189e-02
   10000    1.263200e+03    1.000000e+00    1.371502e-02
   32000    5.084400e+03    1.000000e+00    1.531669e-02
  100000    1.970380e+04    1.000000e+00    1.711450e-02
  320000    1.069282e+05    1.000000e+00    2.636073e-02
 1000000    5.680766e+05    1.000000e+00    4.111876e-02
 3200000    2.466015e+06    1.000000e+00    5.144851e-02
10000000    9.676081e+06    1.000000e+00    6.003241e-02
```

The summing part of `sum3` has indeed the complexity of `n*log(n)`:
```
=================== Complexity of sum3_sum_only ==============
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n*log(n))
     100    1.100000e+01    4.011405e-17    2.388620e-02
     320    3.720000e+01    3.335684e-17    2.015318e-02
    1000    1.198000e+02    4.030136e-17    1.734283e-02
    3200    3.946000e+02    5.114667e-17    1.527864e-02
   10000    1.331800e+03    8.352988e-17    1.445983e-02
   32000    4.651200e+03    8.311811e-17    1.401168e-02
  100000    1.571000e+04    2.650188e-17    1.364553e-02
  320000    9.431800e+04    6.658416e-17    2.325197e-02
 1000000    3.288376e+05    6.388306e-17    2.380206e-02
 3200000    1.139498e+06    5.324321e-17    2.377336e-02
10000000    3.751500e+06    3.404994e-17    2.327508e-02
```


# Better summing algorithms

For summing positive numbers with a double precision Kahan algorithm is sufficient.
It has linear complexity with a relatively low constant:

```
=================== Complexity of kahan_classic ==============
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n)
     100    0.000000e+00    0.000000e+00    0.000000e+00
     320    1.000000e+00    0.000000e+00    3.125000e-03
    1000    3.000000e+00    0.000000e+00    3.000000e-03
    3200    9.200000e+00    0.000000e+00    2.875000e-03
   10000    3.040000e+01    0.000000e+00    3.040000e-03
   32000    9.800000e+01    0.000000e+00    3.062500e-03
  100000    3.090000e+02    0.000000e+00    3.090000e-03
  320000    9.998000e+02    0.000000e+00    3.124375e-03
 1000000    3.194400e+03    0.000000e+00    3.194400e-03
 3200000    1.040360e+04    0.000000e+00    3.251125e-03
10000000    3.108460e+04    0.000000e+00    3.108460e-03
```

It is interesting to notice that a slight modification of the Kahan algorithm works 3 times faster:
```
=================== Complexity of kahan_optimized ============
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n)
     100    0.000000e+00    0.000000e+00    0.000000e+00
     320    0.000000e+00    0.000000e+00    0.000000e+00
    1000    1.000000e+00    0.000000e+00    1.000000e-03
    3200    4.000000e+00    0.000000e+00    1.250000e-03
   10000    1.040000e+01    0.000000e+00    1.040000e-03
   32000    3.200000e+01    0.000000e+00    1.000000e-03
  100000    1.000000e+02    0.000000e+00    1.000000e-03
  320000    3.200000e+02    0.000000e+00    1.000000e-03
 1000000    1.030600e+03    0.000000e+00    1.030600e-03
 3200000    3.378200e+03    0.000000e+00    1.055687e-03
10000000    1.057840e+04    0.000000e+00    1.057840e-03
```
This difference is not understood, as both algorithms perform the same number of arithmetic operations per iteration. Also,  when compiled with g++ 6.3 with "-O3", both algorithms perform three (not four) "assignments". The differences are that the original algorithm performs one addition and three subtractions, and the "optimized" performs two additions and two subtractions. Also the operation sequence is different, which may result in different latencies. 


Even faster and simpler solution would be to perform straightforward summing with `long double` accumulator:
```
=================== Complexity of sum_simple<long double> =====
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n)
     100    0.000000e+00    0.000000e+00    0.000000e+00
     320    0.000000e+00    0.000000e+00    0.000000e+00
    1000    0.000000e+00    0.000000e+00    0.000000e+00
    3200    2.000000e+00    0.000000e+00    6.250000e-04
   10000    7.000000e+00    1.637862e-17    7.000000e-04
   32000    2.400000e+01    0.000000e+00    7.500000e-04
  100000    7.500000e+01    0.000000e+00    7.500000e-04
  320000    2.400000e+02    0.000000e+00    7.500000e-04
 1000000    7.786000e+02    0.000000e+00    7.786000e-04
 3200000    2.576800e+03    2.662311e-17    8.052500e-04
10000000    7.952200e+03    0.000000e+00    7.952200e-04
```


On the given architecure using `long double` works as fast as straightforward summing:
```
=================== Complexity of sum_simple_double ==========
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n)
     100    0.000000e+00    2.047391e-16    0.000000e+00
     320    0.000000e+00    4.205741e-16    0.000000e+00
    1000    0.000000e+00    8.750902e-16    0.000000e+00
    3200    2.000000e+00    1.130620e-15    6.250000e-04
   10000    7.000000e+00    1.462971e-15    7.000000e-04
   32000    2.400000e+01    4.898259e-15    7.500000e-04
  100000    7.500000e+01    6.592808e-15    7.500000e-04
  320000    2.400000e+02    9.472835e-15    7.500000e-04
 1000000    7.794000e+02    1.484691e-14    7.794000e-04
 3200000    2.582000e+03    3.104851e-14    8.068750e-04
10000000    7.949600e+03    6.796279e-14    7.949600e-04
```

# Accurate summing of arbitrary magnitude, signed floating point numbers.


```
=============== Complexity of sum_fast_accurate ========
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n)
     100    4.000000e+00    0.000000e+00    4.000000e-02
     320    8.000000e+00    0.000000e+00    2.500000e-02
    1000    2.160000e+01    0.000000e+00    2.160000e-02
    3200    6.500000e+01    0.000000e+00    2.031250e-02
   10000    1.992000e+02    0.000000e+00    1.992000e-02
   32000    6.360000e+02    0.000000e+00    1.987500e-02
  100000    1.979800e+03    0.000000e+00    1.979800e-02
  320000    6.308600e+03    0.000000e+00    1.971438e-02
 1000000    1.982480e+04    0.000000e+00    1.982480e-02
 3200000    6.370620e+04    0.000000e+00    1.990819e-02
10000000    2.006038e+05    0.000000e+00    2.006038e-02
```



```
=============== Complexity of sum_accurate_power ========
            Avg time of      Avg relative 
 N Items   best 5/7 runs   error all runs   time/(n)
     100    3.200000e+00    0.000000e+00    3.200000e-02
     320    4.800000e+00    0.000000e+00    1.500000e-02
    1000    9.000000e+00    0.000000e+00    9.000000e-03
    3200    3.440000e+01    0.000000e+00    1.075000e-02
   10000    1.528000e+02    0.000000e+00    1.528000e-02
   32000    5.306000e+02    0.000000e+00    1.658125e-02
  100000    1.696600e+03    0.000000e+00    1.696600e-02
  320000    5.451600e+03    0.000000e+00    1.703625e-02
 1000000    1.718820e+04    0.000000e+00    1.718820e-02
 3200000    5.512980e+04    0.000000e+00    1.722806e-02
10000000    1.726314e+05    0.000000e+00    1.726314e-02
```


