# Optimization in Car Factory Production
The project involves the creation of optimization algorithms, including backtracking, greedy algorithms, and metaheuristic approaches, aimed at enhancing the efficiency of production processes in car factories.

## Nomenclature:
- `C`: Number of cars
- `M`: Improvements
- `K`: Classes (subset of improvements)
- `n_e`: Set of `n` consecutive cars at station `e`
- `c_e`: At most `c_e` of the `n_e` require the installation of the improvement

## Penalties:
Non-compliance with `n_e` and `c_e` constraints results in penalties. The total penalty is the sum of the penalties from all stations. Exceeding `c_e` incurs a penalty of `k-c_e` euros, where `k` is the number of consecutive cars that required the improvement and exceeded the allowed maximum at that station.
- Note: Incomplete car windows (start and end of the sequence) are also considered.

## Input Format:
- 3 strictly positive integers: `C, M, K`. Number of cars, improvements, and classes.
- `M` strictly positive integers with the `c_e` for each improvement (improvements are named {0...n} and are made at different stations).
- `M` strictly positive integers indicating `n_e` (also for each improvement)
- `K` lines, each containing: an integer identifying the class, an integer indicating the number of cars to produce in this class, and `M` 0s and 1s indicating whether the class requires an improvement or not {0...n}

## Input:
The program will receive two command-line arguments: 
- the name of the input file
- the name of the output file

## Example of Input and Output
```
Input:          Output:
10 5 3          3 2.3
1 1 1 2 1       0 1 0 1 2 0 2 0 2 1 
2 2 2 3 2
0 4 1 1 0 0 1
1 3 0 1 0 1 0
2 3 0 0 1 0 0

```
### Authors
- Ignacio Gris Martín
- Marc Camps Garreta 
