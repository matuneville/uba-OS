import sys

def generate_alphabet_sequence(n):
    sequence = []
    for i in range(n):
        letters = ""
        num = i
        while num >= 0:
            letters = chr(num % 26 + 65) + letters
            num = num // 26 - 1
        sequence.append(letters)
    return sequence

def write_sequence_to_file(filename, sequence):
    with open(filename, 'w') as file:
        file.write('\n'.join(sequence))

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Uso: {sys.argv[0]} <numero_total_combinaciones>")
        sys.exit(1)

    try:
        total_combinations = int(sys.argv[1])
    except ValueError:
        print("Error: El argumento debe ser un número entero.")
        sys.exit(1)

    alphabet_sequence = generate_alphabet_sequence(total_combinations)
    write_sequence_to_file('alphabet_sequence.txt', alphabet_sequence)

    print(f'Secuencia generada y guardada en "alphabet_sequence.txt" con {total_combinations} elementos.')
