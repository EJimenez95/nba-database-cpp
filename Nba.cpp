#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include "sqlite3.h"
using namespace std;

void printMainMenu(); //prototype function for printing main menu
void viewDivTeam(sqlite3 *); // prototype for viewing a team in a division
void addTeam(sqlite3 *); // prototype for adding a team to a division this function does not correctly work but it still runs
void viewTeamRoster(sqlite3 *); //prototype for viewing a team roster
void addPlayer(sqlite3 *); //prototype for adding a player to a team
void viewGameInfo(sqlite3 *); //prototype for viewing game info
void addGameInfo(sqlite3 *); // prototype for adding game info
int mainMenu(); //prototype main menu function checks for input validation


int main()
{
	int choice;
	
	sqlite3 *mydb; //points to a database

	int rc;  //return code
	
	rc = sqlite3_open_v2("nba.db", &mydb, SQLITE_OPEN_READWRITE, "win32");
	//Need to add code to open the database.
	if (rc) //Checks if database opened or not
	{
		cout << "Error opening nba database: " << sqlite3_errmsg(mydb) << endl; //returns errmsg if nba does not open properly
		return 0;
	}
	else
	{
		cout << "nba database opened successfully" << endl; //if nba database opens successfully
	}
	
	cout << "Welcome to the NBA" << endl; //welcome message in the database
	choice = mainMenu(); //prints out a main menu
	while (true) //loop repeats until not true where -1 is entered
	{
		switch (choice) 
		{
			case 1: viewDivTeam(mydb); break;
			case 2: addTeam(mydb); break;
			case 3: viewTeamRoster(mydb); break;
			case 4: addPlayer(mydb); break;
			case 5: viewGameInfo(mydb); break;
			case 6: addGameInfo(mydb); break;
			case -1: return 0;
			default: cout << "That is not a valid choice." << endl; //error msg if user inputs an invalid value
		}
		cout << "\n\n"; //new line characters to space out from main menu
		choice = mainMenu();
	}
	sqlite3_close(mydb); //command to close the database
}

//prints main menu options where you get choices to decide upon
void printMainMenu() 
{
	cout << "Please choose an option (enter -1 to quit):  " << endl;
	cout << "1. View a team in a division" << endl;
	cout << "2. Add a team to a division" << endl;
	cout << "3. View a team roster" << endl;
	cout << "4. Add a player to team roster" << endl;
	cout << "5. View game information" << endl;
	cout << "6. Add game information" << endl;
	cout << "Enter Choice: ";
}
//function that checks for input validation for main menu 
int mainMenu()
{
	int choice = 0;
	
	printMainMenu(); //prints main menu
	cin >> choice; //gets choice from user
	while ((!cin || choice < 1 || choice > 5) && choice  != -1) //while loop for input validation checking if user inputs correct options
	{		
		if (!cin) //checks if user doesnt provide correct input and resets the input stream
		{
			cin.clear(); //clears errors
			cin.ignore(1000,'/n');
		}
		cout << "That is not a valid choice." << endl << endl; //statement for invalid choice
		printMainMenu(); //re-prints main menu
		cin >> choice; //gets choice again from user
	} 
	return choice;
}

//function for viewing a team in a selected division
void viewDivTeam(sqlite3 *db)
{
	string div_query = "SELECT div_id, div_name FROM Division"; //gets the id and name from the division table
	sqlite3_stmt *pRes;
	string div_Id, div_Name; //attributes from Division table
	string sqlError; //error message

	if(sqlite3_prepare_v2(db, div_query.c_str(), -1, &pRes, NULL) != SQLITE_OK) //calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
	{
		sqlError = sqlite3_errmsg(db); //sqlite error message command
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		cout << "There was an error: " << sqlError << endl; //prints error msg
		return;
	}
	else
	{
		cout << "Select a division to view: " << endl;
		int i = 1, choice; //gets choice for division that will be viewed from the menu
		cout << left;
		while (sqlite3_step(pRes) == SQLITE_ROW)
		{
			cout << i << ".) " << sqlite3_column_text(pRes, 0) << "\t" << sqlite3_column_text(pRes, 1) << endl; //shows division id along with div_Name
			i++;
		}
		do
		{
			if (!cin) //do while checks for input failure
			{
				cin.clear(); //resets the input stream
				cin.ignore(1000, '/n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > i) //checks if its a choice within the menu
				cout << "That is not a valid choice! Try Again!" << endl; //prints statement if invalid choice
		} while (!cin);
		sqlite3_reset(pRes); //resets result set to run through it again
		for (int i = 0; i < choice; i++)
		{
			sqlite3_step(pRes); //steps to the choices user makes
		}
		div_Id = reinterpret_cast<const char*>(sqlite3_column_text(pRes,0)); //sets div id
		div_Name = reinterpret_cast<const char*>(sqlite3_column_text(pRes,1)); //sets div name
		sqlite3_finalize(pRes); //frees memory used by prepared statement
	    //selects the team based off division that is chosen
		string team_query = "SELECT team_id, team_city, team_name, team_arena, team_state, team_coach, team_mascot FROM Team WHERE div_id = " + div_Id;
		
		sqlite3_stmt *pRes2;
	
		if (sqlite3_prepare_v2(db, team_query.c_str(), -1, &pRes2, NULL) != SQLITE_OK) //prepares sql statement
		{
			sqlError = sqlite3_errmsg(db); //error with database
			sqlite3_finalize(pRes2); //frees memory used by prepared statement
			cout << "There was an unexpected error: " << sqlError << endl; //prints error msg
			return;
		}
		else
		{
			cout << "Teams in the " + div_Name + " division:" << endl; //displays team or teams in the selected division
			while(sqlite3_step(pRes2) == SQLITE_ROW) //steps through to display all the columns for the team table along with the values based off selection
			{
				cout << "ID: " << sqlite3_column_text(pRes2, 0) << "\nCity: " << sqlite3_column_text(pRes2, 1) << "\nName: " << sqlite3_column_text(pRes2, 2)
					<< "\nArena: " << sqlite3_column_text(pRes2, 3) << "\nState: " << sqlite3_column_text(pRes2, 4) << "\nCoach: " << sqlite3_column_text(pRes2, 5)
					<< "\nMascot: " << sqlite3_column_text(pRes2, 6) << endl << endl;
			}
			sqlite3_finalize(pRes2); //frees memory used by prepared statement
		}
	}
}
//function for adding a team
void addTeam(sqlite3 *db)
{
	string div_query = "SELECT div_id, div_name FROM Division"; //query for gaining access to the team the user wants to add a player to
	sqlite3_stmt *pRes; //holds a prepared statement
	string sqlError; //error msg
	string div_id, div_name, team_id, team_name, team_city, team_arena, team_state, team_coach, team_mascot; //attributes from player and team table

	if(sqlite3_prepare_v2(db, div_query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
	{
		sqlError = sqlite3_errmsg(db); //sqlite error message command
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		cout << "There was an error: " << sqlError << endl; //error msg displayed
		return;
	}
	else
	{
	    int count = 1, choice;
		cout << left;
		while (sqlite3_step(pRes) == SQLITE_ROW) //Advance an sqlite3_stmt to the next result row or to completion.
		{
			cout << count << ".) " << sqlite3_column_text(pRes, 0) << "\t" << sqlite3_column_text(pRes, 1) << endl; //displays list of teams for user to select
			count++;
		}
		cout << "Select the division the team will play for: "; 
		do //do while checks for input failure 
		{
			if (!cin)
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > count) //checks if choice is less than or greater than the count
				cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
		} while (!cin);
		sqlite3_reset(pRes);
		for (int i = 0; i < choice; i++)
		{
			sqlite3_step(pRes);
		}
		div_id = reinterpret_cast<const char*>(sqlite3_column_text(pRes,0));
		sqlite3_finalize(pRes);
		
		string teamid_query = "SELECT team_id FROM Team;"; //sql statement 
		sqlite3_stmt *pRes2;
	
		if (sqlite3_prepare_v2(db, teamid_query.c_str(), -1, &pRes2, NULL) != SQLITE_OK)
		{
			sqlite3_finalize(pRes2);
			cout << "There was an error: " << sqlError << endl; //error msg displayed
			return;
		}
		else
		{
			count = 1;
			while(sqlite3_step(pRes2) == SQLITE_ROW)
			{
				count++;
			}
			for (int i = 0; i < count - 1; i++)
			{
				sqlite3_step(pRes2);
			}
			team_id = to_string(sqlite3_column_int(pRes2,0) + 1); //adds a new player id into the team after prior ids
			cout << "New team ID: " << team_id << endl;
			sqlite3_finalize(pRes2); //frees memory used by prepared statement
			//user input for adding a new team
				cout << "Enter the team city: ";
			do //do while checks for input failure
			{
				if (!cin)
				{
					cin.clear(); //resets the input stream 
					cin.ignore(1000, '\n');
				}
				cin >> team_city;
				if (!cin)
					cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
			} while (!cin);
			
				cout << "Enter the team name: ";
			do //do while checks for input failure
			{
				if (!cin)
				{
					cin.clear(); //resets the input stream 
					cin.ignore(1000, '\n');
				}
				cin >> team_name;
				if (!cin)
					cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
			} while (!cin);
			
				cout << "Enter the team arena: ";
			do //do while checks for input failure
			{
				if (!cin)
				{
					cin.clear(); //resets the input stream 
					cin.ignore(1000, '\n');
				}
				cin >> team_arena;
				if (!cin)
					cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
			} while (!cin);
			
				cout << "Enter the team state: ";
			do //do while checks for input failure
			{
				if (!cin)
				{
					cin.clear(); //resets the input stream 
					cin.ignore(1000, '\n');
				}
				cin >> team_state;
				if (!cin)
					cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
			} while (!cin);
			
				cout << "Enter the team coach: ";
			do //do while checks for input failure
			{
				if (!cin)
				{
					cin.clear(); //resets the input stream 
					cin.ignore(1000, '\n');
				}
				cin >> team_coach;
				if (!cin)
					cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
			} while (!cin);
			
				cout << "Enter the team mascot: ";
			do //do while checks for input failure
			{
				if (!cin)
				{
					cin.clear(); //resets the input stream 
					cin.ignore(1000, '\n');
				}
				cin >> team_mascot;
				if (!cin)
					cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
			} while (!cin);
			//inserts into team user input options for team
			string insert_team = "INSERT INTO team(team_id, div_id, team_city, team_name, team_arena, team_team_state, team_coach, team_mascot) ";
			insert_team += "VALUES ('" + team_id + "', '" + div_id + "', '" + team_city + "', '" + team_name + "', '" + team_arena + "', '"; 
			insert_team += team_state + "', '" + team_coach + "', '" + team_mascot + "');";
			sqlite3_stmt *pRes3;
			
			if (sqlite3_prepare_v2(db, insert_team.c_str(), -1, &pRes3, NULL) != SQLITE_OK) 
			{
				sqlite3_finalize(pRes3); //frees memory used by prepared statement
				cout << "There was an error: " << sqlite3_errmsg(db) << endl; //error message displayed
				return;
			}
			else
			{
				if(sqlite3_step(pRes3))
				{
					cout << "New team successfully added in." << endl; //team gets added into system
					sqlite3_finalize(pRes3); //frees memory used by prepared statement
				}
			}
		}
	}
}

//function for viewing the teams players Note: only the Raptors and Rockets have players
void viewTeamRoster(sqlite3 *db)
{
	string team_query = "SELECT team_id, team_name FROM Team"; //gets the team id and name from the team table
	sqlite3_stmt *pRes; //command holds a prepared statement
	string sqlError; //error message
	string team_Id, team_name; //team attributes for sql select statement
	
	if(sqlite3_prepare_v2(db, team_query.c_str(), -1, &pRes, NULL) != SQLITE_OK) //calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
	{
		sqlError = sqlite3_errmsg(db); //sqlite error message command
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		cout << "There was an error: " << sqlError << endl; //error msg
		return;
	}
	else
	{
	    int count = 1, choice;
		cout << left;
		while (sqlite3_step(pRes) == SQLITE_ROW) //Advance an sqlite3_stmt to the next result row or to completion.
		{
			cout << count << ".) " << sqlite3_column_text(pRes, 0) << "\t" << sqlite3_column_text(pRes, 1) << endl;
			count++;
		}
		cout << "Select a team to view its roster: ";
		do //do while checks for input failure
		{
			if (!cin)
			{
				cin.clear(); //resets the input stream 
				cin.ignore(1000, '/n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > count) //checks if its a choice within the menu
				cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
		} while (!cin);
		sqlite3_reset(pRes); //reset a prepared statement object back to its initial state
		for (int i = 0; i < choice; i++)
		{
			sqlite3_step(pRes);
		}
		team_Id = reinterpret_cast<const char*>(sqlite3_column_text(pRes,0)); //sets the team id
		team_name = reinterpret_cast<const char*>(sqlite3_column_text(pRes,1)); //sets the team name
		sqlite3_finalize(pRes);
	     //statement to select the roster to be viewed
		string roster_query = "SELECT play_id, play_fname, play_lname, play_number FROM Player WHERE team_id = " + team_Id;
		sqlite3_stmt *pRes2;
		//checks for errors in the roster query
		if(sqlite3_prepare_v2(db, roster_query.c_str(), -1, &pRes2, NULL) != SQLITE_OK)
		{
			sqlite3_finalize(pRes2); //frees memory used by prepared statement
			cout << "There was an error: " << sqlError << endl; //prints error msg
			return;
		}
		else //displays the team roster that was selected 
		{
			cout << "Team roster for the " + team_name + ":" << endl;
			while(sqlite3_step(pRes2) == SQLITE_ROW) //displays the team roster
			{
				cout << "ID: " << sqlite3_column_text(pRes2, 0) << "\nName: " << sqlite3_column_text(pRes2, 1) << " " << sqlite3_column_text(pRes2, 2) 
					<< "\nPlayer Number: " << sqlite3_column_text(pRes2, 3) << endl << endl;
			}
			sqlite3_finalize(pRes2); //frees memory used by prepared statement
		}
	}
}

//function for adding a player to the team roster
void addPlayer(sqlite3 *db)
{
	string team_query = "SELECT team_id, team_name FROM Team"; //query for gaining access to the team the user wants to add a player to
	sqlite3_stmt *pRes; //holds a prepared statement
	string sqlError; //error msg
	string team_Id, play_id, play_fname, play_lname; //attributes from player and team table
	int play_number;

	if(sqlite3_prepare_v2(db, team_query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
	{
		sqlError = sqlite3_errmsg(db); //sqlite error message command
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		cout << "There was an error: " << sqlError << endl; //error msg displayed
		return;
	}
	else
	{
	    int count = 1, choice;
		cout << left;
		while (sqlite3_step(pRes) == SQLITE_ROW) //Advance an sqlite3_stmt to the next result row or to completion.
		{
			cout << count << ".) " << sqlite3_column_text(pRes, 0) << "\t" << sqlite3_column_text(pRes, 1) << endl; //displays list of teams for user to select
			count++;
		}
		cout << "Select the team that the player will play for: "; 
		do //do while checks for input failure 
		{
			if (!cin)
			{
				cin.clear();
				cin.ignore(1000, '/n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > count) //checks if choice is less than or greater than the count
				cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
		} while (!cin);
		sqlite3_reset(pRes); //reset a prepared statement object back to its initial state
		for (int i = 0; i < choice; i++)
		{
			sqlite3_step(pRes);
		}
		team_Id = reinterpret_cast<const char*>(sqlite3_column_text(pRes,0));
		sqlite3_finalize(pRes);
		
		string playid_query = "SELECT play_id FROM Player;"; //sql statement to select play id
		sqlite3_stmt *pRes2;
	
		if (sqlite3_prepare_v2(db, playid_query.c_str(), -1, &pRes2, NULL) != SQLITE_OK)
		{
			sqlite3_finalize(pRes2);
			cout << "There was an error: " << sqlError << endl; //error msg displayed
			return;
		}
		else
		{
			count = 1;
			while(sqlite3_step(pRes2) == SQLITE_ROW)
			{
				count++;
			}
			for (int i = 0; i < count - 1; i++)
			{
				sqlite3_step(pRes2); //Advance an sqlite3_stmt to the next result row or to completion.
			}
			play_id = to_string(sqlite3_column_int(pRes2,0) + 1); //adds a new player id into the team after prior ids
			cout << "New player ID: " << play_id << endl;
			sqlite3_finalize(pRes2); //frees memory used by prepared statement
			//user input for inserting a player on the team roster
			cout << "Enter the first name of the player: ";
			cin >> play_fname;
			cout << "Enter the last name of the player: ";
			cin >> play_lname;
			cout << "Enter the player number: ";
			cin >> play_number;
			do //do while checks for input failure
			{
				if (!cin)
				{
					cin.clear(); //resets the input stream 
					cin.ignore(1000, '/n');
				}
				if (!cin)
					cout << "That is not a valid choice! Try Again!" << endl;
			} while (!cin);
			//inserts into player user input options for player
			string insert_play = "INSERT INTO Player(play_id, team_id, play_fname, play_lname, play_number) ";
			insert_play += "VALUES('" + play_id + "', '" + team_Id + "', '" + play_fname + "', '" + play_lname + "', '" + to_string(play_number) + "');";
			sqlite3_stmt *pRes3;
			
			if (sqlite3_prepare_v2(db, insert_play.c_str(), -1, &pRes3, NULL) != SQLITE_OK) 
			{
				sqlite3_finalize(pRes3); //frees memory used by prepared statement
				cout << "There was an error: " << sqlError << endl; //error message displayed
				return;
			}
			else
			{
				if(sqlite3_step(pRes3))
				{
					cout << "New player successfully added in." << endl; //player gets added into system
					sqlite3_finalize(pRes3); //frees memory used by prepared statement
				}
			}
		}
	}
}
//function for viewing a game and its final score
void viewGameInfo(sqlite3 *db)
{
	string game_query = "Select game_id FROM Game";
	sqlite3_stmt *pRes, *homeRes, *visitRes; //command holds a prepared statement
	string sqlError; //error message
	string home_query, visit_query;
	string home_team, visit_team, game_id; 
	
	if(sqlite3_prepare_v2(db, game_query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
	{
		sqlError = sqlite3_errmsg(db); //sets sqlError equal to sqlite errmsg command
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		cout << "There was an error: " << sqlError << endl; //error msg displayed
		return;
	}
	else
	{
	    int count = 1, choice;
		cout << left;
		while (sqlite3_step(pRes) == SQLITE_ROW) //Advance an sqlite3_stmt to the next result row or to completion.
		{
			cout << count << ".) " << sqlite3_column_text(pRes, 0) << "\t";
			game_id = reinterpret_cast<const char*>(sqlite3_column_text(pRes,0));
			//sets team id = to home_team_id/visitor_team_id
			home_query = "SELECT team_name FROM Game Join Team on Team.team_id = Game.game_home_team_id WHERE game_id = " + game_id;
			visit_query = "SELECT team_name FROM Game Join Team on Team.team_id = Game.game_visitor_team_id WHERE game_id = " + game_id;
			if(sqlite3_prepare_v2(db, home_query.c_str(), -1, &homeRes, NULL) != SQLITE_OK)
			{
				sqlite3_finalize(homeRes);
				cout << "There was an error: " << sqlError << endl; //error msg displayed
				return;
			}
			else
			{
				if(sqlite3_step(homeRes) == SQLITE_ROW)
				{
					home_team = reinterpret_cast<const char*>(sqlite3_column_text(homeRes,0));
					sqlite3_finalize(homeRes); //frees memory used by prepared statement
				}
			}
			if(sqlite3_prepare_v2(db, visit_query.c_str(), -1, &visitRes, NULL) != SQLITE_OK)
			{
				sqlite3_finalize(visitRes); //frees memory used by prepared statement
				cout << "There was an error: " << sqlError << endl; //error msg displayed
				return;
			}
			else
			{
				if(sqlite3_step(visitRes) == SQLITE_ROW)
				{
					visit_team = reinterpret_cast<const char*>(sqlite3_column_text(visitRes,0));
					sqlite3_finalize(visitRes); //frees memory used by prepared statement
				}
			}
			cout << "Home: " << home_team << " vs " << " Visitors: " << visit_team << " " << endl; //displays options of games to view
			count++;
		}
		cout << "Select a game to view its info: ";
		do //do while checks for input failure
		{
			if (!cin)
			{
				cin.clear(); //resets the input stream 
				cin.ignore(1000, '/n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > count)
				cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
		} while (!cin);
		sqlite3_reset(pRes); //reset a prepared statement object back to its initial state
		for (int i = 0; i < choice; i++)
		{
			sqlite3_step(pRes);
		}
		game_id = reinterpret_cast<const char*>(sqlite3_column_text(pRes,0)); //sets the game id
		
		home_query = "SELECT team_name FROM Game Join Team on Team.team_id = Game.game_home_team_id WHERE game_id = " + game_id;
		visit_query = "SELECT team_name FROM Game Join Team on Team.team_id = Game.game_visitor_team_id WHERE game_id = " + game_id;
		
		if(sqlite3_prepare_v2(db, home_query.c_str(), -1, &homeRes, NULL) != SQLITE_OK)
		{
			sqlite3_finalize(homeRes); //frees memory used by prepared statement
			cout << "There was an error: " << sqlError << endl; //error msg displayed
			return;
		}
		else
		{
			if(sqlite3_step(homeRes) == SQLITE_ROW) //Advance an sqlite3_stmt to the next result row or to completion.
			{
				home_team = reinterpret_cast<const char*>(sqlite3_column_text(homeRes,0));
				sqlite3_finalize(homeRes); //frees memory used by prepared statement
			}
		}
		if(sqlite3_prepare_v2(db, visit_query.c_str(), -1, &visitRes, NULL) != SQLITE_OK)//calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
		{
			sqlite3_finalize(visitRes); //frees memory used by prepared statement
			cout << "There was an error: " << sqlError << endl; //error msg displayed
			return;
		}
		else
		{
			if(sqlite3_step(visitRes) == SQLITE_ROW)
			{
				visit_team = reinterpret_cast<const char*>(sqlite3_column_text(visitRes,0));
				sqlite3_finalize(visitRes); //frees memory used by prepared statement
			}
		}
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		
		sqlite3_stmt *pRes2;
		string game_query = "SELECT game_date, game_time, game_finalscore FROM Game WHERE game_id = " + game_id; //statement for viewing a game based off id selection
		
		if(sqlite3_prepare_v2(db, game_query.c_str(), -1, &pRes2, NULL) != SQLITE_OK)//calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
		{
			sqlite3_finalize(pRes2); //frees memory used by prepared statement
			cout << "There was an error: " << sqlError << endl; //error msg displayed
			return;
		}
		else
		{
			//displays the id, teams facing off, date, final score of home vs visitor team
			if(sqlite3_step(pRes2) == SQLITE_ROW)
			{
				cout << "\nGame ID: " << game_id << "\n" << "Home: " << home_team << " vs " << "Visitors: " << visit_team << "\nDate: " 
				<< sqlite3_column_text(pRes2,0) << "\nTime: "<< sqlite3_column_text(pRes2,1) << " \nFinal Score: " <<
					"Home " << sqlite3_column_text(pRes2,2) << " Visitors" << endl;
				sqlite3_finalize(pRes2); //frees memory used by prepared statement
			}
		}
			
	}
}
//function for adding in game information
void addGameInfo(sqlite3 *db)
{
	string team_query = "SELECT team_id, team_name FROM Team";
	sqlite3_stmt *pRes; //command holds a prepared statement
	string sqlError; //error message
	string game_Id, homeTeam_Id, visitTeam_Id, game_date, game_time, game_finalscore;
	
	
	if(sqlite3_prepare_v2(db, team_query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
	{
		sqlError = sqlite3_errmsg(db); //sets sqlError equal to sqlite errmsg command
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		cout << "There was an error: " << sqlError << endl; //error msg displayed
		return;
	}
	else
	{
		int choice, count = 1; 
		cout << left;
		
		while (sqlite3_step(pRes) == SQLITE_ROW) //Advance an sqlite3_stmt to the next result row or to completion.
		{
			cout << count << ".) " << sqlite3_column_text(pRes, 0) << "\t" << sqlite3_column_text(pRes, 1) << endl;
			count++;
		}
		cout << "Choose the home team: ";
		do //do while checks for input failure
		{
			if (!cin)
			{
				cin.clear(); //resets the input stream 
				cin.ignore(1000, '/n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > count)
				cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
		} while (!cin);
		sqlite3_reset(pRes); //reset a prepared statement object back to its initial state
		for (int i = 0; i < choice; i++)
		{
			sqlite3_step(pRes); //Advance an sqlite3_stmt to the next result row or to completion.
		}
		homeTeam_Id = reinterpret_cast<const char*>(sqlite3_column_text(pRes,0));
		cout << "Choose the visiting team: ";
		do //do while checks for input failure
		{
			if (!cin)
			{
				cin.clear(); //resets the input stream 
				cin.ignore(1000, '/n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > count)
				cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
		} while (!cin);
		sqlite3_reset(pRes); //reset a prepared statement object back to its initial state
		for (int i = 0; i < choice; i++)
		{
			sqlite3_step(pRes);
		}
		visitTeam_Id = reinterpret_cast<const char*>(sqlite3_column_text(pRes,0));
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		
		string game_query = "SELECT game_id FROM Game;";
		sqlite3_stmt *pRes2; 
	
		if (sqlite3_prepare_v2(db, game_query.c_str(), -1, &pRes2, NULL) != SQLITE_OK)//calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
		{
			sqlite3_finalize(pRes2);
			cout << "There was an error: " << sqlError << endl; //error msg displayed
			return;
		}
		else
		{
			count = 1;
			while(sqlite3_step(pRes2) == SQLITE_ROW)
			{
				count++;
			}
			for (int i = 0; i < count - 1; i++)
			{
				sqlite3_step(pRes2);
			}
			game_Id = to_string(sqlite3_column_int(pRes2,0) + 1);
			cout << "New game ID: " << game_Id << endl;
			sqlite3_finalize(pRes2); //frees memory used by prepared statement
			
			cout << "Enter the date for the game: ";
			do //do while checks for input failure
			{
				if (!cin)
				{
					cin.clear(); //resets the input stream 
					cin.ignore(1000, '\n');
				}
				cin >> game_date;
				if (!cin)
					cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
			} while (!cin);
			
			cout << "Enter the start time for the game (am or pm): ";
			do //do while checks for input failure
			{
				if (!cin)
				{
					cin.clear();
					cin.ignore(1000, '\n');
				}
				cin >> game_time;
				if (!cin)
					cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
			} while (!cin);
			
			cout << "Enter the final score for the game: ";
			do //do while checks for input failure
			{
				if (!cin)
				{
					cin.clear(); //resets the input stream 
					cin.ignore(1000, '\n');
				}
				cin >> game_finalscore;
				if (!cin)
					cout << "That is not a valid choice! Try Again!" << endl; //error msg if wrong user input
			} while (!cin);
		    //sql command for inserting a new game with user input values inserted
			string insert_game = "INSERT INTO Game(game_id, game_home_team_id, game_visitor_team_id, game_date, game_time, game_finalscore) ";
			insert_game += "VALUES('" + game_Id + "', '" + homeTeam_Id + "', '" + visitTeam_Id + "', '" + game_date + "', '" + game_time + "', '" + game_finalscore + "');";
			sqlite3_stmt *pRes3;
			
			if (sqlite3_prepare_v2(db, insert_game.c_str(), -1, &pRes3, NULL) != SQLITE_OK)
			{
				sqlite3_finalize(pRes3); //frees memory used by prepared statement
				cout << "There was an error: " << sqlError << endl; //error msg displayed
				return;
			}
			else
			{
				
				if(sqlite3_step(pRes3))
				{
					cout << "Game has been successfully added." << endl;
					sqlite3_finalize(pRes3); //frees memory used by prepared statement
				}
			}
		}
	}
}