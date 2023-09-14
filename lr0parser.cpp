#include <bits/stdc++.h>
#include <graphviz/gvc.h>
using namespace std;

// Global Variables
vector<string> r;
string startSymbol, input1;
const int width = 12;
int counter = 0, fault = 0;
set<string> nonTerminals, terminals;
map<pair<int, string>, int> gotoTable;      // pair: (state, non-terminal) -> state
map<pair<int, string>, string> actionTable; // pair: (state, terminal) -> action
map<string, vector<string>> productionRules;

int countSpaceSeparatedElements(const std::string &input)
{
    std::istringstream iss(input); // Create an input string stream
    int count = 0;
    std::string word;

    while (iss >> word)
    {
        count++;
    }

    return count;
}

void removeWhiteSpace(string &token)
{
    size_t startPos = token.find_first_not_of(" \t");
    if (startPos != string::npos)
    {
        token.erase(0, startPos);
    }
    size_t endPos = token.find_last_not_of(" \t");
    if (endPos != string::npos)
    {
        token.erase(endPos + 1);
    }
}

vector<string> getRules(string rule)
{
    vector<string> rules;
    stringstream ss(rule);
    string token;
    while (getline(ss, token, '|'))
    {
        removeWhiteSpace(token);
        rules.push_back(token);
    }
    return rules;
}

struct LR0Item
{
    string production;
    int dotPosition;

    LR0Item(string prod, int pos)
    {
        production = prod;
        dotPosition = pos;
    }
    bool operator<(const LR0Item &other) const
    {
        // Compare based on production and then dotPosition
        if (production != other.production)
        {
            return production < other.production;
        }
        return dotPosition < other.dotPosition;
    }
    bool operator==(const LR0Item &other) const
    {
        return production == other.production && dotPosition == other.dotPosition;
    }
};

struct LR0State
{
    int idx;
    set<LR0Item> items;
    map<string, int> transitions; // Store transitions to other states

    bool operator<(const LR0State &other) const
    {
        return idx < other.idx;
    }
    bool operator==(const LR0State &other) const
    {
        return items == other.items; // You can define the comparison logic based on your requirements
    }
};

string getNextSpaceSeparatedStringAfterDot(string &input, int idx)
{
    size_t dotIndex = idx;

    if (dotIndex != string::npos)
    {
        // Start searching for the next space-separated string after the dot
        size_t startIndex = input.find(' ', dotIndex);

        if (startIndex != string::npos)
        {
            // Find the end of the next space-separated string
            size_t endIndex = input.find(' ', startIndex + 1);

            if (endIndex != string::npos)
            {
                // Extract the substring between startIndex and endIndex
                return input.substr(startIndex + 1, endIndex - startIndex - 1);
            }
            else
            {
                // If there are no more spaces, extract the substring from startIndex to the end
                return input.substr(startIndex + 1);
            }
        }
    }

    // If no dot or no more spaces are found, return an empty string
    return "";
}

set<LR0Item> closure(const set<LR0Item> &itemSet)
{
    set<LR0Item> closureSet = itemSet;
    bool added;

    do
    {
        added = false;
        set<LR0Item> copySet = closureSet;

        for (const LR0Item &item : copySet)
        {
            string production = item.production;
            int dotPosition = item.dotPosition;
            string nextSymbol = getNextSpaceSeparatedStringAfterDot(production, dotPosition);
            removeWhiteSpace(nextSymbol);

            if (dotPosition < production.length() && nonTerminals.find(nextSymbol) != nonTerminals.end())
            {
                for (const string &rule : productionRules[nextSymbol])
                {
                    LR0Item newItem(nextSymbol + " -> " + rule, nextSymbol.size() + 3);

                    if (closureSet.find(newItem) == closureSet.end())
                    {
                        closureSet.insert(newItem);
                        added = true;
                    }
                }
            }
        }
    } while (added);

    return closureSet;
}

void goTo(LR0State &currentState, const string &symbol, vector<LR0State> &states, int i)
{
    set<LR0Item> gotoSet;
    LR0State tempState = currentState;
    bool change = false;
    for (const LR0Item item : tempState.items)
    {
        string production = item.production;
        int dotPosition = item.dotPosition;
        string nextSymbol = getNextSpaceSeparatedStringAfterDot(production, dotPosition);
        removeWhiteSpace(nextSymbol);

        if (dotPosition < production.length() && nextSymbol == symbol)
        {
            change = true;
            LR0Item newItem(production, dotPosition + nextSymbol.size() + 1);
            set<LR0Item> temp = closure({newItem});
            gotoSet.insert(temp.begin(), temp.end()); // Insert the items from temp into gotoSet
        }
    }

    // Check if a state with the same items already exists
    auto it = find_if(states.begin(), states.end(), [&gotoSet](const LR0State &state)
                      { return state.items == gotoSet; });

    if (!change)
        return;
    if (it != states.end())
    {
        // State with the same items exists, return it
        tempState.transitions[symbol] = (*it).idx;
    }
    else
    {
        // State doesn't exist, create a new one
        LR0State newState;
        newState.items = gotoSet;
        newState.idx = counter++;
        tempState.transitions[symbol] = newState.idx;
        states.push_back(newState);
    }
    currentState = tempState;
    states[i] = currentState;
}

vector<LR0State> buildLR0States()
{
    vector<LR0State> states;
    LR0Item initialItem(startSymbol + " -> " + productionRules[startSymbol][0], startSymbol.size() + 3);
    set<LR0Item> lt = closure({initialItem});
    // for (auto i : lt)
    //     cout << i.production << "\n";

    LR0State initialState;
    initialState.idx = counter++;
    initialState.items = lt;

    states.push_back(initialState);

    for (size_t i = 0; i < states.size(); ++i)
    {
        LR0State &currentState = states[i];

        for (const string symbol : nonTerminals)
        {
            goTo(currentState, symbol, states, i);
        }

        for (const string symbol : terminals)
        {
            goTo(currentState, symbol, states, i);
        }
    }
    // int j = 0;
    // for (auto i : states)
    // {
    //     cout << j << " \n";
    //     for (auto j : i.items)
    //     {
    //         j.production[j.dotPosition] = '.';
    //         cout << j.production << " \n";
    //         j.production[j.dotPosition] = ' ';
    //     }
    //     cout << "\n\n\n";
    //     j++;
    // }
    return states;
}

void buildParsingTable(const vector<LR0State> &states)
{
    for (size_t i = 0; i < states.size(); ++i)
    {
        const LR0State &state = states[i];

        for (const LR0Item &item : state.items)
        {
            string production = item.production;
            int dotPosition = item.dotPosition;

            if (dotPosition < production.length())
            {
                string nextSymbol = getNextSpaceSeparatedStringAfterDot(production, dotPosition);
                removeWhiteSpace(nextSymbol);

                if (terminals.find(nextSymbol) != terminals.end())
                {
                    // Shift action
                    if (state.transitions.find(nextSymbol) != state.transitions.end())
                    {
                        int nextState = state.transitions.at(nextSymbol);

                        if (actionTable.find({i, nextSymbol}) == actionTable.end())
                            actionTable[{i, nextSymbol}] = "S" + to_string(nextState);
                        else
                        {
                            fault = 1;
                            if (actionTable[{i, nextSymbol}][0] == 'R')
                                cerr << "\033[31m"
                                     << "Error: Shift Reduce Conflict in grammar at state " << i << " and symbol " << nextSymbol << " !!"
                                     << "\033[0m" << endl;
                            else
                                cerr << "\033[31m"
                                     << "Error: Shift Shift Conflict in grammar at state " << i << " and symbol " << nextSymbol << " !!"
                                     << "\033[0m" << endl;

                            for (auto k : states[i].items)
                            {
                                k.production[k.dotPosition] = '.';
                                cout << k.production << "\n";
                                k.production[k.dotPosition] = ' ';
                            }
                            return;
                        }
                    }
                }
            }
            else
            {
                if ((production == startSymbol + " -> " + productionRules[startSymbol][0]) && dotPosition >= production.length())
                {
                    // Accept action
                    if (actionTable.find({i, "$"}) == actionTable.end())
                        actionTable[{i, "$"}] = "Accept";
                    else
                    {
                        fault = 1;
                        cerr << "\033[31m"
                             << "Error: Faulty grammar at state " << i << " and symbol "
                             << "$"
                             << "\033[0m" << endl;
                        for (auto k : states[i].items)
                        {
                            k.production[k.dotPosition] = '.';
                            cout << k.production << "\n";
                            k.production[k.dotPosition] = ' ';
                        }
                        return;
                    }
                }
                else
                {
                    // Reduce action
                    int ri = 0;
                    for (; ri < r.size(); ri++)
                    {
                        if (r[ri] == production)
                            break;
                    }

                    for (const string &terminal : terminals)
                    {
                        if (actionTable.find({i, terminal}) == actionTable.end())
                        {
                            actionTable[{i, terminal}] = "R" + to_string(ri);
                        }
                        else
                        {
                            fault = 1;
                            if (actionTable[{i, terminal}][0] == 'R')
                                cerr << "\033[31m"
                                     << "Error: Reduce Reduce conflict in grammar at state " << i << " and symbol " << terminal << "\033[0m" << endl;
                            else
                                cerr << "\033[31m"
                                     << "Error: Shift Reduce conflict in Faulty grammar at state " << i << " and symbol " << terminal << "\033[0m" << endl;
                            for (auto k : states[i].items)
                            {
                                k.production[k.dotPosition] = '.';
                                cout << k.production << "\n";
                                k.production[k.dotPosition] = ' ';
                            }
                            return;
                        }
                    }
                }
            }
        }

        for (const string &nonTerminal : nonTerminals)
        {
            if (state.transitions.find(nonTerminal) != state.transitions.end())
            {
                // Goto action
                int nextState = state.transitions.at(nonTerminal);
                gotoTable[{i, nonTerminal}] = nextState;
            }
        }
    }
}

void printParseTableToFile(const std::string &filename)
{
    std::ofstream outputFile(filename);

    if (!outputFile.is_open())
    {
        cerr << "Error: Unable to open file " << filename << " for writing." << endl;
        return;
    }

    outputFile << setw(width) << "State";
    for (const string &terminal : terminals)
    {
        outputFile << setw(width) << terminal;
    }
    for (const string &nonTerminal : nonTerminals)
    {
        outputFile << setw(width) << nonTerminal;
    }
    outputFile << endl;

    // Print table entries
    for (int stateIdx = 0; stateIdx < counter; stateIdx++)
    {
        outputFile << setw(width) << stateIdx;

        for (const string &terminal : terminals)
        {
            pair<int, string> actionKey = make_pair(stateIdx, terminal);
            if (actionTable.find(actionKey) != actionTable.end())
            {
                outputFile << setw(width) << actionTable[actionKey];
            }
            else
            {
                outputFile << setw(width) << "";
            }
        }

        for (const string &nonTerminal : nonTerminals)
        {
            pair<int, string> gotoKey = make_pair(stateIdx, nonTerminal);
            if (gotoTable.find(gotoKey) != gotoTable.end())
            {
                outputFile << setw(width) << gotoTable[gotoKey];
            }
            else
            {
                outputFile << setw(width) << "";
            }
        }

        outputFile << endl;
    }

    outputFile.close();
    cout << "Parsing table saved to '" << filename << "'." << endl;
}

void printParseTableToCSV(const string &filename)
{
    ofstream outputFile(filename);

    if (!outputFile.is_open())
    {
        cerr << "Error: Unable to open file " << filename << " for writing." << endl;
        return;
    }

    // Print the header row to the CSV file
    outputFile << "State";

    for (const string &terminal : terminals)
    {
        outputFile << "," << terminal;
    }

    for (const string &nonTerminal : nonTerminals)
    {
        outputFile << "," << nonTerminal;
    }

    outputFile << endl;

    // Print table entries to the CSV file
    for (int stateIdx = 0; stateIdx < counter; stateIdx++)
    {
        outputFile << stateIdx;

        for (const string &terminal : terminals)
        {
            pair<int, string> actionKey = make_pair(stateIdx, terminal);
            if (actionTable.find(actionKey) != actionTable.end())
            {
                outputFile << "," << actionTable[actionKey];
            }
            else
            {
                outputFile << "," << terminal;
            }
        }

        for (const string &nonTerminal : nonTerminals)
        {
            pair<int, string> gotoKey = make_pair(stateIdx, nonTerminal);
            if (gotoTable.find(gotoKey) != gotoTable.end())
            {
                outputFile << "," << gotoTable[gotoKey];
            }
            else
            {
                outputFile << ",";
            }
        }

        outputFile << endl;
    }

    outputFile.close();
    cout << "CSV file '" << filename << "' created successfully." << endl;
}

bool parseInput(const vector<LR0State> &states, vector<string> &input)
{
    stack<int> stateStack;
    stack<string> symbolStack;
    stateStack.push(0);
    symbolStack.push("$"); // Initial symbol for bottom of stack
    size_t inputIndex = 0;

    while (true)
    {
        int currentState = stateStack.top();
        string currentSymbol = inputIndex < input.size() ? input[inputIndex] : "$";
        string action = actionTable[{currentState, currentSymbol}];

        vector<int> st;
        while (!stateStack.empty())
        {
            int x = stateStack.top();
            st.push_back(x);
            stateStack.pop();
        }
        cout << "STACK -> ";
        for (int j = st.size() - 1; j >= 0; j--)
        {
            cout << st[j] << " ";
            stateStack.push(st[j]);
        }
        cout << "\nInput -> ";
        for (int i = inputIndex; i < input.size(); i++)
        {
            cout << input[i] << " ";
        }
        cout << "$ ";

        cout << "\nACTION -> ";
        cout << action << "\n\n";

        if (action == "Accept")
        {
            cout << "Input accepted." << endl;
            return true;
        }
        else if (!action.empty())
        {
            if (action[0] == 'S')
            {
                // Shift action
                int nextState = stoi(action.substr(1));
                stateStack.push(nextState);
                symbolStack.push(currentSymbol);
                ++inputIndex;
            }
            else if (action[0] == 'R')
            {
                // Reduction action
                int productionIndex = stoi(action.substr(1));
                string prod = r[productionIndex];
                size_t arrowPos = prod.find("->");
                string leftHandSide;
                string rhs;
                if (arrowPos != string::npos)
                {
                    leftHandSide = prod.substr(0, arrowPos);
                    rhs = prod.substr(arrowPos + 2);
                    removeWhiteSpace(leftHandSide);
                    removeWhiteSpace(rhs);
                }
                int length = countSpaceSeparatedElements(rhs); // Remove " -> " from the string

                for (int i = 0; i < length; ++i)
                {
                    stateStack.pop();
                    symbolStack.pop();
                }

                int prevState = stateStack.top();
                symbolStack.push(leftHandSide);
                stateStack.push(gotoTable[{prevState, leftHandSide}]);
            }
        }
        else
        {
            cerr << "\033[31m"
                 << "Error: No valid action for state " << currentState << " and symbol '" << currentSymbol << "'. Parsing failed."
                 << "\033[0m" << endl;
            return false;
        }
    }
}

void drawLR0States(const std::vector<LR0State> &lr0States, const std::string &outputFilename)
{
    std::ofstream dot_file(outputFilename);

    dot_file << "digraph G {\n";
    dot_file << "rankdir=LR;\n";
    dot_file << "node [shape=\"circle\"]\n";

    for (const LR0State &state : lr0States)
    {
        dot_file << "\t" << state.idx << " [label=\"" << state.idx << "\\n";

        for (LR0Item item : state.items)
        {
            item.production[item.dotPosition] = '.';
            dot_file << item.production << "\\n";
            item.production[item.dotPosition] = ' ';
        }

        dot_file << "\"]\n";

        for (const auto &transition : state.transitions)
        {
            dot_file << "\t" << state.idx << " -> " << transition.second << " [label=\"" << transition.first << "\"]\n";
        }
    }

    dot_file << "}\n";
    dot_file.close();
    system("dot -Tpng graph.dot -o graph.png");
    cout << "Parse tree generated and saved as graph.png" << endl;
    system("feh -F graph.png");
}

void takeInput()
{
    ifstream inputFile("Grammar/GrammarLR.txt");
    if (!inputFile.is_open())
    {
        cout << "Failed to open input file." << endl;
        fault = 1;
        return;
    }

    string line;
    getline(inputFile, startSymbol);
    getline(inputFile, line);
    stringstream nonTermStream(line);
    string nonTerm;
    while (getline(nonTermStream, nonTerm, ' '))
    {
        removeWhiteSpace(nonTerm);
        nonTerminals.insert(nonTerm);
    }
    getline(inputFile, line);
    stringstream termStream(line);
    string term;
    while (getline(termStream, term, ' '))
    {
        removeWhiteSpace(term);
        terminals.insert(term);
    }
    getline(inputFile, input1);

    while (getline(inputFile, line))
    {
        size_t arrowPos = line.find("->");
        if (arrowPos != string::npos)
        {
            string nonTerminal = line.substr(0, arrowPos);
            string rule = line.substr(arrowPos + 2);
            removeWhiteSpace(nonTerminal);
            removeWhiteSpace(rule);
            productionRules[nonTerminal] = getRules(rule);
        }
    }
    inputFile.close();
}

void printDFAStates(vector<LR0State> states)
{
    for (auto st : states)
    {
        for (auto it : st.items)
        {
            it.production[it.dotPosition] = '.';
            cout << it.production << "\n";
            it.production[it.dotPosition] = ' ';
        }
        cout << st.idx << "\n";
        for (auto pos : st.transitions)
            cout << pos.first << " " << pos.second << "\n";
        cout << "\n\n";
    }
}

int main()
{
    takeInput();
    if (fault)
        return 0;

    // Build LR(0) states
    vector<LR0State> states = buildLR0States();
    for (auto i : productionRules)
    {
        for (auto j : i.second)
        {
            r.push_back(i.first + " -> " + j);
        }
    }

    // Construct the LR(0) parsing table
    terminals.insert("$");
    buildParsingTable(states);
    if (fault)
        return 0;

    // printDFAStates(states);
    printParseTableToFile("parseTable.txt");
    printParseTableToCSV("parseTableCSV");
    drawLR0States(states, "graph.dot");

    // Input string
    vector<string> input;
    stringstream nonTermStream1(input1);
    string nonTerm1;
    while (getline(nonTermStream1, nonTerm1, ' '))
    {
        removeWhiteSpace(nonTerm1);
        input.push_back(nonTerm1);
    }

    // Parse the input using the constructed LR(0) parsing table
    // Create an output file stream
    std::ofstream outputFile("Parsing Output.txt");

    // Redirect the cout stream to the output file stream
    std::streambuf *originalCoutStream = std::cout.rdbuf();
    std::cout.rdbuf(outputFile.rdbuf());

    // Call the parseInput function
    bool result = parseInput(states, input);

    // Restore the original cout stream
    std::cout.rdbuf(originalCoutStream);

    // Close the output file
    outputFile.close();

    if (result)
    {
        std::cout << "\033[32m"
                  << "Input accepted. Output written to 'Parsing Output.txt'"
                  << "\033[0m" << std::endl;
    }
    else
    {
        std::cerr << "\033[31m"
                  << "Parsing failed. Check 'Parsing Output.txt' for details."
                  << "\033[0m" << std::endl;
    }

    return 0;
}
