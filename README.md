# metnum-tp1

TP1 de Métodos Numéricos

## Estructura del TP

```bash
$ tree -d
.
└── tp1
    ├── data                # Datos para los experimentos
    ├── docs                # Docs provistas por la cátedra y el informe
    ├── exp                 # Experimentos
    │   └── img
    └── src                 # Implementación
        ├── scripts         # Scripts provistos por la catedra
        ├── tests           # Tests provistos por la catedra
        └── tools           # Herramientas para manipulacion de datos y simulador
```

## Implementacion

Se realizó la implementación de CMM (con eliminación gausseana y backwards
substitution) y WP, todo en un *single file*, `tp1.cpp`.

### Compilar

Para compilarlo, basta con:

```bash
g++ tp1.cpp -o tp
```

Luego se puede utilizar el binario `tp` tal como se especifica en el enunciado,

```bash
./tp {in} {out} {method}

# Por ejemplo
./tp atp_matches_2015.dat.norm atp_matches.out 0
```

### Nota sobre datos reales - ATP

Como en todos los datos de entrada, de haber N equipos, sus nombres eran de 0 a
N, entonces el TP fue implementado al rededor de eso.

Lamentablemente, este no es el caso de los datos de ATP, que tienen nombres de
equipos que son más grandes que el total. Para poder procesarlos, se agrega un
script de *preprocessing* de ellos,
[`src/tools/atp_preproc.py`](tp1/src/tools/atp_preproc.py) el cual no hace mas
que mapear los nombres a numeros de 0 a N.

## Experimentacion

Para poder reproducir los experimentos, es necesario hacer lo siguiente:

### Setup

1. [Compilar el tp](#compilar)

2. Entrar a un *virtual environment*

   ```bash
   # Por ejemplo, con virtualenvwrapper
   mkvirtualenv metodos
   workon metodos
   ```

3. Instalar los requerimientos

    ```bash
    pip3 install -r requirements.txt
    ```

4. Entrar a jupyter (el cual se deberia haber instalado como requerimiento)

    ```bash
    jupyter-notebook
    ```

### Notebooks

Para realizar la experimentacion, se armaron distintos notebooks, los cuales
se encuentran en el directorio [`exp/`](tp1/exp)

- `exp-reales.ipynb`: Experimentos con datos reales
- `heuristicas.ipynb`: Experimentos sobre diferentes heuristicas de minimizacion
  de victorias. En este se utiliza el [*simulador*](tp1/exp/simulator.py) para
  probar cada una. Como naturalmente es random, se deja comentada la seed
  que fue utilizada para los resultados de forma tal que sean reproducibles.
- `errores_precision.ipynb`: Experimentos de perdida de precision
- `justo.ipynb`: Experimentos de si CMM es justo
