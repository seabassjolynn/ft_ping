Project should be compliled and run on debian 12

to compile project:
1 cd to project folder
2 run: sudo make run MAKE_ARG=google.com // <-------- project is supposed to be launched with sudo as a workaround, but real ping works without sudo, so project should be adjusted to run without sudo in future

google.com can be replaced with whatever ip v 4 address or host name

Possible outputs.

1 If host name exists (command to run: make run MAKE_ARG=google.com):
PING google.com (142.251.37.14): 56 data bytes // <--------------resolved address is seen here
Send ICPM packet successfuly. Number of bytes sent is: 56
Received ICMP reply. Id: 27496 sequence num: 5 // <--------------sequence num should incese from 0 with each pring statement. I set this sequences numbers, so if they have sequence 0, 1, 2, 3... then we likely to receive correct responses

2 if addr exists (command to run: make run MAKE_ARG=142.251.37.14):
PING 142.251.37.14 (142.251.37.1): 56 data bytes
Send ICPM packet successfuly. Number of bytes sent is: 56
Received ICMP reply. Id: 27942 sequence num: 2

3 if addr / hostname invalid:
getaddrinfo: Name or service not known

4 if add valid, but doesn't reply:
Failed to receive ICPM packetPING 2.2.2.2 (2.2.2.2): 56 data bytes
Send ICPM packet successfuly. Number of bytes sent is: 56
