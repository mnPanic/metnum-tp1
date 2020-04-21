# TODO

## metodos

- CMM (colley matrix mult)
- WP (win %)
- 3er metodo: [**true skill**](https://en.wikipedia.org/wiki/TrueSkill) u otro

## cpp (core)

Arrancar por single file

- parser: Leer un tsv y generar una matriz
- Implementar EG
- Implementar CMM
- Implementar algoritmo alternativo
- Serializar el output

```
main(type, f_partidos, f_ranking_out) {
    matrix = gen_matrix(read(fpartidos), type)
    result = solve(matrix)
    serialize(result, f_ranking_out)
}
```

## teoria

1. *¿Qué tipo de matriz resulta C?*
   estrictamente diagonal dominante
2. *¿Cómo se garantiza la aplicabilidad de EG sin intercambio de filas o columnas?*
   tiene fact. LU pues es estrictamente diagonal dominante
3. *¿Qué se puede decir sobre la estabilidad de los cálculos?*
   ??

- Justificar por que siempre se tiene solucion para CMM (1ra teorica)

## informe

[info](https://campus.exactas.uba.ar/pluginfile.php/163805/course/section/22657/pautas.pdf)

### experimentacion

python & jupyter notebook

- TODO: plantear experimentos

#### cuantitativo

tranqui

#### cualitativo

1. TODO: elegir datos

Leer paper antes de 2 y 3

2. CMM justo?
3. Minimizar victorias



