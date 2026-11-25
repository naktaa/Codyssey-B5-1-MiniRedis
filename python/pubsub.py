"""보너스 5 PubSub.c 포팅: 채널 기반 메시징.

CLI 하나(REPL 자신)가 유일한 구독자다. 네트워크·서버는 만들지 않는다.
  channels: 구독한 채널 이름 → 1 (직접 만든 HashMap을 키 집합처럼 쓴다)
  inbox:    받은 메시지 큐 (직접 만든 LinkedList. insert_back으로 넣고 remove_front로 꺼낸다)
메시지는 Store와 따로 있어 used_memory·DBSIZE·KEYS에 들어가지 않는다.
C의 할당 실패(-1)와 free는 Python에 없어서 뺐다.
"""

from linked_list import Node, LinkedList
from hash_map import HashMap

INITIAL_CAPACITY = 16


class Message:
    """받은 메시지 하나. inbox Node의 data가 가리킨다."""

    def __init__(self, channel, text):
        self.channel = channel
        self.text = text


class PubSub:
    """구독한 채널 집합(HashMap)과 받은 메시지 큐(LinkedList). 구독자는 REPL 하나다."""

    def __init__(self):
        self.channels = HashMap(INITIAL_CAPACITY)
        self.inbox: LinkedList[Message] = LinkedList()  # HEAD = 먼저 온 메시지

    def subscribe(self, channel):
        """채널을 구독한다. 반환: 구독 중인 채널 수. 이미 구독한 채널이면 수가 그대로다."""
        # 이미 있는 키면 put이 값만 덮어쓰고 count를 늘리지 않으므로, 중복 구독이 자연스럽게 무시된다.
        self.channels.put(channel, 1)
        return self.channels.size()

    def publish(self, channel, text):
        """구독한 채널이면 메시지를 inbox 뒤에 넣는다. 반환: 받은 구독자 수(1 또는 0)."""
        if not self.channels.contains(channel):
            return 0  # 구독자가 없는 채널의 메시지는 버린다(Redis와 같음).

        self.inbox.insert_back(Node(Message(channel, text)))  # 큐: 먼저 온 메시지가 먼저 나간다.
        return 1

    def read(self):
        """가장 먼저 온 메시지를 꺼낸다. 없으면 None."""
        if self.inbox.HEAD is None:
            return None
        # remove_front는 data를 돌려주지 않으므로 Message를 먼저 꺼낸다(C와 같은 이유).
        msg = self.inbox.HEAD.data
        self.inbox.remove_front()
        return msg
