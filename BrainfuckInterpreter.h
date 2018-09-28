#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int from16to10(char c);

struct lineAndColumn bracket(char* prog, int programSize, struct lineAndColumn lAC);

int bracketCheck(char* prog, int programSize);

void manual();

int main(int argc, char** argv);