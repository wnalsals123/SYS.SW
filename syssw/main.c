#include"syssw.h"

extern int LC; // asm_tools.c 외부 정의 변수

void main()
{
	char buf[50];

	handle_load(); // asm파일 읽기
	Initialize(); // 레지스터 표, 명령어 표 읽기
	printf("\nPass1:\n");
	while (1) { // 패스1
		fgets(buf, 30, in);
		if (feof(in))break;
		PassI(buf);
	}
	Symbol_Print(); // 기호표 출력

	rewind(in);
	ObjSave = fopen("objCode.txt", "w+");
	LC = 0;
	printf("\nPass2:\n");
	while (1) { // 패스2
		fgets(buf, 30, in);
		if (feof(in))break;
		PassII(buf);
	}
	fclose(in);
	getchar();
}