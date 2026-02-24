# Modern C++ Parking Management System

A **console-based parking management system** implemented in **Modern C++**, utilizing **STL containers** (`map`, `vector`) to manage parking sessions, track active and historical cars, and calculate billing.

---

## Features

- Track **active parking sessions** with car plate, spot number, entry time, and spot type.  
- **Exit cars** and calculate total cost based on spot type and duration.  
- Apply **minimum charge** for regular and VIP spots.  
- Maintain **history of parked cars** with receipts.  
- Show **currently parked cars** and **available spots**.  
- Search for a car in **active sessions** and **history**.  
- Supports multiple spot types:  
  - **Regular (15 spots)**  
  - **VIP (3 spots)**  
  - **Disability (2 spots)**  
- Collect **hourly traffic data** and **total revenue**.  

---

## Technologies & Concepts

- **Modern C++** (C++11/14/17)  
- **STL Containers**: `map`, `vector`  
- **Structs** for data modeling (`ParkingSession`, `Receipt`)  
- **Algorithms**: `std::max`, `std::round`  
- **Console-based user interface**  

---

## Usage Example

```cpp
int main() {
    initSpots();

    enterParking("ABC123", 1, 5, "REGULAR");
    enterParking("ABC124", 0, 6, "REGULAR");

    exitParking("ABC123", 2, 15);

    findCar("ABC123");
    showCarHistory("ABC123");
    showCurrentCars();
    showAvailableSpots();

    return 0;
}
```

## Sample Output

```text
[IN] ABC123 -> spot 1 ( REGULAR ) at 01 : 05
[IN] ABC124 -> spot 2 ( REGULAR ) at 00 : 06

[RECEIPT] ABC123
-----------------------------------
  Spot     : 1 ( REGULAR )
  Entry    : 01 : 05
  Exit     : 02 : 15
  Duration : 1.2 hrs
  Rate     : 5 EGP/hr
  Total    : 20 EGP
-----------------------------------

[ACTIVE]
Plate      : ABC124
Spot       : 2 ( REGULAR )
Entry Time : 00 : 06

Available Spots:
Spot 3 [ REGULAR ] -> AVAILABLE
...
```
## Project Structure

- **ParkingSession** → stores active car details  
- **Receipt** → stores exit history and cost  
- **activeSessions** → `map<plate, ParkingSession>`  
- **history** → `map<plate, vector<Receipt>>`  

**Functions include:**

- `enterParking()`  
- `exitParking()`  
- `showCurrentCars()`  
- `showAvailableSpots()`  
- `showCarHistory()`  
- `findCar()`  

---

## Future Improvements

- Implement **daily revenue and traffic reports**  
- Add **user input menu** for interactive parking management  
- Convert the system into a **class-based structure** for better encapsulation  
- Add **file persistence** to save parking data across program runs  
