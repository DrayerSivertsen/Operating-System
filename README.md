# Operating System
This repo includes various OS level development projects working torward the design and implementation of a REAL embedded operating system.

## Project Descriptions
### Lab1
Development of a BOOTER to boot up an OS, called EOS, from a SD card. The target machine for booting is the ARM virtual machine under QEMU.
### Lab2
Development of LCD drivers for images and text. The lab touches on graphics drivers and how images and text are displayed to the screen.
### Lab3
Development of timer and keyboard drivers through interrupt handling. The timer driver generates a wall-clock to display time since the running began and flash the cursor periodically. 
The keyboard driver receives scan code interrupts and translates them to the appropriate char displaying the interrupt sequence. 
### Mid1
Development of process management and functionality including forking child processes, sleep, wakeup, exit, and wait.
### Mid2
Extension of hardware timer to include process sleeping for a interval of time. This is maintain by the Time Queue Elements (TQEs) where only the relative time is decremented allowing for a more efficient design.
### Mid3
Development of interprocess communication through semaphore locking to manage writing and reading buffers. Previously implemented drivers included to display to screan and handle I/O.
### Lab5
Additions to previous process functions to include umode (user mode) and kernel mode.

### Lab6
Expansion upon kernel fork command to support user mode, and addition of exec function to process different user mode images. This lab is working towards the os layer communication of user mode and kernel mode.

### Lab7
Buffer management support to include multiple buffers and buffer caching. This is to show demonstrate the principles of buffer management algorithms as well as its shortcomings. The project compares the performances of the Unix buffer management algorithm and the PV algorithm. 


## Overview of topics includes:
1. Operating Systems:
    Unix/Linux, MTX. computer system and operations, system development 
    software, PC emulators, link C and assembly programs.

2. Booting OS

3. ARM Archicture, ARM programming and embedded systems
     
4. Processes:
    Concept and implementation of processes; process states,
    context switching, process scheduling.  

5. Process management: 
      fork, wait, exit, exec, signals, pipes in Unix/Linux and Wanix. 
   Processes in (microKernel based) OS:
      Micro vs. Monnolithic Kernels, tasks, servers and user processes in Minix; 

6. Process Synchronization:  
    The process model; mutual exclusion and critical regions, 
    Implementation of low-level mutual exclusion primitives.
    Synchronization primitives; events, event queues, semaphores.

7. Process Communication:    
    High-level process synchronization constructs; messages.

8. Process Control:      
    Scheduling algorithms. Dead lock and starvation problems.

9. Memory Management:
    Memory management schemes  
    Paging and Virtual memory

10. I/O device drivers:         
    Interrupts and interrupt processing
    Interraction between interrupt handler and process.
    Design and implementation of I/O drivers;

11. File Systems: 
    Review of EXT2 file system.
    NFS and RFS based on UDP and TCP/IP

11. Real-time OS
