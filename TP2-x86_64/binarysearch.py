def binary_search(arr, target):
    left = 0
    right = len(arr) - 1
    steps = 0
    while left <= right:
        steps += 1
        mid = (left + right) // 2
        if arr[mid] == target:
            return steps
        elif arr[mid] < target:
            left = mid + 1
        else:
            right = mid - 1
    return steps

def main():
    try:
        with open("/Users/valenpettazi/Desktop/UDeSA/3º (2025)/ACSO/TP_ACSO/TP2-x86_64/bomb45/palabras.txt", "r", encoding="utf-8") as f:
            # Lee cada línea eliminando espacios y saltos de línea.
            words = [line.strip() for line in f if line.strip()]
    except FileNotFoundError:
        print("El archivo 'palabras.txt' no se encontró.")
        return

    # Asegurarse que la lista esté ordenada para la búsqueda binaria.
    words.sort()

    found = False
    for word in words:
        steps = binary_search(words, word)
        if steps < 10:
            print(f"La primera palabra que se encontró en menos de 10 pasos es '{word}' (pasos: {steps}).")
            found = True
            break

    if not found:
        print("No se encontró ninguna palabra que se localizara en menos de 10 pasos.")

if __name__ == "__main__":
    main()
