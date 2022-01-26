# Capacitated Vehicle routing problem (CVRP) Branch-and-cut algorithm using the CVRPSEP package

A C++ implementation of the CVRP three-index model [[1](#references)] branch-and-cut algorithm using the Gurobi's API and CVRPSEP package [[2](#references)].

## Prerequisites

* CMake.

* C++17 compiler or an early version.

* GUROBI solver (9 or an early version). Academics can obtain it via this [link](https://www.gurobi.com/downloads/gurobi-optimizer-eula/#Reg "Gurobi's register page").

## Compile and run instructions

Go to the source code folder and to compile type:

```sh
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

for the debug version or simply

```sh
cmake -H. -Bbuild
cmake --build build
```

for the release version.

To run with a configuration file:

```sh
$ ./build/bc_cvrp -f [configuration file path]
```

See the "example.cfg" file at the "input" folder for an example of the input configuration file.

## References

**[\[1\] P. Toth and D. Vigo. The Vehicle Routing Problem, Discrete Mathematics and Applications, SIAM, 2002](https://epubs.siam.org/doi/book/10.1137/1.9780898718515)**

**[\[2\] J. Lysgaard, A.N. Letchford and R.W. Eglese. A New Branch-and-Cut Algorithm for the Capacitated Vehicle Routing Problem, Mathematical Programming, vol. 100 (2), pp. 423-445](https://pubsonline.informs.org/doi/10.1287/trsc.1060.0188)**