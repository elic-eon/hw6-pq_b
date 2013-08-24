#include "pq_b.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int doubleGT(const void *a, const void *b);

/* Hash Map spec */
#define __DS__HM__NORMAL__        0x00000000
#define __DS__HM__OUT_OF_MEM__    0x00000001
#define __DS__HM__KEY_EXIST__     0x00000002
#define __DS__HM__KEY_NOT_EXIST__ 0x00000004
struct hm_t;

struct hm_t *hmAlloc();
int hash(int sz, void *obj);
int hmInit(struct hm_t *pThis, size_t objSize, size_t keySize);
int hmFree(struct hm_t *pThis);
int hmSize(struct hm_t *pThis);
int hmInsert(struct hm_t *pThis, void *pObj, void *pKey);
int hmDelete(struct hm_t *pThis, void *pObj);
int hmKeyExist(struct hm_t *pThis, void *pObj);
int hmUnion(struct hm_t *pThis1, struct hm_t *pThis2);
int hmGet(struct hm_t *pThis, void *pObj, void *pRetKey);
int hmSet(struct hm_t *pThis, void *pObj, void *pNewKey);
void hmDebug(struct hm_t*);
/* Hash Map spec end */
void hmDebug(struct hm_t*);

inline char* getAddr(void *array, int i, size_t tSize){
	return array+i*tSize;
}
inline void getItem(void *array, int i, void* pRet, size_t tSize){
	memcpy(pRet, array+i*tSize, tSize);
}
inline void putItem(void *array, int i, const void* pRet, size_t tSize){
	memcpy(array+i*tSize, pRet, tSize);
}

/* Very Slow Priority Queue implementation */
struct pq_t{
    void *MaxKey;
    void *MinKey;
    void *MaxObj;
    void *MinObj;
    size_t keySize, objSize, size, cap;
    struct hm_t *pObjToIndex;
    int (*cmp)(const void*, const void*);
    int dynamic;
};

/*
void pqDebug(struct pq_t* pThis){
    printf("size/cap: %zu/%zu\nkey,obj size: %zu, %zu\n", pThis->size, pThis->cap, pThis->keySize, pThis->objSize);
    size_t i;
    printf("keyArray, objArray: %x, %x\n", pThis->keyArray, pThis->objArray);
    printf("key:");
    for(i=0; i<pThis->size; i++){
        printData(__DS__DOUBLE__, pThis->keyArray + i*pThis->keySize);
        printf("%c", (i!=(pThis->size-1))?' ':'\n');
    }
    printf("obj:");
    for(i=0; i<pThis->size; i++){
        printData(__DS__SHORT__, pThis->objArray + i*pThis->objSize);
        printf("%c", (i!=(pThis->size-1))?' ':'\n');
    }
    printf("pObjToIndex:\n");
    hmDebug(pThis->pObjToIndex);
}
*/

struct pq_t *pqAlloc(){
    return malloc(sizeof(struct pq_t));
}

int pqInit(struct pq_t *pThis, size_t keySize, size_t objSize, size_t cap, int (*cmp)(const void*, const void*)){
    if(cap != 0){
        pThis->dynamic = 0;
        pThis->cap = cap;
    }
    else
    {
        pThis->dynamic = 1;
        pThis->cap = 4;
    }
    pThis->MinKey = malloc(pThis->cap*keySize);
    pThis->MaxKey = malloc(pThis->cap*keySize);
    pThis->MinObj = malloc(pThis->cap*objSize);
    pThis->MaxObj = malloc(pThis->cap*objSize);
    if (pThis->MinKey && pThis->MaxKey && pThis->MinObj && pThis->MaxObj)
      ;
    else
        return __DS__HM__OUT_OF_MEM__;
    pThis->objSize = objSize;
    pThis->keySize = keySize;
    pThis->size = 0;
    pThis->pObjToIndex = hmAlloc();
    pThis->cmp = cmp;
    hmInit(pThis->pObjToIndex, objSize, keySize);
    return __DS__PQ__NORMAL__;
}

int pqFree(struct pq_t *pThis){
    if(pThis->pObjToIndex != NULL)
        hmFree(pThis->pObjToIndex);
    if (pThis->MinKey)
        free(pThis->MinKey);
    if (pThis->MaxKey)
        free(pThis->MaxKey);
    if (pThis->MinObj)
        free(pThis->MinObj);
    if (pThis->MaxObj)
        free(pThis->MaxObj);
    free(pThis);
}

size_t pqCap(struct pq_t *pThis){
    return pThis->cap;
}
size_t pqSize(struct pq_t *pThis){
    return pThis->size;
}
size_t pqKeySize(struct pq_t *pThis){
    return pThis->keySize;
}
size_t pqObjSize(struct pq_t *pThis){
    return pThis->objSize;
}
int pqEmpty(struct pq_t *pThis){
    return pThis->size == 0;
}

int pqInsert(struct pq_t *pThis, void *pKey, void *pObj){
    if(pThis->size == pThis->cap)
    {
        if(pThis->dynamic)
        {
            if(hmKeyExist(pThis->pObjToIndex, pObj))
                return __DS__PQ__OBJ_EXIST__;
            void *temp = malloc(pThis->cap*2*pThis->keySize);
            memcpy(temp, pThis->MinKey, pThis->cap*pThis->keySize);
            free(pThis->MinKey);
            pThis->MinKey = temp;

            temp = malloc(pThis->cap*2*pThis->keySize);
            memcpy(temp, pThis->MaxKey, pThis->cap*pThis->keySize);
            free(pThis->MaxKey);
            pThis->MaxKey = temp;

            temp = malloc(pThis->cap*2*pThis->objSize);
            memcpy(temp, pThis->MinObj, pThis->cap*pThis->objSize);
            free(pThis->MinObj);
            pThis->MinObj = temp;

            temp = malloc(pThis->cap*2*pThis->keySize);
            memcpy(temp, pThis->MaxObj, pThis->cap*pThis->objSize);
            free(pThis->MaxObj);
            pThis->MaxObj = temp;

            pThis->cap *= 2;
        }
        else
            return __DS__PQ__FULL__;
    }
    if(hmKeyExist(pThis->pObjToIndex, pObj))
        return __DS__PQ__OBJ_EXIST__;
    size_t end = pThis->size/2;
    int type;
    if (pThis->size%2)
    {
        putItem(pThis->MinKey, end, pKey, pThis->keySize);
        putItem(pThis->MinObj, end, pObj, pThis->objSize);
        if (pThis->cmp(getAddr(pThis->MaxKey, end, pThis->keySize), getAddr(pThis->MinKey, end, pThis->keySize)) < 0)
        {
            void *kTemp = malloc(pThis->keySize);
            void *oTemp = malloc(pThis->objSize);
            memcpy(kTemp, getAddr(pThis->MaxKey, end, pThis->keySize), pThis->keySize);
            memcpy(oTemp, getAddr(pThis->MaxObj, end, pThis->objSize), pThis->objSize);
            memcpy(getAddr(pThis->MaxKey, end, pThis->keySize), getAddr(pThis->MinKey, end, pThis->keySize), pThis->keySize);
            memcpy(getAddr(pThis->MaxObj, end, pThis->objSize), getAddr(pThis->MinObj, end, pThis->objSize), pThis->objSize);
            memcpy(getAddr(pThis->MinKey, end, pThis->keySize), kTemp, pThis->keySize);
            memcpy(getAddr(pThis->MinObj, end, pThis->objSize), oTemp, pThis->objSize);
            free(kTemp);
            free(oTemp);
            type = 1;
            /*last node Max*/
        }
        else
        {
            type = 2;
            /*last node Min*/
        }
    }
    else
    {
        putItem(pThis->MaxKey, end, pKey, pThis->keySize);
        putItem(pThis->MaxObj, end, pObj, pThis->objSize);
        type = 3;
        /*last node Max without Min*/
    }
    size_t itr = pThis->size/2;
    size_t parent;
    void *kTemp = malloc(pThis->keySize);
    void *oTemp = malloc(pThis->objSize);
    if (type == 1)
    {
        /*last node Max*/
        memcpy(kTemp, getAddr(pThis->MaxKey, itr, pThis->keySize), pThis->keySize);
        memcpy(oTemp, getAddr(pThis->MaxObj, itr, pThis->objSize), pThis->objSize);
        while(itr > 0)
        {
            parent = (itr-1)/2;
            if (pThis->cmp(kTemp, getAddr(pThis->MaxKey, parent, pThis->keySize))>0)
            {
                memcpy(getAddr(pThis->MaxKey, itr, pThis->keySize), getAddr(pThis->MaxKey, parent, pThis->keySize), pThis->keySize);
                memcpy(getAddr(pThis->MaxObj, itr, pThis->objSize), getAddr(pThis->MaxObj, parent, pThis->objSize), pThis->objSize);
                itr = parent;
            }
            else
                break;
        }
        memcpy(getAddr(pThis->MaxKey, itr, pThis->keySize), kTemp, pThis->keySize);
        memcpy(getAddr(pThis->MaxObj, itr, pThis->objSize), oTemp, pThis->objSize);
    }
    else if (type == 2)
    {
        /*last node Min*/
        memcpy(kTemp, getAddr(pThis->MinKey, itr, pThis->keySize), pThis->keySize);
        memcpy(oTemp, getAddr(pThis->MinObj, itr, pThis->objSize), pThis->objSize);
        while(itr > 0)
        {
            parent = (itr-1)/2;
            if (pThis->cmp(kTemp, getAddr(pThis->MinKey, parent, pThis->keySize))<0)
            {
                memcpy(getAddr(pThis->MinKey, itr, pThis->keySize), getAddr(pThis->MinKey, parent, pThis->keySize), pThis->keySize);
                memcpy(getAddr(pThis->MinObj, itr, pThis->objSize), getAddr(pThis->MinObj, parent, pThis->objSize), pThis->objSize);
                itr = parent;
            }
            else
                break;
        }
        memcpy(getAddr(pThis->MinKey, itr, pThis->keySize), kTemp, pThis->keySize);
        memcpy(getAddr(pThis->MinObj, itr, pThis->objSize), oTemp, pThis->objSize);
    }
    else if (type == 3)
    {
        /*last node Max without Min*/
        memcpy(kTemp, getAddr(pThis->MaxKey, itr, pThis->keySize), pThis->keySize);
        memcpy(oTemp, getAddr(pThis->MaxObj, itr, pThis->objSize), pThis->objSize);
        int flag;
        while(itr > 0)
        {
            parent = (itr-1)/2;
            if (pThis->cmp(kTemp, getAddr(pThis->MinKey, parent, pThis->keySize))<0)
            {
                if (itr == pThis->size/2)
                {
                    memcpy(getAddr(pThis->MaxKey, itr, pThis->keySize), getAddr(pThis->MinKey, parent, pThis->keySize), pThis->keySize);
                    memcpy(getAddr(pThis->MaxObj, itr, pThis->objSize), getAddr(pThis->MinObj, parent, pThis->objSize), pThis->objSize);
                    flag = 2;
                }
                memcpy(getAddr(pThis->MinKey, itr, pThis->keySize), getAddr(pThis->MinKey, parent, pThis->keySize), pThis->keySize);
                memcpy(getAddr(pThis->MinObj, itr, pThis->objSize), getAddr(pThis->MinObj, parent, pThis->objSize), pThis->objSize);
                itr = parent;
                flag = 1;
            }
            else if (pThis->cmp(kTemp, getAddr(pThis->MaxKey, parent, pThis->keySize))>0)
            {
                memcpy(getAddr(pThis->MaxKey, itr, pThis->keySize), getAddr(pThis->MaxKey, parent, pThis->keySize), pThis->keySize);
                memcpy(getAddr(pThis->MaxObj, itr, pThis->keySize), getAddr(pThis->MaxObj, parent, pThis->keySize), pThis->objSize);
                itr = parent;
                flag = 2;
            }
            else
                break;
        }
        if (flag == 1)
        {
            memcpy(getAddr(pThis->MinKey, itr, pThis->keySize), kTemp, pThis->keySize);
            memcpy(getAddr(pThis->MinObj, itr, pThis->objSize), oTemp, pThis->objSize);
        }
        else
        {
            memcpy(getAddr(pThis->MaxKey, itr, pThis->keySize), kTemp, pThis->keySize);
            memcpy(getAddr(pThis->MaxObj, itr, pThis->keySize), oTemp, pThis->objSize);
        }
    }
    free(kTemp);
    free(oTemp);
    hmInsert(pThis->pObjToIndex, pObj, pKey);
    pThis->size++;
    return __DS__PQ__NORMAL__;
}
int pqExtractMax(struct pq_t *pThis, void *pRetKey, void *pRetObj){
    if(pThis->size == 0)
        return __DS__PQ__EMPTY__;
    memcpy(pRetKey, pThis->MaxKey, pThis->keySize);
    memcpy(pRetObj, pThis->MaxObj, pThis->objSize);
    size_t end = (pThis->size-1)/2, itr = 0;
    if (end == 0)
    {
        void *keyTemp = malloc(pThis->keySize);
        void *objTemp = malloc(pThis->objSize);
        memcpy(keyTemp, pThis->MaxKey, pThis->keySize);
        memcpy(objTemp, pThis->MaxObj, pThis->objSize);
        memcpy(pThis->MaxKey, pThis->MinKey, pThis->keySize);
        memcpy(pThis->MaxObj, pThis->MinObj, pThis->objSize);
        memcpy(pThis->MinKey, keyTemp, pThis->keySize);
        memcpy(pThis->MinObj, objTemp, pThis->objSize);
        free(keyTemp);
        free(objTemp);
        pThis->size--;
        hmDelete(pThis->pObjToIndex, pRetObj);
        return __DS__PQ__NORMAL__;
    }
    void *kTemp = malloc(pThis->keySize);
    memcpy(kTemp, getAddr(pThis->MaxKey, end, pThis->keySize), pThis->keySize);
    void *oTemp = malloc(pThis->objSize);
    memcpy(oTemp, getAddr(pThis->MaxObj, end, pThis->objSize), pThis->objSize);
    if (pThis->size%2 == 0)
    {
        void *keyTemp = malloc(pThis->keySize);
        void *objTemp = malloc(pThis->objSize);
        memcpy(keyTemp, getAddr(pThis->MaxKey, end, pThis->keySize), pThis->keySize);
        memcpy(objTemp, getAddr(pThis->MaxObj, end, pThis->objSize), pThis->objSize);
        memcpy(getAddr(pThis->MaxKey, end, pThis->keySize), getAddr(pThis->MinKey, end, pThis->keySize), pThis->keySize);
        memcpy(getAddr(pThis->MaxObj, end, pThis->objSize), getAddr(pThis->MinObj, end, pThis->objSize), pThis->objSize);
        memcpy(getAddr(pThis->MinKey, end, pThis->keySize), keyTemp, pThis->keySize);
        memcpy(getAddr(pThis->MinObj, end, pThis->objSize), objTemp, pThis->objSize);
        free(keyTemp);
        free(objTemp);
    }
    if (itr*2+2 == end && pThis->size%2 == 1)
    {
        if (pThis->cmp(kTemp, getAddr(pThis->MaxKey,2*itr+1, pThis->keySize)) < 0)
        {
            memcpy(getAddr(pThis->MaxKey, itr, pThis->keySize), getAddr(pThis->MaxKey, 2*itr+1, pThis->keySize), pThis->keySize);
            memcpy(getAddr(pThis->MaxObj, itr, pThis->objSize), getAddr(pThis->MaxObj, 2*itr+1, pThis->objSize), pThis->objSize);
            itr = 2*itr+1;
        }
    }
    while(itr <= end)
    {
        if (itr*2+1 >= end)
            break;
        if (pThis->cmp(kTemp, getAddr(pThis->MaxKey, 2*itr+1, pThis->keySize)) > 0 && pThis->cmp(kTemp, getAddr(pThis->MaxKey, 2*itr+2, pThis->keySize)) > 0)
            break;
        if (pThis->cmp(getAddr(pThis->MaxKey, 2*itr+1, pThis->keySize), getAddr(pThis->MaxKey, 2*itr+2, pThis->keySize)) > 0)
        {
            memcpy(getAddr(pThis->MaxKey, itr, pThis->keySize), getAddr(pThis->MaxKey, 2*itr+1, pThis->keySize), pThis->keySize);
            memcpy(getAddr(pThis->MaxObj, itr, pThis->objSize), getAddr(pThis->MaxObj, 2*itr+1, pThis->objSize), pThis->objSize);
            itr = 2*itr+1;
        }
        else
        {
            memcpy(getAddr(pThis->MaxKey, itr, pThis->keySize), getAddr(pThis->MaxKey, 2*itr+2, pThis->keySize), pThis->keySize);
            memcpy(getAddr(pThis->MaxObj, itr, pThis->objSize), getAddr(pThis->MaxObj, 2*itr+2, pThis->objSize), pThis->objSize);
            itr = 2*itr+2;
        }
    }
    memcpy(getAddr(pThis->MaxKey, itr, pThis->keySize), kTemp, pThis->keySize);
    memcpy(getAddr(pThis->MaxObj, itr, pThis->objSize), oTemp, pThis->objSize);
    hmDelete(pThis->pObjToIndex, pRetObj);
    pThis->size--;
    return __DS__PQ__NORMAL__;
}

int pqMax(struct pq_t *pThis, void *pRetKey, void *pRetObj){
    if(pThis->size == 0)
        return __DS__PQ__EMPTY__;
    memcpy(pRetKey, pThis->MaxKey, pThis->keySize);
    memcpy(pRetObj, pThis->MaxObj, pThis->objSize);
    return __DS__PQ__NORMAL__;
}

/* bonus2 */
int pqExtractMin(struct pq_t *pThis, void *pRetKey, void *pRetObj){
    if(pThis->size == 0)
        return __DS__PQ__EMPTY__;
    size_t end = (pThis->size-1)/2, itr = 0;
    if ( pThis->size == 1)
    {
        memcpy(pRetKey, pThis->MaxKey, pThis->keySize);
        memcpy(pRetObj, pThis->MaxObj, pThis->objSize);
    }
    else
    {
        memcpy(pRetKey, pThis->MinKey, pThis->keySize);
        memcpy(pRetObj, pThis->MinObj, pThis->objSize);
    }
    if (end == 0)
    {
        pThis->size--;
        hmDelete(pThis->pObjToIndex, pRetObj);
        return __DS__PQ__NORMAL__;
    }
    void *kTemp = malloc(pThis->keySize);
    void *oTemp = malloc(pThis->objSize);
    if (pThis->size%2)
    {
        memcpy(kTemp, getAddr(pThis->MaxKey, end, pThis->keySize), pThis->keySize);
        memcpy(oTemp, getAddr(pThis->MaxObj, end, pThis->objSize), pThis->objSize);
    }
    else
    {
        memcpy(kTemp, getAddr(pThis->MinKey, end, pThis->keySize), pThis->keySize);
        memcpy(oTemp, getAddr(pThis->MinObj, end, pThis->objSize), pThis->objSize);
    }
    if (itr*2+2 == end && pThis->size%2 == 1)
    {
        if (pThis->cmp(kTemp, getAddr(pThis->MinKey,2*itr+1, pThis->keySize)) > 0)
        {
            memcpy(getAddr(pThis->MinKey, itr, pThis->keySize), getAddr(pThis->MinKey, 2*itr+1, pThis->keySize), pThis->keySize);
            memcpy(getAddr(pThis->MinObj, itr, pThis->objSize), getAddr(pThis->MinObj, 2*itr+1, pThis->objSize), pThis->objSize);
            itr = 2*itr+1;
        }
    }
    while(itr <= end)
    {
        if (itr*2+1 >= end)
            break;
        if (pThis->cmp(kTemp, getAddr(pThis->MinKey, 2*itr+1, pThis->keySize)) < 0 && pThis->cmp(kTemp, getAddr(pThis->MinKey, 2*itr+2, pThis->keySize)) < 0)
            break;
        if (pThis->cmp(getAddr(pThis->MinKey, 2*itr+1, pThis->keySize), getAddr(pThis->MinKey,2*itr+2, pThis->keySize)) < 0)
        {
            memcpy(getAddr(pThis->MinKey, itr, pThis->keySize), getAddr(pThis->MinKey, 2*itr+1, pThis->keySize), pThis->keySize);
            memcpy(getAddr(pThis->MinObj, itr, pThis->objSize), getAddr(pThis->MinObj, 2*itr+1, pThis->objSize), pThis->objSize);
            itr = 2*itr+1;
        }
        else
        {
            memcpy(getAddr(pThis->MinKey, itr, pThis->keySize), getAddr(pThis->MinKey, 2*itr+2, pThis->keySize), pThis->keySize);
            memcpy(getAddr(pThis->MinObj, itr, pThis->objSize), getAddr(pThis->MinObj, 2*itr+2, pThis->objSize), pThis->objSize);
            itr = 2*itr+2;
        }
    }
    memcpy(getAddr(pThis->MinKey, itr, pThis->keySize), kTemp, pThis->keySize);
    memcpy(getAddr(pThis->MinObj, itr, pThis->objSize), oTemp, pThis->objSize);
    hmDelete(pThis->pObjToIndex, pRetObj);
    pThis->size--;
    return __DS__PQ__NORMAL__;
}
int pqMin(struct pq_t *pThis, void *pRetKey, void *pRetObj){
    if(pThis->size == 0)
        return __DS__PQ__EMPTY__;
    if (pThis->size == 1)
    {
        memcpy(pRetKey, pThis->MaxKey, pThis->keySize);
        memcpy(pRetObj, pThis->MaxObj, pThis->objSize);
    }
    else
    {
        memcpy(pRetKey, pThis->MinKey, pThis->keySize);
        memcpy(pRetObj, pThis->MinObj, pThis->objSize);
    }
    return __DS__PQ__NORMAL__;
}

/* bonus3 */
int pqGetKey(struct pq_t *pThis, void *pObj, void *pRetKey){
    if(hmGet(pThis->pObjToIndex, pObj, pRetKey) == __DS__HM__KEY_NOT_EXIST__ )
        return __DS__PQ__OBJ_NOT_EXIST__;
    return __DS__PQ__NORMAL__;
}

int pqChangeKey(struct pq_t *pThis, void *pObj, void *pNewKey){
    if(hmSet(pThis->pObjToIndex, pObj, pNewKey) == __DS__HM__KEY_NOT_EXIST__ )
        return __DS__PQ__OBJ_NOT_EXIST__;
    void *nowKey;
    hmGet(pThis->pObjToIndex, pObj, nowKey);
    return __DS__PQ__NORMAL__;
}

/* Very Slow Hash Map Implementation */
#define MIN_HASH_CAP 1024
struct hm_t{
    struct h_node **objlist;
    struct h_node **objend;
    size_t size, objSize, keySize;
};

struct hm_t *hmAlloc(){
    return malloc(sizeof(struct hm_t));
}

struct h_node{
    void *obj;
    void *key;
    struct h_node* next;
};

/*
void hmDebug(struct hm_t* pThis){
    printf("size/cap: %zu/%zu\nkey,val size: %zu, %zu\n", pThis->size, pThis->cap, pThis->keySize, pThis->keySize);
    size_t i;
    printf("key:");
    for(i=0; i<pThis->size; i++){
        printData(__DS__SHORT__, pThis->keyArray + i*pThis->keySize);
        printf("%c", (i!=(pThis->size-1))?' ':'\n');
    }
    printf("val:");
    for(i=0; i<pThis->size; i++){
        printData(__DS__SHORT__, pThis->valArray + i*pThis->valSize);
        printf("%c", (i!=(pThis->size-1))?' ':'\n');
    }
}
*/
int hash(int sz, void *obj)
{
    short i = *( ((short *)obj) );
    return (int)(i&(MIN_HASH_CAP-1));
}

struct h_node *new_h_node(void *obj, size_t objSize, void *key, size_t keySize){
    struct h_node *new = malloc(sizeof(struct h_node));
    new->obj = malloc(objSize);
    memcpy(new->obj, obj, objSize);
    new->key = malloc(keySize);
    memcpy(new->key, key,  keySize);
    new->next = NULL;
    return new;
}
int hmInit(struct hm_t *pThis, size_t objSize, size_t keySize){
    pThis->size = 0;
    pThis->objSize = objSize;
    pThis->objlist = malloc(sizeof(struct h_node *)*MIN_HASH_CAP);
    pThis->objend = malloc(sizeof(struct h_node *)*MIN_HASH_CAP);
    if (pThis->objlist == NULL || pThis->objend == NULL)
    {
        return __DS__HM__OUT_OF_MEM__;
    }
    int i;
    for (i = 0; i < MIN_HASH_CAP; i++)
    {
        pThis->objlist[i] = NULL;
        pThis->objend[i] = NULL;
    }
    return __DS__HM__NORMAL__;
}
int hmFree(struct hm_t *pThis){
    if (pThis->objlist)
    {
        int i;
        for (i = 0; i < MIN_HASH_CAP; i++)
        {
            if (pThis->objlist[i])
                free(pThis->objlist[i]);
        }
        free(pThis->objlist);
    }
    free(pThis);
}

int hmSize(struct hm_t *pThis){
    return pThis->size;
}
int hmInsert(struct hm_t *pThis, void *pObj, void *pKey){
    if(hmKeyExist(pThis, pObj))
        return __DS__HM__KEY_EXIST__;
    int val = hash(pThis->objSize, pObj);
    if (pThis->objend[val] == NULL)
    {
        pThis->objend[val] = pThis->objlist[val] = new_h_node(pObj, pThis->objSize, pKey, pThis->keySize);
    }
    else
    {
        pThis->objend[val]->next = new_h_node(pObj, pThis->objSize, pKey, pThis->keySize);
        pThis->objend[val] = pThis->objend[val]->next;
    }
    pThis->size++;
    return __DS__HM__NORMAL__;
}
int hmDelete(struct hm_t *pThis, void *pObj){
    int val = hash(pThis->objSize, pObj);
    struct h_node* itr = pThis->objlist[val];
    while (1)
    {
        if (memcmp(itr->obj, pObj, pThis->size) == 0)
        {
            if ( itr->next == NULL)
            {
                free(itr);
                pThis->objlist[val] = NULL;
            }
            else
            {
                struct h_node *temp = itr;
                itr = itr->next;
                free(temp);
            }
            return __DS__HM__NORMAL__;
        }
        if (itr->next == NULL)
            return __DS__HM__KEY_NOT_EXIST__;
        /*if (memcmp(itr->next->obj, pObj, pThis->size) == 0)*/
        /*{*/
            /*struct h_node *temp = itr->next;*/
            /*itr->next = itr->next->next;*/
            /*free(temp);*/
            /*return __DS__HM__NORMAL__;*/
        /*}*/
        itr = itr->next;
    }

}
int hmKeyExist(struct hm_t *pThis, void *pObj){
    int val = hash(pThis->objSize, pObj);
    struct h_node* itr = pThis->objlist[val];
    while (1)
    {
        if (itr == NULL)
            return 0;
        if (memcmp(itr->obj, pObj, pThis->size) == 0)
            return 1;
        if (itr->next == NULL)
            return 0;
        if (memcmp(itr->next->obj, pObj, pThis->size == 0))
            return 1;
        itr = itr->next;
    }
}
int hmUnion(struct hm_t *pThis1, struct hm_t *pThis2)
{
    int i;
    for(i = 0; i < MIN_HASH_CAP; i++)
    {
        if (pThis1->objend[i] != NULL || pThis2->objlist[i] != NULL)
        {
            if (pThis1->objend[i] == NULL)
            {
                pThis1->objlist[i] = pThis2->objlist[i];
                pThis1->objend[i] = pThis2->objend[i];
            }
            if (pThis2->objlist[i] == NULL)
                continue;
            pThis1->objend[i]->next = pThis2->objlist[i];
            pThis1->objend[i] = pThis2->objend[i];
        }
    }
    pThis2->objlist = pThis2->objend = NULL;
    return __DS__HM__NORMAL__;
}
int hmGet(struct hm_t *pThis, void *pObj, void *pRetKey)
{
    int val = hash(pThis->objSize, pObj);
    struct h_node* itr = pThis->objlist[val];
    while (1)
    {
        if (itr == NULL)
            return __DS__HM__KEY_NOT_EXIST__;
        if (memcmp(itr->obj, pObj, pThis->objSize) == 0)
        {
            memcpy(pRetKey, itr->key, pThis->keySize);
        }
        if (itr->next == NULL)
            return __DS__HM__KEY_NOT_EXIST__;
        if (memcmp(itr->next->obj, pObj, pThis->objSize == 0))
        {
            memcpy(pRetKey, itr->next->key, pThis->keySize);
        }
        itr = itr->next;
    }
    return __DS__HM__NORMAL__;
}
int hmSet(struct hm_t *pThis, void *pObj, void *pNewKey)
{
    int val = hash(pThis->objSize, pObj);
    struct h_node* itr = pThis->objlist[val];
    while (1)
    {
        if (itr == NULL)
            return __DS__HM__KEY_NOT_EXIST__;
        if (memcmp(itr->obj, pObj, pThis->objSize) == 0)
        {
            memcpy(itr->key, pNewKey, pThis->keySize);
        }
        if (itr->next == NULL)
            return __DS__HM__KEY_NOT_EXIST__;
        if (memcmp(itr->next->obj, pObj, pThis->objSize == 0))
        {
            memcpy(itr->next->key, pNewKey, pThis->keySize);
        }
        itr = itr->next;
    }
    return __DS__HM__NORMAL__;
}
/* Hash Map Implementation End */
