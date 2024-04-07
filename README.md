**Note:** Project is developed under debian 12, so it must be run under this OS
## To Compile Project:

1. `cd` to project folder
2. Run: `sudo make run MAKE_ARG=google.com`

**Note:** `google.com` can be replaced with whatever IPv4 address or hostname.

### Possible Outputs:

1. **If hostname exists** (command to run: `make run MAKE_ARG=google.com`):

    ```
    PING google.com (142.251.37.14): 56 data bytes
    Send ICMP packet successfully. Number of bytes sent is: 56
    Received ICMP reply. Id: 27496 sequence num: 5
    ```

    *Resolved address is seen here. Sequence number should increase from 0 with each ping statement. If you see sequence numbers like 0, 1, 2, 3..., then we are likely to receive correct responses.*

2. **If address exists** (command to run: `make run MAKE_ARG=142.251.37.14`):

    ```
    PING 142.251.37.14 (142.251.37.14): 56 data bytes
    Send ICMP packet successfully. Number of bytes sent is: 56
    Received ICMP reply. Id: 27942 sequence num: 2
    ```

3. **If address/hostname is invalid**:

    ```
    getaddrinfo: Name or service not known
    ```

4. **If address is valid, but doesn't reply**:

    ```
    Failed to receive ICMP packet
    PING 2.2.2.2 (2.2.2.2): 56 data bytes
    Send ICMP packet successfully. Number of bytes sent is: 56

## Program functionality:
1. cntrl + c key combination stops ping execution and displays
ping statistics.
**currently only statistics header is displayed, and statistics will be**
**implemented further.**