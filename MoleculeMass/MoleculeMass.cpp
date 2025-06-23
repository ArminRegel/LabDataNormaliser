



#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

#ifdef _DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

void loadAtomicMassesFromCSV();
float calculateMolecularMass(const std::string& molecularFormula);

std::unordered_map<std::string, float> atomicMasses;
float calculateAtomicMass(const std::string& atomicSymbol);

const std::string ATOMIC_MASSES_FILEPATH = "AtomicMasses.csv";

bool isOpeningBracket(char character);
bool isClosingBracket(char character);

std::pair<int, int> parseNumber(const std::string& str, int start);

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


bool isOpeningBracket(char character) {
    return character == '(' || character == '[' || character == '{';
}
bool isClosingBracket(char character) {
    return character == ')' || character == ']' || character == '}';
}

float calculateMolecularMass(const std::string& molecularFormula) {
    DEBUG_PRINT("molecular formula = " << molecularFormula);
    if (molecularFormula.length() <= 0) return 0;
    if (molecularFormula.length() == 1) return calculateAtomicMass(molecularFormula);

    float atomicMass;
    int atomAmountIndex;


    if (isOpeningBracket(molecularFormula.at(0))) {
        int openBrackets = 1;
        for (int i = 1; i < molecularFormula.length(); ++i) {
            DEBUG_PRINT("open brackets = " << openBrackets );
            if (isOpeningBracket(molecularFormula.at(i))) {
                ++openBrackets;
            }
            else if (isClosingBracket(molecularFormula.at(i))) {
                --openBrackets;
                DEBUG_PRINT("closing bracket detected");
                if (openBrackets <= 0) {
                    std::pair<int, int> atomAmountAndEndIndex;
                    if (molecularFormula.length() >= i + 2 && isdigit(molecularFormula.at(i + 1))) {
                        atomAmountAndEndIndex = parseNumber(molecularFormula, i + 1);

                    }
                    else {
                        atomAmountAndEndIndex = { 1,i + 1 };
                    }
                    DEBUG_PRINT("atomAmmountAndEndIndex: " << atomAmountAndEndIndex.first << ", " << atomAmountAndEndIndex.second);
                    return calculateMolecularMass(molecularFormula.substr(1, i - 1)) * atomAmountAndEndIndex.first + calculateMolecularMass(molecularFormula.substr(atomAmountAndEndIndex.second));
                }
            }
        } throw "bracket not closed";

    }

    if (molecularFormula.at(0) < 'A' || molecularFormula.at(0) > 'Z') throw "molecularFormulaBroken";

    
    //Figure out Atomic Mass
    if ( isOpeningBracket(molecularFormula.at(1)) || (molecularFormula.at(1) >= 'A' && molecularFormula.at(1) <= 'Z') ) return calculateAtomicMass(molecularFormula.substr(0,1)) + calculateMolecularMass(molecularFormula.substr(1));


    else if (molecularFormula.at(1) >= '1' && molecularFormula.at(1) <= '9') {//if number
        atomicMass = calculateAtomicMass(molecularFormula.substr(0, 1));
        
        atomAmountIndex = 1;
    }

    else {
        
        atomicMass = calculateAtomicMass(molecularFormula.substr(0, 2));

        if (molecularFormula.length() <= 2 || molecularFormula.at(2) < '0' || molecularFormula.at(2) > '9') return atomicMass + calculateMolecularMass(molecularFormula.substr(2)); ;//if (no number after Atomic Symbol)
        atomAmountIndex = 2; 
    }
        
    std::pair<int,int> atomAmmountAndEndIndex = parseNumber(molecularFormula, atomAmountIndex);



    return atomicMass * atomAmmountAndEndIndex.first + calculateMolecularMass(molecularFormula.substr(atomAmmountAndEndIndex.second));

}

//creates a number from a string at the position start and gives the position AFTER the last number character; returns 0 and start if not a number
std::pair<int, int> parseNumber(const std::string& string, int start) {

    int number = 0;
    //std::cout << "atomicMass = " << atomicMass << "\n";

    while (start < string.length() && string.at(start) >= '0' && string.at(start) <= '9') {
        number *= 10;
        number += string.at(start) - '0'; // character - '0' turns it into int
        ++start;
    }
    return{ number, start };
}

float calculateAtomicMass(const std::string& atomicSymbol) {

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

