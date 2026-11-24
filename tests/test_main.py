import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "python"))

from main import tokenize, execute
from store import Store


class TestTokenize(unittest.TestCase):
    def test_plain_and_quoted(self):
        self.assertEqual(tokenize("SET user:1 Alice"), ["SET", "user:1", "Alice"])
        self.assertEqual(tokenize('SET user:1 "Alice"'), ["SET", "user:1", "Alice"])
        self.assertEqual(tokenize('SET k "a b  c"'), ["SET", "k", "a b  c"])
        self.assertEqual(tokenize('SET k ""'), ["SET", "k", ""])
        self.assertEqual(tokenize("   "), [])
        self.assertIsNone(tokenize('SET k "abc'))


class TestExecute(unittest.TestCase):
    def setUp(self):
        self.store = Store()

    def run_cmd(self, line, now=0):
        return execute(self.store, tokenize(line), now)

    def test_mission_example(self):
        # 미션 8절 실행 예시. 시각은 직접 넣는다.
        self.assertEqual(self.run_cmd("CONFIG SET maxmemory 30"), "OK")
        self.assertEqual(self.run_cmd('SET user:1 "Alice"'), "OK")
        self.assertEqual(self.run_cmd('SET user:2 "Bob"'), "OK")
        self.assertEqual(self.run_cmd('SET user:3 "Charlie"'), "OK")
        self.assertEqual(self.run_cmd("GET user:1"), "(nil)")
        self.assertEqual(self.run_cmd("INFO memory"), "used_memory:22\nmaxmemory:30\nevicted_keys:1")
        keys = sorted(line.split(". ")[1] for line in self.run_cmd("KEYS").split("\n"))
        self.assertEqual(keys, ['"user:2"', '"user:3"'])
        self.assertEqual(self.run_cmd("EXPIRE user:2 3", 0), "(integer) 1")
        self.assertEqual(self.run_cmd("TTL user:2", 500), "(integer) 2")
        self.assertEqual(self.run_cmd("GET user:2", 3000), "(nil)")
        self.assertEqual(self.run_cmd("TTL user:2", 3000), "(integer) -2")

    def test_keys_format(self):
        self.assertEqual(self.run_cmd("KEYS"), "(empty array)")
        self.run_cmd("SET a 1")
        self.run_cmd("SET b 2")
        lines = self.run_cmd("KEYS").split("\n")
        self.assertEqual([line.split(". ")[0] for line in lines], ["1", "2"])
        self.assertEqual(sorted(line.split(". ")[1] for line in lines), ['"a"', '"b"'])

    def test_string_commands(self):
        self.assertEqual(self.run_cmd("SET a 1"), "OK")
        self.assertEqual(self.run_cmd("GET a"), '"1"')
        self.assertEqual(self.run_cmd("get a"), '"1"')  # 명령 대소문자 무시
        self.assertEqual(self.run_cmd("EXISTS a"), "(integer) 1")
        self.assertEqual(self.run_cmd("EXISTS none"), "(integer) 0")
        self.assertEqual(self.run_cmd("DBSIZE"), "(integer) 1")
        self.assertEqual(self.run_cmd("DEL a"), "(integer) 1")
        self.assertEqual(self.run_cmd("DEL a"), "(integer) 0")
        self.assertEqual(self.run_cmd("TTL a"), "(integer) -2")
        self.assertEqual(self.run_cmd("EXPIRE a 10"), "(integer) 0")

    def test_errors(self):
        self.assertEqual(self.run_cmd("CONFIG SET maxmemory abc"),
                         "(error) ERR value is not an integer or out of range")
        self.assertEqual(self.run_cmd("CONFIG SET maxmemory -1"),
                         "(error) ERR value is not an integer or out of range")
        self.assertEqual(self.run_cmd("EXPIRE a 1.5"),
                         "(error) ERR value is not an integer or out of range")
        self.assertEqual(self.run_cmd("GET"), "(error) ERR wrong number of arguments for 'GET' command")
        self.assertEqual(self.run_cmd("SET a"), "(error) ERR wrong number of arguments for 'SET' command")
        self.assertEqual(self.run_cmd("HELLO"), "(error) ERR unknown command 'HELLO'")
        self.run_cmd("CONFIG SET maxmemory 5")
        self.assertEqual(self.run_cmd("SET key value"),
                         "(error) OOM command not allowed when used_memory > 'maxmemory'")


if __name__ == "__main__":
    unittest.main()
