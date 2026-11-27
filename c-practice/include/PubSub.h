#ifndef PUBSUB_H
#define PUBSUB_H

#include "HashMap.h"
#include "LinkedList.h"

/*
 * 보너스 5 Pub/Sub. CLI 하나(REPL 자신)가 유일한 구독자다. 네트워크·서버는 만들지 않는다.
 * 구독한 채널로 발행된 메시지는 inbox 큐 뒤에 쌓이고, 읽을 때 앞에서부터 꺼낸다.
 */

typedef struct Message {
    char* channel; // 복사해서 소유한다.
    char* text;    // 복사해서 소유한다.
} Message;

typedef struct PubSub {
    HashMap channels; // 구독한 채널 이름 → 1. 값은 쓰지 않고 키만 집합처럼 쓴다. 채널 이름을 소유한다.
    LinkedList inbox; // 받은 메시지 큐. HEAD = 먼저 온 메시지. Node와 Message를 소유한다.
} PubSub;

/*
 * 반환 규칙은 Store와 같다: 0 이상은 Redis 응답 값, 음수(-1)는 할당 실패.
 * subscribe: 구독 중인 채널 수. 이미 구독한 채널이면 수가 그대로다.
 * publish: 메시지를 받은 구독자 수(구독한 채널이면 1, 아니면 0).
 * read: 가장 먼저 온 메시지를 큐에서 꺼낸다. 없으면 NULL. 꺼낸 Message와 문자열은 호출한 쪽이 해제한다.
 */
int pubsub_subscribe(PubSub* ps, const char* channel);
int pubsub_publish(PubSub* ps, const char* channel, const char* text);
Message* pubsub_read(PubSub* ps);

#endif
