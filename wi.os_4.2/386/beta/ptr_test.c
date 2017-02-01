
#include <stdio.h>

int  main(void);
void leveltwo(char *second, char *_second);
void levelthree(char *second, char *_second, char *_tmp);

int main()
{

char *first = "/dev/tty1a";
char _first[12] = "1A";
/*
char _first[12] = "/dev/tty";

	strcpy(_first, "/dev/tty");
	strcat(_first, "1A"); */

	printf("main: first, addr = %p\n", &first);
	printf("main: first = %s\n", first);
	printf("main: _first, addr = %p\n", &_first);
	printf("main: _first = %s\n\n", _first);

	leveltwo(first, _first);

	printf("return to main: first, addr = %p\n", &first);
	printf("return to main: first = %s\n", first);
	printf("return to main: _first, addr = %p\n", &_first);
	printf("return to main: _first = %s\n\n", _first);

	exit(0);
}

void leveltwo(char *second, char *_second)
{

char _tmp[12] = "/dev/tty";

	strcpy(_tmp, "/dev/tty");
	strcat(_tmp, _second);

	printf("	-> leveltwo: second, addr = %p\n", &second);
	printf("	-> leveltwo: second = %s\n", second);
	printf("	-> leveltwo: second, addr = %p\n", &_second);
	printf("	-> leveltwo: second = %s\n", _second);
	printf("	-> leveltwo: _tmp, addr = %p\n", &_tmp);
	printf("	-> leveltwo: _tmp = %s\n", _tmp);
	printf("	-> Change value second to /dev/tty2a\n");
	printf("	-> Change value _second to /dev/tty2A\n\n");

	strcpy(second, "/dev/tty2a");	
	strcpy(_second, "/dev/tty2A");

	levelthree((char *)second, (char *)_second, (char *)_tmp);

	printf("	-> return to leveltwo: second, addr = %p\n", &second);
	printf("	-> return to leveltwo: second = %s\n", second);
	printf("	-> return to leveltwo: _second, addr = %p\n", &_second);
	printf("	-> return to leveltwo: _second = %s\n\n", _second);
	printf("	-> return to leveltwo: _tmp, addr = %p\n", &_tmp);
	printf("	-> return to leveltwo: _tmp = %s\n\n", _tmp);

	return;

}	

void levelthree(char *third, char *_third, char *_tmp)
{

	printf("		-> levelthree: third, addr = %p\n", &third);
	printf("		-> levelthree: third = %s\n", third);
	printf("		-> levelthree: _third, addr = %p\n", &_third);
	printf("		-> levelthree: _third = %s\n", _third);
	printf("		-> levelthree: _tmp, addr = %p\n", &_tmp);
	printf("		-> levelthree: _tmp = %s\n", _tmp);
	printf("		-> change value third to /dev/tty3a\n");
	printf("		-> change value _third to /dev/tty3a\n");

	strcpy(third, "/dev/tty3a");
	strcpy(_third, "/dev/tty3A");

	printf("		-> levelthree: third, addr = %p\n", &third);
	printf("		-> levelthree: third = %s\n\n", third);
	printf("		-> levelthree: _third, addr = %p\n", &_third);
	printf("		-> levelthree: _third = %s\n\n", _third);
	return;
}	


