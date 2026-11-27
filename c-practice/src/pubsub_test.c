#include <stdio.h>
#include <stdlib.h>

#include "PubSub.h"

/* 메시지를 하나 읽어 "채널: 내용"으로 출력하고 해제한다. 비었으면 (없음). */
static void read_one(PubSub* ps, const char* expected) {
    Message* msg = pubsub_read(ps);
    if (msg == NULL) {
        printf("read -> (없음) (기대: %s)\n", expected);
        return;
    }
    printf("read -> %s: %s (기대: %s)\n", msg->channel, msg->text, expected);
    free(msg->channel);
    free(msg->text);
    free(msg);
}

/* 실습 확인용 main. 생성 함수 대신 여기서 직접 준비한다. 각 줄의 (기대: ...)와 출력이 같은지 확인한다. */
int main(void) {
    PubSub ps;
    ps.channels.capacity = 4; // 작게 시작해서 확장 후에도 구독이 유지되는지 확인
    ps.channels.count = 0;
    ps.channels.buckets = calloc(ps.channels.capacity, sizeof(LinkedList));
    if (ps.channels.buckets == NULL) {
        return 1;
    }
    ps.inbox.HEAD = ps.inbox.TAIL = NULL;

    printf("== 구독 전 발행 ==\n");
    printf("publish news hello -> %d (기대: 0)\n", pubsub_publish(&ps, "news", "hello"));
    read_one(&ps, "(없음)");

    printf("\n== 구독 ==\n");
    printf("subscribe news   -> %d (기대: 1)\n", pubsub_subscribe(&ps, "news"));
    printf("subscribe news   -> %d (기대: 1, 중복 구독은 수가 그대로)\n", pubsub_subscribe(&ps, "news"));
    printf("subscribe sports -> %d (기대: 2)\n", pubsub_subscribe(&ps, "sports"));

    printf("\n== 발행과 읽기 순서 ==\n");
    printf("publish news hello   -> %d (기대: 1)\n", pubsub_publish(&ps, "news", "hello"));
    printf("publish weather sun  -> %d (기대: 0, 구독 안 한 채널)\n", pubsub_publish(&ps, "weather", "sun"));
    printf("publish sports goal  -> %d (기대: 1)\n", pubsub_publish(&ps, "sports", "goal"));
    printf("publish news bye     -> %d (기대: 1)\n", pubsub_publish(&ps, "news", "bye"));
    read_one(&ps, "news: hello");
    read_one(&ps, "sports: goal");
    read_one(&ps, "news: bye");
    read_one(&ps, "(없음)");

    printf("\n== 채널 해시맵 확장 후 ==\n");
    pubsub_subscribe(&ps, "a");
    printf("subscribe b      -> %d (기대: 4, 4 > 4 * 0.75라 버킷 4 → 8)\n", pubsub_subscribe(&ps, "b"));
    printf("capacity         -> %zu (기대: 8)\n", ps.channels.capacity);
    printf("publish news hi  -> %d (기대: 1, 확장 후에도 구독 유지)\n", pubsub_publish(&ps, "news", "hi"));
    printf("publish b x      -> %d (기대: 1)\n", pubsub_publish(&ps, "b", "x"));

    // 정리: 읽지 않은 메시지(news: hi, b: x)를 다 꺼내 해제하고, 채널 해시맵을 해제한다.
    printf("\n== 정리 ==\n");
    read_one(&ps, "news: hi");
    read_one(&ps, "b: x");
    hashmap_free(&ps.channels);

    return 0;
}
