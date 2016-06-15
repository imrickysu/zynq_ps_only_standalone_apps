## Application List and Simple Descriptions ##

### AWDT_reset_its_own_core_only ###
Setup AWDT. When AWDT times out, the core will be reset. Since the code is located at 0x0, the standalone application will reboot.

### AWDT_reset_its_own_core_by_FIQ ###
Setup AWDT in FIQ handler so that when FIQ signal arrives, the core will be reset and reboot.

### FIQ_simple_example ###
Setup FIQ handler to print a line.