#include <stdio.h>
int input_function(void);
void decode_opcode(int);
void decode_rformat(int);
void decode_iformat(int);
void decode_uformat(int);
void decode_ujformat(int);
void decode_sbformat(int);
void decode_sformat(int);

char str[8];
int place = -2;

int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[1], "rb");
    (void)argc;
    int input, number = 0;
    while (1)
    {
        for(int i = 0; i < 4; i++) {
            str[i] = fgetc(fp);
        }
        if(place == ftell(fp))
            break;
        
        place = ftell(fp);
        input = input_function();
        printf("inst %d: %08x ", number, input);
        decode_opcode(input);

        number++;
        printf("\n");
    }
    fclose(fp);
    return 0;
}

int input_function(void)
{
    int input = 0, i;
    for (i = 0; i < 4; i++)
    {
        int tmp = (int)str[i];
        tmp <<= (8 * i);
        input += (tmp & (0xff << (8 * i)));
    }
    for(i = 0; i < 8; i++) {
        str[i] = 0;
    }
    return input;
}

void decode_opcode(int input)
{
    int opcode = input & 0x7f;
    switch (opcode)
    {
    case 0x33: // add, sub, sll, slt, sltu, xor, srl, sra, or, and
        decode_rformat(input);
        break;
    case 0x3:  // lb, lh, lw, lbu, lhu
    case 0x67: // jalr
    case 0x13: // addi, slti, lstiu, xori, ori, andi, slli, srli, slli, srli, srai
        decode_iformat(input);
        break;
    case 0x37: // lui
    case 0x17: // auipc
        decode_uformat(input);
        break;
    case 0x6F: // jal
        decode_ujformat(input);
        break;
    case 0x63: // beq, bne, blt, bge, gltu, bgeu
        decode_sbformat(input);
        break;
    case 0x23: // sb, sh, sw
        decode_sformat(input);
        break;
    default:
        printf("unknown instruction");
        break;
    }
}

void decode_rformat(int input)
{
    int op = input & 0x7f;
    int rd = (input & 0xf80) >> 7;
    int rs1 = (input & 0xf8000) >> 15;
    int rs2 = (input & 0x1f00000) >> 20;
    int funct3 = (input & 0x7000) >> 12;
    int funct7 = ((input & 0xfe000000) >> 25) & 0x7f;
    if (funct3 == 0 && funct7 == 0)
        printf("add ");
    else if (funct3 == 0 && funct7 == 0x20)
        printf("sub ");
    else if (funct3 == 1 && funct7 == 0)
        printf("sll ");
    else if (funct3 == 2 && funct7 == 0)
        printf("slt ");
    else if (funct3 == 3 && funct7 == 0)
        printf("sltu ");
    else if (funct3 == 4 && funct7 == 0)
        printf("xor ");
    else if (funct3 == 5 && funct7 == 0)
        printf("srl ");
    else if (funct3 == 5 && funct7 == 0x20)
        printf("sra ");
    else if (funct3 == 6 && funct7 == 0)
        printf("or ");
    else if (funct3 == 7 && funct7 == 0)
        printf("and ");
    else
    {
        printf("unknown instruction");
        return;
    }
    printf("x%d, x%d, x%d", rd, rs1, rs2);
}

void decode_iformat(int input)
{
    int op = input & 0x7f;
    int rd = (input & 0xf80) >> 7;
    int rs1 = (input & 0xf8000) >> 15;
    int funct3 = (input & 0x7000) >> 12;
    int imm = input & 0xfff00000;
    imm = imm >> 20;
    int shampt = (input & 0x1f00000) >> 20;
    int funct7 = ((input & 0xfe000000) >> 25) & 0x7f;
    if (op == 3)
    {
        if (funct3 == 0)
            printf("lb ");
        else if (funct3 == 1)
            printf("lh ");
        else if (funct3 == 2)
            printf("lw ");
        else if (funct3 == 4)
            printf("lbu ");
        else if (funct3 == 5)
            printf("lhu ");
        else
        {
            printf("unknown instruction");
            return;
        }
        printf("x%d, %d(x%d)", rd, imm, rs1);
        return;
    }
    else if (op == 0x67)
    {
        if (funct3 == 0)
            printf("jalr ");
        else
        {
            printf("unknown instruction");
            return;
        }
        printf("x%d, %d(x%d)", rd, imm, rs1);
        return;
    }
    else if (op == 0x13)
    {
        if (funct3 == 0)
            printf("addi x%d, x%d, %d", rd, rs1, imm);
        else if (funct3 == 2)
            printf("slti x%d, x%d, %d", rd, rs1, imm);
        else if (funct3 == 3)
            printf("sltiu x%d, x%d, %d", rd, rs1, imm);
        else if (funct3 == 4)
            printf("xori x%d, x%d, %d", rd, rs1, imm);
        else if (funct3 == 6)
            printf("ori x%d, x%d, %d", rd, rs1, imm);
        else if (funct3 == 7)
            printf("andi x%d, x%d, %d", rd, rs1, imm);
        else if (funct3 == 1)
            printf("slli x%d, x%d, %d", rd, rs1, shampt);
        else if (funct3 == 5 && funct7 == 0)
            printf("srli x%d, x%d, %d", rd, rs1, shampt);
        else if (funct3 == 5 && funct7 == 0x20)
            printf("srai x%d, x%d, %d", rd, rs1, shampt);
        else
            printf("unknown instruction");
    }
}

void decode_uformat(int input)
{
    int op = input & 0x7f;
    int rd = (input & 0xf80) >> 7;
    int imm = input & 0xfffff000;
    if(op == 0x37)
        printf("lui ");
    else if(op == 0x17)
        printf("auipc ");
    else {
        printf("unknown instruction");
        return;
    }
    printf("x%d, %d", rd, imm);
}

void decode_ujformat(int input)
{
    int rd = (input & 0xf80) >> 7;
    int imm = 0;
    int tmp1 = input & 0x80000000;
    tmp1 >>= 11;
    int tmp2 = (input & 0x7fe00000) >> 20;
    int tmp3 = (input & 0x100000) >> 9;
    int tmp4 = input & 0xff000;
    imm += (tmp1 + tmp2 + tmp3 + tmp4);
    printf("jal x%d, %d", rd, imm);
}

void decode_sbformat(int input)
{
    int rs1 = (input & 0xf8000) >> 15;
    int rs2 = (input & 0x1f00000) >> 20;
    int funct3 = (input & 0x7000) >> 12;
    int tmp1 = input & 0x80000000;
    tmp1 >>= 19;
    int tmp2 = (input & 0x7e000000) >> 20;
    int tmp3 = (input & 0xf00) >> 7;
    int tmp4 = (input & 0x80) << 4;
    int imm = tmp1 + tmp2 + tmp3 + tmp4;
    if(funct3 == 0)
        printf("beq ");
    else if(funct3 == 1)
        printf("bne ");
    else if(funct3 == 4)
        printf("blt ");
    else if(funct3 == 5)
        printf("bge ");
    else if(funct3 == 6)
        printf("bltu ");
    else if(funct3 == 7)
        printf("bgeu ");
    else {
        printf("unknown instruction");
        return;
    }
    printf("x%d, x%d, %d", rs1, rs2, imm);
}

void decode_sformat(int input)
{
    int rs1 = (input & 0xf8000) >> 15;
    int rs2 = (input & 0x1f00000) >> 20;
    int funct3 = (input & 0x7000) >> 12;
    int tmp1 = (input & 0xf80) >> 7;
    int tmp2 = input & 0xfe000000;
    tmp2 >>= 20;
    int imm = tmp1 + tmp2;
    if(funct3 == 0)
        printf("sb ");
    else if(funct3 == 1)
        printf("sh ");
    else if(funct3 == 2)
        printf("sw ");
    else {
        printf("unknown instruction");
        return;
    }
    printf("x%d, %d(x%d)", rs2, imm, rs1);
}