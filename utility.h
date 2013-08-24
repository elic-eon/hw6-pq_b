#ifndef __ds__utility__header__
#define __ds__utility__header__
#define __DS__INT__			0x00000001
#define __DS__SHORT__		0x00000002
#define __DS__FLOAT__		0x00000004
#define __DS__DOUBLE__		0x00000008
#define __DS__LONG__		0x00000010
#define __DS__LONGLONG__	0x00000020
#define __DS__CHAR__		0x00000040
#define __DS__STRING__		0x00000080

int getData(int type,void *data);
void printData(int type, void *data);
void sPrintData(char *str, int type, void *data);

#endif
