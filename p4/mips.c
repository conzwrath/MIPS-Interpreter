#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "hashtable.h"
#include "mips.h"

/************** BEGIN HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ***************/
const int R_TYPE = 0;
const int I_TYPE = 1;
const int MEM_TYPE = 2;
const int UNKNOWN_TYPE = 3;

/**
 * Return the type of instruction for the given operation
 * Available options are R_TYPE, I_TYPE, MEM_TYPE, UNKNOWN_TYPE
 */
int get_op_type(char *op) {
    const char *r_type_op[] = { "addu", "subu", "and", "or", "xor", "nor", "slt", "sltu" , "movn", "movz", "sllv", "srlv", "srav" };
    const char *i_type_op[] = { "addiu", "andi", "ori", "xori", "slti", "sltiu", "sll", "srl", "sra", "lui" };
    const char *mem_type_op[] = { "lw", "lb", "lbu", "sw", "sb" };
    for (int i = 0; i < (int) (sizeof(r_type_op) / sizeof(char *)); i++) {
        if (strcmp(r_type_op[i], op) == 0) {
            return R_TYPE;
        }
    }
    for (int i = 0; i < (int) (sizeof(i_type_op) / sizeof(char *)); i++) {
        if (strcmp(i_type_op[i], op) == 0) {
            return I_TYPE;
        }
    }
    for (int i = 0; i < (int) (sizeof(mem_type_op) / sizeof(char *)); i++) {
        if (strcmp(mem_type_op[i], op) == 0) {
            return MEM_TYPE;
        }
    }
    return UNKNOWN_TYPE;
}
/*************** END HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ****************/

registers_t *registers;
// TODO: create any additional variables to store the state of the interpreter
hashtable_t *mem;

void init(registers_t *starting_registers) {
    registers = starting_registers;
    // TODO: initialize any additional variables needed for state
    mem = ht_init(32);
}

// TODO: create any necessary helper functions

// Converts from an integer to an array of 4 bytes
int *  word_to_bytes(int word) {
    int* bytes = (int *)malloc(4 * sizeof(int));
    for(int i = 3; i >= 0; i--) {
        bytes[i] = (((int) (word >> (8*(3-i)))) & 0xff);
    }
    return bytes;
}

// converts from an array of bytes to an integer value
int bytes_to_word(int * bytes) {
    int word = 0;
    for(int i = 0; i <=3; i++) {
        word = word + (bytes[i] << (8*(3-i)));
    }
    return word;
}

// Removes the spaces from a string
char* removespaces(char* instruction) {
    char* blank = (char*)malloc(100*sizeof(char));
    int c = 0;
    int d = 0;
    while (instruction[c] != '\0') {
        if (instruction[c] != ' ') {
            blank[d] = instruction[c];
            d++;
        }
        c++;
    }
    blank[d] = '\0';
    return blank;
}

// Returns an array of ints where the first element is the first register, the second element is the second register
// and the third element is the third register or the shift amount
int* R_stuff(char* instruction) {
    int r1 = 0;
    int r2 = 0;
    int r3_or_shamt = 0;
    int comma1;
    int comma2;
    int end;
    int count = 0;
    while(instruction[count] != ',') {
        count++;
    }
    comma1 = count;
    count = comma1 + 1;
    while(instruction[count] != ',') {
        count++;
    }
    comma2 = count;
    count = comma2 + 1;
    while(instruction[count] != '\0') {
        count++;
    }
    end = count;
    count = comma1 - 1;
    int multi = 1;
    while(instruction[count] != '$') {
        r1 = r1 + (multi*((int)(instruction[count]) - '0'));
        count--;
        multi = multi * 10;
    }
    count = comma2 - 1;
    multi = 1;
    while(instruction[count] != '$') {
        r2 = r2 +  (multi*((int)(instruction[count]) - '0'));
        count--;
        multi = multi * 10;
    }
    count = end - 1;
    multi = 1;
    int hexflag = 0;
    while(instruction[count] != ',') {
        if (instruction[count] == 'x') {
            hexflag = 1;
        }
        count--;
    }
    count = end - 1;
    multi = 1;
    if (hexflag == 0) {
        while(instruction[count] != ',' && instruction[count] != '-') {
            if(instruction[count] != '$') {
                r3_or_shamt = r3_or_shamt + (multi*((int)(instruction[count]) - '0'));
                count--;
                multi = multi * 10;
            }
            else {
                count--;
            }
        }
    }
    else {
         count = comma2 + 1;
         char* hexstring = &instruction[count];
         r3_or_shamt = (int)strtol(hexstring, NULL, 0);
    }
    int* values = (int*)malloc(3*sizeof(int));
    values[0] = r1;
    values[1] = r2;
    values[2] = r3_or_shamt;
    count = comma2 + 1;
    if(instruction[count] == '-') {
        values[2] = (-1) * values[2];
    }
    return values;
}

//returns an array of ints just like the R_stuff function but it has a special case for lui because it only has
// two inputs.
int* I_stuff(char* instruction, char* op) {
    if(op[0] == 'l' && op[1] == 'u' && op[2] == 'i') {
        int r1 = 0;
        int r2 = 0;
        int comma1;
        int end;
        int count = 0;
        while(instruction[count] != ',') {
            count++;
        }
        comma1 = count;
        count = comma1 + 1;
        while(instruction[count] != '\0') {
            count++;
        }
        end = count;
        count = comma1 - 1;
        int multi = 1;
        while(instruction[count] != '$') {
            r1 = r1 + (multi*((int)(instruction[count]) - '0'));
            count--;
            multi = multi * 10;
        }
        count = end - 1;
        multi = 1;
        int hexflag = 0;
        while(instruction[count] !=',') {
            if (instruction[count] == 'x') {
                hexflag = 1;
            }
            count--;
        }
        count = end - 1;
        if (hexflag == 0) {
            while(instruction[count] != ',') {
                if(instruction[count] != '$') {
                    r2 = r2 + (multi*((int)(instruction[count]) - '0'));
                    count--;
                    multi = multi * 10;
                }
                else {
                    count--;
                }
            }
        }
        else {
             count = comma1 + 1;
             char* hexstring = &instruction[count];
             r2 = (int)strtol(hexstring, NULL, 0);
        }
        int* values = (int*)malloc(3*sizeof(int));
        values[0] = r1;
        values[1] = r2;
        values[2] = 0;
        return values;
    }
    else {
        return R_stuff(instruction);
    }
}

// same but for mmemory instructions
int* m_stuff(char* instruction) {
    int r1 = 0;
    int offset = 0;
    int r2 = 0;
    int comma1 = 0;
    int par1 = 0;
    int par2 = 0;
    int count = 0;
    while(instruction[count] != ',') {
        count++;
    }
    comma1 = count;
    while(instruction[count] != '(') {
        count++;
    }
    par1 = count;
    while(instruction[count] != ')') {
        count++;
    }
    par2 = count;
    count = comma1 - 1;
    int multi = 1;
    while(instruction[count] != '$') {
        r1 = r1 + (multi*((int)(instruction[count]) - '0'));
        count--;
        multi = multi * 10;
    }
    count = comma1;
    multi = 1;
    int hexflag = 0;
    while(instruction[count] !='\0') {
        if (instruction[count] == 'x') {
            hexflag = 1;
        }
        count++;
    }
    if (hexflag == 0) {
        count = par1 - 1;
        int multi = 1;
        while(instruction[count] != ',') {
            offset = offset + (multi*((int)(instruction[count]) - '0'));
            count--;
            multi = multi * 10;
        }
    }
    else {
        count = comma1 + 1;
        char* hexstring = &instruction[count];
        offset = (int)strtol(hexstring, NULL, 0);
    }
    count = par2 - 1;
    multi = 1;
    while (instruction[count] != '$') {
        r2 = r2 + (multi*((int)(instruction[count]) - '0'));
        count--;
        multi = multi * 10;
    }
    int* values = (int*)malloc(3*sizeof(int));
    values[0] = r1;
    values[1] = offset;
    values[2] = r2;
    return values;
}

void step(char *instruction) {
    // Extracts and returns the substring before the first space character,
    // by replacing the space character with a null-terminator.
    // `instruction` now points to the next character after the space
    // See `man strsep` for how this library function works
    char *op = strsep(&instruction, " ");
    // Uses the provided helper function to determine the type of instruction
    int op_type = get_op_type(op);
    // Skip this instruction if it is not in our supported set of instructions
    if (op_type == UNKNOWN_TYPE) {
        return;
    }

    // TODO: write logic for evaluating instruction on current interpreter state
    char* instruction_nospace = removespaces(instruction);
    if (op_type == R_TYPE) {
        int* values = R_stuff(instruction_nospace);
        if(op[0] == 'a' && op[1] == 'd' && op[2] == 'd' && op[3] == 'u') {
            registers -> r[values[0]] = registers -> r[values[1]] + registers -> r[values[2]];
        }
        if(op[0] == 's' && op[1] == 'u' && op[2] == 'b' && op[3] == 'u') {
            registers -> r[values[0]] = registers -> r[values[1]] - registers -> r[values[2]];
        }
        if(op[0] == 'a' && op[1] == 'n' && op[2] == 'd') {
            registers -> r[values[0]] = registers -> r[values[1]] & registers -> r[values[2]];
        }
        if(op[0] == 'o' && op[1] == 'r') {
            registers -> r[values[0]] = registers -> r[values[1]] | registers -> r[values[2]];
        }
        if(op[0] == 'x' && op[1] == 'o' && op[2] == 'r') {
            registers -> r[values[0]] = registers -> r[values[1]] ^ registers -> r[values[2]];
        }
        if(op[0] == 'n' && op[1] == 'o' && op[2] == 'r') {
            registers -> r[values[0]] = ~(registers -> r[values[1]] ^ registers -> r[values[2]]);
        }
        if(op[0] == 's' && op[1] == 'l' && op[2] == 't') {
            registers -> r[values[0]] = (registers -> r[values[1]] < registers -> r[values[2]]);
        }
        if(op[0] == 'm' && op[1] == 'o' && op[2] == 'v' && op[3] == 'z') {
            if(registers -> r[values[2]] == 0) {
                registers -> r[values[0]] = registers -> r[values[1]];
            }
        }
    }
    else if(op_type == I_TYPE) {
        int* values = I_stuff(instruction_nospace, op);
        if(op[0] == 'a' && op[1] == 'd' && op[2] == 'd' && op[3] == 'i' && op[4] == 'u') {
            registers -> r[values[0]] = registers -> r[values[1]] + values[2];
        }
        if(op[0] == 'a' && op[1] == 'n' && op[2] == 'd' && op[3] == 'i') {
            registers -> r[values[0]] = registers -> r[values[1]] & values[2];
        }
        if(op[0] == 'o' && op[1] == 'r' && op[2] == 'i') {
            registers -> r[values[0]] = registers -> r[values[1]] | values[2];
        }
        if(op[0] == 'x' && op[1] == 'o' && op[2] == 'r' && op[3] == 'i') {
            registers -> r[values[0]] = registers -> r[values[1]] ^ values[2];
        }
        if(op[0] == 's' && op[1] == 'l' && op[2] == 't' && op[3] == 'i') {
            registers -> r[values[0]] = (registers -> r[values[1]]) < values[2];
        }
        if(op[0] == 's' && op[1] == 'l' && op[2] == 'l') {
            registers -> r[values[0]] = (registers -> r[values[1]]) << values[2];
        }
        if(op[0] == 's' && op[1] == 'r' && op[2] == 'a') {
            registers -> r[values[0]] = (registers -> r[values[1]]) >> values[2];
        }
        if(op[0] == 'l' && op[1] == 'u' && op[2] == 'i') {
            registers -> r[values[0]] = values[1] << 16;
        }
    }
    else if(op_type = MEM_TYPE) {
        int* values = m_stuff(instruction_nospace);
        if(op[0] == 's' && op[1] == 'w') {
            int* bytes = word_to_bytes(registers -> r[values[0]]);
            ht_add(mem, values[1] + (registers -> r[values[2]]) + 0, bytes[3]);
            ht_add(mem, values[1] + (registers -> r[values[2]]) + 1, bytes[2]);
            ht_add(mem, values[1] + (registers -> r[values[2]]) + 2, bytes[1]);
            ht_add(mem, values[1] + (registers -> r[values[2]]) + 3, bytes[0]);
        }
       if(op[0] == 'l' && op[1] == 'w') {
            int bytes[4] = {0, 0, 0, 0};
            bytes[3] = ht_get(mem, values[1] + (registers -> r[values[2]]));
            bytes[2] = ht_get(mem, values[1] + registers -> r[values[2]] + 1);
            bytes[1] = ht_get(mem, values[1] + registers -> r[values[2]] + 2);
            bytes[0] = ht_get(mem, values[1] + registers -> r[values[2]] + 3);
            int word = bytes_to_word(bytes);
            registers -> r[values[0]] = word;
       }
       if(op[0] == 's' && op[1] == 'b') {
            int byte = (registers -> r[values[0]] & 0xff);
            ht_add(mem, values[1] + (registers -> r[values[2]]), byte);
       }
       if(op[0] == 'l' && op[1] == 'b') {
            int byte = ht_get(mem, values[1] + (registers -> r[values[2]]));
            registers -> r[values[0]] = byte;
       }
       if(op[0] == 'l' && op[1] == 'b' && op[2] == 'u') {
            unsigned int byte = ht_get(mem, values[1] + (registers -> r[values[2]]));
            registers -> r[values[0]] = byte;
       }
    }
}
