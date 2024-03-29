# Information about Databases

[ATC](../README.md) program is equipped with two databases __Thermo__ and __HSC__. Both are in sqlite3 format, which are located in the databases directory.

## Thermo database

Thermodynamic data are taken from the old program [Thermo](http://www.ism.ac.ru/). Most of the data in this database are taken from [Glu]. Temperature dependencies in Thermo database presented by the Free Enthalpy Function FEF. The database consists of the following tables:

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

Thermodynamic functions can be calculated using the following formulas.

$$ F^{\circ}(T)\left[\frac{J}{mol \thinspace K}\right]=\varphi_1 + \varphi_2 \ln x + \varphi_3 x^{-2} + \varphi_4 x^{-1} + \varphi_5 x + \varphi_6 x^2 + \varphi_7 x^3 $$

$$ S^{\circ}(T)\left[\frac{J}{mol \thinspace K}\right]=\varphi_1 + \varphi_2 (1+\ln x) - \varphi_3 x^{-2} + 2 \varphi_5 x + 3 \varphi_6 x^2 + 4 \varphi_7 x^3 $$

$$ H^{\circ}(T)\left[\frac{kJ}{mol}\right]=H^{\circ}(0) + 10(\varphi_2 x -2 \varphi_3 x^{-1} - \varphi_4 + \varphi_5 x^2 + 2 \varphi_6 x^3 + 3 \varphi_7 x^4) $$

$$ C_p(T)\left[\frac{J}{mol \thinspace K}\right]= \varphi_2 +2 \varphi_3 x^{-2} + 2 \varphi_5 x + 6 \varphi_6 x^2 + 12 \varphi_7 x^3 $$

$$ G^{\circ}(T)\left[\frac{kJ}{mol}\right]=H^{\circ}(0) -T \cdot F^{\circ}(T)\left[\frac{J}{mol \thinspace K}\right] \cdot 10^{-3} $$

$$ \text{where} \space x = T[K] \cdot 10^{-4}. $$


## HSC database

HSC database are made by program [HSCtoSQL (HSC to SQLite database converter)](https://github.com/alexonemore/hsctosql). The HSC database may not be supplied with ATC program to avoid licensing problems. HSC database consists of the same tables as Thermo database. Temperature dependencies in HSC database presented by the Heat capacity function $C_p$. Thermodynamic functions can be calculated using the following formulas. At standard state, the superscript ° is omitted.

The extended Kelley equation

$$
C_p(T)=A+BT\cdot 10^{-3}+CT^{-2}\cdot 10^{5} + DT^2 \cdot 10^{-6} +ET^{-3} \cdot 10^8 + FT^3 \cdot 10^{-9}
$$

$$
\frac{C_p(T)}{T}=AT^{-1}+B\cdot 10^{-3}+CT^{-3}\cdot 10^{5} + DT \cdot 10^{-6} +ET^{-4} \cdot 10^8 + FT^2 \cdot 10^{-9}
$$

$$
\int{C_p\left(T\right)dT}=A\cdot T+2\cdot{10}^{-3}\cdot B\cdot T^2-{10}^5\cdot C\cdot T^{-1}+\frac{1}{3}\cdot{10}^{-6}\cdot D\cdot T^3-5\cdot{10}^7\cdot E\cdot T^{-2}+2.5\cdot{10}^{-10}\cdot F\cdot T^4
$$

$$
\int{\frac{C_p\left(T\right)}{T}dT}=A\cdot\ln{T}+{10}^{-3}\cdot B\cdot T-5\cdot{10}^4\cdot C\cdot T^{-2}+5\cdot{10}^{-7}\cdot D\cdot T^2-\frac{1}{3}\cdot{10}^8\cdot E\cdot T^{-3}+\frac{1}{3}\cdot{10}^{-9}\cdot F\cdot T^3
$$

$$
H(T)=H(298.15K)+\int\limits_{298.15K}^{T_1}C_{p1}(T)\thinspace dT + \Delta H_{1} +\int\limits_{T_1}^{T_2}C_{p2}(T)\thinspace dT + \Delta H_{2} + \cdots
$$

$$
S(T) = S(298.15K)+\int\limits_{298.15K}^{T_1}\frac{C_{p1}(T)}{T}\thinspace dT + \frac{\Delta H_{1}}{T_{1}} +\int\limits_{T_1}^{T_2}\frac{C_{p2}(T)}{T}\thinspace dT + \frac{\Delta H_{2}}{T_{2}} + \cdots
$$

$$G^{\circ}(T)=H^{\circ}(T)-TS^{\circ}(T)$$

$$G^{\circ}(T)=H^{\circ}(0)-TF^{\circ}(T)$$

## Litetarure
[Glu]: Glusko, V. P.; Gurvich, L. V.; Bergman, G. A.; Veitz, I. V.; Medvedev, V. A.; Khachkuruzov, G. A.; Jungman, V. S. Thermodynamic Properties of Pure Substances, Vols. I-IV, 3rd ed.; Nauka: Moscow, 1982; in Russian. Gurvich, L. V.; Veyts, I. V.; Alcock, C. B. Thermodynamic Properties of Individual Substances, Vols. I and II, 4th ed.; Hemisphere: New York, 1989. Gurvich, L. V.; Veyts, I. V.; Alcock, C. B. Thermodynamic Properties of Individual Substances, Vol. III, 4th ed.; Begell House: New York, 1989. Gurvich, L. V.; Veyts, I. V.; Alcock, C. B. Thermodynamic Properties of Individual Substances, Vol. IV, 4th ed.; Begell House: New York, in press. Gurvich, L. V.; Iorish, V. S.; Chekhovskoi, D. V.; Yungman, V. S. IVTANTHERMO-A Thermodynamic Database and Software System for the Personal Computer; NIST Special Database 5; National Institutes of Standards and Technology: Gaithersburg, MD, 1993. These tables are also carefully assessed data. A detailed text describes the sources of the data and a level of accuracy is given to each substance. Listed above are the Russian version, the English translation of Vols. I-III (Vol. IV is expected out soon), and the electronic version, which has the option of selecting the format of the output: JANAF form, TPIS form, or functional form of Cp°, FEF(0), FEF(298), H°(T) - H°(0), H°(T) - H°(298), and S°(T).


