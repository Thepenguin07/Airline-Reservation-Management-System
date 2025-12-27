# Airline Reservation System (C)

A console-based Airline Management System developed in C. This system allows users to book tickets, cancel reservations, and manage flight schedules using file-based persistence.

## 🚀 Features
* **Secure Login:** Password-masked login system.
* **Flight Booking:** Real-time seat availability checks and PNR generation.
* **Cancellation:** Automatic seat count updates upon cancellation.
* **Admin Reporting:** Generates a detailed `Admin.txt` report with revenue and occupancy stats.
* **File Persistence:** Data is stored in `.txt` files to simulate a database.



## 🛠️ Requirements
- A C compiler (GCC/MinGW)
- Windows OS (Required for `<conio.h>` and `system("cls")`)

## 🖥️ Installation & Running
1. Clone the repository:
  
Navigate to the src folder.

Compile the code:
[https://github.com/Thepenguin07/Airline-Reservation-Management-System/tree/main]
Bash
gcc main.c -o airline_system
Run the executable:

Bash
./airline_system
🔐 Login Credentials
Username: (Any)

Password: rawr1234

📊 File System Logic
The program interacts with the following files:

Flights.txt: Stores flight details and availability.

Reservation.txt: Stores passenger details linked to PNRs.

pnr.txt: Maintains the sequence for unique PNR generation.


---
Important Note for Testing:

Your program will not show any flights unless you create a file named Flights.txt in the same directory. Create it and add a line like this: AI101 Mumbai Delhi 2025-01-01 10:00 Economy 100 0 5000.00
