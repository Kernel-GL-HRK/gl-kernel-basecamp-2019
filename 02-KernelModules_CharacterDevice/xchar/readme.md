# Test scenario

1. Write `12345` to `/dev/xchar0`
    - Checking `/proc/xchar/used_buffer_volume`... `6` - Ok.
    - Checking `/proc/xchar/buffer_size`... `1024` - Ok.
    - Checking `/dev/xchar0`... `12345` - Ok.

2. Write `abc` to `/dev/xchar0`
    - Checking `/proc/xchar/used_buffer_volume`... `4` - Ok.

3. Write `1` to `/sys/class/xchar/clean_up_buffer`
    - Checking `/proc/xchar/used_buffer_volume`... `0` - Ok.
    - Checking `/dev/xchar0`... `` - Ok.
