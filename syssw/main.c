#include"syssw.h"

extern int LC; // asm_tools.c �ܺ� ���� ����

void main()
{
	char buf[50];

	handle_load(); // asm���� �б�
	Initialize(); // �������� ǥ, ��ɾ� ǥ �б�
	printf("\nPass1:\n");
	while (1) { // �н�1
		fgets(buf, 30, in);
		if (feof(in))break;
		PassI(buf);
	}
	Symbol_Print(); // ��ȣǥ ���

	rewind(in);
	ObjSave = fopen("objCode.txt", "w+");
	LC = 0;
	printf("\nPass2:\n");
	while (1) { // �н�2
		fgets(buf, 30, in);
		if (feof(in))break;
		PassII(buf);
	}
	fclose(in);
	getchar();
}