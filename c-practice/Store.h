#ifndef STORE_H
#define STORE_H

#include <stddef.h>

#include "HashMap.h"
#include "LinkedList.h"
#include "MinHeap.h"

// store_set 반환값. 나중에 CLI가 OOM 에러 문구를 따로 출력하려고 구분한다.
// 규칙: store 함수 전체에서 반환 0 이상은 성공(Redis 응답 값), 음수는 내부 에러. 예외 없음.
#define STORE_OK 1
#define STORE_ERR_ALLOC -2
#define STORE_ERR_OOM -1

// Data.expire_at이 이 값이면 만료 시간이 없다.
#define NO_EXPIRE 0

typedef struct Store {
    HashMap map;    // key → Data. Data와 key 문자열을 소유한다.
    LinkedList lru; // 사용 순서. HEAD = 가장 최근, TAIL = 가장 오래됨. LRU Node를 소유한다.
    MinHeap heap;   // (expire_at, key) 만료 순서. 오래된 항목이 남을 수 있다(lazy deletion).
    size_t used_memory;  // 모든 키의 entry_size 합. 노드·포인터·버킷 크기는 세지 않는다.
    size_t maxmemory;    // 0이면 무제한.
    size_t evicted_keys; // LRU로 자동 제거된 키 개수(누적). 만료로 지운 키는 세지 않는다.
} Store;

/*
 * now: 현재 시각(ms). 테스트는 숫자를 직접 넣고, 실제 프로그램은 현재 시각을 넣는다.
 * 반환: set은 STORE_OK / STORE_ERR_ALLOC / STORE_ERR_OOM.
 *       del은 삭제 1, 없는 키 0. get은 찾으면 1, 없으면 0.
 *       expire는 설정(또는 즉시 만료) 1, 없는 키 0, 힙 할당 실패 STORE_ERR_ALLOC.
 *       ttl은 성공 0 이상 / 에러 음수. 응답 값(없는 키 -2, 만료 없음 -1, 남은 초 내림)은 *out_ttl로.
 * 소유권: LRU Node의 data는 해시맵의 Data를 빌려 가리킬 뿐 해제하지 않는다.
 */
int store_set(Store* store, const char* key, int value, long long now);
int store_get(Store* store, const char* key, int* out_value, long long now);
int store_del(Store* store, const char* key, long long now);
int store_expire(Store* store, const char* key, long long seconds, long long now);
int store_ttl(Store* store, const char* key, long long now, long long* out_ttl);
void store_free(Store* store);

#endif
