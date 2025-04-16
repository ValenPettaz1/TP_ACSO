def cuenta_recursiva(arr, target, low, high, counter):
    # Incrementa contador en cada llamada recursiva
    counter[0] += 1
    
    # Si contador > 11, la bomba explotaría
    if counter[0] > 11:
        return "BOMB!"
    
    # Cálculo del punto medio como en el assembly: ((low XOR high) >> 1) + (low AND high)
    mid = ((low ^ high) >> 1) + (low & high)
    
    # Comparación de strings (similar a lo que haría el strcasecmp)
    if arr[mid] == target:
        return counter[0]  # Encontró la palabra
    elif arr[mid] < target:
        if high > mid:
            # Búsqueda en mitad derecha
            return cuenta_recursiva(arr, target, mid + 1, high, counter)
    else:  # arr[mid] > target
        if low < mid:
            # Búsqueda en mitad izquierda
            return cuenta_recursiva(arr, target, low, mid - 1, counter)
    
    # Retorna el contador actual si no puede seguir buscando
    return counter[0]

def main():
    try:
        with open("/Users/valenpettazi/Desktop/UDeSA/3º (2025)/ACSO/TP_ACSO/TP2-x86_64/bomb45/palabras.txt", "r", encoding="utf-8") as f:
            words = [line.strip() for line in f if line.strip()]
    except FileNotFoundError:
        print("El archivo 'palabras.txt' no se encontró.")
        return

    # Se busca la palabra válida de menor longitud que requiera entre 7 y 11 pasos
    solucion_valida = None  # Guardará la tupla (steps, word)
    
    for word in words:
        counter = [0]
        steps = cuenta_recursiva(words, word, 0, len(words) - 1, counter)
        
        if isinstance(steps, str):
            continue  # Salta palabras que harían explotar la bomba
        
        if 6 < steps <= 11:  # Condición para ser válida
            if solucion_valida is None or len(word) < len(solucion_valida[1]):
                solucion_valida = (steps, word)

    if solucion_valida:
        print("Entrada válida para desactivar la bomba:")
        print(f"{solucion_valida[0]} {solucion_valida[1]}")
    else:
        print("No se encontraron soluciones válidas.")

if __name__ == "__main__":
    main()
