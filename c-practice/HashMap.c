#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HashMap.h"

/*
 * C-2 학습용 선언 템플릿: 문자열 key와 정수 value를 저장한다.
 * 구조체와 함수 원형만 제공하며, 모든 함수 본문은 직접 구현한다.
 * 기존 Linked_list.c의 전역 HEAD/TAIL 대신 버킷마다 체인을 관리한다.
 * Data/HashMap 구조체와 원형은 Store에서도 쓰도록 HashMap.h로 옮겼다.
 */

/*
 * key의 문자들을 바탕으로 해시값을 계산한다. 계산 규칙은 직접 설계한다.
 * TODO: 해시값과 capacity로 버킷 번호를 정하는 책임을 어디에 둘지 정하기.
 */
size_t hashmap_hash(const char* key) {
    size_t sum = 0;

    for (int i = 0; key[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)key[i];
        sum = sum * 31 + ch;
    }

    return sum;
}

/*
 * key가 들어 있는 노드를 찾는다. 없으면 NULL.
 * out_list가 NULL이 아니면 key가 속한 버킷 리스트의 주소를 담아준다
 * (put의 삽입, remove의 삭제에 필요).
 */
static Node* hashmap_find_node(const HashMap* map, const char* key, LinkedList** out_list) {
    size_t idx = hashmap_hash(key) % map->capacity;
    LinkedList* list = &map->buckets[idx];

    // 못 찾아도 put이 이 리스트에 넣어야 하므로 탐색 전에 먼저 채운다.
    if (out_list) {
        *out_list = list;
    }

    for (Node* now = list->HEAD; now; now = now->next) {
        Data* data = now->data;
        if (strcmp(data->key, key) == 0) {
            return now;
        }
    }
    return NULL;
}

/*
 * 키가 있으면 값을 갱신하고, 없으면 새 항목을 저장한다.
 * 반환: 성공 1, 할당 등 저장 실패 0. 기존 키 갱신 시 count는 늘지 않는다.
 * TODO: 키 문자열의 보관 방식, 충돌 처리, 새 항목 연결을 구현하기.
 * TODO: 로드 팩터 0.75 초과 시 버킷 2배 확장과 항목 재배치를 구현하기.
 * 새 Data는 calloc으로 할당해 lru_node = NULL, expire_at = 0(NO_EXPIRE)으로 시작한다(Store가 채우기 전 쓰레기 값 방지).
 */
int hashmap_put(HashMap* map, const char* key, int value) {
    LinkedList* list;
    Node* found = hashmap_find_node(map, key, &list);

    // 이미 있는 키: 값만 바꾸고 count는 그대로 둔다.
    if (found) {
        Data* data = found->data;
        data->value = value;
        return 1;
    }

    Data* NewData = calloc(1, sizeof(Data)); // calloc: lru_node·expire_at을 0(NULL·NO_EXPIRE)으로 시작해 쓰레기 값을 없앤다.
    if (NewData == NULL) {
        return 0;
    }

    NewData->key = malloc(strlen(key) + 1);
    if (NewData->key == NULL) {
        free(NewData);
        return 0;
    }

    strcpy(NewData->key, key);
    NewData->value = value;

    Node* NewNode = malloc(sizeof(Node));
    if (NewNode == NULL) {
        free(NewData->key);
        free(NewData);
        return 0;
    }

    NewNode->data = NewData;
    // prev/next는 insert_back이 설정한다. list는 find_node가 채워 준 버킷이다.
    insert_back(list, NewNode);
    map->count++;

    // 버킷 2배 확장
    if (map->count * 4 > map->capacity * 3) {
        LinkedList* new_buckets = calloc(map->capacity * 2, sizeof(LinkedList));
        if (new_buckets == NULL) {
            return 1;
        }

        size_t new_capacity = map->capacity * 2;

        for (size_t i = 0; i < map->capacity; i++) {
            LinkedList* prev_list = &map->buckets[i];
            Node* now = prev_list->HEAD;

            while (now) {
                Data* data = now->data;
                size_t idx = hashmap_hash(data->key) % new_capacity;
                LinkedList* nextList = &new_buckets[idx];
                Node* now_temp = now->next;
                insert_back(nextList, now);
                now = now_temp;
            }
        }

        LinkedList* temp_buckets = map->buckets;
        map->buckets = new_buckets;
        free(temp_buckets);
        map->capacity = new_capacity;
    }

    return 1;
}

/*
 * 반환: 키를 찾으면 1, 없으면 0. 찾은 정수 값은 *out_value에 전달한다.
 * 값 0도 저장할 수 있으므로 값 자체와 찾음 여부를 구분한다.
 * out_value는 결과를 쓸 수 있는 유효한 int 변수의 주소여야 한다.
 * TODO: 같은 버킷의 후보들을 실제 키 내용으로 구별하기.
 * 참고: 문자열의 ==는 주소 비교이고, strcmp는 문자열 내용 비교다.
 */
int hashmap_get(const HashMap* map, const char* key, int* out_value) {
    // 리스트는 필요 없으므로 NULL을 넘긴다.
    Node* found = hashmap_find_node(map, key, NULL);
    if (found == NULL) {
        return 0;
    }

    Data* data = found->data;
    *out_value = data->value;
    return 1;
}

/*
 * 반환: 삭제했으면 1, 해당 키가 없으면 0.
 * TODO: 체인 연결, 버킷의 시작 주소, count, 메모리 수명을 함께 관리하기.
 * stdio.h의 remove와 충돌하지 않도록 hashmap_remove라는 이름을 쓴다.
 */
int hashmap_remove(HashMap* map, const char* key) {
    LinkedList* list;
    Node* found = hashmap_find_node(map, key, &list);
    if (found == NULL) {
        return 0;
    }

    // remove_node가 노드를 해제하므로 Data 주소를 먼저 꺼내 둔다.
    Data* data = found->data;
    remove_node(list, found);
    free(data->key);
    free(data);
    map->count--;

    return 1;
}

/*
 * 반환: 키가 있으면 1, 없으면 0.
 * TODO: 조회와 중복되는 부분을 어떻게 사용할지 생각하기.
 */
int hashmap_contains(const HashMap* map, const char* key) {
    // get을 부르지 않는다: 나중에 get에만 LRU 갱신이 붙기 때문이다.
    return hashmap_find_node(map, key, NULL) != NULL;
}

/*
 * C 실습에서는 저장된 모든 키를 출력하는 형태로 시작한다. 순서는 무관하다.
 * TODO: 전체 버킷과 각 체인에서 키를 빠뜨리지 않고 확인하기.
 */
void hashmap_keys(const HashMap* map) {
    if (!map->count) {
        printf("(empty array)\n");
    }
    else {
        for (size_t i = 0; i < map->capacity; i++) {
            LinkedList* list = &map->buckets[i];

            for (Node* now = list->HEAD; now; now = now->next) {
                Data* data = now->data;
                printf("\"%s\"\n", data->key);
            }
        }
    }
}

/* 반환: 현재 저장된 서로 다른 키의 개수. TODO: 직접 구현하기. */
size_t hashmap_size(const HashMap* map) {
    return map->count;
}

/*
 * key의 Data 주소를 돌려준다. 없으면 NULL. Store가 lru_node에 접근할 때 쓴다.
 * TODO: hashmap_find_node 결과로 Data 주소를 꺼내기.
 */
Data* hashmap_get_data(const HashMap* map, const char* key) {
    Node* found = hashmap_find_node(map, key, NULL);

    if (found == NULL) return NULL;

    return found->data;
}

/*
 * 해시맵이 가진 모든 메모리를 해제한다. lru_node는 Store가 따로 해제한다.
 * TODO: 모든 버킷의 노드마다 key → Data → Node 순서로 해제하고, 마지막에 buckets 배열을 해제하기.
 */
void hashmap_free(HashMap* map) {
    for (size_t i = 0; i < map->capacity; i++) {
        Node* now = map->buckets[i].HEAD;

        while (now) {
            Node* next = now->next;
            Data* data = now->data;

            free(data->key);
            free(data);
            free(now);
            now = next;
        }
    }
    free(map->buckets);
}

/*
 * 구현 전 준비 TODO:
 * - 초기 capacity, buckets 저장 공간, 빈 버킷과 count의 초기 상태를 정한다.
 * - key 문자열과 Entry, 버킷 배열을 누가 소유하고 언제 해제할지 정한다.
 * - 생성·정리 및 확장 보조 함수가 필요하면 원형과 본문을 직접 추가한다.
 * - 생성 실패나 확장 실패 시 기존 항목을 보존할 방법을 생각한다.
 *
 * 입력 전제: map과 key는 유효하고, map은 준비된 상태여야 한다.
 * 조회용 const char*는 그 인자를 통해 키 문자를 변경하지 않는다는 뜻이다.
 * const HashMap*는 그 인자를 통해 구조체 멤버를 변경하지 않는다는 뜻이다.
 * 포인터가 가리키는 Entry까지 자동으로 불변이 되는 것은 아니다.
 *
 * 함수 정의가 아직 없으므로 아래 main에 호출을 추가하면 정의도 필요하다.
 * 구현 순서 권장: 준비/정리 → hash → put/get → remove → contains/keys/size.
 * 충돌 처리에 익숙해진 뒤 원문의 로드 팩터 조건과 확장 동작을 확인한다.
 */
