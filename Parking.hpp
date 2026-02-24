#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <cmath>

// Constants
const double RATE_REGULAR = 5.0;
const double RATE_VIP = 10.0;
const double RATE_DISABILITYES = 0.0;
const int TOTAL_SPOTS = 20;
const double MIN_CHARGE = 20;

struct ParkingSession
{
    std::string m_plate;
    int m_spotNumber;
    int m_entryHour;
    int m_entryMin;
    std::string m_spotType;

};


struct Receipt
{
    std::string m_plate;
    int m_spotNumber;
    std::string m_entryTime;
    std::string m_exitTime;
    double m_duration; 
    double m_totalCost;


};

//DATA STORAGE
std::map<std::string,ParkingSession>activeSessions;
std::map<std::string, std::vector<Receipt>> history;
std::map <int,bool> spots; // true empty, false not empty
std::map<int,std::string> spotType;
/**
 * 0:15 --> Reg
 * 15:18 --> vip 
 * 18:20 --> dis
 */
std::map<int,int> hourlyTraffic;

double totalRevenue;

//helper functions
//"HH:MM"
std::string FormatTime(int hour, int min) 
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hour << " : " << std::setfill('0') << std::setw(2) << min;
    return ss.str();

}

double getRate(std::string spottype)
{
    if(spottype == "VIP") return RATE_VIP;
    if(spottype == "DISABILITYES") return RATE_DISABILITYES;
    return RATE_REGULAR;

}

void initSpots()
{
    // i = 1 --> i <= 15 - REGULAR SPOTS defualt state is true
    // i = 16 --> i <= 18 - vip SPOTS defualt state is true
    // i = 19 --> i <= 20 - dis SPOTS defualt state is true

    for(int i = 1 ; i <= 15 ; i++)
    {
        spots[i] = true;
        spotType[i] = "REGULAR";

    }


      for(int i = 16 ; i <= 18 ; i++)
    {
        spots[i] = true;
        spotType[i] = "VIP";

    }

      for(int i = 19 ; i <= 20 ; i++)
    {
        spots[i] = true;
        spotType[i] = "DISABILITYES";

    }
}



bool enterParking(std::string plate, int  hour, int min, 
                 std::string preferreftype = "REGULAR")
                {
                    //bool functionFlg = true;
                   
                    // Make sure that car isn't found
                    if(activeSessions.count(plate))
                    {
                        std::cout<<"[WARN]" << plate << "is already parked" << std::endl;
                        return false;
                    }

                    //search for prefferd type

                    int foundSpot = -1;
                    for(auto &[spotNum, available] : spots)
                    {
                        if(available && spotType[spotNum] == preferreftype)
                        {
                            foundSpot = spotNum;
                            break;
                        }
                    }



                    //if not found search in any place but not disapales
                    if(foundSpot == -1)
                    {
                        for(auto &[spotNum, available] : spots)
                    {
                        if(available && spotType[spotNum] != "DISABILITYES")
                        {
                            foundSpot = spotNum;
                            break;
                        }
                    }
                    }




                    // else return false full
                    if(foundSpot == -1) 
                    {
                        std::cout << "FULL" <<std::endl;
                        return false;
                    }


                    //fill data (parkingsession struct )
                    spots[foundSpot] = false;
                    ParkingSession session;

                    session.m_plate = plate;
                    session.m_spotNumber = foundSpot;
                    session.m_entryHour = hour;
                    session.m_entryMin = min;
                    session.m_spotType = spotType[foundSpot];

                    activeSessions[plate] = session;
                    hourlyTraffic[hour]++;


                    std::cout<< "[IN]" << plate << " -> spot " << foundSpot
                    << " ( " << spotType[foundSpot] << " )"
                    << "at  " << FormatTime(hour,min) << std::endl;

                    return true;

}

bool exitParking(std::string plate, int exithour, int exitmin)
{
    //founded
   
    if(!activeSessions.count(plate))
    {
       std::cout<< "[ ERR ]" << plate << "not found" << std::endl;
    }
    


    //session info
    ParkingSession & session = activeSessions[plate];



    // calculate duration in minutes
    int entryTotal  = session.m_entryHour * 60 + session.m_entryMin;
    int exitTotal   = exithour * 60 + exitmin;
    int durationMin = exitTotal - entryTotal;

    // handle overnight parking
    if (durationMin < 0) durationMin += 24 * 60;

    double durationHours = durationMin / 60.0;

    // calculate cost
    double rate = getRate(session.m_spotType);
    double cost = durationHours * rate;

    // apply minimum charge (except for disabled spots)
    if (session.m_spotType != "DISABILITYES")
        cost = std::max(cost, MIN_CHARGE);

    // round to nearest 0.5 EGP
    cost = round(cost * 2) / 2.0;


    //history 
    Receipt receipt;
    receipt.m_plate = plate;
    receipt.m_spotNumber = session.m_spotNumber;
    receipt.m_entryTime = FormatTime(session.m_entryHour,session.m_entryMin);
    receipt.m_exitTime = FormatTime(exithour,exitmin);
    receipt.m_duration = durationHours;
    receipt.m_totalCost = cost;

    //fill 
    history[plate].push_back(receipt);
    totalRevenue += cost;
    spots[session.m_spotNumber] = true;
    activeSessions.erase(plate);


    std::cout << "\n[RECEIPT] " << plate << "\n"
    << std::string(35, '-') << "\n"
    << "  Spot     : " << receipt.m_spotNumber
                    << " (" << session.m_spotType << ")\n"
    << "  Entry    : " << receipt.m_entryTime  << "\n"
    << "  Exit     : " << receipt.m_exitTime   << "\n"
    << "  Duration : " << std::fixed << std::setprecision(1)
                    << durationHours << " hrs\n"
    << "  Rate     : " << rate << " EGP/hr\n"
    << "  Total    : " << cost << " EGP\n"
    << std::string(35, '-') << "\n";



    return true;


}

void showAvailableSpots()
{
    std::cout << "\n===== Available Spots =====\n";

    bool anyFree = false;

    for (const auto& [spotNum, available] : spots)
    {
        if (available)
        {
            anyFree = true;
            std::cout << "Spot " << spotNum
                      << " [ " << spotType[spotNum] << " ]"
                      << " -> AVAILABLE\n";
        }
    }

    if (!anyFree)
        std::cout << "No available spots.\n";

    std::cout << "===========================\n";
}

void findCar(const std::string plate)
{
    std::cout << "\n===== RESULT =====\n";

    auto activeIt = activeSessions.find(plate);

    if (activeIt != activeSessions.end())
    {
        ParkingSession& session = activeIt->second;

        std::cout << "[ACTIVE]\n";
        std::cout << "Plate      : " << session.m_plate << "\n";
        std::cout << "Spot       : " << session.m_spotNumber
                  << " (" << session.m_spotType << ")\n";
        std::cout << "Entry Time : "
                  << FormatTime(session.m_entryHour, session.m_entryMin)
                  << "\n";
        return;
    }

    auto historyIt = history.find(plate);

    if (historyIt != history.end())
    {
        std::cout << "[HISTORY]\n";

        for (const auto& receipt : historyIt->second)
        {
            std::cout << "Spot: " << receipt.m_spotNumber
                      << " | Entry: " << receipt.m_entryTime
                      << " | Exit: " << receipt.m_exitTime
                      << " | Duration: " << receipt.m_duration
                      << " hrs\n";
        }
        return;
    }

    std::cout << "Car not found.\n";
}


void showCurrentCars()
{
    std::cout << "\n===== CURRENTLY PARKED CARS =====\n";

    if (activeSessions.empty())
    {
        std::cout << "No cars currently parked.\n";
        return;
    }

    for (const auto& [plate, session] : activeSessions)
    {
        std::cout << "Plate      : " << plate << "\n";
        std::cout << "Spot       : " << session.m_spotNumber
                  << " (" << session.m_spotType << ")\n";
        std::cout << "Entry Time : "
                  << FormatTime(session.m_entryHour,
                                session.m_entryMin) << "\n";
        std::cout << "----------------------------------\n";
    }
}


void showCarHistory(const std::string plate)
{
    if (!history.count(plate))
    {
        std::cout << "No history found for plate: "
                  << plate << "\n";
        return;
    }

    std::cout << "\nHistory for plate: " << plate << "\n";

    for (const auto& receipt : history[plate])
    {
        std::cout << "Spot: " << receipt.m_spotNumber
                  << " | Entry: " << receipt.m_entryTime
                  << " | Exit: " << receipt.m_exitTime
                  << " | Cost: " << receipt.m_totalCost
                  << " EGP\n";
    }
}


void showDailyReport()
{


}
    

void Parking_main()
{
    initSpots();
    enterParking("ABC123",1,5,"REGULAR");
    enterParking("ABC124",00,6,"REGULAR");
    enterParking("ABC125",1,4,"REGULAR");
    enterParking("ABC126",00,8,"REGULAR");
    exitParking("ABC123",2,5);

    findCar("ABC123");
    showCarHistory("ABC123");
    showCurrentCars();

}