# Complexity of the original algorithms.

Let `N` be number of elements in the original vector. 
#### `sum1`
The algorithm performs the sorting `N-1` times, and the number of items being sorted changes from `N` to `2`. STL guarantees the complexity of sorting ` n*log(n)`. The sorting algorithm is not aware of the fact that from the second iteration on the vector is already almost sorted (only the first element is not on it's place), and the sorting could be accomplished with `O(n)` comparisons and one move of a memory block (also `O(n)`). It looks like the actual complexity of the sorting is indeed `n*log(n)` (see benchmarks below). 
The number of operations, that the summing algorithm will perform, is roughly proportional to:
`Integral( n*log(n) ) dn ` from `1` to `N`,  which after neglecting the lower terms and constants, gives the complexity of: `N^2 * log(N)`.

#### `sum2`
The algorithm performs `N-1` partial sortings. The number of elements below the median is always `2`. The number of elements above (and including) the median changes from `N-2` to `0`. The guranteed complexity of the soring is 
`n_above * log( n_below )`.
Hence, the complexity of the summing algorithm is 
`Integral( n_above * log( const )  ) ~ N^2`

#### `sum3`
There are two time-consuming actions done: first the multiset is constructed, and then the summing is done. The complexity of constructing the multiset is `N*log(N)`. This estimate considers only arranging the elements in the underlying data structur (RB-tree), and does not take the memory allocation into account. 
In the summing part the most time-consuming operations are insertion and erasure of the elements. For both the complexity is `log(n)` (where `n` is the size of the multiset). The complexity of the summing algorithm is therefore `N*log(N)`.



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


