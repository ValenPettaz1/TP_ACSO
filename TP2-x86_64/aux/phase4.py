from itertools import product
import string

# Arreglo usado en el binario (0x4cde20)
array = [2, 13, 7, 14, 5, 10, 6, 15, 1, 12, 3, 4, 11, 8, 16, 9]

# Caracteres válidos: alfanuméricos visibles
caracteres_validos = string.printable.strip()  # sin \n, \t, etc.

def bits_bajos(c):
    return ord(c) & 0xF

def valor(c):
    return array[bits_bajos(c)]

# Enumerar todas las combinaciones posibles de 6 caracteres
for combinacion in product(caracteres_validos, repeat=6):
    suma = sum(valor(c) for c in combinacion)
    if suma == 55:
        clave = ''.join(combinacion)
        print(f"Clave candidata: {clave}")
        break
