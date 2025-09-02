# LabDataNormalizer

**LabDataNormalizer** is a C++ proof-of-concept program that normalises and validates experimental lab CSV data.  
It began as a simple molar mass calculator (*MoleculeMass*), and evolved into a tool for **data cleaning, unit standardisation, and plausibility checking** in laboratory workflows.

> **Assumptions:** The input file must be named `Experiment_Raw.csv`. The program expects the same columns and order as in the provided example table (comma-separated CSV).

---

## ✨ Features
- **Molecular formula parsing** with UTF-8 subscripts (e.g. `H₂O`).
- **Molar mass calculation** using an atomic masses table (`AtomicMasses.csv`).
- **Unit normalisation** for concentration values:
  - Mass concentration (g/L)
  - Molar concentration (mol/L)
- **Auto-completion of missing values**: if **one concentration** (mass or molar) **and** the **molar mass** are given, the other concentration is calculated.
- **Validation**: checks `c_molar ≈ c_mass / M` within a tolerance (default **5%**) when both concentrations are provided.
- **Data integrity**: unknown or unsupported formats (e.g. `G/l`) are **preserved and flagged** — never silently modified.
- **Timestamp normalisation**: converts the timestamp column to a consistent format.
- **Portable release**: runs inside Visual Studio and as a standalone `.exe`.

---


## 🧭 Known limitations / room for improvement
- File names are fixed (`Experiment_Raw.csv` → `Experiment_Cleaned.csv`).  
  Future improvement: custom file names or batch processing of all files in a folder.
- Tolerance is currently set via code (default 5%).  
  Future improvement: make tolerance configurable via CLI.
- Date handling currently just converts one date format into another.  
  Future improvement: support more formats and stricter validation of timestamp correctness.

---

## 🛠️ Build (Visual Studio)
1. Open the solution (`MoleculeMass.sln`).
2. Set **Configuration** = `Release`, **Platform** = `x64`.
3. Compile (F5) or build the solution; the executable is produced under `x64\Release\` 