# NBA Database C++  

A **C++ console-based database management system** that interacts with a **SQLite NBA relational database**. The application allows users to view and manage divisions, teams, player rosters, and game information through a menu-driven interface.  

It demonstrates **database integration, SQL queries, joins, and CRUD operations** using the SQLite C API.  

## Features
- View divisions, teams, and players  
- Add new teams, players, and game information  
- Menu-driven console interface with input validation  
- Integration with SQLite database  

## Technologies
- C++  
- SQLite  
- SQL  

## How to Run
1. Make sure you have a **C++ compiler** installed.  
2. Ensure the **`nba.db`** database file is in the same directory as `main.cpp`.  
3. Compile the program:  
   ```bash
   g++ main.cpp -o nba_app -lsqlite3

## Notes 
- The program uses a menu system for navigating database options.
- Some functions (like adding a team) may have limited validation and may not handle all edge cases.
