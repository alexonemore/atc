![atc logo](images/logo_atc.svg) 

# ATC - Adiabatic Temperature Calculator

## Description

ATC is a high quality, visual, open source tool for thermodynamic calculations.

## Features

* Tabulate thermodynamic functions for substances from the database
* Plot thermodynamic functions for substances from the database
* Calculate thermodynamic equilibrium for any composition and any temperature at standard pressure
* Calculate adiabatic temperature for any composition at standard pressure
* Calculate and plot thermodynamic equilibrium or adiabatic temperature for a range of compositions
	* Plot 2D graph
	* Plot 2D heat map
	* Plot 3D graph
* Temperature dependencies in the database can be presented in two different ways
	* By the Free Enthalpy Function FEF
	* By the Heat Capacity Cp

## Optimization algorithm

To minimize the Gibbs energy function, the following methods are used:

* Augmented Lagrangian method
* Sequential Quadratic Programming

Optimization of the objective function is performed using the [NLopt](http://github.com/stevengj/nlopt) library.

## Plotting

Plotting is provided by the [QCustomPlot](https://www.qcustomplot.com/) library.

## Database structure

All data is stored in the sqlite3 database, which has the following structure:

### Structre of Free Enthalpy Function database

1. Element compositions of substances

	|composition_id |sub_id |element_id |amount |
	|---------------|-------|-----------|-------|
	|1              |1      |47         |1.0    |
	|2              |2      |47         |1.0    |
	|3              |3      |47         |1.0    |
	|...            |...    |...        |...    |

2. Coefficients of thermodynamic functions of substances

	|coef_id |sub_id |T_min  |T_max   |f1      |f2      |f3          |f4           |f5        |f6        |f7       |
	|--------|-------|-------|--------|--------|--------|------------|-------------|----------|----------|---------|
	|1       |1      |298.15 |1235.08 |97.8052 |22.693  |0.000264    |0.115144     |34.605    |0.0       |0.0      |
	|2       |2      |298.15 |4000.0  |126.928 |33.4    |0.0         |-0.186133    |0.0       |0.0       |0.0      |
	|3       |3      |298.15 |3000.0  |225.102 |20.7809 |1.15504e-06 |-0.000189049 |0.0561557 |-0.126879 |0.131983 |
	|...     |...    |...    |...     |...     |...     |...         |...          |...       |...       |...      |

3. Substances

	|sub_id |state_id |name |alt_name |number_of_elements |H0      |T_min  |T_max   |ranges |weight  |source |
	|-------|---------|-----|---------|-------------------|--------|-------|--------|-------|--------|-------|
	|1      |3        |Ag1  |         |1                  |-5.745  |298.15 |1235.08 |1      |107.868 |2349   |
	|2      |2        |Ag1  |         |1                  |-5.745  |298.15 |4000.0  |1      |107.868 |2349   |
	|3      |1        |Ag1  |         |1                  |278.703 |298.15 |10000.0 |2      |107.868 |2350   |
	|...    |...      |...  |...      |...                |...     |...    |...     |...    |...     |...    |

4. Elements

	|element_id |symbol |name     |weight   |density |
	|-----------|-------|---------|---------|--------|
	|1          |H      |Hydrogen |1.00794  |0.0     |
	|2          |He     |Helium   |4.002602 |0.0     |
	|3          |Li     |Lithium  |6.941    |0.53    |
	|...        |...    |...      |...      |...     |

5. State

	|state_id |symbol |name   |
	|---------|-------|-------|
	|1        |G      |gas    |
	|2        |L      |liquid |
	|3        |S      |solid  |

Thermodynamic data are taken from the old program [Thermo](http://www.ism.ac.ru/). Atomic weight and density are taken from [Wikipedia 1](https://en.wikipedia.org/wiki/List_of_chemical_elements) and [Wikipedia 2](https://en.wikipedia.org/wiki/Standard_atomic_weight).

### Structre of Heat Capacity database

1. TODO

## Adiabatic temperature

TODO

## Compiling

+ Clone and update NLopt

	```shell
	git clone git@github.com:alexonemore/atc.git
	cd atc
	git remote add nlopt git@github.com:stevengj/nlopt.git
	git subtree pull --prefix=libs/nlopt nlopt master --squash
	```

+ Compiling on Windows by MSVC
	+ Install Visual Studio that contains MSVC compiler
	+ Install Qt 5.15.2 msvc2019_x library (or later Qt5 LTS version)
	+ Run the appropriate script from the build directory

+ Compiling on Windows by MINGW
	+ Install Qt MinGW 8.1.0 compiler
	+ Install Qt 5.15.2 mingw81_x library
	+ Run the appropriate script from the build directory

+ Compiling on Linux
	+ Install qt5 libraries and gcc compiler
		```shell
		apt install qtbase5-dev qtbase5-dev-tools gcc g++ cmake make
		```
	+ Run the appropriate script from the build directory

## License

ATC is licensed under the GNU General Public License Version 3.
Modification or redistribution is permitted under the conditions of these license.

## Mics

1. Add nlopt library to project
	
	```shell
		git remote add nlopt git@github.com:stevengj/nlopt.git
		git subtree add --prefix=libs/nlopt nlopt master --squash
		git subtree pull --prefix=libs/nlopt nlopt master --squash
	```

2. Fix qcustomplot for qt6:
	https://www.qcustomplot.com/index.php/support/forum/2380

3. Form for logo
	https://squircley.app/

