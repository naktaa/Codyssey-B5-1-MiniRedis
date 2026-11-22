#ifndef STORE_H
#define STORE_H

#include <stddef.h>

#include "HashMap.h"
#include "LinkedList.h"

// store_set 반환값. 나중에 CLI가 OOM 에러 문구를 따로 출력하려고 구분한다.
#define STORE_OK 1
#define STORE_ERR_ALLOC 0
#define STORE_ERR_OOM -1

typedef struct Store {
    HashMap map;    // key → Data. Data와 key 문자열을 소유한다.
    LinkedList lru; // 사용 순서. HEAD = 가장 최근, TAIL = 가장 오래됨. LRU Node를 소유한다.
    size_t used_memory;  // 모든 키의 entry_size 합. 노드·포인터·버킷 크기는 세지 않는다.
    size_t maxmemory;    // 0이면 무제한.
    size_t evicted_keys; // LRU로 자동 제거된 키 개수(누적).
} Store;

/*
 * 반환: set은 STORE_OK / STORE_ERR_ALLOC / STORE_ERR_OOM.
 *       del은 삭제 1, 없는 키 0. get은 찾으면 1, 없으면 0.
 * 소유권: LRU Node의 data는 해시맵의 Data를 빌려 가리킬 뿐 해제하지 않는다.
 */
int store_set(Store* store, const char* key, int value);
int store_get(Store* store, const char* key, int* out_value);
int store_del(Store* store, const char* key);
void store_free(Store* store);

#endif
