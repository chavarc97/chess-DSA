# CHESS GAME IN C

## Descripción

Generar un programa capaz de identificar todos los movimientos posibles de una pieza de ajedrez de acuerdo con el estado
actual del tablero.
Además, deberá de ordenar la lista de los movimientos en un stack, en el orden de los movimientos más valiosos de acuerdo
con el valor de las piezas que podría eliminar.

## Entradas

El programa necesitará recibir 1 archivo con los siguientes requerimientos:

* Archivo de texto con exactamente 9 líneas.
* Las primeras 8 líneas tendrán exactamente 8 caracteres, cada una separada por un espacio. Cada letra representará una de las siguientes piezas.

    | Pieza | Carácter | Valor |
    | ----- | --------- | ----- |
    | Vacío | .         | 0     |
    | Peon  | P         | 1     |
    | Alfil | P         | 3     |
    | Caballero | C     | 3     |
    | Torre | T         | 5     |
    | Reina | Q         | 8     |
    | Rey   | K         | 10    |

* La última línea contendrá 1 dato: Las coordenadas de la pieza “protagonista”, representadas por una letra y un número, conforme a la notación normal de un tablero de ajedrez.

![alt text](./images/ReadMe/image.png)
