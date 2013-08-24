#include <stdlib.h>
#ifndef __ds__pq__header__
#define __ds__pq__header__

#define __DS__PQ__NORMAL__        0x00000000
#define __DS__PQ__FULL__          0x00000001
#define __DS__PQ__EMPTY__         0x00000002
#define __DS__PQ__OUT_OF_MEM__    0x00000004
#define __DS__PQ__OBJ_EXIST__     0x00000008
#define __DS__PQ__OBJ_NOT_EXIST__ 0x00000010
#define __DS__PQ__DIFF_SIZE__     0x00000020
#define __DS__PQ__NON_INCRE__     0x00000040

struct pq_t;
/* It's bonus version of priority queue, 
 * you can implement DEPQ or/and pq support Change key */
/* Priority Queue store various but fixed-size data */
/* Support <key, object> pair data */
/* Support customized comparison */

/* If it is OOM(out of memory) in the memory allocation, 
 * Please return __DS__PQ__OUT_OF_MEM__ for each function which supports to 
 * return __DS__PQ__NORMAL__ when normal execution. 
 */

struct pq_t *pqAlloc();
/* Allocate memory for pq. Return NULL if not enough memory */

int pqInit(struct pq_t *pThis, size_t keySize, size_t objSize, size_t cap, 
           int (*cmp)(const void*, const void*));
/* Initialize pThis: set the size per (key, object) as (keySize, objSize)
 * and the capacity as cap.
 * the comparison function as cmp: 
 *     cmp(keyA, keyB) returns -1 if keyA < keyB
 *     cmp(keyA, keyB) returns 0 if keyA = keyB
 *     cmp(keyA, keyB) returns 1 if keyA > keyB
 * After the initialization, there's nothing. So size is 0.
 * Return 0 for normal execution.
 * Bonus1: Using dynamic capacity when cap=0.
 */

int pqFree(struct pq_t *pThis);
/* Release the memory.
 * Deep release: You should release the memory allocated by any operation.
 * Return 0 for normal execution.
 */

size_t pqCap(struct pq_t *pThis);
/* Return current capacity */

size_t pqSize(struct pq_t *pThis);
/* Return number of elements in the priority queue */

size_t pqKeySize(struct pq_t *pThis);
/* Return the size of key in the priority queue */

size_t pqObjSize(struct pq_t *pThis);
/* Return the size of obj in the priority queue */

int pqEmpty(struct pq_t *pThis);
/* Return zero if priority queue is not empty */

int pqInsert(struct pq_t *pThis, void *pKey, void *pObj);
/* Insert <key, obj> pair into Priority Queue
 * Return __DS__PQ__FULL__ if PQ is full
 * Return __DS__PQ__OBJ_EXIST__ if *pObj is in the PQ now
 * Return 0 for normal execution
 * Performance requirement: O(logn)-time per operation.
 */

int pqExtractMax(struct pq_t *pThis, void *pRetKey, void *pRetObj);
/* Remove the obj with maximum key in qriority queue
 * Write result to <*pRetKey, *pRetObj>
 * Return __DS__PQ__EMPTY__ if PQ is empty
 * Return 0 for normal execution
 * Performance requirement: O(logn)-time per operation.
 */

int pqMax(struct pq_t *pThis, void *pRetKey, void *pRetObj);
/* Return the obj with maximum key in priority queue
 * Write result to <*pRetKey, *pRetObj>
 * Return __DS__PQ__EMPTY__ if PQ is empty
 * Return 0 for normal execution
 * Performance requirement: O(1)-time per operation.
 */

/* bonus2: DEPQ(Double-Ended Priority Queue) */
int pqExtractMin(struct pq_t *pThis, void *pRetKey, void *pRetObj);
/* Remove the obj with minimum key in qriority queue
 * Write result to <*pRetKey, *pRetObj>
 * Return __DS__PQ__EMPTY__ if PQ is empty
 * Return 0 for normal execution
 * Performance requirement: O(logn)-time per operation.
 */

int pqMin(struct pq_t *pThis, void *pRetKey, void *pRetObj);
/* Return the obj with minimum key in priority queue
 * Write result to <*pRetKey, *pRetObj>
 * Return __DS__PQ__EMPTY__ if PQ is empty
 * Return 0 for normal execution
 * Performance requirement: O(1)-time per operation.
 */
/* bonus2 end */

/* bonus3: change key */
int pqGetKey(struct pq_t *pThis, void *pObj, void *pRetKey);
/* Write the key of obj *pObj to *pRetKey 
 * Return __DS__PQ__OBJ_NOT_EXIST__ if *pObj doesn't exist in the priority queue
 * Return 0 for normal execution
 */

int pqChangeKey(struct pq_t *pThis, void *pObj, void *pNewKey);
/* Change the key of obj *pObj to *pNewKey in the priority queue 
 * Just increasing key in SEPQ(Single-Ended Priority Queue) version
 * So, return __DS__PQ__NON_INCRE__ in SEPQ(Single-Ended Priority Queue) if *pNewKey < origin key
 * Return __DS__PQ__OBJ_NOT_EXIST__ if *pObj doesn't exist in the priority queue
 * Return 0 for normal execution
 * Performance requirement: O(logn)-time per operation.
 */
/* bonus3 end */

#endif
