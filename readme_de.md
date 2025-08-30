# LabDataNormalizer

**LabDataNormalizer** ist ein C++-Proof-of-Concept-Programm zur Normalisierung und Validierung experimenteller Labor-CSV-Daten.  
Es begann als einfacher Molmassen-Rechner (*MoleculeMass*) und hat sich zu einem Tool für **Datenbereinigung, Einheiten-Standardisierung und Plausibilitätsprüfungen** in Labor-Workflows entwickelt.  
Die zu bereinigende Datei muss **Experiment_Raw.csv** heißen; das Programm geht davon aus, dass die Daten wie in der Beispiel­tabelle formatiert sind (gleiche Spalten, kommaseparierte CSV).

---

## ✨ Funktionen
- **Parsing chemischer Formeln** mit Unterstützung für UTF-8-Tiefstellungen (z. B. `H₂O`).
- **Berechnung der Molmasse** mithilfe einer Tabelle mit Atom­massen (`AtomicMasses.csv`).
- **Einheiten-Standardisierung** für Konzentrationswerte:
  - Massenkonzentration (g/L)  
  - Stoffmengenkonzentration (mol/L)
- **Automatisches Ergänzen fehlender Werte**: Wenn eine der beiden Konzentrationen (Massen- oder Stoffmengen­konzentration) **und** die Molmasse gegeben sind, berechnet das Programm die jeweils andere Konzentration.
- **Validierung**: Prüft, dass `c_molar ≈ c_mass / M` innerhalb einer Toleranz liegt (standardmäßig max. **5 %** Abweichung), sofern beide Konzentrationswerte in der Tabelle vorhanden sind.
- **Datenintegrität**: Unbekannte oder nicht unterstützte Formate (z. B. `G/l`) werden **beibehalten und markiert** – niemals stillschweigend verändert.
- **Datumsformat-Konvertierung** zwischen vorgegebenen Formaten.

---

## Verbesserungspotenzial

Aktuell erwartet das Programm die Eingabedatei mit dem Namen **Experiment_Raw.csv**; die Ausgabedatei heißt **Experiment_Cleaned.csv**.  
Mögliche Erweiterungen:
- frei wählbare Dateinamen oder ein Eingabeordner, in dem **alle** Dateien verarbeitet werden,
- leichter konfigurierbare Toleranz für Abweichungen der Konzentrationen,
- Unterstützung weiterer Datumsformate sowie **gründlichere Prüfungen**, ob im Zeitstempel tatsächlich ein korrektes Datumsformat vorliegt.
