#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct cpu    //	cpu components
{
    char ir[4];    //	instruction register
    int ic;        //	instruction counter
    int t;        //	toggle
    char r[4];    //	general register
};

/*	Global variables	*/
struct cpu os;
int word, byte, address, si;
char mem[100][4];    //	memory
FILE *fp1, *fp2;

/*	Function prototype	*/
void mos();

void read();        //	si=1
void write();        //	si=2
void terminate();    //	si=3
void load();

void start_execution();

void execute_user_program();

int to_int(char, char);    //	convert character to integer
void clear_memory();        //	clear memory after execution of one program

int main() {
    fp1 = fopen("input.txt", "r");
    fp2 = fopen("output.txt", "w");
    si = 3;    // initially calls terminate
    mos();
    return 0;
}

void mos() {
    switch (si) {
        case 1:
            read();
            break;
        case 2:
            write();
            break;
        case 3:
            terminate();
            break;
        default:
            break;
    }
}

void read() {
    int i, j;
    char data[41];
    for (i = 0; i < 40; i++) {
        data[i] = getc(fp1);
        if (data[i] == '\n') {
            if (i == 0)
                i--;
            else {
                for (j = i; j < 40; j++)
                    data[j] = ' ';
                data[40] = '\0';
                break;
            }
        }
    }
    byte = 0;
    for (i = 0; i < 40; i++) {
        mem[address][byte] = data[i];    //	storing data in memory
        byte++;
        if (byte == 4) {
            byte = 0;
            address++;
        }
    }
}

void write() {
    int i, j;
    for (i = address; i < address + 10; i++)
        for (j = 0; j < 4; j++)
            putc(mem[i][j], fp2);    //	writing to output file
    fprintf(fp2, "\n");
}

void terminate() {
    fprintf(fp2, "\n\n");    //	write 2 blank lines in output file
    load();
}

void load() {
    int i, j;
    word = 0;
    byte = 0;
    char program[41];
    clear_memory();
    while (!feof(fp1)) {
        for (i = 0; i < 40; i++) {
            program[i] = getc(fp1);    //	loading program to buffer from input file
            if (program[i] == '\n') {
                if (i == 0)
                    i--;
                else {
                    for (j = i; j < 40; j++)
                        program[j] = ' ';
                    program[40] = '\0';
                    break;
                }
            }
        }
        if (strncmp(program, "$AMJ", 4) == 0 || strncmp(program, "$END", 4) == 0)
            continue;
        if (strncmp(program, "$DTA", 4) == 0)
            start_execution();
        for (i = 0; i < 40; i++) {
            mem[word][byte] = program[i];    //	loading program to memory from buffer
            byte++;
            if (byte == 4) {
                byte = 0;
                word++;
                if (word == 100)    //	In case of overflow
                {
                    printf("Memory exceeded");
                    exit(1);
                }
            }
        }
    }
    fclose(fp1);
    fclose(fp2);
    printf("Successful");
    exit(0);
}

void start_execution() {
    os.ic = 0;
    execute_user_program();
}

void execute_user_program() {
    int i;
    do {
        for (i = 0; i < 4; i++)
            os.ir[i] = mem[os.ic][i];    //	load instruction to IR

        os.ic++;    //	points to next instruction
        si = 0;

        //	Executing of instruction, parsing
        if (os.ir[0] == 'L' && os.ir[1] == 'R') {
            address = to_int(os.ir[2], os.ir[3]);
            for (i = 0; i < 4; i++)
                os.r[i] = mem[address][i];
        } else if (os.ir[0] == 'S' && os.ir[1] == 'R') {
            address = to_int(os.ir[2], os.ir[3]);
            for (i = 0; i < 4; i++)
                mem[address][i] = os.r[i];
        } else if (os.ir[0] == 'C' && os.ir[1] == 'R') {
            address = to_int(os.ir[2], os.ir[3]);
            if (strncmp(os.r, mem[address], 4) == 0)
                os.t = 1;
            else
                os.t = 0;
        } else if (os.ir[0] == 'B' && os.ir[1] == 'T') {
            address = to_int(os.ir[2], os.ir[3]);
            if (os.t)
                os.ic = address;
        } else if (os.ir[0] == 'G' && os.ir[1] == 'D') {
            os.ir[3] = '0';
            address = to_int(os.ir[2], os.ir[3]);
            si = 1;
        } else if (os.ir[0] == 'P' && os.ir[1] == 'D') {
            os.ir[3] = '0';
            address = to_int(os.ir[2], os.ir[3]);
            si = 2;
        } else if (os.ir[0] == 'H')
            si = 3;
        else {
            printf("Error");
            exit(1);
        }
        if (si == 1 || si == 2 || si == 3)
            mos();
    } while (1);
}

void clear_memory() {
    int i, j;
    for (i = 0; i < 100; i++)
        for (j = 0; j < 4; j++)
            mem[i][j] = ' ';
}

int to_int(char a, char b) {
    return (a - '0') * 10 + b - '0';
}
