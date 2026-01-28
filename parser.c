#include <stdio.h>
#include <string.h>
#include  <stdlib.h>

/*Given a character it will return it's opposite 
closing brace or opening brace. AKA what is the
expected value*/
char expectedCharacter(char character){
    char output; 

    switch(character){
        case '{': 
            output = '}'; 
            break;
        case '(': 
            output = ')'; 
            break;
        case '[':
            output = ']'; 
            break;
        case '}':
            output = '{';
            break;
        case ')':
            output = '(';
            break;
        case ']':
            output = '[';
            break;
        default: 
            output = '?'; 
            break;
    }

    return output;
}

void parse(FILE *filePointer){
    char *currentLinePointer = NULL;
    size_t currentLineLen = 0;
    char stack[256];
    int stackPosition = -1; 
    int lineNumber = 1;

    while (getline(&currentLinePointer, &currentLineLen, filePointer) != -1) {
        int i = 0;

        //Process a line 
        while (currentLinePointer[i]) {
            char currentChar = currentLinePointer[i];
            
            //Checks for comment and skips the rest of line if detects single line
            if(currentChar == '/' && currentLinePointer[i+1] == '/') {
                 break;
            }

            //Checks for a block comment. Loops through until finds closing symbol then moves to the next line
            if(currentChar == '/' && currentLinePointer[i+1] == '*') {
                i += 2;
                while(currentLinePointer[i] && !(currentLinePointer[i] == '*' && currentLinePointer[i+1] == '/')) {
                    i++;
                }

                //Checks to see if we're at end of line and if not skip to the closing symbol and move to next line
                if(currentLinePointer[i]){
                    i += 2;
                }
                continue;
            }

            // Skip strings
            if(currentChar == '"') {
                i++; 
                while(currentLinePointer[i]) {
                    if(currentLinePointer[i] == '"' && currentLinePointer[i-1] != '\\') {
                        i++; 
                        break;
                    }
                    i++;
                }
                continue;
            }

            // Skip character literals
            if(currentChar == '\'') {
                i++; 
                while(currentLinePointer[i]) {
                    if(currentLinePointer[i] == '\'' && currentLinePointer[i-1] != '\\') {
                        i++; 
                        break;
                    }
                    i++;
                }
                continue;
            }

            // Push opening symbols
            if(currentChar == '{' || currentChar == '(' || currentChar == '['){
                stack[++stackPosition] = currentChar;
            }

            // Look at closing symbols and check for matching from the stack. Only pop stack if there is not a mismatch
            else if(currentChar == '}' || currentChar == ')' || currentChar == ']'){

                //Checks if there is something extra 
                if (stackPosition < 0) {
                    printf("ERROR: missing %c or extra %c, %d.\n",expectedCharacter(currentChar),currentChar,lineNumber);
                    return;
                }

                char topOfStack = stack[stackPosition--];

                if((topOfStack == '{' && currentChar != '}') || (topOfStack == '(' && currentChar != ')') || (topOfStack == '[' && currentChar != ']')){ 

                    printf("ERROR: found a %c, but expected %c, line %d.\n",currentChar, expectedCharacter(topOfStack), lineNumber);
                    return;
                }
            }

            i++;
        }
        lineNumber++;
    }

    if (stackPosition >= 0) {
        printf("ERROR: missing }, ), or ] at EOF.\n");
    }

    free(currentLinePointer);
}


int main(){
    char *fileName = NULL;
    size_t fileNameLen = 0;
    FILE *filePointer;

    printf("Please enter the name of the file: ");
    int bytesRead = getline(&fileName, &fileNameLen, stdin);

    //Removes the new line character at the end
    if (fileName[bytesRead - 1] == '\n') {
        fileName[bytesRead - 1] = '\0';
    }

    filePointer = fopen(fileName,"r");

    if(filePointer == NULL){
        printf("ERROR: Invalid file name!");
    }else{
        parse(filePointer);
    }


    return 0;
}