#include"syssw.h"

int MaxI, InstrP = 0;
int MaxS = 0;
int LC;

int read_line(char str[], int limit) // 파일 제목 읽기
{
	int ch, i = 0;
	while ((ch = getchar()) != '\n')
		if (i < limit - 1)
			str[i++] = ch;
	str[i] = '\0';
	return i;
}

void handle_load() // .asm파일 로드
{
	char buffer[BUFFER_LENGTH];

	while (1) {
		printf("Data file name ? ");
		if (read_line(buffer, BUFFER_LENGTH) <= 0) {
			puts("잘못된 입력입니다.");
			continue;
		}
		in = fopen(buffer, "r");
		if (in == NULL) {
			puts("파일이 존재하지 않습니다.");
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
	} // 레지 테이블 작성
	
	while (!feof(inst)) {
		fscanf(inst, "%6s%2s%2s%4s%3s%2s%9s\n", Instr[j].instruct, Instr[j].dest, Instr[j].sour, Instr[j].word_type, Instr[j].ins_code, Instr[j].ins_len, Instr[j].mod_reg);
		j++;
	} // 명령어 테이블 작성
	MaxI = j - 1;
	fclose(regi);
	fclose(inst);
}

int Analyze(char *operand)
{
	int i = 0;
	char *regist[] = { "AX", "BX", "CX", "DX", "AL", "BL", "CL", "DL", "AH", "BH", "CH", "DH", 0x00 }; // 레지 이름 저장

	if (isdigit(operand[0])) return 0;
	else {
		while (regist[i] != 0x00) {
			if (!strcmp(operand, regist[i])) {
				if (i < 4) return 1; // 레지 + 워드
				else return 2; // 레지 + 바이트
			}
			else i++;
		}
	}
	return 3; // 메모리 지정 방식
}

int Add_Chk(char *sen) // 오퍼랜드의 어드레스 모드를 판정
{
	register k = MaxI;
	int i = 0, j = 0, l = 0, wp = 0;
	char op[6][10], *opcode[] = { "mov", "" };

	while (sen[wp] != '\n') {
		while (sen[wp] == ' ' || sen[wp] == '\t' || sen[wp] == ',') wp++; // 공백, 탭, 콤마 통과
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

	while (strcmp(opcode[i], "")) { // 명령어 끝까지 비교
		if (stricmp(opcode[i], op[0])) i++; // mov 명령어와 op[0]가 다르면, 다음 명령어
		else {
			strcpy(Sen._operator, op[0]); // 명령어를 _operator에 저장
			for (l = 1; l < j; l++)
				strcpy(Sen.operand[l - 1], op[l]); // 명령어 뒷 내용 operand에 저장
			break;
		}
	}

	if (i == MAX_INS) {  // i = 1이면 명령어를 찾지 못한 것
		strcpy(Sen.lable, op[0]); // 첫 단어를 레이블에 등록
		strcpy(Sen._operator, op[1]); // 두번째를 명령어에 등록
		for (l = 2; l < j; l++)
			strcpy(Sen.operand[l - 2], op[l]); // 그 다음 내용을 operand에 저장
	} // 한 문자을 분석, 레이블과 오퍼레이터 그리고 오퍼랜드로 분류
	strcpy(Instr[0].instruct, op[0]); // 명령어를 instruct에 저장

	switch (Analyze(op[1])) { // 도착지 분석
		case 0: strcpy(Instr[0].dest, "i"); // 값 즉시 입력
			break;
		case 1: strcpy(Instr[0].dest, "r"); // 레지 + 워드
			strcpy(Instr[0].word_type, "w"); 
			break;
		case 2: strcpy(Instr[0].dest, "r"); // 레지 + 워드
			strcpy(Instr[0].word_type, "b"); // 레지 + 바이트
			break;
		case 3: strcpy(Instr[0].dest, "m"); // 메모리 지정
			break;
	}

	switch (Analyze(op[2])) { // 출발지 분석
	case 0: strcpy(Instr[0].sour, "i"); // 값 즉시 입력
		break;
	case 1: strcpy(Instr[0].sour, "r"); // 레지 + 워드
		strcpy(Instr[0].word_type, "w");
		break;
	case 2: strcpy(Instr[0].sour, "r"); // 레지 + 워드
		strcpy(Instr[0].word_type, "b"); // 레지 + 바이트
		break;
	case 3: strcpy(Instr[0].sour, "m"); // 메모리 지정
		break;
	}

	while (stricmp(Instr[k].instruct, Instr[0].instruct) || strcmp(Instr[k].dest, Instr[0].dest) || strcmp(Instr[k].sour, Instr[0].sour) || strcmp(Instr[k].word_type, Instr[0].word_type)) {
		k--; // Instr, 현재 명령어와 inst.tbl에서 가져온 명령어를 비교 / 같은 값 찾을 때까지, k값 감소
	}
	return k; // 인덱스 k를 반환
}

void PassI(char *buf)
{
	int i;
	static int j = 0;
	i = Add_Chk(buf); // 원시 코드의 명령어와 주소 방식을 받음

	if (i) { // mov 명령어의 처리
		printf("%04X: %s", LC, buf); // 주소값, 원시 코드 출력
		LC += atoi(Instr[i].ins_len); // LC = LC + 명령어 길이
	}
	else { // 기호 처리
		if (!stricmp(Sen._operator, "dw"))
			strcpy(Symbol[j].word_type, "w"); // dw면 w를 저장
		else if(!stricmp(Sen._operator, "db"))
			strcpy(Symbol[j].word_type, "b"); // db면 b를 저장
		strcpy(Symbol[j].symbol, Sen.lable); // 레이블을 심볼에 저장
		strcpy(Symbol[j].data, Sen.operand[0]); // 레이블의 값을 저장
		Symbol[j].lc = LC; // 주소값을 저장
		printf("%04X: %s", LC, buf); // 주소값, 원시 코드 출력
		if (*Symbol[j].word_type == 'w') LC += 2; // 주소값 w = 2, b = 1 증가
		else if (*Symbol[j].word_type == 'b') LC += 1;
		j++; // j를 증가, 다음번 심볼 인텍스를 맞춤
		MaxS++;
	}
}

int btoi(char *dig) // 2진수 문자열을 10진 정수로
{
	unsigned long res = strtoul(dig, NULL, 2);

	return res;
}

void PassII(char *buf)
{
	int i, j = 0, k = 0;
	ObjSave = fopen("objcode.txt", "a+");
	i = Add_Chk(buf);

	if (i) { // i != 0, 명령어일 경우
		modInstr[InstrP] = Instr[i];
		printf("%04x: %3s", LC, Instr[i].ins_code); // LC와 명령어 코드 출력
		fprintf(ObjSave, "%04x: %3s", LC, Instr[i].ins_code);

		if (!strcmp(Instr[i].dest, "r")) { // 도착지가 r이면
			while (stricmp(Reg[j].reg_name, Sen.operand[0])) j++; // 도착지와 레지스터 이름이 같게 맞춤				
			strncpy(strchr(modInstr[InstrP].mod_reg, '?'), Reg[j].reg_num, 3);
		}

		j = 0;
		if (!strcmp(Instr[i].sour, "r")) {// 출발지가 r이면
			while (stricmp(Reg[j].reg_name, Sen.operand[1])) j++; // 출발지와 레지스터 이름이 같게 맞춤
			strncpy(strchr(modInstr[InstrP].mod_reg, '?'), Reg[j].reg_num, 3); // ???를 레지스터 표의 값으로 채움
		}

		if (strcmp(Instr[i].dest, "m") && strcmp(Instr[i].sour, "m")) { // 도착지, 출발지가 둘다 m이 아니면
			printf("%02X\t\t%s", btoi(modInstr[InstrP].mod_reg), buf); // Instr의 명령어 코드를 16진수 변환 후 출력, 원시 코드 추력
			fprintf(ObjSave, "%02X\t\t%s", btoi(modInstr[InstrP].mod_reg), buf);
		}
		else { // 도착 or 출발에 기호가 들어간 경우
			if (!strcmp(Instr[i].dest, "m")) // 도착지가 m이면
				while (strcmp(Symbol[k].symbol, Sen.operand[0])) k++; // 기호를 찾음
			else if (!strcmp(Instr[i].sour, "m")) // 출발지가 m이면
				while (strcmp(Symbol[k].symbol, Sen.operand[1])) k++; // 기호를 찾음

			if (Symbol[k].lc == (atoi(Symbol[k].data))) { // 같으면, 재배치 불필요
				printf("  %02X\t%04X\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf);
				fprintf(ObjSave, "  %02X\t%04X\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf);
			}
			else { // 다르면 재배치 필요
				printf("  %02X\t%04X R\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf); // Instr의 명령어 코드를 16진수 변환 후 출력, 원시 코드 추력
				fprintf(ObjSave, "  %02X\t%04X R\t%s", btoi(modInstr[InstrP].mod_reg), Symbol[k].lc, buf);
			}
		}
		LC += atoi(Instr[i].ins_len); // LC = LC + 명령어 길이
	}
	else { // 기호일 경우
		k = 0;
		
		while (strcmp(Symbol[k].symbol, Sen.lable)) k++; // symbol[k]과 lable이 같을 때까지

		if (!strcmp(Symbol[k].word_type, "w")) { // 타입이 w면
			printf("%04X:%04X\t\t%s", LC, atoi(Symbol[k].data), buf);
			fprintf(ObjSave, "%04X:%04X\t\t%s", LC, atoi(Symbol[k].data), buf);
		}
		if (!strcmp(Symbol[k].word_type, "b")) { // 타입이 b면
			printf("%04X:%02X\t\t\t%s", LC, atoi(Symbol[k].data), buf);
			fprintf(ObjSave, "%04X:%02X\t\t\t%s", LC, atoi(Symbol[k].data), buf);
		}
		if (*Symbol[k].word_type == 'w') LC += 2; // waus 주소 값 2증가, b면 주소 값 1증가
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

	for (i = 0; i < MaxS; i++) { // symbol[k]과 lable가 같은 때까지
		if (!strcmp(Symbol[i].word_type, "w")) { // 타입이 w
			printf("%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0)); // 주소 값, 레이블 값, 원시 코드
			fprintf(SymbolSave, "%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0)); // 주소 값, 레이블 값, 원시 코드
		}
		if (!strcmp(Symbol[i].word_type, "b")) { // 타입이 b
			printf("%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0)); // 주소 값, 레이블 값, 원시 코드
			fprintf(SymbolSave, "%s\t%X\t\t%d\n", Symbol[i].symbol, Symbol[i].lc, (Symbol[i].lc != atoi(Symbol[i].data) ? 1 : 0)); // 주소 값, 레이블 값, 원시 코드
		}
	}
	fclose(SymbolSave);
}