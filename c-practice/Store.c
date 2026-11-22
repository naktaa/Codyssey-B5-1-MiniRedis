#include <stdlib.h>

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
 */

/*
 * 키를 저장하거나 갱신하고, 그 키를 LRU 맨 앞으로 둔다.
 * TODO(1): put 전에 hashmap_get_data로 기존 키인지 확인해 두기.
 * TODO(2): hashmap_put. 실패하면 0.
 * TODO(3): 기존 키 → move_to_front.
 *          새 키 → LRU Node를 만들어 data에 Data를 연결하고 insert_front, Data의 lru_node도 채우기.
 *          Node 할당이 실패하면 방금 넣은 키를 hashmap_remove로 되돌리고 0.
 */
int store_set(Store* store, const char* key, int value) {
    HashMap* map = &store->map;
    Data* data = hashmap_get_data(map, key);
    int res;

    res = hashmap_put(map, key, value);
    if (!res) return 0;

    if (data == NULL) {
        Node* new_node = malloc(sizeof(Node));
        if (new_node == NULL) {
            hashmap_remove(map, key);
            return 0;
        }

        Data* new_data = hashmap_get_data(map, key);
        new_node->data = new_data;
        new_data->lru_node = new_node;
        insert_front(&store->lru, new_data->lru_node);
    }
    else {
        move_to_front(&store->lru, data->lru_node);
    }

    return 1;
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
 */
int store_del(Store* store, const char* key) {
    HashMap* map = &store->map;
    LinkedList* lru = &store->lru;

    Data* data = hashmap_get_data(map, key);
    if (data == NULL) return 0;

    remove_node(lru, data->lru_node);
    hashmap_remove(map, key);

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
