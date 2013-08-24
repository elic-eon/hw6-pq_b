#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"pq_b.h"
#include"utility.h"

typedef struct pq_t pq;

int doubleGT(const void *a, const void *b)
{
	double da=*(double*)a;
	double db=*(double*)b;
    // printf("da, db: %f, %f\n", da, db);
	if(da==db)return 0;
	return (da>db)?1:-1;
}

int shortGT(const void *a, const void *b)
{
	short da=*(short*)a;
	short db=*(short*)b;
	if(da==db)return 0;
	return (da>db)?1:-1;
}

int main()
{
	struct pq_t *p=NULL; //, *p2=NULL;
	char cmd[10240];
	size_t cap, keySize, objSize;
	int ret;
	void *data1, *data2;
	int opChoice; //, pqChoice = 1;
    // struct pq_t *now_pq;
    // now_pq = p1;
	int keyType, objType;
    int (*cmp)(const void *, const void*);
	data1=malloc(sizeof(double)*10);
	data2=malloc(sizeof(double)*10);
	for(;;)
	{
		if(p==NULL)
		{
			printf("The pq needs to be initialized.\n");
            printf("Init step 1. key type: d for double, others for short:\n");
            scanf("%s",cmd);
            keyType=(cmd[0]=='d')?__DS__DOUBLE__:__DS__SHORT__;
            printf("Init step 2. object type: d for double, others for short:\n");
            scanf("%s",cmd);
            objType=(cmd[0]=='d')?__DS__DOUBLE__:__DS__SHORT__;
			printf("Init step 3. Capacity: ");
			scanf("%zu",&cap);
            p = pqAlloc();
            /*
            if(pqChoice == 1)
                now_pq = p1 = pqAlloc();
            else
                now_pq = p2 = pqAlloc();
            */

			if(keyType == __DS__DOUBLE__){
                keySize = sizeof(double);
                cmp = doubleGT; 
            }
            else{
                keySize = sizeof(short);
                cmp = shortGT;
            }
			if(objType == __DS__DOUBLE__)
                objSize = sizeof(double);
            else
                objSize = sizeof(short);

            ret=pqInit(p, keySize, objSize, cap, cmp);
			if(ret)
			{
				puts("Not enough memory.");
				pqFree(p);
				p=NULL;
			}
			else
				puts("Initialization done.");
		}
		else
		{
			printf("size/capacity: %zu/%zu\n", pqSize(p), pqCap(p));
			// printf("now is in Priority Queue %d:\n", pqChoice);
			size_t keySize=pqKeySize(p);
			size_t objSize=pqObjSize(p);
			printf("Valid operations: 1)insert, 2)extract max, 3)extract min, 4)max, 5)min,\n");
            printf("                  6)change key, 7)get key, 8)empty, 9)free, 10)quit\n");
			for(opChoice=0;opChoice<=0||opChoice>10;sscanf(cmd,"%d",&opChoice))
				scanf("%s",cmd);
			if(opChoice==1)
			{
				printf("Input a %s value for key: ",keySize==sizeof(double)?"double":"short");
				getData(keyType,data1);
				printf("Input a %s value for obj: ",objSize==sizeof(double)?"double":"short");
				getData(objType,data2);
				ret=pqInsert(p, data1, data2);
				if(ret == __DS__PQ__FULL__) printf("The pq is full!\n");
				else if(ret == __DS__PQ__OBJ_EXIST__){ 
                    printf("The object ");
                    printData(objType, data2);
                    printf(" is in the pq now!\n");
                }
			}
			else if(opChoice>=2 && opChoice<=5)
			{
                if(opChoice == 2) ret=pqExtractMax(p, data1, data2);
                else if(opChoice == 3) ret=pqExtractMin(p, data1, data2);
                else if(opChoice == 4) ret=pqMax(p, data1, data2);
                else if(opChoice == 5) ret=pqMin(p, data1, data2);
                
				if(!ret)
				{
					printf("The result is <");
					printData(keyType, data1);
                    printf(", ");
					printData(objType, data2);
					puts(">.");
				}
				else
					printf("The pq is empty!\n");
			}
            else if(opChoice==6){
				printf("Input a %s value for new key: ", keySize==sizeof(double)?"double":"short");
				getData(keyType, data1);
				printf("Input a %s value for obj: ", objSize==sizeof(double)?"double":"short");
				getData(objType, data2);
                ret = pqChangeKey(p, data2, data1);
                if(ret == __DS__PQ__OBJ_NOT_EXIST__){
                    printf("The obj ");
                    printData(objType, data2);
                    printf(" isn't in the pq.\n");
                }
                else if(ret == __DS__PQ__NON_INCRE__){
                    printf("The new key ");
                    printData(objType, data1);
                    printf(" is less than origin key.\n");
                }
            }
            else if(opChoice==7){
				printf("Input a %s value for object: ", objSize==sizeof(double)?"double":"short");
				getData(objType, data2);
                ret = pqGetKey(p, data2, data1);
                if(ret == __DS__PQ__OBJ_NOT_EXIST__){
                    printf("The obj ");
                    printData(objType, data2);
                    printf(" isn't in the pq.\n");
                }
                else{
                    printf("The key of obj ");
                    printData(objType, data2);
                    printf(" in pq is ");
                    printData(keyType, data1);
                    printf(".\n");
                }
            }
			else if(opChoice==8)
			{
				if(pqEmpty(p)) printf("The pq is empty.\n");
				else printf("The pq%d is not empty.\n");
			}
			else if(opChoice==9)
			{
                if(p != NULL)
                    pqFree(p);
                p = NULL;
			}
			else
				break;
		}
	}
    free(data1);
    free(data2);
	return 0;
}
