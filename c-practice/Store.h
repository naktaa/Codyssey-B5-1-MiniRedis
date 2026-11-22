#ifndef STORE_H
#define STORE_H

#include "HashMap.h"
#include "LinkedList.h"

typedef struct Store {
    HashMap map;    // key → Data. Data와 key 문자열을 소유한다.
    LinkedList lru; // 사용 순서. HEAD = 가장 최근, TAIL = 가장 오래됨. LRU Node를 소유한다.
} Store;

/*
 * 반환: set·del은 성공 1, 실패(할당 실패·없는 키) 0. get은 찾으면 1, 없으면 0.
 * 소유권: LRU Node의 data는 해시맵의 Data를 빌려 가리킬 뿐 해제하지 않는다.
 */
int store_set(Store* store, const char* key, int value);
int store_get(Store* store, const char* key, int* out_value);
int store_del(Store* store, const char* key);
void store_free(Store* store);

#endif
