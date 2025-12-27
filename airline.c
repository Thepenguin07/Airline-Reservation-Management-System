#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <conio.h> 

#define TRUE 1

// Structure Definitions
typedef struct { 
    int PNR;
    char name[50];
    char gender[10]; 
    int age; 
    char flight[10]; 
} passenger;

typedef struct { 
    char flight[10]; 
    char origin[20]; 
    char destination[20]; 
    char date[12]; 
    char time[10]; 
    char seatClass[10]; 
    int totalSeats; 
    int bookedSeats; 
    float cost; 
} FlightSchedule; // Fixed: Added missing name

// Function Prototypes 
void getpassword(char[]); 
void bookFlight(); 
void cancelFlight(); 
int generatePNR(); 
void showAvailableFlights(); 
void modifyBooking(); 
void checkSeatAvailability(); 
void generateReport();
void trimNewline(char *str);

// Password Input with Masking 
void getpassword(char password[]) {
    char ch; 
    int i = 0; 
    while (TRUE) { 
        ch = getch(); 
        if (ch == 13) { // Enter key
            password[i] = '\0'; 
            break; 
        } else if (ch == 8) { // Backspace
            if (i > 0) { 
                printf("\b \b"); 
                i--; 
            } 
        } else if (i < 8) { 
            password[i++] = ch; 
            printf("*");
        } 
    } 
}

int generatePNR() { 
    int pnr = 1000; 
    FILE *fp = fopen("pnr.txt", "r"); 
    if (fp != NULL) { 
        fscanf(fp, "%d", &pnr); 
        fclose(fp); 
    } 
    fp = fopen("pnr.txt", "w"); 
    if (fp) {
        fprintf(fp, "%d", pnr + 1); 
        fclose(fp); 
    }
    return pnr; 
}

void showAvailableFlights() { 
    FILE *fp = fopen("Flights.txt", "r"); 
    if (!fp) { 
        printf("\nError: Flights.txt not found. Please create the database first.\n"); 
        return; 
    } 
    FlightSchedule f; 
    printf("\n%-10s %-10s %-10s %-12s %-8s %-10s %-10s %-8s\n", 
           "Flight", "From", "To", "Date", "Time", "Class", "Seats", "Cost");
    printf("--------------------------------------------------------------------------------\n");
    while (fscanf(fp, "%s %s %s %s %s %s %d %d %f", f.flight, f.origin, f.destination, f.date, f.time, f.seatClass, &f.totalSeats, &f.bookedSeats, &f.cost) == 9) { 
        printf("%-10s %-10s %-10s %-12s %-8s %-10s %-10d %-8.2f\n", 
               f.flight, f.origin, f.destination, f.date, f.time, f.seatClass, f.totalSeats - f.bookedSeats, f.cost); 
    } 
    fclose(fp); 
}

void bookFlight() { 
    showAvailableFlights(); 
    FlightSchedule f; 
    char flightNo[10]; 
    int found = 0;

    printf("\nEnter flight number to book: ");
    scanf("%s", flightNo);

    FILE *fpFlight = fopen("Flights.txt", "r");
    if (!fpFlight) return;

    while (fscanf(fpFlight, "%s %s %s %s %s %s %d %d %f",
                  f.flight, f.origin, f.destination, f.date, f.time,
                  f.seatClass, &f.totalSeats, &f.bookedSeats, &f.cost) == 9) {
        if (strcmp(f.flight, flightNo) == 0) {
            found = 1;
            if (f.bookedSeats >= f.totalSeats) {
                printf("No seats left.\n");
                fclose(fpFlight);
                return;
            }
            break;
        }
    }
    fclose(fpFlight);

    if (!found) {
        printf("Flight not found.\n");
        return;
    }

    int numPassengers;
    printf("How many passengers? ");
    scanf("%d", &numPassengers);
    if (f.bookedSeats + numPassengers > f.totalSeats) {
        printf("Not enough seats available.\n");
        return;
    }

    FILE *fp = fopen("Reservation.txt", "a");
    if (!fp) return;

    int pnr = generatePNR();
    passenger p;
    for (int i = 0; i < numPassengers; i++) {
        getchar(); // clear buffer
        printf("\nPassenger %d\n", i + 1);
        printf("Name: ");
        fgets(p.name, sizeof(p.name), stdin);
        p.name[strcspn(p.name, "\n")] = '\0';

        printf("Gender: ");
        scanf("%s", p.gender);

        printf("Age: ");
        scanf("%d", &p.age);

        strcpy(p.flight, flightNo);
        p.PNR = pnr;

        fprintf(fp, "%d,%s,%s,%d,%s\n", p.PNR, p.name, p.gender, p.age, p.flight);
    }
    fclose(fp);

    // Update Seat Counts
    fpFlight = fopen("Flights.txt", "r");
    FILE *temp = fopen("TempFlights.txt", "w");
    while (fscanf(fpFlight, "%s %s %s %s %s %s %d %d %f",
                  f.flight, f.origin, f.destination, f.date, f.time,
                  f.seatClass, &f.totalSeats, &f.bookedSeats, &f.cost) == 9) {
        if (strcmp(f.flight, flightNo) == 0) f.bookedSeats += numPassengers;
        fprintf(temp, "%s %s %s %s %s %s %d %d %.2f\n",
                f.flight, f.origin, f.destination, f.date, f.time,
                f.seatClass, f.totalSeats, f.bookedSeats, f.cost);
    }
    fclose(fpFlight);
    fclose(temp);
    remove("Flights.txt");
    rename("TempFlights.txt", "Flights.txt");

    printf("Booking done successfully. Your PNR: %d\n", pnr);
}

void cancelFlight() {
    int pnr;
    printf("Enter PNR to cancel: ");
    scanf("%d", &pnr);

    FILE *fp = fopen("Reservation.txt", "r");
    FILE *temp = fopen("Temp.txt", "w");
    if (!fp) {
        printf("No reservations found.\n");
        return;
    }

    passenger p;
    int found = 0, cancelCount = 0;
    char flightCode[10] = "";
    char line[200];

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d,%49[^,],%9[^,],%d,%9[^\n]", &p.PNR, p.name, p.gender, &p.age, p.flight) == 5) {
            if (p.PNR == pnr) {
                found = 1;
                cancelCount++;
                strcpy(flightCode, p.flight); 
            } else {
                fprintf(temp, "%d,%s,%s,%d,%s\n", p.PNR, p.name, p.gender, p.age, p.flight);
            }
        }
    }
    fclose(fp);
    fclose(temp);

    if (!found) {
        remove("Temp.txt");
        printf("PNR not found.\n");
        return;
    }

    remove("Reservation.txt");
    rename("Temp.txt", "Reservation.txt");

    // Update Flight Seats
    FILE *fpFlight = fopen("Flights.txt", "r");
    FILE *tempFlight = fopen("TempFlights.txt", "w");
    FlightSchedule f;

    while (fscanf(fpFlight, "%s %s %s %s %s %s %d %d %f",
                  f.flight, f.origin, f.destination, f.date, f.time,
                  f.seatClass, &f.totalSeats, &f.bookedSeats, &f.cost) == 9) {
        if (strcmp(f.flight, flightCode) == 0)
            f.bookedSeats -= cancelCount;
        fprintf(tempFlight, "%s %s %s %s %s %s %d %d %.2f\n",
                f.flight, f.origin, f.destination, f.date, f.time,
                f.seatClass, f.totalSeats, f.bookedSeats, f.cost);
    }
    fclose(fpFlight);
    fclose(tempFlight);
    remove("Flights.txt");
    rename("TempFlights.txt", "Flights.txt");

    printf("Booking cancelled. %d seats released.\n", cancelCount);
}

void modifyBooking() { 
    int pnr; 
    char flightnew[10]; 
    printf("Enter PNR to modify: "); 
    scanf("%d", &pnr);

    FILE *fp = fopen("Reservation.txt", "r");
    FILE *temp = fopen("Temp.txt", "w");
    if (!fp) return;

    passenger p;
    int found = 0;
    char line[200];

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%d,%49[^,],%9[^,],%d,%9[^,\n]", &p.PNR, p.name, p.gender, &p.age, p.flight);
        if (p.PNR == pnr) {
            found = 1;
            printf("Current flight for %s: %s\n", p.name, p.flight);
            printf("Enter new flight number: ");
            scanf("%s", flightnew);
            strcpy(p.flight, flightnew);
        }
        fprintf(temp, "%d,%s,%s,%d,%s\n", p.PNR, p.name, p.gender, p.age, p.flight);
    }
    fclose(fp);
    fclose(temp);
    remove("Reservation.txt");
    rename("Temp.txt", "Reservation.txt");

    if (found) printf("Booking updated.\n");
    else printf("PNR not found.\n");
}

void checkSeatAvailability() {
    char flightNo[10]; 
    int found = 0;
    FlightSchedule f; 
    printf("Enter flight number: "); 
    scanf("%s", flightNo); 
    FILE *fp = fopen("Flights.txt", "r"); 
    if(!fp) return;

    while (fscanf(fp, "%s %s %s %s %s %s %d %d %f", f.flight, f.origin, f.destination, f.date, f.time, f.seatClass, &f.totalSeats, &f.bookedSeats, &f.cost) == 9) {
        if (strcmp(f.flight, flightNo) == 0) {
            printf("\nFlight: %s | Seats Available: %d\n", f.flight, f.totalSeats - f.bookedSeats);
            found = 1; 
            break;
        } 
    } 
    fclose(fp); 
    if (!found) printf("Flight not found.\n"); 
}

void trimNewline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

void generateReport() {
    FILE *fpFlight = fopen("Flights.txt", "r");
    FILE *fpRes = fopen("Reservation.txt", "r");
    FILE *freport = fopen("Admin.txt", "w");
    if (!fpFlight || !freport) {
        printf("Error reading flight data.\n");
        return;
    }

    FlightSchedule f;
    passenger p;
    int totalPassengers = 0;
    float totalRevenue = 0.0;

    fprintf(freport, "--- Airline Admin Report ---\n");
    while (fscanf(fpFlight, "%s %s %s %s %s %s %d %d %f",
                  f.flight, f.origin, f.destination, f.date, f.time,
                  f.seatClass, &f.totalSeats, &f.bookedSeats, &f.cost) == 9) {

        int flightCount = 0;
        if (fpRes) {
            rewind(fpRes);
            char line[200];
            while (fgets(line, sizeof(line), fpRes)) {
                sscanf(line, "%d,%49[^,],%9[^,],%d,%9[^\n]", &p.PNR, p.name, p.gender, &p.age, p.flight);
                trimNewline(p.flight);
                if (strcmp(f.flight, p.flight) == 0) flightCount++;
            }
        }

        float revenue = flightCount * f.cost;
        totalPassengers += flightCount;
        totalRevenue += revenue;

        fprintf(freport, "\nFlight: %s | %s -> %s\n", f.flight, f.origin, f.destination);
        fprintf(freport, "Booked: %d/%d | Revenue: %.2f\n", flightCount, f.totalSeats, revenue);
    }
    fprintf(freport, "\nTOTALS -> Passengers: %d | Revenue: %.2f\n", totalPassengers, totalRevenue);
    fclose(fpFlight);
    if(fpRes) fclose(fpRes);
    fclose(freport);
    printf("Admin report generated to Admin.txt\n");
}

int main() { 
    char username[20], password[9]; 
    while (TRUE) {
        system("cls");
        printf("--- Login ---\n");
        printf("Username: ");
        scanf("%s", username); 
        printf("Password: "); 
        getpassword(password);

        if (strcmp(password, "rawr1234") == 0) {
            printf("\nLogin successful!\n");
            break;
        } else {
            printf("\nWrong password. Try again.\n");
            getch();
        }
    }

    int key;
    while (1) {
        printf("\n--- Airline Reservation System by Shifa Parveen---\n");
        printf("1. Book Flight\n2. Cancel Flight\n3. Modify Booking\n");
        printf("4. Check Seat Availability\n5. Generate Admin Report\n6. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &key) != 1) break;

        switch (key) {
            case 1: bookFlight(); break;
            case 2: cancelFlight(); break;
            case 3: modifyBooking(); break;
            case 4: checkSeatAvailability(); break;
            case 5: generateReport(); break;
            case 6: exit(0);
            default: printf("Invalid option.\n");
        }
    }
    return 0;
}
