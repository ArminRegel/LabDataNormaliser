# LabDataNormalizer

**LabDataNormalizer** is a C++ proof-of-concept program that normalises and validates experimental lab CSV data.  
It began as a simple molar mass calculator (*MoleculeMass*), but has since evolved into a tool for **data cleaning, unit standardisation, and plausibility checking** in laboratory workflows.
The file to be cleaned up needs to be called Experiment_Raw.csv and the program assumes the data is formated the same way the example table is (same columns, comma seperated CSV).
---

## ✨ Features
- **Molecular formula parsing** with support for UTF-8 subscripts (e.g. `H₂O`).
- **Molar mass calculation** using an atomic masses table (AtomicMasses.csv).
- **Unit normalisation** for concentration values:
  - Mass concentration (g/L)  
  - Molar concentration (mol/L)
- **Auto-completion of missing values**: if one concentration (mass conc or molar conc) and molar mass is given, the program calculates the other concentration.
- **Validation**: checks that `c_molar ≈ c_mass / M` within a tolerance (maximum of 5% deviation per default), if both values are given in a table.
- **Data integrity**: unknown or unsupported formats (e.g. `G/l`) are **preserved and flagged**, never silently modified.
- Converts one date format into another

---

## Room for improvement

The program currently expects the inputfile to be named Experiment_Raw.csv and the output file will be named Experiment_Cleaned.csv. The program could instead have custom names or have a folder of input files where any file will be processed. The tolerance for concentration deviations could be made easier to change. More date formats could be supported and there could be more through checks if a correct date format is actually present in the timestamp
