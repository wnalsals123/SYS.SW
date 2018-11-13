#ifndef SYSSW_H
#define SYSSW_H
#define MAX_INS 1
#define BUFFER_LENGTH 200
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include<stdlib.h>

FILE *in;

struct reg {
	char reg_name[3];
	char reg_num[4];
}Reg[20]; // ���� �̸� ��ȣ ����

struct ins {
	char instruct[6];
	char dest[2];
	char sour[2];
	char word_type[2];
	char ins_code[3];
	char ins_len[2];
	char mod_reg[9];
}Instr[100], modInstr[100]; // �ν�Ʈ���� ���� ����

struct symbol_tb1 {
	char symbol[10];
	char word_type[2];
	int lc;
	char data[10];
}Symbol[30]; // �ɺ� ���̺�

struct sentence {
	char lable[10];
	char _operator[10];
	char operand[3][10];
}Sen; // �����ڵ� �ӽ� ����

FILE *ObjSave;
FILE *SymbolSave;

int read_line(char str[], int);
void handle_load();
void Initialize();
int Analyze(char*);
int Add_Chk(char*);
void PassI(char*);
int btoi(char*);
void PassII(char*);
void Symbol_Print();

#endif