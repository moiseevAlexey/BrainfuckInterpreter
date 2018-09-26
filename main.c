#include <stdio.h>
#include <string.h>
#include <stdlib.h>



struct lineAndColumn
{
	int line;
	int column;
	int p;
};



int from16to10(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	if (c >= 'a' && c <= 'f')
	{
		return c - 'a' + 10;
	}
	if (c >= 'A' && c <= 'F')
	{
		return c - 'A' + 10;
	}
	return -1;
}



struct lineAndColumn bracket(char* prog, int programSize, struct lineAndColumn lAC)
{
	struct lineAndColumn tLAC = lAC;

	while (lAC.p < programSize)
	{
		if (prog[lAC.p] == '[')
		{
			lAC.p++;
			lAC.column++;
			lAC = bracket(prog, programSize, lAC);
		}
		else if (prog[lAC.p] == ']')
		{
			return lAC;
		}

		if (prog[lAC.p] == '\n')
		{
			lAC.line++;
			lAC.column = 0;
		}
		else
		{
			lAC.column++;
		}
		lAC.p++;
	}

	fprintf(stderr, "Bracket without pair in (%i;%i)\n", tLAC.line, tLAC.column);
	lAC.column = -1;
	return lAC;
}



int bracketCheck(char* prog, int programSize)
{
	struct lineAndColumn lAC;
	lAC.line = 0;
	lAC.column = 0;
	lAC.p = 0;

	while (lAC.p < programSize)
	{
		if (prog[lAC.p] == '[')
		{
			lAC.p++;
			lAC.column++;
			lAC = bracket(prog, programSize, lAC);
			if (lAC.column == -1)
			{
				return 0;
			}
		}
		else if (prog[lAC.p] == ']')
		{
			fprintf(stderr, "Bracket without pair in (%i;%i)\n", lAC.line, lAC.column);
			return 0;
		}

		if (prog[lAC.p] == '\n')
		{
			lAC.line++;
			lAC.column = 0;
		}
		else
		{
			lAC.column++;
		}
		lAC.p++;
	}

	return 1;
}



int main(int argc, char** argv)
{
	errno_t err;

	if (argc < 4)
	{
		fprintf(stderr, "Too few arguments\n");
	}

	FILE* programFile;
	err = fopen_s(&programFile, argv[1], "r");
	if (err != 0)
	{
		printf("%s", argv[1]);
		fprintf(stderr, "Program file error\n");
		return 0;
	}

	FILE* input;
	if (!strcmp(argv[2], "-"))
	{
		input = stdin;
	}
	else
	{
		err = fopen_s(&input, argv[2], "r");
		if (err != 0)
		{
			fprintf(stderr, "Input file error\n");
			return 0;
		}
	}

	FILE* output;
	if (!strcmp(argv[3], "-"))
	{
		output = stdout;
	}
	else
	{
		err = fopen_s(&output, argv[3], "w");
		if (err != 0)
		{
			fprintf(stderr, "Output file error\n");
			return 0;
		}
	}

	int sModeOn = 0;
	int cModeOn = 0;
	int dModeOn = 0;
	FILE* debug = NULL;

	int i;
	int additionalArgumets = argc - 4;
	for (i = 0; i < additionalArgumets; i++)
	{
		if (!strcmp(argv[4 + i], "-s"))
		{
			sModeOn = 1;
		}
		else if (!strcmp(argv[4 + i], "-c"))
		{
			cModeOn = 1;
		}
		else if (!strcmp(argv[4 + i], "-d"))
		{
			dModeOn = 1;
			i++;
			if (i == additionalArgumets)
			{
				fprintf(stderr, "Debug file not entered\n");
				return 0;
			}
			err = fopen_s(&debug, argv[4 + i], "w");
			if (err != 0)
			{
				fprintf(stderr, "Debug file error\n");
				return 0;
			}
		}
		else
		{
			fprintf(stderr, "Wrong argument in %i position\n", 4 + i);
			return 0;
		}
	}

	long programSize = 1;
	char* program = (char*) malloc(programSize * sizeof(char));
	if (program == NULL)
	{
		fprintf(stderr, "Not enough memory");
		return 0;
	}
	unsigned char tempC;
	long ui = 0;
	while ((tempC = getc(programFile)) != (unsigned char)EOF)
	{
		if (cModeOn && tempC == '/')
		{
			if (tempC = getc(programFile) == '/')
			{
				tempC = getc(programFile);
				while ((tempC != '\n') && (tempC != (unsigned char)EOF))
				{
					tempC = getc(programFile);
				}
				if (tempC == (unsigned char)EOF)
				{
					break;
				}
			}
		}

		if (ui >= programSize)
		{
			programSize *= 2;
			program = (char*) realloc(program, programSize * sizeof(char));
			if (program == NULL)
			{
				fprintf(stderr, "Not enough memory");
				return 0;
			}
		}

		program[ui++] = tempC;
	}
	programSize = ui;
	program = (char*) realloc(program, programSize * sizeof(char));
	if (program == NULL)
	{
		fprintf(stderr, "Not enough memory");
		return 0;
	}
	fclose(programFile);

	if (!bracketCheck(program, programSize))
	{
		return 0;
	}

	unsigned long tapeSize = 1;
	unsigned char* tape = (char*) malloc(programSize * sizeof(char));
	if (tape == NULL)
	{
		fprintf(stderr, "Not enough memory");
		return 0;
	}
	tape[0] = 0;

	int programI;
	unsigned int tapeI = 0;
	int line = 0;
	int tLine;
	int symbolInLine;
	int brackets = 0;
	int globalBrackets = 0;
	int tBracket;
	unsigned char temp;
	int isTempCHaveInformation = 0;
	long step = 0;

	for (programI = 0; programI < programSize; programI++)
	{
		switch (program[programI])
		{
		case '>':
			tapeI++;
			if (tapeI >= tapeSize)
			{
				tapeSize *= 2;
				tape = (char*) realloc(tape, tapeSize * sizeof(char));
				if (tape == NULL)
				{
					fprintf(stderr, "Not enough memory");
					return 0;
				}
				memset(tape + tapeSize / 2, 0, tapeSize / 2);
			}
			if (dModeOn)
			{
				fprintf(debug, "%i. > %i:%i->%i:%i\n", step, tapeI - 1, tape[tapeI - 1], tapeI, tape[tapeI]);
				step++;
			}
			break;
		case '<':
			if (tapeI == 0)
			{
				if (line == 0)
				{
					fprintf(stderr, "Tape cursor went beyond the border in (%i;%i)\n", 0, programI);
				}
				else
				{
					symbolInLine = 0;
					while (program[programI - symbolInLine - 1] != '\n')
					{
						symbolInLine++;
					}
					fprintf(stderr, "Tape cursor went beyond the border in (%i;%i)\n", line, symbolInLine);
				}
				return 0;
			}
			tapeI--;
			if (dModeOn)
			{
				fprintf(debug, "%i. < %i:%i->%i:%i\n", step, tapeI + 1, tape[tapeI + 1], tapeI, tape[tapeI]);
				step++;
			}
			break;
		case '+':
			tape[tapeI]++;
			if (dModeOn)
			{
				fprintf(debug, "%i. + %i:%i->%i\n", step, tapeI, tape[tapeI] - 1, tape[tapeI]);
				step++;
			}
			break;
		case '-':
			tape[tapeI]--;
			if (dModeOn)
			{
				fprintf(debug, "%i. - %i:%i->%i\n", step, tapeI, tape[tapeI] + 1, tape[tapeI]);
				step++;
			}
			break;
		case '.':
			if (sModeOn)
			{
				if ((tape[tapeI] >= 0 && tape[tapeI] <= 31) || (tape[tapeI] >= 128 && tape[tapeI] <= 255))
				{
					fprintf(output, "\\x%x", tape[tapeI]);
				}
				else
				{
					putc(tape[tapeI], output);
				}
			}
			else
			{
				putc(tape[tapeI], output);
			}
			break;
		case ',':
			if (sModeOn)
			{
				if (isTempCHaveInformation)
				{
					isTempCHaveInformation = 0;
				}
				else
				{
					tempC = getc(input);
				}
				if (tempC == '\\')
				{
					tempC = getc(input);
					if (tempC == '\\')
					{
						tape[tapeI] = '\\';
					}
					else if (tempC >= '0' && tempC <= '7')
					{
						temp = tempC - '0';
						for (i = 0; i < 2; i++)
						{
							tempC = getc(input);
							if (tempC >= '0' && tempC <= '7')
							{
								temp = temp * 8 + (tempC - '0');
							}
							else
							{
								isTempCHaveInformation = 1;
								break;
							}
						}
						tape[tapeI] = temp;
					}
					else if (tempC == 'x')
					{
						temp = 0;
						tempC = getc(input);
						if (from16to10(tempC) == -1)
						{
							fprintf(stderr, "Unresolved symbol in input file");
							return 0;
						}
						while (from16to10(tempC) != -1)
						{
							temp = (temp* 16 + from16to10(tempC)) % 256;
							tempC = getc(input);
						}
						isTempCHaveInformation = 1;
						tape[tapeI] = temp;
					}
					else
					{
						tape[tapeI] = tempC;
					}
				}
				else
				{
					tape[tapeI] = tempC;
				}
			}
			else
			{
				tape[tapeI] = getc(input);
			}
			break;
		case '[':
			if (tape[tapeI] == 0)
			{
				tLine = line;
				tBracket = programI;

				brackets++;
				while (brackets)
				{
					programI++;

					if (cModeOn)
					{
						if (program[programI] == '\n')
						{
							line++;
						}
					}

					if (program[programI] == '[')
					{
						brackets++;
					}
					if (program[programI] == ']')
					{
						brackets--;
					}
				}
			}
			break;
		case ']':
			if (tape[tapeI] != 0)
			{
				tLine = line;
				tBracket = programI;

				brackets++;
				while (brackets)
				{
					programI--;

					if (cModeOn)
					{
						if (program[programI] == '\n')
						{
							line--;
						}
					}

					if (program[programI] == '[')
					{
						brackets--;
					}
					if (program[programI] == ']')
					{
						brackets++;
					}
				}
			}
			break;
		case '\n':
			if (cModeOn)
			{
				line++;
				break;
			}
		case ' ':
			if (cModeOn)
			{
				break;
			}
		case '\t':
			if (cModeOn)
			{
				break;
			}
		case '\r':
			if (cModeOn)
			{
				break;
			}
		default:
			if (line == 0)
			{
				fprintf(stderr, "Unresolved symbol in (%i;%i)\n", 0, programI);
			}
			else
			{
				symbolInLine = 0;
				while (program[programI - symbolInLine - 1] != '\n')
				{
					symbolInLine++;
				}
				fprintf(stderr, "Unresolved symbol in (%i;%i)\n", line, symbolInLine);
			}
			return 0;
		}
	}

	free(program);
	free(tape);

	fclose(input);
	fclose(output);
	if (dModeOn)
	{
		fclose(debug);
	}

	return 1;
}