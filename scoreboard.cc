/**
 * @file scoreboard.cc
 * @date 24.8.2018
 * @author Kentril Despair
 * @version 1.0
 * @brief Denfinitions of scoreboard terminal like app, with no GUI
 *	Scoreboard shows player scores and ranking a table-like manner.
 */

#include "scoreboard.h"
#include <algorithm>
#include <sstream>		// std::ostringstream
#include <sys/ioctl.h>	// get terminal
#include <unistd.h>


/* ------------------------------------------------------------ */


	// TODO mode file-saving if empty string
//	if (!s_f.empty())		// TODO exceptions
//		save_f.open(s_f, std::ios::out);	// initialize save file
//
//	if (!s_hf.empty())
//		h_file.open(s_hf, std::ios::out);	// initialize history file


/**
 * @brief Initializes scoreboard with number of players with default
 *	names and scores
 * @param num	Number of players to be initialized
 */
void Scoreboard::init_players(int num)
{
	// number of available players to be created
	int avail_plrs = max_players - players.size();

	if (num > avail_plrs)
	{
		report_war("Only " << avail_plrs << " players will be created, "
					"change this limit later: set plimit N, where N is "
					"the new limit");
		num = avail_plrs;
	}

	std::ostringstream aux;

	// init vector of players to plyrs number of players
	for(int i = 1; i <= num; i++)
	{	// optimized version of add_player() method
		aux.str(std::string());				// clear aux
		aux << "Player" << "(" << i << ")";
		players[aux.str()] = 0;				// adding player
	}

	sort_scb();								// need to sort

	std::cout << "Initialized with " << num << " players." << std::endl;
}

/**
 * @brief Sets maximum number of players that can be created
 * @param num Maximum number of players
 */
void Scoreboard::set_max_players(int num)
{
	debug_info();

	if (num < 0 || num > USHRT_MAX)	// hard limit 
		report_err("Incorrect number of maximum players", void());

	int i = players.size()-1;	// index
	while (i > num)			// remove players above limit
	{
		rm_player(i);		// TODO range delete
		i--;
	}

	max_players = num;
	std::cout << "Player limit set to: " << max_players << std::endl;
}

/**
 * @brief Adds a player to the scoreboard
 * 	Checks player limit, uniqueness of player's name, if not unique, then
 *	(N) is prepended, wher N is the number of that many times used name
 * @param name Name of the player
 * @param score Score of the player
 */
void Scoreboard::add_player(const std::string &name, int score)
{
	debug_info();

	if (players.size() >= max_players)		// checking limit of players
		report_err("Cannot create another player, at limit!", void());
	
	if (name.length() > MAX_PNAME)			// max limit of chars exceeded
		report_err("Player name too long, maximum 32 characters!", void());

	std::ostringstream aux;
	aux << name;
	// checking uniqueness of player's name
	std::map<std::string, int>::iterator it = players.find(name);
	for (int i = 1; it != players.end(); i++)
	{
		aux.str(std::string());				// clear aux
		aux << name << "(" << i << ")";		// max (65534)
		it = players.find(aux.str());
	}

	players[aux.str()] = score;				// adding player

	sort_scb();								// need to sort
}

/**
 * @brief Removes a player with certain rank
 * @param rank Rank of the player to be removed
 */
void Scoreboard::rm_player(int rank)
{
	debug_info();
	
	// use exceptions TODO
	if (rank < 1 || (static_cast<unsigned int>(rank) > players.size()))
		report_err("Incorrect player rank", void());
	
	std::vector<std::pair<std::string, int>>::iterator it = 
		std::next(pl_sort.begin(), rank-1);
	
	players.erase(it->first);
	pl_sort.erase(it);						// TODO sort again?? or only?
}

/**
 * @brief Removes a player with a certain name
 * @param Name of the player to be removed
 */
void Scoreboard::rm_player(const std::string &name)
{
	debug_info();

	if (name.empty())
		return;

	Pl_it it = players.find(name);
	if (it != players.end())
	{
		players.erase(it);
		sort_scb();
		return;
	}

	report_war("Player with that name does not exist");
}

/**
 * @brief Renames player defined by his rank to new_name. No empty string
 *	and maximally 32 characters long string.
 * @param rank Rank of the player whoose name will be changed
 * @param new_name New name of the player
 */
void Scoreboard::rename_player(int rank, const std::string &new_name)
{
	debug_info();

	if (new_name.empty() || new_name.length() > MAX_PNAME)
		report_err("Incorrect new name specified", void());

	Pl_it it = get_player(rank);	// checking rank
	if (it == players.end())
		report_err("Player with that rank does not exist", void());

	// checking uniqueness of player's name
	std::ostringstream aux;
	aux << new_name;
	std::map<std::string, int>::iterator a_it = players.find(new_name);
	for (int i = 1; a_it != players.end(); i++)
	{
		aux.str(std::string());				// clear aux
		aux << new_name << "(" << i << ")";
		a_it = players.find(aux.str());
	}

	// overwrite key
	auto nodeHandler = players.extract(it);	// detaches node
	nodeHandler.key() = aux.str();			// changes key
	players.insert(std::move(nodeHandler));	// inserts back the node

	sort_scb();								// need to sort
}

/**
 * @brief Renames player defined by his name to a new_name
 * @param name Name of the player to be renamed
 * @param new_name A new name for the player
 */
void Scoreboard::rename_player(const std::string &name, 
								const std::string &new_name)
{
	debug_info();

	if (new_name.empty() || new_name.length() > MAX_PNAME)
		report_err("Incorrect new name specified", void());

	Pl_it it = get_player(name);		// checking name
	if (it == players.end())
		report_err("Player with that name does not exist", void());

	// checking uniqueness of player's name
	std::ostringstream aux;
	aux << new_name;
	std::map<std::string, int>::iterator a_it = players.find(new_name);
	for (int i = 1; a_it != players.end(); i++)
	{
		aux.str(std::string());				// clear aux
		aux << new_name << "(" << i << ")";
		a_it = players.find(aux.str());
	}

	// overwrite key
	auto nodeHandler = players.extract(it);	// detaches node
	nodeHandler.key() = aux.str();			// changes key
	players.insert(std::move(nodeHandler));	// inserts back the node

	sort_scb();								// need to sort
}

/**
 * @brief Adds a number to a player's score, identified by his rank
 * @param rank Rank of player
 * @param num Number added to the player's score (can be negative)
 */
void Scoreboard::add_pscore(int rank, int num)
{
	debug_info();

	auto it = get_player(rank);
	if (it == players.end())
		report_err("Player with that rank does not exist", void());

	if (num > MAX_SCORE)		
		num = MAX_SCORE;		// automatically sets to upper limit
	else if (num < MIN_SCORE)
		num = MIN_SCORE;		// automatically sets to lower limit

	it->second += num;

	sort_scb();					// need to sort again
}

/**
 * @brief Adds a number to a player's score, identified by his name
 * @param name Name of the player
 * @param num Number added to the player's score (can be negative)
 */
void Scoreboard::add_pscore(const std::string &name, int num)
{
	debug_info();

	auto it = get_player(name);
	if (it == players.end())
		report_err("Player with that name does not exist", void());

	if (num > MAX_SCORE)		
		num = MAX_SCORE;		// automatically sets to upper limit
	else if (num < MIN_SCORE)
		num = MIN_SCORE;		// automatically sets to lower limit

	it->second += num;

	sort_scb();					// need to sort again
}

/**
 * @brief Resets player's score to 0
 * @param rank Player's rank
 */
void Scoreboard::reset_pscore(int rank)
{
	debug_info();

	auto it = get_player(rank);
	if (it == players.end())
		report_err("Player with that rank does not exist", void());

	it->second = 0;

	sort_scb();					// need to sort again
}

/**
 * @brief Resets player's score to 0
 * @param name Player's name
 */
void Scoreboard::reset_pscore(const std::string &name)
{
	debug_info();

	auto it = get_player(name);
	if (it == players.end())
		report_err("Player with that name does not exist", void());

	it->second = 0;

	sort_scb();					// need to sort again
}
	
/**
 * @brief TODO
 */
bool Scoreboard::save_to_file(std::ostream file)
{
	debug_info();
	
	(void)file;

	return false;
}

/**
 * @brief TODO
 */
bool Scoreboard::load_players_from_file(std::istream file)
{
	debug_info();
	
	(void)file;

	return false;
}

/**
 * @brief TODO
 */
bool Scoreboard::load_history(std::istream file)
{
	debug_info();

	(void)file;

	return false;
}

/**
 * @brief Prints the actual scores, players and ranking
 * 	Layout:
 *  _______________________________________________________________________
 * | RANK   | PLAYER NAME									| SCORE		   |
 *	-----------------------------------------------------------------------
 * | 1.		| Dudefish										| 22		   |
 * .
 * .
 * .
 * | 23.	| Kentril										| -20		   |
 *  -----------------------------------------------------------------------
 */
void Scoreboard::print(std::ostream & strm)
{
	debug_info();

	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	// printing header TODO consider using std AFTER to check any problems
	strm << " " << std::string(w.ws_col-2, '_') << std::endl <<
		"| RANK   | PLAYER NAME " <<
		std::string(w.ws_col-WIN_PADDING, ' ') << "| SCORE |" << std::endl;

	LINE_BREAK;

	int i = 1;
	for (auto it = pl_sort.begin(); it != pl_sort.end(); it++)
	{
		strm << "| " << i;
		// just one or two digits
		(i < 100) ? strm << ".\t | " : strm <<  ".\t| ";

		strm << it->first << " " << 
			std::string(w.ws_col-21-(it->first).length(), ' ') << "| " << 
			it->second << "\t|" << std::endl;
		LINE_BREAK;
		i++;
	}

	// TODO FIX TABS
}

/**
 * @brief Sorts the scoreboard players based on their score using
 *	set structure descending and aplhabetically when scores match
 */
void Scoreboard::sort_scb()
{
	debug_info();

	pl_sort.clear();

	// copying to the vector from map
	std::copy(players.begin(), players.end(),
		std::back_inserter<std::vector<std::pair<std::string,
													int>>>(pl_sort));
	// sorting the vector based on my rule
	std::sort(pl_sort.begin(), pl_sort.end(), 
				[](std::pair<std::string, int> const &a, 
					std::pair<std::string, int> const &b)
					{
						return a.second != b.second ? a.second > b.second :
											a.first < b.first;
					});
}
