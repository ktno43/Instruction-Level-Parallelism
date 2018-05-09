/*****************************************
 Kyle Nguyen
 COMP 222
 Spring 2018
 Prof. Robert McIlhenny
 T/R 3:30 P.M - 4:45 P.M

 Assignment #5: Instruction-Level Parallelism
 Source.c

 Version 4.0

 The program works as expected and 
 calculates the performance of a program
 with dependent arithmetic instructions
 by simulating the execution on a
 i) 6-stage pipline architecture
 ii) 6-stage superscalar architecutre of 
 degree 2

 It calculates the performance of a set 
 of arithmetic register assignment 
 statements and prints out the aligned 
 pipeline instructions based on the menu
 of options.

 It follows the specifications and 
 guidelines along with the added option 
 c) Calculate total cycle count on a
 6-stage superscalar architecture
 and yields the correct results.
 *****************************************/
#include <stdio.h>
#include <stdlib.h>
#define NUM_STAGES 6

 // Function Declarations
void displayMenu(void);
void setParamaters(void);
void calcPipe(void);
void calcSuper(void);
void printInstr(void);
void freeStuff(void);

// Global variables
int g_nInstrCount;

// Structure for scoreboard node
struct scoreboardNode
{
	int m_nDestReg; // Field for destination register
	int m_nSrcReg1; // Field for source 1 register
	int m_nSrcReg2; // Field for source 2 register
	int m_nDelay; // Field for delay
} *scoreboard = NULL; // Pointer to structure for creating dynamic array of nodes/instructions

typedef struct scoreboardNode instr; // Define variable row for scoreboard node

int main()
{
	displayMenu(); // Function call to Display Menu of Options

	printf("\n\nProgram Ending . . .\n");  //Program Ending
	printf("Press ENTER to close --->  ");
	getchar();
	getchar();

	return EXIT_SUCCESS;
}

// Display menu of options
void displayMenu()
{
	char cInput = '\0'; // Variable for input data

	while (cInput != 'd')
	{
		printf("Instruction-level parallelism:\n");
		printf("------------------------------\n");
		printf("a) Enter instructions\n");
		printf("b) Calculate total cycle count on a 6-stage pipelined architecture\n");
		printf("c) Calculate total cycle count on a 6-stage superscalar architecture\n");
		printf("d) Quit program\n\n");

		printf("Enter selection:  ");
		scanf(" %c", &cInput); // Get the character pressed from user

		switch (cInput)
		{
			case 'a':
				setParamaters(); // Function call to assign values to parameters
				break;

			case 'b':
				calcPipe(); // Function call to Calculate delay of set of instructions on a 6-stage pipeline architecture
				break;

			case 'c':
				calcSuper(); // Function call Calculate delay of set of instructions on a 6-stage superscalar architecture
				break;

			case 'd':
				freeStuff(); // Function call to free dynamically allocated memory and exit the program
				return;

			default:
				printf("Invalid input. \n"); // Invalid character read
		}
		printf("\n\n");
		getchar();
	}
	return;
}

// Function to set parameters
void setParamaters()
{
	char rInstrString[9]; // Array of characters to store user input

	// Prompt for total number of instructions 
	printf("\nEnter total number of instructions: ");
	scanf("%d", &g_nInstrCount);
	printf("\n");


	scoreboard = (instr *)malloc((g_nInstrCount + 1) * sizeof(instr)); 	// Allocate memory to hold a set of instructions based on the total number of instructions + 1, instruction 0 will be used for dependency checking 


	scoreboard[0].m_nDestReg = -1; // Initialize instruction 0's destination register to -1 to prevent false RAW dependency w/ instruction 2 
	scoreboard[1].m_nDelay = 0; // Initialize instruction 1's delay to 0 because it will always be 0

	/* For each instruction, prompt for user input with instruction number, such as: 1)
	 and read instruction as a string and store at proper field of appropriate index within dynamic array of instructions */
	int i = 0;
	for (i = 1; i <= g_nInstrCount; i++) // For each instruciton prompt the user with instruction number followed by an instruction string that will store appropriately
	{
		printf("%d) ", i);
		scanf("%s", rInstrString);

		scoreboard[i].m_nDestReg = rInstrString[1]; // Location of destination register
		scoreboard[i].m_nSrcReg1 = rInstrString[4]; // Location of source 1 register
		scoreboard[i].m_nSrcReg2 = rInstrString[7]; // Location of source 2 register
	}

	return;
}

// Calculate delay of set of instructions on a 6-stage pipeline architecture
void calcPipe()
{
	int nTotCycles = 0; // Variable for total cycles taken
	int bFlag_Overlap = 0; // Flag for overlap

	int i = 0;
	for (i = 2; i <= g_nInstrCount; i++) // For each instruction i from 2 to the number of instructions check for dependncies
	{
		int nDelay = 0; // Delay will start at 0 for each iteration

		// If i depends on instruction (i - 2)
		if (scoreboard[i - 2].m_nDestReg == scoreboard[i].m_nSrcReg1 || scoreboard[i - 2].m_nDestReg == scoreboard[i].m_nSrcReg2)
		{
			// If i depends on instruction (i - 2) & no overlap, delay is 1
			if (bFlag_Overlap == 0)
			{
				nDelay = 1;
				bFlag_Overlap = 1;
			}

			// Else overlap of dependencies 
			else
			{
				nDelay = 0;
				bFlag_Overlap = 0;
			}
		}

		// No overlap dependencies
		else
			bFlag_Overlap = 0;

		// If i depends on instruction (i - 1), delay is 2
		if (scoreboard[i - 1].m_nDestReg == scoreboard[i].m_nSrcReg1 || scoreboard[i - 1].m_nDestReg == scoreboard[i].m_nSrcReg2)
		{
			nDelay = 2;
			bFlag_Overlap = 1;
		}

		scoreboard[i].m_nDelay = scoreboard[i - 1].m_nDelay + nDelay + 1; // Calculate individual delay for current instruction
	}

	nTotCycles = scoreboard[g_nInstrCount].m_nDelay + NUM_STAGES; // Calculate total cycles 
	printf("\nTotal number of cycles: %d", nTotCycles); // Print total cycles
	printInstr(); // Print pipeline architecture

	return;
}

// Calculate delay of set of instructions on a 6-stage superscalar architecture
void calcSuper()
{
	int nTotCycles = 0; // Variable for total number of cycles taken
	int nPipeDelay = 1; // Pipe delay for superscalar architecture
	int bFlag_Overlap1 = 0; // Overlap flag 1
	int bFlag_Overlap2 = 0; // Overlap flag 2
	int bFlag_Overlap3 = 0; // Overlap flag 3
	int bFlag_Overlap4 = 0; // Overlap flag 4
	int bFlag_Overlap5 = 0; // Overlap flag 5

	int i = 0;
	for (i = 2; i <= g_nInstrCount; i++) // For each instruction i from 2 to the total number of instructions, check dependencies
	{
		int nDelay = 0; // Delay will start at 0 for each iteration

		nPipeDelay = 1 - nPipeDelay; // Flip the flag used for expected/non-expected pipeline delay to resemble delay pattern for superscalar: 
									// if flag is 0, set it to 1, if it is 1, reset it to 0

		if (i >= 5) // To be sure no out of bounds error
		{
			// If i depends on instruction (i - 5)
			if (scoreboard[i - 5].m_nDestReg == scoreboard[i].m_nSrcReg1 || scoreboard[i - 5].m_nDestReg == scoreboard[i].m_nSrcReg2)
			{
				nPipeDelay = 1; // Pipeline delay will start at 1 for each iteration

				// If i depends on instruction (i - 5) & no overlap, delay is 0
				if ((bFlag_Overlap1 == 0) && (bFlag_Overlap2 == 0) && (bFlag_Overlap3 == 0) && (bFlag_Overlap4 == 0) && (bFlag_Overlap5 == 0))
				{
					nDelay = 0;
					bFlag_Overlap5 = 1;
				}

				// Else overlap of dependencies, delay is 0
				else
				{
					nDelay = 0;
					bFlag_Overlap5 = 0;
				}
			}

			// No overlap dependencies, delay is 0
			else
				bFlag_Overlap5 = 0;
		}

		if (i >= 4) // To be sure no out of bounds error
		{
			// If i depends on instruction (i - 4)
			if (scoreboard[i - 4].m_nDestReg == scoreboard[i].m_nSrcReg1 || scoreboard[i - 4].m_nDestReg == scoreboard[i].m_nSrcReg2)
			{
				nPipeDelay = 1; // Pipeline delay will start at 1 for each iteration

				// If i depends on instruction (i - 4) & no overlap
				if ((bFlag_Overlap1 == 0) && (bFlag_Overlap2 == 0) && (bFlag_Overlap3 == 0) && (bFlag_Overlap4))
				{

					// If there is an expected pipeline delay, delay is 1
					if (nPipeDelay == 1)
					{
						nDelay = 1;
					}

					// Else there is no expected pipeline delay, delay is 0
					else
					{
						nDelay = 0;
					}

					// Toggle overlap flag
					bFlag_Overlap4 = 1;
				}

				// Else overlap of dependencies, delay is 0
				else {
					nDelay = 0;
					bFlag_Overlap4 = 0;
				}
			}

			// No overlap dependencies, delay is 0
			else
				bFlag_Overlap4 = 0;
		}

		if (i >= 3) // To be sure no out of bounds error
		{
			// If i depends on instruction (i - 3)
			if (scoreboard[i - 3].m_nDestReg == scoreboard[i].m_nSrcReg1 || scoreboard[i - 3].m_nDestReg == scoreboard[i].m_nSrcReg2)
			{
				nPipeDelay = 1; // Pipeline delay will start at 1 for each iteration

				// If i depends on instruction (i - 3) & no overlap
				if ((bFlag_Overlap1 == 0) && (bFlag_Overlap2 == 0) && (bFlag_Overlap3 == 0))
				{
					// If there is an expected pipeline delay, delay is 1
					if (nPipeDelay == 1)
					{
						nDelay = 1;
					}


					// Else there is no expected pipeline delay, delay is 0
					else
					{
						nDelay = 0;
					}

					// Toggle overlap flag
					bFlag_Overlap3 = 1;
				}

				// Else overlap of dependencies, delay is 0
				else {
					nDelay = 0;
					bFlag_Overlap3 = 0;
				}
			}

			// No overlap dependencies, delay is 0
			else
				bFlag_Overlap3 = 0;
		}

		// If i depends on instruction (i - 2)
		if (scoreboard[i - 2].m_nDestReg == scoreboard[i].m_nSrcReg1 || scoreboard[i - 2].m_nDestReg == scoreboard[i].m_nSrcReg2)
		{
			nPipeDelay = 1; // Pipeline delay will start at 1 for each iteration

			// If i depends on instruction (i - 2) & no overlap
			if ((bFlag_Overlap1 == 0) && (bFlag_Overlap2 == 0))
			{
				// If there is an expected pipeline delay, delay is 2
				if (nPipeDelay == 1)
				{
					nDelay = 2;
				}


				// Else there is no expected pipeline delay, delay is 0
				else
				{
					nDelay = 0;
				}

				// Toggle overlap flag
				bFlag_Overlap2 = 1;
			}

			// Else overlap of dependencies, delay is 0
			else {
				nDelay = 0;
				bFlag_Overlap2 = 0;
			}
		}

		// No overlap dependencies, delay is 0
		else
			bFlag_Overlap2 = 0;

		// If i depends on instruction (i - 1)
		if (scoreboard[i - 1].m_nDestReg == scoreboard[i].m_nSrcReg1 || scoreboard[i - 1].m_nDestReg == scoreboard[i].m_nSrcReg2)
		{
			nPipeDelay = 1; // Pipeline delay will start at 1 for each iteration

			// If i depends on instruction (i - 1) & no overlap
			if (bFlag_Overlap1 == 0)
			{
				// If there is an expected pipeline delay, delay is 2
				if (nPipeDelay == 1)
				{
					nDelay = 2;
				}

				// Else there is no expected pipeline delay, delay is 0
				else
				{
					nDelay = 0;
				}

				// Toggle overlap flag
				bFlag_Overlap1 = 1;
			}

			// Else overlap of dependencies, delay is 0
			else {
				nDelay = 0;
				bFlag_Overlap1 = 0;
			}
		}

		// No overlap dependencies, delay is 0
		else 
			bFlag_Overlap1 = 0;


		scoreboard[i].m_nDelay = scoreboard[i - 1].m_nDelay + nDelay + nPipeDelay; // Calculate individual delay for current instruction
	}

	nTotCycles = scoreboard[g_nInstrCount].m_nDelay + NUM_STAGES; // Calculate total cycles 
	printf("\nTotal number of cycles: %d", nTotCycles); // Print total cycles
	printInstr(); // Print superscalar architecture

	return;
}

// Print chart of instructions with delays
void printInstr()
{
	printf("\n");
	int i = 0;
	for (i = 1; i <= g_nInstrCount; i++) // For each instruction, it will print & align the 6-stages with proper tabbings (delays)
	{
		printf("%d)", i);

		int j = 0;
		for (j = 0; j < scoreboard[i].m_nDelay; j++) // Print the number of tabs necessary according to the delay of the node
		{
			printf("\t");
		}

		printf("FI\tDI\tCO\tFO\tEI\tWO\n");
	}

	return;
}

// Deallocate memory
void freeStuff()
{
	if (scoreboard != NULL) // If scoreboard is not null
	{
		free(scoreboard); // Free memory
		scoreboard = NULL; // Set back to null
	}

	return;
}
