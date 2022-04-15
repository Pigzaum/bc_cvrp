# Capacitated Vehicle routing problem (CVRP) Branch-and-cut algorithm using the CVRPSEP package

A C++ implementation of the CVRP three-index model [[1](#references)] branch-and-cut algorithm using the Gurobi's API and CVRPSEP package [[2](#references)].

Let $G = (V, E)$ be an undirected graph, $K$ a set of vehicles with capacity of $Q$, and let vertex 0 be the depot and vertices $V' = V \setminus \{0\}$ be the customers. Consider that there is a demand $d_i$ for each $i \in V'$. The undirected three-index CVRP model can be defined as below [[1](#references)]:

$$
  \begin{align}
    \min & \sum\limits_{(i, j) \in E} c_{ij} \sum\limits_{k \in K} x_{ij}^k \\
    \text{subject to} & \nonumber \\
    & \sum\limits_{k \in K} y_{i}^k = 1, & \forall i \in V', \\
    & \sum\limits_{k \in K} y_{0}^k = K, \\
    & \sum\limits_{(i, j) \in E} x_{ij}^k = 2y_i^k, & \forall i \in V, \forall k \in K, \\
    & \sum\limits_{i \in V} d_i y_{i}^k \leq Q, & \forall k \in K,\\
    & \sum\limits_{(i, j) \in S} x_{ij}^k \leq |S| - 1, & \forall S \subseteq V', |S| \geq 2, \forall k \in K,\\
    & y_{i}^{k} \in \{0, 1\}, & \forall i \in V, \forall k \in K, \\
    & x_{ij}^{k} \in \{0, 1\}, & \forall i, j \in V', \forall k \in K,\\
    & x_{0j}^{k} \in \{0, 1 , 2\}, & \forall j \in V', \forall k \in K.
  \end{align}
$$

## Prerequisites

* CMake.

* C++17 compiler or an early version.

* Boost library (program_options)

* GUROBI solver (9 or an early version). Academics can obtain it via this [link](https://www.gurobi.com/downloads/gurobi-optimizer-eula/#Reg "Gurobi's register page").

## Compile and run instructions

Go to the source code folder and to compile type:

```sh
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

for the debug version or

```sh
cmake -H. -Bbuild
cmake --build build
```

for the release version.

To run with a configuration file:

```sh
$ ./build/bc_cvrp -f [configuration file path]
```

See the "example.cfg" file at the "input" folder for an example of the input configuration file and 

```sh
$ ./build/bc_cvrp --help
```

to see usage.

## References

**[\[1\] P. Toth and D. Vigo. The Vehicle Routing Problem, Discrete Mathematics and Applications, SIAM, 2002](https://epubs.siam.org/doi/book/10.1137/1.9780898718515)**

**[\[2\] J. Lysgaard, A.N. Letchford and R.W. Eglese. A New Branch-and-Cut Algorithm for the Capacitated Vehicle Routing Problem, Mathematical Programming, vol. 100 (2), pp. 423-445](https://pubsonline.informs.org/doi/10.1287/trsc.1060.0188)**