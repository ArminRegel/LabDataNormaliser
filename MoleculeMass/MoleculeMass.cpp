



#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <bitset>
#include <algorithm>


std::ofstream debugLogger("debugLog.txt");

#ifdef _DEBUG
#define DEBUG_PRINT(x) do {std::cout << x << std::endl; \
debugLogger << x << "\n";} while(0)


#else
#define DEBUG_PRINT(x)
#endif


#define RELEASE_PRINT(x) do {std::cout << x << std::endl; \
debugLogger << x << "\n";} while(0)

std::vector<std::string> splitCSV(const std::string& line);
void loadAtomicMassesFromCSV();
float calculateMolecularMass(const std::string& molecularFormula);

std::unordered_map<std::string, float> atomicMasses;
float calculateAtomicMass(const std::string& atomicSymbol);

std::string normaliseMolecularFormula(const std::string& formula);

const std::string ATOMIC_MASSES_FILEPATH = "AtomicMasses.csv";
const std::string LABDATA_FILEPATH = "Experiment_Roh.csv";
const std::string LABDATA_CLEANED_FILEPATH = "Experiment_Cleaned.csv";

bool isOpeningBracket(char character);
bool isClosingBracket(char character);

std::pair<int, int> parseNumber(const std::string& str, int start);

const size_t CSV_Line_Size = 6;

std::unordered_map<std::string, double> si_prefixes = {
    {"", 1},        //No Prefix
    {"m", 1e-3},   // Milli
    {"c", 1e-2},   // Zenti
    {"d", 1e-1},   // Dezi
    {"da", 1e1},   // Deka
    {"h", 1e2},    // Hekto
    {"k", 1e3},    // Kilo
    {"M", 1e6},    // Mega
    {"G", 1e9},    // Giga
    {"T", 1e12},   // Tera
    {"P", 1e15},   // Peta
    {"n", 1e-9},   // Nano
    {"µ", 1e-6},    // Mikro
    {"u", 1e-6 }    // Mikro Alternative
};

int main()
{
    
    //std::ofstream debugLogger("debugLog.txt");
    //if (!out) ;             // Fehlerprüfung

    std::string text = "Hallo Welt! ₀₁₂"; // Beispieltext mit Subskripten
    debugLogger << text << "\n";
    
    //DEBUG_PRINT( "why dis not in file? \n");

    loadAtomicMassesFromCSV();


    std::ofstream labdataWriter(LABDATA_CLEANED_FILEPATH);
    labdataWriter << "Timestamp,Sample,Compound,Molar Mass (g/mol),Mass conc (g/L),Molar conc (mol/L),Comment,Corrupted?\n";


    std::ifstream labdataReader(LABDATA_FILEPATH);

    if (!labdataReader.is_open()) {
        throw "atomicMassesTableNotFound";
    }

    std::string line;

    std::getline(labdataReader, line);//Skip header

    while (std::getline(labdataReader, line)) {

        DEBUG_PRINT("read line from labdata file: " << line);

        std::vector<std::string> CSV_Line = splitCSV(line);
        
        //calculate molar mass
        std::string molecularFormula = CSV_Line[2];
        DEBUG_PRINT("initial molecularFormula = " << molecularFormula);

        //molecularFormula = normaliseMolecularFormula(molecularFormula);

        double mass = calculateMolecularMass(molecularFormula);
        RELEASE_PRINT("Molecular Mass = " << mass << "\n");
        labdataWriter << ",," << molecularFormula << "," << mass << ",";

        //correct mass concentration (replace commas with points)

        //removes all spaces from the string
        std::string originalMassConcentrationLine = CSV_Line[3];

        CSV_Line[3].erase(std::remove(CSV_Line[3].begin(), CSV_Line[3].end(), ' '), CSV_Line[3].end());
        DEBUG_PRINT("line after whitespace removal : " << CSV_Line[3] << "\n");
        

        //bool stillNumber = true;
        std::string numberString = "";
        size_t i = 0;
        bool massConcentrationCorrupted = false;
        bool numberOnly = true;

        for (; i < CSV_Line[3].size(); ++i) {
            if ((CSV_Line[3][i] >= '0' && CSV_Line[3][i] <= '9') || CSV_Line[3][i] == ',' || CSV_Line[3][i] == '.') {
                numberString.push_back(CSV_Line[3][i] == ',' ? '.' : CSV_Line[3][i]);
                

            }
            else {
                numberOnly = false;
                break;
                
            }
        }

        DEBUG_PRINT("Concentration base number = " << numberString);

        std::string massPrefixString = "";
        
        
        if (!numberOnly) for (; true; ++i) {
            //if no mass unit found before string end
            if (i >= CSV_Line[3].size()) {
                massConcentrationCorrupted = true;
                DEBUG_PRINT("Corruption 1");
                break;
            }
            
            if (CSV_Line[3][i] != 'g') massPrefixString += CSV_Line[3][i];
            else {
                if (i < CSV_Line[3].size() && CSV_Line[3][i+1] == '/') {
                    i += 2;
                    break;
                }
                else {
                    massConcentrationCorrupted = true;
                    DEBUG_PRINT("Corruption 2");
                    i = CSV_Line[3].size();//skip next loop if corrupted
                    break;
                }
            }

        }
        DEBUG_PRINT("massPrefixString = " << massPrefixString);


        std::string volumePrefixString = "";
        
        if (!numberOnly) for (; true; ++i) {
            if (CSV_Line[3][i] == 'l' || CSV_Line[3][i] == 'L') {
                //if theres still other characters after l char
                if (i+1 < CSV_Line[3].size() ) {
                    massConcentrationCorrupted = true;
                    DEBUG_PRINT("Corruption 3");
                        
                }
                break;
   
            } else  volumePrefixString += CSV_Line[3][i];
            //if no mass unit found before string end
            if (i >= CSV_Line[3].size()) {
                massConcentrationCorrupted = true;
                DEBUG_PRINT("Corruption 4");
                break;
            }
        }

        DEBUG_PRINT("volumePrefixString = " << volumePrefixString);
        double massConcentration;
        
        if (!originalMassConcentrationLine.empty()) {
            try {
                DEBUG_PRINT("Entering massConcentration calculation");

                size_t charAfterNumber;
                massConcentration = std::stod(numberString, &charAfterNumber);
                if (charAfterNumber < numberString.size()) massConcentrationCorrupted = true;
                DEBUG_PRINT("initial massConcentration = " << massConcentration);

                double massPrefix = si_prefixes[massPrefixString];
                if (massPrefix == 0) massConcentrationCorrupted = true;
                DEBUG_PRINT("massPrefix = " << massPrefix);

                double volumePrefix = si_prefixes[volumePrefixString];
                if (volumePrefix == 0) massConcentrationCorrupted = true;
                DEBUG_PRINT("volumePrefix = " << volumePrefix);

                massConcentration *= massPrefix / volumePrefix;
            }
            catch (...) {
                massConcentrationCorrupted = true;
            }

            if (massConcentrationCorrupted == true) {
                labdataWriter << "\"" << originalMassConcentrationLine << "; corruption detected\"";
            }
            else {
                labdataWriter << massConcentration;
            }
        }
        labdataWriter << ", , ," << (massConcentrationCorrupted ? "True": "False") << "\n";

    }
    


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

std::vector<std::string> splitCSV(const std::string& line) {
    std::vector<std::string> result;
    result.reserve(CSV_Line_Size);
    std::string currentCollumn;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        
        if (line.at(i) == '"') {
            if (!inQuotes) inQuotes = true;
            else {
                if (i + 1 < line.size() && line.at(i + 1) == '"') {
                    //needs to add one " if there are two " in a row
                    currentCollumn.push_back('"');
                    ++i;
                }
                //if single quote (closing quote)
                else {
                    inQuotes = false;
                }
                
            }

        }


        else if (line.at(i) == ',' && !inQuotes) {
            
            if (result.size() > CSV_Line_Size) {
                RELEASE_PRINT( "Warning: CSV line has more columns than expected: " << CSV_Line_Size << " expected, actual size: " << result.size() << "\n");
                RELEASE_PRINT ("Ignoring additional data because of this!\n");
                break;
            }
            result.push_back(currentCollumn);
            
            currentCollumn.clear();
        }
        else {
            currentCollumn.push_back(line.at(i));
        }
    }
    result.push_back(currentCollumn);
    
    return result;
}



std::string normaliseMolecularFormula(const std::string& formula) {

    std::string normalisedFormula = "";
    normalisedFormula.reserve(formula.size());

    for (size_t i = 0; i < formula.size();) {
        unsigned char currentChar = formula.at(i);
        DEBUG_PRINT(std::bitset<8>(currentChar));
        size_t byteLen = 0;
        unsigned char checkedBit = 0b10000000;
        while ((currentChar & checkedBit) && (byteLen <= 5)) {
            
            ++byteLen;
            checkedBit >>= 1;
        }
        DEBUG_PRINT("byteLen = " << byteLen << "\n");

        if (byteLen == 0) {
            normalisedFormula.push_back(formula[i]);
            ++i;
            continue;
        }
        if (byteLen <= 5 || byteLen == 1 || i+byteLen > formula.size()) throw "faulty input";
        

        std::string utf8_char = formula.substr(i, byteLen);
        std::cout << "UTF8 char detected: " << utf8_char << "\n";
        i += byteLen;




    }

    std::cout << "normalisedFormula = " << normalisedFormula << "\n";
    return normalisedFormula;

}