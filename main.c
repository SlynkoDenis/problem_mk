#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "header.h"

void execute_instr (reg_state_t *st, struct instr_t instr);

struct instr_t decode_instr (unsigned char cmd);

void execute_program (FILE *prog);

unsigned char execute_op (int opc, unsigned char rd, unsigned char rs);

int main (int argc, char **argv)
{
    FILE *prog;

    if (argc < 2)
    {
        fprintf (stderr, "Usage: %s <programm>\n", argv[0]);
        abort ();
    }

    prog = fopen (argv[1], "r");
    if (NULL == prog)
    {
        fprintf (stderr, "Usage: %s <programm>\n", argv[0]);
        fprintf (stderr, "Where file with programm shall exist\n");
        abort ();
    }

    execute_program (prog);

    fclose(prog);
    return 0;
}

//---------------------------------------------------------------------------------------------

void execute_instr (reg_state_t *st, struct instr_t instr)
{
    assert (st);

    switch (instr.opcode)
    {
        case MOVI:
            st[D] = instr.opnd.imm;
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
            {
                unsigned char rs = instr.opnd.ops.rs;
                unsigned char rd = instr.opnd.ops.rd;
                st[rd] = execute_op (instr.opcode, st[rd], st[rs]);

                break;
            }
        case IN:
            {
                unsigned u = 0;
                scanf ("%u", &u);
                st[instr.opnd.rop] = u & 0xff;
                break;
            }
        case OUT:
            {
                fprintf (stdout, "%u\n", st[instr.opnd.rop]);
                break;
            }
        default:
            {
                fprintf (stderr, "Unknown opcode\n");
                abort ();
            }
    }
}

//---------------------------------------------------------------------------------------------

struct instr_t decode_instr (unsigned char cmd)
{
    struct instr_t instr;

    if ((cmd & (1 << 7)) == 0)
    {
        instr.opcode = MOVI;
        instr.opnd.imm = cmd;

        return instr;
    }

    if ((cmd & (1 << 6)) == 0)
    {
        switch (cmd >> 4)
        {
            case 8:
                instr.opcode = ADD;
                break;
            case 9:
                instr.opcode = SUB;
                break;
            case 10:
                instr.opcode = MUL;
                break;
            case 11:
                instr.opcode = DIV;
                break;
        }
        instr.opnd.ops.rd = (cmd >> 2) & 3;
        instr.opnd.ops.rs = cmd & 3;

        return instr;
    }

    if ((cmd & (1 << 2)) == 0)
    {
        instr.opcode = IN;
        instr.opnd.rop = cmd & 3;

        return instr;
    }
    if ((cmd & (1 << 2)) == 4)
    {
        instr.opcode = OUT;
        instr.opnd.rop = cmd & 3;

        return instr;
    }

    fprintf (stderr, "Unknown opcode\n");
    abort ();
}

//---------------------------------------------------------------------------------------------

void execute_program (FILE *prog)
{
    assert (prog);

    unsigned command;
    reg_state_t regs[RLAST] = {0};

    while (fscanf (prog, "%x", &command) == 1)
    {
        unsigned char cmd = command & 0xff;

        assert (cmd == command);

        struct instr_t instr = decode_instr (cmd);

        execute_instr (regs, instr);
    }
}

//---------------------------------------------------------------------------------------------

unsigned char execute_op (int opc, unsigned char rd, unsigned char rs)
{
    switch (opc)
    {
        case ADD:
            return rd + rs;
        case SUB:
            return rd - rs;
        case MUL:
            return rd * rs;
        case DIV:
            return rd / rs;
        default:
            {
                fprintf (stderr, "Unknown opcode\n");
                abort ();
            }
    }
}
