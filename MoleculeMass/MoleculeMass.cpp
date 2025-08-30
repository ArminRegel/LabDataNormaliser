



#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <bitset>
#include <algorithm>
#include <utility>

#include <chrono>
#include <filesystem>

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

namespace fs = std::filesystem;
fs::path filepath;

const std::string ATOMIC_MASSES_FILEPATH = "AtomicMasses.csv";
const std::string LABDATA_FILEPATH = "Experiment_Raw.csv";
const std::string LABDATA_CLEANED_FILEPATH = "Experiment_Cleaned.csv";

bool isOpeningBracket(char character);
bool isClosingBracket(char character);

std::pair<int, int> parseNumber(const std::string& str, int start);

const size_t CSV_Line_Size = 6;

double concentrationFromString(const std::string& concentration, const std::string& numeratorUnit = "g");

void make_csv_safe(std::string & string);

void normaliseMolecularFormula(std::string & formula);

std::string currentWarning = "";
//keeps adding warning messages, they will be written down in the last column of the current csvline
void addWarning(std::string warning) { currentWarning += warning + ";"; }

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


std::unordered_map<std::string, char> subscripts = {
    {"₀", '0'},
    {"₁", '1'},
    {"₂", '2'},
    {"₃", '3'},
    {"₄", '4'},
    {"₅", '5'},
    {"₆", '6'},
    {"₇", '7'},
    {"₈", '8'},
    {"₉", '9'}
};

int main()
{
    
    auto start = std::chrono::high_resolution_clock::now();

    RELEASE_PRINT("LabDataNormaliser started");

    filepath = fs::current_path();
    //yeah I know this is a bit cheap, but it's still within reason :D
    if (!(fs::exists(filepath / ATOMIC_MASSES_FILEPATH) && fs::exists(filepath / LABDATA_FILEPATH)))
        filepath = filepath.parent_path();
    if (!(fs::exists(filepath / ATOMIC_MASSES_FILEPATH) && fs::exists(filepath / LABDATA_FILEPATH)))
        filepath = filepath.parent_path();
    if (!(fs::exists(filepath / ATOMIC_MASSES_FILEPATH) && fs::exists(filepath / LABDATA_FILEPATH))) {
        RELEASE_PRINT("fatal error: labdata table or atomic masses table not found!");
        return -1;
    }



    loadAtomicMassesFromCSV();



    std::ofstream labdataWriter(filepath / LABDATA_CLEANED_FILEPATH);
    labdataWriter << "Timestamp,Sample,Compound,Molar Mass (g/mol),Mass conc (g/L),Molar conc (mol/L),Comment,Corrupted?\n";


    std::ifstream labdataReader(filepath / LABDATA_FILEPATH);

    if (!labdataReader.is_open()) {
        throw "labdata file not found";
    }

    std::string line;

    std::getline(labdataReader, line);//Skip header

    while (std::getline(labdataReader, line)) {

        DEBUG_PRINT("read line from labdata file: " << line);

        std::vector<std::string> CSV_Line = splitCSV(line);
        
        //just simple proof of concept, no error checking like with concentration
        if (CSV_Line[0].size() == 16 && CSV_Line[0][2] == '-') {
            
            std::string output = CSV_Line[0].substr(6, 4) + '-' +
                CSV_Line[0].substr(3,2) + '-' +
                CSV_Line[0].substr(0,2) + ' '
                + CSV_Line[0].substr(11, 5);
            CSV_Line[0] = output;
        }
        
        make_csv_safe(CSV_Line[0]);
        labdataWriter << CSV_Line[0] << ',';

        make_csv_safe(CSV_Line[1]);
        labdataWriter << CSV_Line[1] << ',';

        //calculate molar mass
        DEBUG_PRINT("initial molecularFormula = " << CSV_Line[2]);
        normaliseMolecularFormula(CSV_Line[2]);
        DEBUG_PRINT("normalised molecularFormula = " << CSV_Line[2]);
        

        //molecularFormula = normaliseMolecularFormula(molecularFormula);
        double mass = std::nan("");
        try {
            mass = calculateMolecularMass(CSV_Line[2]);
            DEBUG_PRINT("Molecular Mass = " << mass << "\n");
            labdataWriter << CSV_Line[2] << "," << mass << ",";
        }
        catch (...) {
            addWarning("Could not parse sum formula: " + CSV_Line[2]);
            make_csv_safe(CSV_Line[2]);
            labdataWriter << CSV_Line[2] << "," << "" << ",";
            
            mass = std::nan("");
        }



        //will become safified if it has corrupted content
        std::string massConcentrationLine = CSV_Line[3];
        std::string molConcentrationLine = CSV_Line[4];

        double massConcentrationParsed;
        double molConcentrationParsed;

        //if(massConcentration exists -> write massConcentration

        


        massConcentrationParsed = concentrationFromString(massConcentrationLine, "g");
        molConcentrationParsed = concentrationFromString(molConcentrationLine, "mol");

        double molConcentrationCalculated = std::nan("");
        

        if (massConcentrationLine != "") {
            if (std::isnan(massConcentrationParsed)) {
                addWarning("mass concentration corrupted");
                make_csv_safe(massConcentrationLine);
                labdataWriter << massConcentrationLine << ',';
                //is mol concentration missing? YES!
            }
            //else == if (proper massConcentration)
            else {
                labdataWriter << massConcentrationParsed;
                molConcentrationCalculated = massConcentrationParsed / mass;
            
                
                labdataWriter << ',';

                if (molConcentrationLine == "") {
                    if (!std::isnan(molConcentrationCalculated)) labdataWriter << molConcentrationCalculated;
                }
                else {
                    if (std::isnan(molConcentrationParsed)) {
                        addWarning("mol concentration corrupted");
                        make_csv_safe(molConcentrationLine);
                        labdataWriter << molConcentrationLine;

                    }
                    else {
                        labdataWriter << molConcentrationParsed;
                        double molConcentrationProportion = molConcentrationParsed / molConcentrationCalculated;
                        if (std::isnan(molConcentrationProportion)) addWarning("concentration ratio check unsucessful");
                        else if (molConcentrationProportion < 0.95 || molConcentrationProportion > 1.05) addWarning("concentration ratio implausible");
                    }
                }
            }
        }
        //else == if (massConcentrationLine == "")
        else {
            if (molConcentrationLine != "") {
                if (std::isnan(molConcentrationParsed)) {
                    addWarning("mol concentration corrupted");
                    make_csv_safe(molConcentrationLine);
                    labdataWriter << ',' << molConcentrationLine;
                }
                else {
                    double massConcentrationCalculated = molConcentrationParsed * mass;
                    
                    if (!std::isnan(massConcentrationCalculated)) labdataWriter << massConcentrationCalculated;
                    labdataWriter << ',' << molConcentrationParsed;
                }
                
            }
            else {
                labdataWriter << ',';
            }
        }


        labdataWriter << ",";
        std::string commentLine = CSV_Line[5];
        make_csv_safe(commentLine);
        labdataWriter << commentLine;
        if(!currentWarning.empty()) currentWarning.pop_back();
        labdataWriter << "," << currentWarning << "\n";
        currentWarning = "";

    }
    

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    RELEASE_PRINT("LabDataNormaliser finished after " << duration.count() << " ms");
}


void make_csv_safe(std::string& string) {

    std::string safeString = "\"";
    bool problematicChar = false;
    for (char c : string) {
        safeString.push_back(c);
        if (c == '"') {
            safeString.push_back('"');
            problematicChar = true;
        }
        else if (c == ',' || c == '\n' || c == '\r') {
            problematicChar = true;

        }


    }
    safeString += '"';
    if (problematicChar) string = safeString;
}


double concentrationFromString(const std::string& originalConcentrationString, const std::string& numeratorUnit) {

    std::string concentration = originalConcentrationString;
    
    //removes all spaces from the string
    concentration.erase(std::remove(concentration.begin(), concentration.end(), ' '), concentration.end());
    DEBUG_PRINT("line after whitespace removal : " << concentration << "\n");


    //bool stillNumber = true;
    std::string numberString = "";
    size_t i = 0;
    //bool massConcentrationCorrupted = false;
    bool numberOnly = true;

    for (; i < concentration.size(); ++i) {
        if ((concentration[i] >= '0' && concentration[i] <= '9') || concentration[i] == ',' || concentration[i] == '.') {
            numberString.push_back(concentration[i] == ',' ? '.' : concentration[i]);


        }
        else {
            numberOnly = false;
            break;

        }
    }

    DEBUG_PRINT("Concentration base number = " << numberString);

    double massConcentration;

    

    try {
        DEBUG_PRINT("calculation massConcentration base number");
        size_t charAfterNumber;
        massConcentration = std::stod(numberString, &charAfterNumber);
        if (charAfterNumber < numberString.size()) return std::nan("");
        DEBUG_PRINT("initial massConcentration = " << massConcentration);
    }
    catch (...) {
        return std::nan("");
    }

    if (numberOnly) return massConcentration;


    //calculate mass prefix modifier
    std::string massPrefixString = "";


    size_t numeratorStart = concentration.find(numeratorUnit, i);
    if (numeratorStart == std::string::npos) {
        DEBUG_PRINT("Corruption 0.9");
        return std::nan("");
    }
    massPrefixString = concentration.substr(i, numeratorStart - i);

    DEBUG_PRINT("massPrefixString = " << massPrefixString);
    i = numeratorStart + numeratorUnit.size() + 1;//i = first char after '/'
    if (i >= concentration.size() || concentration[i-1] != '/') {
        DEBUG_PRINT("Corruption 0.95");
        return std::nan("");
    }
    

    


    DEBUG_PRINT("massPrefixString = " << massPrefixString);


    std::string volumePrefixString = "";

    if (!numberOnly) for (; true; ++i) {
        if (concentration[i] == 'l' || concentration[i] == 'L') {
            //if theres still other characters after l char
            if (i + 1 < concentration.size()) {
                DEBUG_PRINT("Corruption 3");
                return std::nan("");
            }
            break;

        }
        else  volumePrefixString += concentration[i];
        //if no mass unit found before string end
        if (i >= concentration.size()) {
            DEBUG_PRINT("Corruption 4");
            return std::nan("");
            break;
        }
    }

    DEBUG_PRINT("volumePrefixString = " << volumePrefixString);


    if (!originalConcentrationString.empty()) {
        try {


            double massPrefix = si_prefixes[massPrefixString];
            if (massPrefix == 0) return std::nan("");
            DEBUG_PRINT("massPrefix = " << massPrefix);

            double volumePrefix = si_prefixes[volumePrefixString];
            if (volumePrefix == 0) return std::nan("");
            DEBUG_PRINT("volumePrefix = " << volumePrefix);

            massConcentration *= massPrefix / volumePrefix;
        }
        catch (...) {
            return std::nan("");
        }


        return massConcentration;

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


    std::ifstream file(filepath/ ATOMIC_MASSES_FILEPATH);

    if (!file.is_open()) {
        throw "atomicMassesTableNotFound";
    }

    std::string line;

    std::getline(file, line);//Skip header

    while (std::getline(file, line)) {
        

        size_t delimiterPos = line.find(',');

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



void normaliseMolecularFormula(std::string& formula) {

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
        if (byteLen >= 5 || byteLen == 1 || i + byteLen > formula.size()) {
            addWarning("molecular formula corrupted characters or wrong encoding");//replace with warning added
            RELEASE_PRINT("molecular formula corrupted characters or wrong encoding");
            return;
        }

        std::string utf8_char = formula.substr(i, byteLen);
        DEBUG_PRINT("UTF8 char detected: " << utf8_char << "\n");
        //char replacedChar = subscripts.at(utf8_char);
        auto container = subscripts.find(utf8_char);
        if (container == subscripts.end()) {
            //maybe don't push back but ignore unknown utf8_char?
            normalisedFormula += utf8_char;
        }
        else {
            normalisedFormula.push_back(container->second);
        }


        i += byteLen;




    }

    DEBUG_PRINT("normalisedFormula = " << normalisedFormula);
    formula = normalisedFormula;

}