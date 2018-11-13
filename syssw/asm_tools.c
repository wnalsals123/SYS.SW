#include"syssw.h"

int MaxI, InstrP = 0;
int MaxS = 0;
int LC;

int read_line(char str[], int limit) // ���� ���� �б�
{
	int ch, i = 0;
	while ((ch = getchar()) != '\n')
		if (i < limit - 1)
			str[i++] = ch;
	str[i] = '\0';
	return i;
}

void handle_load() // .asm���� �ε�
{
	char buffer[BUFFER_LENGTH];

	while (1) {
		printf("Data file name ? ");
		if (read_line(buffer, BUFFER_LENGTH) <= 0) {
			puts("�߸��� �Է��Դϴ�.");
			continue;
		}
		in = fopen(buffer, "r");
		if (in == NULL) {
			puts("������ �������� �ʽ��ϴ�.");
			continue;
		}
		break;
	}
	puts("Data file loaded.");
}

void Initialize()
{
	int i = 0, j = 1;
	FILE *regi, *inst;

	regi = fopen("reg.tbl", "r");
	inst = fopen("inst.tbl", "r");
	while (!feof(regi)) {
		fscanf(regi, "%s %s\n", Reg[i].reg_name, Reg[i].reg_num);
		i++;
	} // ���� ���̺� �ۼ�
	
	while (!feof(inst)) {
		fscanf(inst, "%6s%2s%2s%4s%3s%2s%9s\n", Instr[j].instruct, Instr[j].dest, Instr[j].sour, Instr[j].word_type, Instr[j].ins_code, Instr[j].ins_len, Instr[j].mod_reg);
		j++;
	} // ��ɾ� ���̺� �ۼ�
	MaxI = j - 1;
	fclose(regi);
	fclose(inst);
}

int Analyze(char *operand)
{
	int i = 0;
	char *regist[] = { "AX", "BX", "CX", "DX", "AL", "BL", "CL", "DL", "AH", "BH", "CH", "DH", 0x00 }; // ���� �̸� ����

	if (isdigit(operand[0])) return 0;
	else {
		while (regist[i] != 0x00) {
			if (!strcmp(operand, regist[i])) {
				if (i < 4) return 1; // ���� + ����
				else return 2; // ���� + ����Ʈ
			}
			else i++;
		}
	}
	return 3; // �޸� ���� ���
}

int Add_Chk(char *sen) // ���۷����� ��巹�� ��带 ����
{
	register k = MaxI;
	int i = 0, j = 0, l = 0, wp = 0;
	char op[6][10], *opcode[] = { "mov", "" };

	while (sen[wp] != '\n') {
		while (sen[wp] == ' ' || sen[wp] == '\t' || sen[wp] == ',') wp++; // ����, ��, �޸� ���
		while (sen[wp] != ' ' && sen[wp] != '\n' && sen[wp] != ',') {
			*(op[j] + i) = sen[wp];
			i++;
			wp++;
		}
		*(op[j] + i) = '\0';
		i = 0;
		j++;
	}
	i = 0;

	while (strcmp(opcode[i], "")) { // ��ɾ� ������ ��
		if (stricmp(opcode[i], op[0])) i++; // mov ��ɾ�� op[0]�� �ٸ���, ���� ��ɾ�
		else {
			strcpy(Sen._operator, op[0]); // ��ɾ _operator�� ����
			for (l = 1; l < j; l++)
				strcpy(Sen.operand[l - 1], op[l]); // ��ɾ� �� ���� operand�� ����
			break;
		}
	}

	if (i == MAX_INS) {  // i = 1�̸� ��ɾ ã�� ���� ��
		strcpy(Sen.lable, op[0]); // ù �ܾ ���̺� ���
		strcpy(Sen._operator, op[1]); // �ι�°�� ��ɾ ���
		for (l = 2; l < j; l++)
			strcpy(Sen.operand[l - 2], op[l]); // �� ���� ������ operand�� ����
	} // �� ������ �м�, ���̺�� ���۷����� �׸��� ���۷���� �з�
	strcpy(Instr[0].instruct, op[0]); // ��ɾ instruct�� ����

	switch (Analyze(op[1])) { // ������ �м�
		case 0: strcpy(Instr[0].dest, "i"); // �� ��� �Է�
			break;
		case 1: strcpy(Instr[0].dest, "r"); // ���� + ����
			strcpy(Instr[0].word_type, "w"); 
			break;
		case 2: strcpy(Instr[0].dest, "r"); // ���� + ����
			strcpy(Instr[0].word_type, "b"); // ���� + ����Ʈ
			break;
		case 3: strcpy(Instr[0].dest, "m"); // �޸� ����
			break;
	}

	switch (Analyze(op[2])) { // ����� �м�
	case 0: strcpy(Instr[0].sour, "i"); // �� ��� �Է�
		break;
	case 1: strcpy(Instr[0].sour, "r"); // ���� + ����
		strcpy(Instr[0].word_type, "w");
		break;
	case 2: strcpy(Instr[0].sour, "r"); // ���� + ����
		strcpy(Instr[0].word_type, "b"); // ���� + ����Ʈ
		break;
	case 3: strcpy(Instr[0].sour, "m"); // �޸� ����
		break;
	}

	while (stricmp(Instr[k].instruct, Instr[0].instruct) || strcmp(Instr[k].dest, Instr[0].dest) || strcmp(Instr[k].sour, Instr[0].sour) || strcmp(Instr[k].word_type, Instr[0].word_type)) {
		k--; // Instr, ���� ��ɾ�� inst.tbl���� ������ ��ɾ �� / ���� �� ã�� ������, k�� ����
	}
	return k; // �ε��� k�� ��ȯ
}

void PassI(char *buf)
{
	int i;
	static int j = 0;
	i = Add_Chk(buf); // ���� �ڵ��� ��ɾ�� �ּ� ����� ����

	if (i) { // mov ��ɾ��� ó��
		printf("%04X: %s", LC, buf); // �ּҰ�, ���� �ڵ� ���
		LC += atoi(Instr[i].ins_len); // LC = LC + ��ɾ� ����
	}
	else { // ��ȣ ó��
		if (!stricmp(Sen._operator, "dw"))
			strcpy(Symbol[j].word_type, "w"); // dw�� w�� ����
		else if(!stricmp(Sen._operator, "db"))
			strcpy(Symbol[j].word_type, "b"); // db�� b�� ����
		strcpy(Symbol[j].symbol, Sen.lable); // ���̺��� �ɺ��� ����
		strcpy(Symbol[j].data, Sen.operand[0]); // ���̺��� ���� ����
		Symbol[j].lc = LC; // �ּҰ��� ����
		printf("%04X: %s", LC, buf); // �ּҰ�, ���� �ڵ� ���
		if (*Symbol[j].word_type == 'w') LC += 2; // �ּҰ� w = 2, b = 1 ����
		else if (*Symbol[j].word_type == 'b') LC += 1;
		j++; // j�� ����, ������ �ɺ� ���ؽ��� ����
		MaxS++;
	}
}

int btoi(char *dig) // 2���� ���ڿ��� 10�� ������
{
	unsigned long res = strtoul(dig, NULL, 2);

	return res;
}

void PassII(char *buf)
{
	int i, j = 0, k = 0;
	ObjSave = fopen("objcode.txt", "a+");
	i = Add_Chk(buf);

	if (i) { // i != 0, ��ɾ��� ���
		modInstr[InstrP] = Instr[i];
		printf("%04x: %3s", LC, Instr[i].ins_code); // LC�� ��ɾ� �ڵ� ���
		fprintf(ObjSave, "%04x: %3s", LC, Instr[i].ins_code);

		if (!strcmp(Instr[i].dest, "r")) { // �������� r�̸�
			while (stricmp(Reg[j].reg_name, Sen.operand[0])) j++; // �������� �������� �̸��� ���� ����				
			strncpy(strchr(modInstr[InstrP].mod_reg, '?'), Reg[j].reg_num, 3);
		}

		j = 0;
		if (!strcmp(Instr[i].sour, "r")) {// ������� r�̸�
			while (stricmp(Reg[j].reg_name, Sen.operand[1])) j++; // ������� �������� �̸��� ���� ����
			strncpy(strchr(modInstr[InstrP].mod_reg, '?'), Reg[j].reg_num, 3); // ???�� �������� ǥ�� ������ ä��
		}

		if (strcmp(Instr[i].dest, "m") && strcmp(Instr[i].sour, "m")) { // ������, ������� �Ѵ� m�� �ƴϸ�
			printf("%02X\t\t%s", btoi(modInstr[InstrP].mod_reg), buf); // Instr�� ��ɾ� �ڵ带 16���� ��ȯ �� ���, ���� �ڵ� �߷�
			fprintf(ObjSave, "%02X\t\t%s", btoi(modInstr[InstrP].mod_reg), buf);
		}
		else { // ���� or ��߿� ��ȣ�� �� ���
			if (!strcmp(Instr[i].dest, "m")) // �������� m�̸�
				while (strcmp(Symbol[k].symbol, Sen.operand[0])) k++; // ��ȣ�� ã��
			else if (!strcmp(Instr[i].sour, "m")) // ������� m�̸�
				while (strcmp(Symbol[k].symbol, Sen.operand[1])) k++; // ��ȣ�� ã��

			if (Symbol[k].lc == (atoi(Symbol[k].data))) { // ������, ���ġ ���ʿ�
				printf("  %02X\t%04X\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf);
				fprintf(ObjSave, "  %02X\t%04X\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf);
			}
			else { // �ٸ��� ���ġ �ʿ�
				printf("  %02X\t%04X R\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf); // Instr�� ��ɾ� �ڵ带 16���� ��ȯ �� ���, ���� �ڵ� �߷�
				fprintf(ObjSave, "  %02X\t%04X R\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf);
			}
		}
		LC += atoi(Instr[i].ins_len); // LC = LC + ��ɾ� ����
	}
	else { // ��ȣ�� ���
		k = 0;
		
		while (strcmp(Symbol[k].symbol, Sen.lable)) k++; // symbol[k]�� lable�� ���� ������

		if (!strcmp(Symbol[k].word_type, "w")) { // Ÿ���� w��
			printf("%04X:%04X\t\t%s", LC, atoi(Symbol[k].data), buf);
			fprintf(ObjSave, "%04X:%04X\t\t%s", LC, atoi(Symbol[k].data), buf);
		}
		if (!strcmp(Symbol[k].word_type, "b")) { // Ÿ���� b��
			printf("%04X:%02X\t\t\t%s", LC, atoi(Symbol[k].data), buf);
			fprintf(ObjSave, "%04X:%02X\t\t\t%s", LC, atoi(Symbol[k].data), buf);
		}
		if (*Symbol[k].word_type == 'w') LC += 2; // waus �ּ� �� 2����, b�� �ּ� �� 1����
		else if (*Symbol[k].word_type == 'b') LC += 1;
	}
	InstrP++;
	fclose(ObjSave);
}

void Symbol_Print()
{
	int i = 0;

	SymbolSave = fopen("symbol.txt", "w+");
	printf("\n* Symbol Table *\n");
	printf("SYMBOL\tDATA(ADDRESS)\tRELOCATION\n");

	for (i = 0; i < MaxS; i++) { // symbol[k]�� lable�� ���� ������
		if (!strcmp(Symbol[i].word_type, "w")) { // Ÿ���� w
			printf("%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0)); // �ּ� ��, ���̺� ��, ���� �ڵ�
			fprintf(SymbolSave, "%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0)); // �ּ� ��, ���̺� ��, ���� �ڵ�
		}
		if (!strcmp(Symbol[i].word_type, "b")) { // Ÿ���� b
			printf("%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0)); // �ּ� ��, ���̺� ��, ���� �ڵ�
			fprintf(SymbolSave, "%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0)); // �ּ� ��, ���̺� ��, ���� �ڵ�
		}
	}
	fclose(SymbolSave);
}