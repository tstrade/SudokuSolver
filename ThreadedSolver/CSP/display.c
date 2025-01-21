#include "CSP.h"

uint8_t SudokuBoard[NUM_SLOTS];

void showVars()
{
    printf("Variables: ");
    for (uint8_t var = 0; var < csp.nvars; var++)
        printf("%d  ", csp.variables[var]);
    printf("\n\n");
}

void showCurrentDomains()
{
    printf("Current domains are: \n");
    for (uint8_t slot = 0; slot < NUM_SLOTS; slot++)
        showVariableDomain(slot);
}

void showVariableDomain(uint8_t variable)
{
    printf("Slot %d's current domain: ", variable);
    for (uint8_t values = 0; values < NUM_VALUES; values++) {
        if (csp.domains[variable][values] != 0)
            printf("%d  ", csp.domains[variable][values]);
    }
    printf("\n");
}

void display()
{
    for (uint8_t slot = 0; slot < NUM_SLOTS; slot++)
        SudokuBoard[slot] = assignments[slot] + 48;

    printf("\n\n   ---------------------------------\n");

    for (uint8_t slot = 0; slot < NUM_SLOTS; slot += 9) {
        printf("  | %c  %c  %c  |  %c  %c  %c  |  %c  %c  %c |\n",
               SudokuBoard[slot], SudokuBoard[slot + 1], SudokuBoard[slot + 2],
               SudokuBoard[slot + 3], SudokuBoard[slot + 4], SudokuBoard[slot + 5],
               SudokuBoard[slot + 6], SudokuBoard[slot + 7], SudokuBoard[slot + 8]);

        if (slot == NUM_VALUES * 2 || slot == NUM_VALUES * 5) {
            // Only need two dividers (board is in thirds)
            printf("   ----------+-----------+----------\n");
        }
    }
    printf("   ---------------------------------\n\n");
}