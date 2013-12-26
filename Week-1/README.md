This assignment shows the power of fork and exec.
Prog1 creates a new process and executes prog2, prog2 in turn creates and runs prog3.
Prog3 in turn executes prog1, making it a infinite loop.