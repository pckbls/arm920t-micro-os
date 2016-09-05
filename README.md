This repository contains a very simple Operating System that I have written with a fellow student as part of our CE bachelor courses.
It is designed to run on a Taskit Portux 920T mini PC which is powered by an Atmel ARM920T.

During development we had to deal with the following problems/aspects:
* ARM specific processor modes and stacks
* exception handling
* device drivers for an UART, a timer and a couple of attached LEDs
* multi-threading and -processing with scheduling and context switches
* memory mapping via a dedicated MMU
* separation between kernel and user space
* implementation of syscalls

Code comments and prints are written in German language.

