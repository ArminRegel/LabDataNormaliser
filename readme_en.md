
# Molecule Mass Calculator

A C++ program to calculate the molar mass of chemical compounds based on their molecular formulas. Supports nested brackets, one- and two-letter element symbols, and arbitrary quantities.

---

## Example Usage

```text
> Give me a molecule!
> enter q to quit.
H2O
Molecular mass: 18.015

> Ca(OH)2
Molecular mass: 74.092

> q
```

---

## Supported Features

- **Reading atomic masses** from a CSV file (`AtomicMasses.csv`)
- **Parsing molecular formulas** including:
  - One- and two-letter elements (e.g., `H`, `He`, `Fe`)
  - Bracket expressions (e.g., `Mg(OH)2`, `Al2(SO4)3`)
  - Arbitrary nesting depth: `K4[ON(SO3)2]2`
- **Recursive calculation of molecular mass**
- Error handling for invalid formulas

---

## Build Instructions

### Requirements

- A C++17 compatible compiler (tested with `g++`, Visual Studio)

### Compile (on Linux/Mac)

```bash
g++ -std=c++17 -o MoleculeMass MoleculeMass.cpp
./MoleculeMass
```

### Compile (on Windows with g++)

```cmd
g++ -std=c++17 -o MoleculeMass.exe MoleculeMass.cpp
MoleculeMass.exe
```

Alternatively, build with Visual Studio in Debug or Release mode (uses MSVC compiler).

---

## CSV File: AtomicMasses.csv

The file `AtomicMasses.csv` must be in the same directory as the `.exe` or binary. Format:

```csv
Symbol,AtomicMass
H,1.008
He,4.002602
...
```

(The first line is a header and will be ignored.)

---

## Project Structure

```text
MoleculeMass/
├── MoleculeMass/               # Visual Studio project folder
│   ├── MoleculeMass.cpp        # Main program
│   └── AtomicMasses.csv        # Atomic mass table
├── MoleculeMass.sln            # Microsoft Visual Studio solution file
├── .gitignore                  # Git configuration
└── README.md                   # This file
```

---

## Known Limitations

- No support for charged ions like `Fe3+`
- No comprehensive chemical error checking (e.g., valence validation)

---

## Example Molecules for Testing

| Formula       | Molar Mass (g/mol) |
| ------------- | ------------------ |
| H2O           | 18.015             |
| CO2           | 44.009             |
| C6H12O6       | 180.156            |
| Ca(OH)2       | 74.092             |
| ((CH3)3C)2O   | 130.23             |

---

## About the Project

This project was created as a private programming exercise focusing on:

- Recursion and parsing
- Error handling in C++
- File processing
- Using Git and branch strategies

---

## License

This project is provided without a license for personal use only. Please do not reuse without permission.
