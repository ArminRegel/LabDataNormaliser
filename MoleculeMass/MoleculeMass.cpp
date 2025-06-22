

#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

void loadAtomicMassesFromCSV();
float calculateMolecularMass(std::string molecularFormula);

std::unordered_map<std::string, float> atomicMasses;
float calculateAtomicMass(std::string atomicSymbol);

const std::string ATOMIC_MASSES_FILEPATH = "AtomicMasses.csv";



int main()
{
    loadAtomicMassesFromCSV();
    /*
    atomicMasses["H"] = 1.008;
    atomicMasses["Na"] = 22.990;
    atomicMasses["O"] = 15.999;
    atomicMasses["C"] = 12.011;
    atomicMasses["He"] = 4.002602;
    */

    std::cout << "Give me a molecule!\n";
    std::string molecularFormula;
    std::cin >> molecularFormula;
    std::cout << calculateMolecularMass(molecularFormula);

}



float calculateMolecularMass(std::string molecularFormula) {
    if (molecularFormula.length() <= 0) return 0;
    if (molecularFormula.length() == 1) return calculateAtomicMass(molecularFormula);

    float atomicMass;
    int atomAmmountIndex;

    //Figure out Atomic Mass
    if (molecularFormula.at(0) <= 'A' || molecularFormula.at(0) >= 'Z') throw "molecularFormulaBroken";



    if (molecularFormula.at(1) >= 'A' && molecularFormula.at(1) <= 'Z') return calculateAtomicMass(molecularFormula.substr(0,1)) + calculateMolecularMass(molecularFormula.substr(1));


    else if (molecularFormula.at(1) >= '1' && molecularFormula.at(1) <= '9') {//if number
        atomicMass = calculateAtomicMass(molecularFormula.substr(0, 1));
        
        atomAmmountIndex = 1;
    }

    else {
        
        atomicMass = calculateAtomicMass(molecularFormula.substr(0, 2));

        if (molecularFormula.length() <= 2 || molecularFormula.at(2) < '0' || molecularFormula.at(2) > '9') return atomicMass + calculateMolecularMass(molecularFormula.substr(2)); ;//if (no number after Atomic Symbol)
        atomAmmountIndex = 2; 
    }
        

    //Figure out Ammount of Atoms, right now this part of the code will only be reached, if a number for the ammount of atoms exists
    int atomAmmount = 0;
    //std::cout << "atomicMass = " << atomicMass << "\n";

    while (atomAmmountIndex < molecularFormula.length() && molecularFormula.at(atomAmmountIndex) >= '0' && molecularFormula.at(atomAmmountIndex) <= '9') {
        atomAmmount *= 10;
        atomAmmount += molecularFormula.at(atomAmmountIndex) - '0'; // character - '0' turns it into int
        ++atomAmmountIndex;
    }

    return atomicMass * atomAmmount + calculateMolecularMass(molecularFormula.substr(atomAmmountIndex));

}

float calculateAtomicMass(std::string atomicSymbol) {

    auto iterator = atomicMasses.find(atomicSymbol);
    
    if (iterator != atomicMasses.end()) {
        return iterator->second;
    }
    else {
        throw "atomNotFound";
    }
}
 

void loadAtomicMassesFromCSV() {


    std::ifstream file(ATOMIC_MASSES_FILEPATH);

    if (!file.is_open()) {
        throw "atomicMassesTableNotFound";
    }

    std::string line;

    std::getline(file, line);//Skip header

    while (std::getline(file, line)) {
        

        int delimiterPos = line.find(',');

        if (delimiterPos == std::string::npos) throw "atomicMassesDelimeterMissing";

        std::string atomName = line.substr(0, delimiterPos);
        std::string atomMassString = line.substr(delimiterPos + 1);
        float atomMass;
        try {
            atomMass = std::stof(atomMassString);
        }
        catch (...) {
            throw "atomicMassNotANumber";
        }

        atomicMasses[atomName] = atomMass;

    }
}

