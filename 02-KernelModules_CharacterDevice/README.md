# Lesson 5-7 (Kernel Modules, Character Devices)

## The deadline is April 12 for all Parts

# Task - Implement a character device driver for text messaging between users.

#Part 1. Implement a character device driver with the following requirements:
- 1kB buffer size;
- buffer increase as a module parameter ;
- should be available for all users;
- works only ASCII strings.

#Part 2.  Add the interfaces into the driver:
- sysfs for buffer clean up;
- procfs for displaying used buffer volume and buffer size

#Part 3. Implement shell/bash script for the driver testing. 

#Part 4. Provide a test scenario.

Implemented character device should be committed to GitHub. During implementation, please follow to code and git style. 
