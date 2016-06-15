## Expected Prints ##

```
Entry to the program

This is a clean boot
This is printed in fiq handler
System will be reboot in 5 seconds

Entry to the program

CPU1 was reset by AWDT
cleaning up reset status...reset status cleaned up
This is printed in fiq handler
System will be reboot in 5 seconds

```

The system is waited at `This is a clean boot` or `reset status cleaned up` until nFIQ is triggered.

Note: nFIQ from PL is active high.

Note: To demo this application, connect any signal to nFIQ of ZYNQ. VIO core would be a good one to be used because it's easy to control the signals via Vivado Hardware Manager.