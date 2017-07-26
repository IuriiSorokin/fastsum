# Complexity of the original algorithms.

Let `N` -- number of elements in the original vector. 
#### `sum1`
The algorithm performs the sorting `N-1` times, and the number of items being sorted, changes from `N` to `2`. STL guarantees the complexity of sorting ` n*log(n)`. The sorting algorithm is not aware of the fact that from the second iteration on the vector is already almost sorted (only the first element is not on it's place), and the sorting could be accomplished with `O(n)` comparisons and one move of a memory block (also `O(n)`). It looks like the actual complexity of the sorting is indeed `n*log(n)`. 

So the number of operations, that the summing algorithm will perform, is roughly proportional to:
`Integral( n*log(n) ) dn ` from `1` to `N`,  which after neglecting the lower terms and constants, gives the complexity of: `N^2 * log(N)`.
