# Molecule Mass Calculator

Ein C++-Programm zur Berechnung der molaren Masse chemischer Verbindungen anhand ihrer Summenformeln. Unterstützt sind verschachtelte Klammern, zwei- und einbuchstabige Elementsymbole sowie beliebige Vorkommensanzahlen.

---

## Beispielhafte Nutzung

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

## Unterstützte Features

- **Einlesen von Atommassen** aus einer CSV-Datei (`AtomicMasses.csv`)
- **Parsing von Summenformeln** inklusive:
  - Ein- und Zweibuchstaben-Elemente (z. B. `H`, `He`, `Fe`)
  - Klammerausdrücke (z. B. `Mg(OH)2`, `Al2(SO4)3`)
  - Verschachtelung beliebiger Tiefe: `K4[ON(SO3)2]2`
- **Rekursive Berechnung der Molekülmasse**
- Fehlerbehandlung bei ungültigen Formeln

---

## Build-Anleitung

### Voraussetzungen

- Ein C++17-kompatibler Compiler (getestet mit `g++`, Visual Studio)

### Kompilieren (unter Linux/Mac)

```bash
g++ -std=c++17 -o MoleculeMass MoleculeMass.cpp
./MoleculeMass
```

### Kompilieren (unter Windows mit g++)

```cmd
g++ -std=c++17 -o MoleculeMass.exe MoleculeMass.cpp
MoleculeMass.exe
```

Alternativ über Visual Studio im Debug- oder Release-Modus (nutzt MSVC-Compiler).

---

## CSV-Datei: AtomicMasses.csv

Die Datei `AtomicMasses.csv` muss im selben Verzeichnis wie die `.exe` bzw. das Binary liegen. Format:

```csv
Symbol,AtomicMass
H,1.008
He,4.002602
...
```

(Die erste Zeile ist eine Kopfzeile und wird ignoriert.)

---

## Projektstruktur

```text
MoleculeMass/
├── MoleculeMass/               # Visual-Studio-Projektordner
│   ├── MoleculeMass.cpp        # Hauptprogramm
│   └── AtomicMasses.csv        # Tabelle mit Atommassen
├──  MoleculeMass.sln           # Microsoft Visual Studio Projektdatei
├── .gitignore                  # Git-Konfiguration
└── README.md                   # Diese Datei
```

---

## Bekannte Einschränkungen

- Keine Unterstützung für geladene Ionen wie `Fe3+`
- Kein vollständiges chemisches Fehlerhandling (z. B. Valenzprüfung)

---

## Beispiel-Moleküle zum Testen

| Formel        | Molarität (g/mol) |
| ------------- | ----------------- |
| H2O           | 18.015            |
| CO2           | 44.009            |
| C6H12O6       | 180.156           |
| Ca(OH)2       | 74.092            |
| ((CH3)3C)2O   | 130.23            |

---

## Über das Projekt

Dieses Projekt entstand als private Programmierübung zur vertieften Beschäftigung mit:

- Rekursion und Parsing
- Fehlerbehandlung in C++
- Dateiverarbeitung
- Umgang mit Git und Branch-Strategien


---

## Lizenz

Dieses Projekt steht ohne Lizenz unter persönlicher Nutzung. Bitte nicht ohne Zustimmung weiterverwenden.

