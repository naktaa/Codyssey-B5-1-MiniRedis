#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Store.h"

/*
 * C-4 Store: 해시맵(찾기·저장)과 LRU 리스트(사용 순서)를 묶는다.
 * 해시맵은 LRU를 모르고, LRU 갱신은 모두 이 파일에서 한다.
 *
 *   Data { key, value, lru_node ─┐ }
 *                                ▼
 *   lru:  HEAD → [최근] ⇄ ... ⇄ [오래됨] ← TAIL
 *                  └─data─▶ Data (빌린 주소)
 *
 * 준비: 확인용 main에서 map 필드를 채우고 lru의 HEAD/TAIL을 NULL로 둔다.
 * 구현 순서 권장: get → set → del → free
 *
 * 아래 원형의 ;를 지우고 { } 본문을 직접 작성한다.
 *
 * 메모리 제한 구현 순서 권장: entry_size → delete_data(+store_del 정리) → evict_if_needed → store_set
 */

/*
 * 키 하나가 차지하는 used_memory 크기 = key 바이트 수 + value 길이.
 * C는 value가 int라서 10진수 자릿수로 대체한다(-5 → 2). Python은 value 문자열의 UTF-8 바이트 수를 쓴다.
 * TODO: strlen(key) + snprintf(NULL, 0, "%d", value) 반환하기.
 */
static size_t entry_size(const char* key, int value) {
    return strlen(key) + snprintf(NULL, 0, "%d", value);
}

/*
 * 이미 찾은 Data를 LRU·used_memory·해시맵에서 모두 제거한다. store_del과 evict_if_needed가 함께 쓴다.
 * used_memory 감소는 이 함수 한 곳에서만 한다.
 * TODO: ① used_memory에서 entry_size 빼기 ② remove_node로 LRU 노드 제거
 *       ③ 마지막에 hashmap_remove(map, data->key). 이 호출이 data와 key를 해제하므로 그 뒤로는 읽지 않는다.
 */
static void delete_data(Store* store, Data* data) {
    size_t delete_memory = entry_size(data->key, data->value);

    store->used_memory -= delete_memory;

    remove_node(&store->lru, data->lru_node);
    hashmap_remove(&store->map, data->key);
}

/*
 * SET 이후 maxmemory를 넘었으면 LRU의 가장 오래된 키부터 지운다.
 * TODO: maxmemory > 0 && used_memory > maxmemory && lru.TAIL != NULL 인 동안
 *       TAIL의 Data로 delete_data, evicted_keys++.
 */
static void evict_if_needed(Store* store) {
    while (store->maxmemory > 0 && store->used_memory > store->maxmemory && store->lru.TAIL != NULL) {
        delete_data(store, store->lru.TAIL->data);
        store->evicted_keys++;
    }
}

/*
 * 키를 저장하거나 갱신하고, 그 키를 LRU 맨 앞으로 둔다.
 * TODO(1): put 전에 hashmap_get_data로 기존 키인지 확인해 두기.
 * TODO(2): hashmap_put. 실패하면 0.
 * TODO(3): 기존 키 → move_to_front.
 *          새 키 → LRU Node를 만들어 data에 Data를 연결하고 insert_front, Data의 lru_node도 채우기.
 *          Node 할당이 실패하면 방금 넣은 키를 hashmap_remove로 되돌리고 0.
 * 메모리 제한 TODO:
 *   - 키 하나만으로 maxmemory를 넘는 경우를 거부하기(덮어쓰기면 기존 값 유지).
 *   - 새 키와 기존 키 갱신 각각에 맞게 used_memory 갱신하기.
 *   - 저장 후 메모리 제한을 넘으면 LRU로 제거하기. 반환값은 Store.h의 매크로로.
 */
int store_set(Store* store, const char* key, int value) {
    size_t new_memory = entry_size(key, value);

    // 단일 엔트리가 maxmemory 초과하면 OOM
    if (store->maxmemory > 0 && new_memory > store->maxmemory) {
        return STORE_ERR_OOM;
    }

    HashMap* map = &store->map;
    Data* data = hashmap_get_data(map, key);
    int res;
    size_t prev_memory = 0;
    if (data != NULL) prev_memory = entry_size(data->key, data->value);

    res = hashmap_put(map, key, value);
    if (!res) return STORE_ERR_ALLOC;

    if (data == NULL) {
        Node* new_node = malloc(sizeof(Node));
        if (new_node == NULL) {
            hashmap_remove(map, key);
            return STORE_ERR_ALLOC;
        }

        Data* new_data = hashmap_get_data(map, key);
        new_node->data = new_data;
        new_data->lru_node = new_node;
        insert_front(&store->lru, new_data->lru_node);
    }
    else {
        move_to_front(&store->lru, data->lru_node);
    }

    store->used_memory -= prev_memory;
    store->used_memory += new_memory;
    evict_if_needed(store);
    return STORE_OK;
}

/*
 * 키가 있으면 값을 전달하고 LRU 맨 앞으로 옮긴다.
 * TODO: hashmap_get_data로 찾고, 없으면 0. 있으면 move_to_front 후 값을 전달하기.
 *       (C-4 다음 단계에서 "만료 확인 후에만 갱신" 규칙이 여기에 붙는다)
 */
int store_get(Store* store, const char* key, int* out_value) {
    Data* data = hashmap_get_data(&store->map, key);
    if (data == NULL) return 0;

    move_to_front(&store->lru, data->lru_node);
    *out_value = data->value;

    return 1;
}

/*
 * 키를 버킷과 LRU 양쪽에서 제거한다.
 * TODO: 순서 주의. hashmap_remove가 Data를 해제하므로
 *       ① hashmap_get_data로 Data 얻기 ② remove_node로 LRU 노드 제거
 *       ③ hashmap_remove로 버킷 노드·key·Data 해제.
 * 메모리 제한 TODO: ②③을 delete_data(store, data) 호출로 바꾸기(used_memory 감소도 거기서 한다).
 */
int store_del(Store* store, const char* key) {
    HashMap* map = &store->map;

    Data* data = hashmap_get_data(map, key);
    if (data == NULL) return 0;

    delete_data(store, data);

    return 1;
}

/*
 * Store가 가진 모든 메모리를 해제한다.
 * TODO: LRU 노드만 해제(data가 가리키는 Data는 건드리지 않음)한 뒤 hashmap_free 호출하기.
 */
void store_free(Store* store) {
    Node* now = store->lru.HEAD;

    while (now) {
        Node* next = now->next;
        free(now);
        now = next;
    }

    hashmap_free(&store->map);
}
