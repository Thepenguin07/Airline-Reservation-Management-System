#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <conio.h> 
#define True 1
//Structure Definitions
typedef struct { 
int PNR;
char name[50];
char gender[10]; 
int age; 
char flight[10]; 
}passenger;
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
}
FlightSchedule;

// Function Prototypes 
void getpassword(char[]); 
void bookFlight(); 
void cancelFlight(); 
int generatePNR(); 
void showAvailableFlights(); 
void modifyBooking(); 
void checkSeatAvailability(); 
void generateReport();

// Password Input with Masking 
void getpassword(char password[]) {
 char ch; 
 int i = 0; 
 while (True) { 
 ch = getch(); 
 if (ch == 13) { 
 password[i] = '\0'; 
 break; 
 } if (ch == 8) { 
 if (i > 0) { 
 printf("\b \b"); 
 i--; } }
  else if (i < 8) 
  { password[i++] = ch; 
  printf("*");
   } } }

int generatePNR() { 
int pnr = 1000; 
FILE *fp = fopen("pnr.txt", "r"); 
if (fp != NULL) { 
fscanf(fp, "%d", &pnr); 
fclose(fp); } 
fp = fopen("pnr.txt", "w"); 
fprintf(fp, "%d", pnr + 1); 
fclose(fp); return pnr; }

void showAvailableFlights() { 
FILE *fp = fopen("Flights.txt", "r"); 
if (!fp) { printf("Flights file not found.\n"); 
return; } 
FlightSchedule f; 
printf("\nFlight\tFrom\tTo\tDate\t\tTime\tClass\tSeats Left\tCost\n"); 
while (fscanf(fp, "%s %s %s %s %s %s %d %d %f", f.flight, f.origin, f.destination, f.date, f.time, f.seatClass, &f.totalSeats, &f.bookedSeats, &f.cost) == 9) { printf("%s\t%s\t%s\t%s\t%s\t%s\t%d\t\t%.2f\n", f.flight, f.origin, f.destination, f.date, f.time, f.seatClass, f.totalSeats - f.bookedSeats, f.cost); } fclose(fp); }

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
int i;
for (i = 0; i < numPassengers; i++) {
    getchar();
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

printf("Booking done. Your PNR: %d\n", pnr);

}void cancelFlight() {
    int pnr;
    printf("Enter PNR: ");
    scanf("%d", &pnr);

    FILE *fp = fopen("Reservation.txt", "r");
    FILE *temp = fopen("Temp.txt", "w");
    if (!fp || !temp) {
        printf("Error accessing reservation files.\n");
        return;
    }

    passenger p;
    int found = 0, cancelCount = 0;
    char flightCode[10] = "";
    char line[200];

    // First: Remove passengers with matching PNR and store flight code
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%d,%49[^,],%9[^,],%d,%9[^\n]", &p.PNR, p.name, p.gender, &p.age, p.flight);

        if (p.PNR == pnr) {
            found = 1;
            cancelCount++;
            strcpy(flightCode, p.flight);  // Save flight code to update seats
        } else {
            fprintf(temp, "%d,%s,%s,%d,%s\n", p.PNR, p.name, p.gender, p.age, p.flight);
        }
    }

    fclose(fp);
    fclose(temp);

    if (!found) {
        remove("Temp.txt");
        printf("PNR not found.\n");
        return;
    }

    // Update reservation file
    remove("Reservation.txt");
    rename("Temp.txt", "Reservation.txt");

    // Second: Update booked seats in Flights.txt
    FILE *fpFlight = fopen("Flights.txt", "r");
    FILE *tempFlight = fopen("TempFlights.txt", "w");
    FlightSchedule f;

    while (fscanf(fpFlight, "%s %s %s %s %s %s %d %d %f",
                  f.flight, f.origin, f.destination, f.date, f.time,
                  f.seatClass, &f.totalSeats, &f.bookedSeats, &f.cost) == 9) {

        if (strcmp(f.flight, flightCode) == 0 && f.bookedSeats >= cancelCount)
            f.bookedSeats -= cancelCount;

        fprintf(tempFlight, "%s %s %s %s %s %s %d %d %.2f\n",
                f.flight, f.origin, f.destination, f.date, f.time,
                f.seatClass, f.totalSeats, f.bookedSeats, f.cost);
    }

    fclose(fpFlight);
    fclose(tempFlight);

    remove("Flights.txt");
    rename("TempFlights.txt", "Flights.txt");

    printf("Booking with PNR %d cancelled. %d passenger(s) removed.\n", pnr, cancelCount);
}


void modifyBooking() { 
int pnr; 
char flightnew[10]; 
printf("Enter PNR to modify: "); 
scanf("%d", &pnr);

FILE *fp = fopen("Reservation.txt", "r");
FILE *temp = fopen("Temp.txt", "w");
if (!fp || !temp) {
    printf("Error accessing files.\n");
    return;
}

passenger p;
int found = 0;
char line[200];

while (fgets(line, sizeof(line), fp)) {
    sscanf(line, "%d,%49[^,],%9[^,],%d,%9[^,\n]", &p.PNR, p.name, p.gender, &p.age, p.flight);
    if (p.PNR == pnr) {
        found = 1;
        printf("Current flight of %s: %s\n", p.name, p.flight);
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

if (found)
    printf("Booking updated.\n");
else
    printf("PNR not found.\n");

}

void checkSeatAvailability() {
 char flightNo[10]; 
 int found = 0;
FlightSchedule f; 
printf("Enter flight number: "); 
scanf("%s", flightNo); 
FILE *fp = fopen("Flights.txt", "r"); 
while (fscanf(fp, "%s %s %s %s %s %s %d %d %f", f.flight, f.origin, f.destination, f.date, f.time, f.seatClass, &f.totalSeats, &f.bookedSeats, &f.cost) == 9) {
 if (strcmp(f.flight, flightNo) == 0) {
  printf("\nFlight: %s\nFrom: %s\nTo: %s\nDate: %s\nTime: %s\nClass: %s\nCost: %.2f\nSeats Available: %d\n", f.flight, f.origin, f.destination, f.date, f.time, f.seatClass, f.cost, f.totalSeats - f.bookedSeats);
   found = 1; 
   break;
} } 
fclose(fp); 
if (!found) 
printf("Flight not found.\n"); 
}
void trimNewline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}
void generateReport() {
    FILE *fpFlight = fopen("Flights.txt", "r");
    FILE *fpRes = fopen("Reservation.txt", "r");
    FILE *freport=fopen("Admin.txt","w");
    if (!fpFlight || !fpRes || !freport) {
        printf("Error reading files.\n");
        return;
    }
    FlightSchedule f;
    passenger p;
    int totalPassengers = 0;
    float totalRevenue = 0.0;
    fprintf(freport,"\n--- Admin Report ---\n");
    while (fscanf(fpFlight, "%s %s %s %s %s %s %d %d %f",
                  f.flight, f.origin, f.destination, f.date, f.time,
                  f.seatClass, &f.totalSeats, &f.bookedSeats, &f.cost) == 9) {

        rewind(fpRes);  // reset file to beginning
        int flightPassengerCount = 0;
        char line[200];

        while (fgets(line, sizeof(line), fpRes)) {
            sscanf(line, "%d,%49[^,],%9[^,],%d,%9[^\n]",
                   &p.PNR, p.name, p.gender, &p.age, p.flight);
            trimNewline(p.flight);

            if (strcmp(f.flight, p.flight) == 0)
                flightPassengerCount++;
        }

        float revenue = flightPassengerCount * f.cost;
        totalPassengers += flightPassengerCount;
        totalRevenue += revenue;

        fprintf(freport,"\nFlight: %s | From: %s | To: %s\n", f.flight, f.origin, f.destination);
        fprintf(freport,"Date: %s | Time: %s | Class: %s\n", f.date, f.time, f.seatClass);
        fprintf(freport,"Booked: %d / %d | Occupancy: %.2f%%\n", flightPassengerCount, f.totalSeats,
               f.totalSeats?(100.0 * flightPassengerCount / f.totalSeats): 0);
        fprintf(freport,"Revenue: %.2f\n", revenue);
    }
    fprintf(freport,"\nTotal Passengers: %d\nTotal Revenue: %.2f\n", totalPassengers, totalRevenue);
    printf("Admin report is generated to admin.txt");
    fclose(fpFlight);
    fclose(fpRes);
    fclose(freport);
}
int main() { 
while (True) {
char username[20], password[9]; 
printf("Username: ");
scanf("%s", username); 
printf("Password: "); 
getpassword(password);

if (strcmp(password, "rawr1234") == 0) {
        printf("\nLogin successful!\n");
        break;
    } else {
        printf("\nWrong password.\n\n");
        getchar();
        system("cls");
    }
}

int key;
while (1) {
    printf("\n--- Airline Reservation System by Shifa Parveen ---\n");
    printf("1. Book Flight\n");
    printf("2. Cancel Flight\n");
    printf("3. Modify Booking\n");
    printf("4. Check Seat Availability\n");
    printf("5. Generate Admin Report\n");
    printf("6. Exit\n");
    printf("Enter choice: ");
    scanf("%d", &key);

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
