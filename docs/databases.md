# Information about Databases

* Temperature dependencies in the database can be presented in two different ways
	* By the Free Enthalpy Function FEF
	* By the Heat Capacity Cp

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

