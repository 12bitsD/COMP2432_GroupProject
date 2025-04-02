==========================================================
                     COMP2432 - Group Project
                  Smart Parking Management System
==========================================================

Group Number: G46

Group Members:
    1. LIN Hao
    2. LIANG Ruin
    3. YANG Xinyi
    4. ZHEN Zihao

----------------------------------------------------------
                     Project Description
----------------------------------------------------------
The Smart Parking Management System (SPMS) is a software solution designed to efficiently manage parking reservations,
event bookings, and related facility requests. The system supports automated conflict detection, comprehensive validation
of input data, and multiple scheduling algorithms including First-Come-First-Served (FCFS), Priority-Based Scheduling (PRIO),
and Optimized Scheduling (OPTI) to allocate parking spaces and essential facilities effectively.

----------------------------------------------------------
                    Directory and Files
----------------------------------------------------------

Your project directory should contain:

├── sourceCode (Executable)
├── main.c
├── inputModule.c
├── inputModule.h
├── scheduling.c
├── scheduling.h
├── booking.h
├── output.c
├── output.h
├── batch001.dat (Sample batch file)
└── demo_script.txt (Provided demonstration script)

----------------------------------------------------------
                  Compilation and Execution
----------------------------------------------------------

Compilation:
- This project uses CMake and is tested on CLion IDE.
- Alternatively, you can compile using gcc from terminal:

  gcc -o sourceCode main.c inputModule.c output.c scheduling.c -std=c99

Execution:
- Run the compiled executable from the terminal:

  ./sourceCode

----------------------------------------------------------
                  Example Commands for Testing
----------------------------------------------------------

Interactive Examples:

- Valid booking:
  addParking -member_A 2025-05-10 09:00 2.0 battery cable;

- Invalid booking (missing essentials):
  addParking -member_A 2025-05-14 12:00 2.0 battery;

- Scheduling commands:
  printBookings -fcfs;
  printBookings -prio;
  printBookings -opti;
  printBookings -ALL;

- Batch import command:
  addBatch -batch001.dat;

- Exit the system:
  endProgram;

----------------------------------------------------------
                  Notes and Recommendations
----------------------------------------------------------

- Ensure batch files (e.g., batch001.dat) are placed in the same directory as the executable or provide a full path.
- Verify paths and file names carefully, especially on non-Windows operating systems.
- Refer to the provided demonstration script (`demo_script.txt`) for step-by-step project demonstration and speech guidelines.

==========================================================
                    Thank You for Your Attention!
==========================================================