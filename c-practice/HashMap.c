#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedList.h"

#define CAP 100

/*
 * C-2 학습용 선언 템플릿: 문자열 key와 정수 value를 저장한다.
 * 구조체와 함수 원형만 제공하며, 모든 함수 본문은 직접 구현한다.
 * 기존 Linked_list.c의 전역 HEAD/TAIL 대신 버킷마다 체인을 관리한다.
 */

typedef struct Data {
    char* key; // 문자열 주소. 복사·할당·해제 책임은 구현 전에 정한다.
    int value;
} Data;

typedef struct HashMap {
    // 각 버킷이 첫 Entry의 주소를 저장하므로 배열의 원소 타입은 Entry*이다.
    // 그 배열을 동적으로 관리할 시작 주소이므로 Entry**로 표현한다.
    LinkedList* buckets;
    size_t capacity; // 버킷 개수. 항목 개수와 다르며, 사용 전 0보다 커야 한다.
    size_t count;    // 저장된 서로 다른 키의 개수.
} HashMap;


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

    return sum % CAP;
};

/*
 * 키가 있으면 값을 갱신하고, 없으면 새 항목을 저장한다.
 * 반환: 성공 1, 할당 등 저장 실패 0. 기존 키 갱신 시 count는 늘지 않는다.
 * TODO: 키 문자열의 보관 방식, 충돌 처리, 새 항목 연결을 구현하기.
 * TODO: 로드 팩터 0.75 초과 시 버킷 2배 확장과 항목 재배치를 구현하기.
 */
int hashmap_put(HashMap* map, const char* key, int value) {
    size_t idx = hashmap_hash(key);
    LinkedList* list = &map->buckets[idx];
    Node* now = list->HEAD;

    while (now) {
        Data* data = now->data;
        if (strcmp(data->key, key)) {
            now = now->next;
            continue;
        }
        else {
            data->value = value;
            return 1;
        }
    }

    Data* NewData = malloc(sizeof(Data));
    if (NewData == NULL) {
        return 0;
    }

    NewData->key = malloc(strlen(key) + 1);
    if (NewData->key == NULL) {
        return 0;
    }

    strcpy(NewData->key, key);
    NewData->value = value;

    Node* NewNode = malloc(sizeof(Node));
    NewNode->data = NewData;
    NewNode->prev = now;
    NewNode->next = NULL;
    insert_back(list, NewNode);
    return 1;
};

/*
 * 반환: 키를 찾으면 1, 없으면 0. 찾은 정수 값은 *out_value에 전달한다.
 * 값 0도 저장할 수 있으므로 값 자체와 찾음 여부를 구분한다.
 * out_value는 결과를 쓸 수 있는 유효한 int 변수의 주소여야 한다.
 * TODO: 같은 버킷의 후보들을 실제 키 내용으로 구별하기.
 * 참고: 문자열의 ==는 주소 비교이고, strcmp는 문자열 내용 비교다.
 */
int hashmap_get(const HashMap* map, const char* key, int* out_value);

/*
 * 반환: 삭제했으면 1, 해당 키가 없으면 0.
 * TODO: 체인 연결, 버킷의 시작 주소, count, 메모리 수명을 함께 관리하기.
 * stdio.h의 remove와 충돌하지 않도록 hashmap_remove라는 이름을 쓴다.
 */
int hashmap_remove(HashMap* map, const char* key);

/*
 * 반환: 키가 있으면 1, 없으면 0.
 * TODO: 조회와 중복되는 부분을 어떻게 사용할지 생각하기.
 */
int hashmap_contains(const HashMap* map, const char* key);

/*
 * C 실습에서는 저장된 모든 키를 출력하는 형태로 시작한다. 순서는 무관하다.
 * TODO: 전체 버킷과 각 체인에서 키를 빠뜨리지 않고 확인하기.
 */
void hashmap_keys(const HashMap* map);

/* 반환: 현재 저장된 서로 다른 키의 개수. TODO: 직접 구현하기. */
size_t hashmap_size(const HashMap* map);

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

int main() {
    //
}
