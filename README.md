# LR(0) Parser Readme

This document provides a detailed explanation of the C++ code for an LR(0) parser, including the code structure, algorithms, and usage. The code performs the following tasks:

1. **Constructs LR(0) States**: The code generates LR(0) states for a given context-free grammar.

2. **Builds Parsing Table**: It constructs the parsing table for the LR(0) automaton.

3. **Parses Input**: The code parses an input string using the constructed LR(0) parsing table.

4. **Visualizes LR(0) States**: It visualizes the LR(0) states and transitions using Graphviz.

## Code Structure

The code is organized into functions that perform specific tasks. Here's an overview of the code structure:

### Global Variables and Includes
- Includes necessary libraries and declares global variables.

### Utility Functions
1. `countSpaceSeparatedElements`: Counts the number of space-separated elements in a string.
2. `removeWhiteSpace`: Removes leading and trailing whitespace from a string.
3. `getRules`: Splits a rule into multiple production rules.

### Data Structures
1. `LR0Item`: Represents an LR(0) item, which includes a production and a dot position.
2. `LR0State`: Represents an LR(0) state, containing a set of LR(0) items and transitions to other states.

### LR(0) State Generation
1. `getNextSpaceSeparatedStringAfterDot`: Gets the next symbol after the dot in a production.
2. `closure`: Computes the closure of an LR(0) item set.
3. `goTo`: Computes the GOTO operation for LR(0) states.
4. `buildLR0States`: Constructs LR(0) states for the given grammar.

### Parsing Table Construction
1. `buildParsingTable`: Constructs the parsing table for the LR(0) automaton.
2. `printParseTableToFile`: Prints the parsing table to a file.
3. `printParseTableToCSV`: Prints the parsing table to a CSV file.

### Parsing
1. `parseInput`: Parses an input string using the LR(0) parsing table.

### Visualization
1. `drawLR0States`: Visualizes LR(0) states and transitions using Graphviz.

### Input and Main Function
1. `takeInput`: Reads the grammar from a file and initializes the global variables.
2. `main`: The main function that orchestrates the entire process.

## Algorithms

### LR(0) State Generation (`buildLR0States`)
- Starts with an initial LR(0) item for the start symbol.
- Computes the closure of this initial item set.
- Uses the GOTO operation to create new states and transitions to other LR(0) item sets.
- Continues this process until no new items can be added to existing states.
- Outputs a vector of LR(0) states.

### Parsing Table Construction (`buildParsingTable`)
- For each LR(0) state and item within that state, constructs the parsing table entries.
- Determines shift actions for terminals and reduce actions for non-terminals.
- Handles shift-reduce and reduce-reduce conflicts.

### Parsing (`parseInput`)
- Initializes a stack for LR(0) states and a stack for symbols.
- Reads symbols from the input string and follows the parsing table entries to perform shift, reduce, or accept actions.
- Outputs detailed information about the parsing process, including the stack, input, and actions.

### Visualization (`drawLR0States`)
- Generates a Graphviz DOT file representing the LR(0) states and transitions.
- Uses Graphviz to convert the DOT file to a PNG image for visualization.

## Usage

1. Ensure you have the Graphviz library installed for visualizing the LR(0) states.
2. Create a text file named "GrammarLR.txt" with your context-free grammar.
3. Compile and run the code, which will generate the parsing table, visualizations, and parse the input string.

## Example Grammar File ("GrammarLR.txt")

```plaintext
D'
D T L
id : ; , var integer real d
var id : integer ; d
D' -> D
D -> var L : T ; d
T -> integer | real
L -> L , id | id
```

## Parsing Table

```plaintext
       State           $           ,           :           ;           d          id     integer        real         var           D           L           T
           0                                                                                                          S2           1                        
           1      Accept                                                                                                                                    
           2                                                                      S4                                                           3            
           3                      S5          S6                                                                                                            
           4          R3          R3          R3          R3          R3          R3          R3          R3          R3                                    
           5                                                                      S7                                                                        
           6                                                                                  S9         S10                                               8
           7          R2          R2          R2          R2          R2          R2          R2          R2          R2                                    
           8                                             S11                                                                                                
           9          R4          R4          R4          R4          R4          R4          R4          R4          R4                                    
          10          R5          R5          R5          R5          R5          R5          R5          R5          R5                                    
          11                                                         S12                                                                                    
          12          R0          R0          R0          R0          R0          R0          R0          R0          R0                                    

```


## Parsing Output

```plaintext
STACK -> 0 
Input -> var id : integer ; d $ 
ACTION -> S2

STACK -> 0 2 
Input -> id : integer ; d $ 
ACTION -> S4

STACK -> 0 2 4 
Input -> : integer ; d $ 
ACTION -> R3

STACK -> 0 2 3 
Input -> : integer ; d $ 
ACTION -> S6

STACK -> 0 2 3 6 
Input -> integer ; d $ 
ACTION -> S9

STACK -> 0 2 3 6 9 
Input -> ; d $ 
ACTION -> R4

STACK -> 0 2 3 6 8 
Input -> ; d $ 
ACTION -> S11

STACK -> 0 2 3 6 8 11 
Input -> d $ 
ACTION -> S12

STACK -> 0 2 3 6 8 11 12 
Input -> $ 
ACTION -> R0

STACK -> 0 1 
Input -> $ 
ACTION -> Accept

Input accepted.

```



## Running the Code

1. Compile the code using a C++ compiler.
2. Run the compiled executable.
3. Check the parsing table in "parseTable.txt," the CSV file in "parseTableCSV.csv," and the visualization in "graph.png."

## Note

- The code handles shift-reduce and reduce-reduce conflicts, providing detailed error messages when conflicts occur.
- The parsing process can be visualized using the generated PNG image ("graph.png").
- Detailed parsing information is logged in "Parsing Output.txt."

Ensure you have the necessary libraries and tools installed for the code to work correctly.![App Screenshot](https://drive.google.com/uc?export=view&id=1DVFJMVdUThb1qSPWgJIzrHfDNFechUbo)