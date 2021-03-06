/**
 * @file scoreboard.h
 * @date 24. 8. 2018
 * @author Kentril Despair
 * @version 1.0
 * @brief Declarations of scoreboard program
 * 	TODO
 */


#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <iostream>
#include <fstream>
#include <climits>
#include <map>
#include <vector>
#include <functional>

// debugging macros
#ifndef DEBUG
#define debug_msg(...)

#define debug_info()

#else
#define debug_msg(x) do { std::cerr << __FUNCTION__ << ": " <<			\
	__LINE__ << ": |" << x << "|" << std::endl; } while(0)

#define debug_info() do { std::cerr << __FUNCTION__ << ": " <<			\
							__LINE__ << std::endl; } while(0)

#endif

#define report_err(x, y) do { std::cerr << "<Error>: " << x << std::endl; \
	return y; } while(0)

#define report_war(x) do { std::cerr << "<Warning>: " << x << std::endl; \
	} while(0)

#define LINE_BREAK std::cout << " " << std::string(w.ws_col-2, '-') <<	\
	std::endl;

/**
 * @brief An enum for all constants used across the program
 */
enum Consts
{
	// hard limit for number of players - USHRT_MAX
	S_PLIMIT = UCHAR_MAX,	// soft player limit
	H_PLIMIT = USHRT_MAX,	// hard player limit

	// number of players shown, set by the current height of terminal
	HGHT_LIMIT = -110,

	// score limits
	MAX_SCORE = 9999,
	MIN_SCORE = -9999,

	// name limits
	MAX_PNAME = 32,			// 40 chars, 32 + 7 optional + \0 - max length
	PNAME_LIMIT = 40,		// hard limit, also true size of string

	// terminal constants
	WIN_PADDING = 32		// window padding
};

// For convenience use, Player iterator type
typedef std::map<std::string, int>::iterator Pl_it;


/**
 * @brief TODO DEPRECATED NOW TODO
 *
class Player
{
	std::string name;	///< Player name, by default "Player_%"
	int score;			///< Current score, by default 0
	
public:
	// implicit constructor
	Player(const std::string new_name, int sc = 0): name{new_name}, 
													score{sc} {}
	// name modifying methods
	void set_name(const std::string &name);
	std::string const *get_name();

	// score modifying methods
	void set_score(int num);
	int get_score();
	void inc_score();
	void dec_score();

	~Player() {}					// destructor
};
*/

/** TODO DEPRECATED NOW TODO
 * @brief Sets a player name
 * @param name Desired new name
 *
inline void Player::set_name(const std::string &name)
{
	this->name = name;
}
*/

/** TODO DEPRECATED NOW TODO
 * @brief Provides access to the name of player
 * @return Const pointer to name of player
 *
inline std::string const *Player::get_name()
{
	return &name;
}
*/

/** TODO DEPRECATED NOW TODO
 * @brief Sets score of a player
 * @param num New score of player
 *
inline void Player::set_score(int num)
{
	if (num > MAX_SCORE)		
		score = MAX_SCORE;		// automatically sets to upper limit
	else if (num < MIN_SCORE)
		score = MIN_SCORE;		// automatically sets to lower limit

	score = num;
}
*/

/** TODO DEPRECATED NOW TODO
 * @brief Get player score
 * @return player score
 *
inline int Player::get_score()
{
	return score;
}
*/

/** TODO DEPRECATED NOW TODO
 * @brief Increments player score by one
 *
inline void Player::inc_score()
{
	if (score < MAX_SCORE)
		score++;
	else
		report_war("Maximum score reached");
}
*/

/** TODO DEPRECATED NOW TODO
 * @brief Decrements player score by one
 *
inline void Player::dec_score()
{
	if (score > MIN_SCORE)
		score--;
	else
		report_war("Minimum score reached");
}
*/

// ----------------------------------------------------------------------

/**
 * @brief Scoreboard class
 */
class Scoreboard
{
		///< map of player names and player scores
		std::map<std::string, int> players;	
		///< used for sorting and printing the output
		std::vector<std::pair<std::string, int>> pl_sort;	

		int show_max;				///< How many players are shown
		unsigned int max_players;	///< Max. players to save info about
		std::filebuf save_f;		///< Can be printed to a file
		std::filebuf h_file;		///< History file saved players & scores
	public:
		// default constructor
		Scoreboard(): show_max{HGHT_LIMIT}, max_players{S_PLIMIT} {}
		
		void init_players(int num);
		void set_show_max(int num);
		void set_max_players(int num);

		// player modification methods
		void add_player(const std::string &name = "Player", int score = 0);

		void rm_player(int rank);
		void rm_player(const std::string &name);
		void rm_players();

		void rename_player(int rank, const std::string &new_name);
		void rename_player(const std::string &name, 
							const std::string &new_name);

		// score modification methods
		void add_pscore(int rank, int num = 1);
		void add_pscore(const std::string &name, int num = 1);
		void reset_pscore(int rank);
		void reset_pscore(const std::string &name);
		void reset_score();
		
		bool save_to_file(std::ostream file);
		bool load_players_from_file(std::istream file);
		bool load_history(std::istream file);

		void print(std::ostream & strm = std::cout);

		~Scoreboard() {	rm_players(); }	///< destructor
	private:
		void sort_scb();				///< sorting function for vector
		Pl_it get_player(int rank);
		Pl_it get_player(const std::string &name);
};

/**
 * @brief Sets current number of shown players
 * @param num Maximum number of shown players
 */
inline void Scoreboard::set_show_max(int num)
{
	debug_info();

	if (num < 0 || num > USHRT_MAX)
		report_err("Incorrect number of maximum players shown", void());

	show_max = num;
	std::cout << "Player show limit set to: " << show_max << std::endl;
}

/**
 * @brief Gets a pointer reference to a player using his rank
 * @param rank A position in the table score system
 * @return Pointer to the player iterator
 */ 
inline Pl_it Scoreboard::get_player(int rank)
{
	debug_info();

	// use exceptions TODO
	if ( (rank < 1) || (static_cast<unsigned int>(rank) > players.size()))	
		report_err("Incorrect player rank", players.end());
	
	std::vector<std::pair<std::string, int>>::iterator it = 
		std::next(pl_sort.begin(), rank-1);
	
	return players.find(it->first);
}

/**
 * @brief Gets a pointer reference to a player using his name
 * @param name Player's identifiable name
 * @param Pointer to the player iterator
 */
inline Pl_it Scoreboard::get_player(const std::string &name)
{
	debug_info();

	if (name.empty() || name.length() > PNAME_LIMIT)
		return players.end();

	return players.find(name);
}

/**
 * @brief Empties both structures
 */
inline void Scoreboard::rm_players()
{
	debug_info();

	players.clear();
	pl_sort.clear();
}

/**
 * @brief Resets score of all players to zero
 */
inline void Scoreboard::reset_score()
{
	debug_info();

	for (auto it = players.begin(); it != players.end(); it++)
		it->second = 0;

	sort_scb();					// need to sort again
}
		
#endif	// include SCOREBOARD_H
