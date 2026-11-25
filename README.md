# Mini Redis

해시맵·이중 연결 리스트·최소 힙을 직접 구현해 만든 CLI Mini Redis입니다. LRU 제거, TTL 만료, 메모리 제한, Pub/Sub을 지원합니다.

## 구조

```text
.
├── c-practice/   # C 선행 실습
├── python/       # Mini Redis 본체 (CLI)
├── scripts/      # 시연 스크립트
└── docs/
```

## 실행

Python 3.8 이상, 외부 의존성 없음. 레포 루트에서 실행합니다.

```bash
python3 python/main.py          # CLI
python3 scripts/demo.py         # 시연 (엔터로 한 단계씩, --auto: 끝까지 자동)
```

## 기능과 사용법

공백이 있는 값은 큰따옴표로 감쌉니다: `SET user:1 "Alice Kim"`. 명령 이름은 대소문자를 구분하지 않습니다.

### 키-값

| 명령 | 기능 | 출력 |
| --- | --- | --- |
| `SET key value` | 저장. 기존 키를 덮어쓰면 TTL 삭제 | `OK` |
| `GET key` | 조회. 성공 시 최근 사용으로 갱신 | `"value"` / `(nil)` |
| `DEL key` | 삭제 | `(integer) 1` / `(integer) 0` |
| `EXISTS key` | 존재 확인 | `(integer) 1` / `(integer) 0` |
| `DBSIZE` | 키 개수 | `(integer) N` |
| `KEYS` | 전체 키 목록 | `1. "key"` … / `(empty array)` |

### 메모리 제한

| 명령 | 기능 | 출력 |
| --- | --- | --- |
| `CONFIG SET maxmemory bytes` | 최대 메모리 설정(0 = 무제한). 초과하면 다음 SET에서 가장 오래 안 쓴 키부터 제거 | `OK` |
| `INFO memory` | 사용량·제한·제거된 키 수 | `used_memory:N` / `maxmemory:N` / `evicted_keys:N` |

### 만료 (TTL)

| 명령 | 기능 | 출력 |
| --- | --- | --- |
| `EXPIRE key seconds` | 만료 설정. 0 이하는 즉시 삭제 | `(integer) 1` / `(integer) 0`(없는 키) |
| `TTL key` | 남은 초 | `(integer) N` / `-1`(만료 없음) / `-2`(없는 키) |

### Pub/Sub

| 명령 | 기능 | 출력 |
| --- | --- | --- |
| `SUBSCRIBE channel [channel ...]` | 채널 구독 | `1) "subscribe"` / `2) "channel"` / `3) (integer) 구독 채널 수` |
| `PUBLISH channel message` | 발행. 구독한 채널이면 받은 메시지가 바로 뒤에 출력 | `(integer) 1` / `(integer) 0` |

### 종료와 에러

- `exit` / `quit`로 종료합니다.
- 없는 명령, 인자 개수 오류, 정수 파싱 실패, 메모리 초과(OOM)는 `(error) ...`로 출력합니다.
