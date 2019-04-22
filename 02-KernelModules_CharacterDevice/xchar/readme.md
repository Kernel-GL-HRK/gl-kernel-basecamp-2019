# Test scenario as a result of `test.sh` execution

1. Install module...
Ok.

2. Write "12345" to "/dev/xchar0"...
Ok.

3. Checking "/proc/xchar/used_buffer_volume"...
"6"
Ok.

4. Checking "/proc/xchar/buffer_size"...
"1024"
Ok.

5. Checking "/dev/xchar0"...
"12345"
Ok.

6. Write "abc" to "/dev/xchar0"...
Ok.

7. Checking "/proc/xchar/used_buffer_volume"...
"4"
Ok.

8. Write "1" to "/sys/class/xchar/clean_up_buffer"...
Ok.

9. Checking "/proc/xchar/used_buffer_volume"...
"0"
Ok.

10. Checking "/dev/xchar0"...
""
Ok.

11. Remove module...
Ok.

12. Install module with parameters...
Ok.

13. Checking "/proc/xchar/buffer_size"...
"512"
Ok.

14. Remove module...
Ok.
