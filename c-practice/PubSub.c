#include <stdlib.h>
#include <string.h>

#include "PubSub.h"

/* 채널을 구독 목록에 넣는다. 반환: 구독 중인 채널 수, 할당 실패 -1. */
int pubsub_subscribe(PubSub* ps, const char* channel) {
    // 이미 있는 키면 put이 값만 덮어쓰고 count를 늘리지 않으므로, 중복 구독이 자연스럽게 무시된다.
    if (!hashmap_put(&ps->channels, channel, 1)) {
        return -1;
    }
    return (int)hashmap_size(&ps->channels);
}

/* 구독한 채널이면 메시지를 복사해 inbox 뒤에 넣는다. 반환: 받은 구독자 수(1 또는 0), 할당 실패 -1. */
int pubsub_publish(PubSub* ps, const char* channel, const char* text) {
    if (!hashmap_contains(&ps->channels, channel)) {
        return 0; // 구독자가 없는 채널의 메시지는 버린다(Redis와 같음).
    }

    Message* msg = malloc(sizeof(Message));
    if (msg == NULL) {
        return -1;
    }
    // 호출한 쪽의 문자열은 곧 사라질 수 있으므로 복사해 둔다.
    msg->channel = malloc(strlen(channel) + 1);
    msg->text = malloc(strlen(text) + 1);
    Node* NewNode = malloc(sizeof(Node));
    if (msg->channel == NULL || msg->text == NULL || NewNode == NULL) {
        // free(NULL)은 아무것도 하지 않으므로 성공한 것만 골라 해제하지 않아도 된다.
        free(msg->channel);
        free(msg->text);
        free(NewNode);
        free(msg);
        return -1;
    }
    strcpy(msg->channel, channel);
    strcpy(msg->text, text);

    NewNode->data = msg;
    insert_back(&ps->inbox, NewNode); // 큐: 뒤에 넣고 앞에서 꺼낸다(먼저 온 메시지가 먼저).
    return 1;
}

/* 가장 먼저 온 메시지를 꺼낸다. 없으면 NULL. */
Message* pubsub_read(PubSub* ps) {
    if (ps->inbox.HEAD == NULL) {
        return NULL;
    }
    // remove_front가 Node를 해제하므로 Message 주소를 먼저 꺼내 둔다.
    Message* msg = ps->inbox.HEAD->data;
    remove_front(&ps->inbox);
    return msg;
}
