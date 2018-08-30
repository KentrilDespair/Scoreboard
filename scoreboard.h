/**
 * @file scoreboard.h
 * @date 24. 8. 2018
 * @author Kentril Despair
 * @version 1.0
 * @brief Declarations of scoreboard program
 * 
 */


#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <iostream>
#include <fstream>
#include <climits>
#include <map>
#include <vector>
#include <functional>

// debugging macro
#ifndef DEBUG
#define debug_msg(...)

#define debug_info()

#else
#define debug_msg(x) do { std::cerr << __FUNCTION__ << ": " <<			\
								__LINE__ << ": " << x << std::endl; 	\
						} while(0)

#define debug_info() do { std::cerr << __FUNCTION__ << ": " <<			\
								__LINE__ << std::endl; } while(0)

#endif

#define report_err(x, y) do { std::cerr << "<Error>: " << x << std::endl; \
	return y; } while(0)

#define report_war(x) do { std::cerr << "<Warning>: " << x << std::endl; \
	} while(0)

/**
 * @brief An enum for all constants
 */
enum Consts
{
	// hard limit for number of players - USHRT_MAX
	S_PLIMIT = UCHAR_MAX,	// soft player limit
	H_PLIMIT = USHRT_MAX,	// hard player limit

	// that many players defined by the height of the terminal will be shown
	HGHT_LIMIT = -110,

	// score limits
	MAX_SCORE = 999,
	MIN_SCORE = -999,

	// name limits
	MAX_PNAME = 32,			// 40 chars, 32 + 7 optional + \0 - max length
	PNAME_LIMIT = 40		// hard limit, also true size of string
};


typedef std::map<char *, int>::iterator Pl_it;


/**
 * @brief TODO DEPRECATED NOW TODO
 */
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

/**
 * @brief
 */
class Scoreboard
{
		std::map<char *, int> players;	//< map of player names and scores
		std::vector<std::pair<char *, int>> pl_sort;
		int show_max;				///< How many players are shown
		unsigned int max_players;	///< Max. players to save info about
		std::filebuf save_f;		///< Can be printed to a file
		std::filebuf h_file;		///< History file saved players & scores
	public:
		// constructor
		Scoreboard(int plyrs = 0, int s_max = HGHT_LIMIT, 
					int m_plyrs = S_PLIMIT, 
					const std::string &s_f = std::string(),
					const std::string &s_hf = std::string());

		void set_show_max(int num);
		void set_max_players(int num);

		// player modification methods
		void add_player(const std::string &name = "", int score = 0);

		void rm_player(int rank);
		void rm_player(std::string &name);
		void rm_players();

		void rename_player(int rank, std::string &new_name);
		void rename_player(std::string &name, std::string &new_name);

		// score modification methods
		void add_pscore(int rank, int num);
		void add_pscore(std::string &name, int num);
		void reset_pscore(int rank);
		void reset_pscore(std::string &name);
		void reset_score();
		
		bool save_to_file(std::ostream file);
		bool load_players_from_file(std::istream file);
		bool load_history(std::istream file);

		void print(std::ostream & strm = std::cout);

		~Scoreboard();			// destructor
	private:
		void sort_scb();				// sorting function for set pl_sort
		Pl_it get_player(int rank);
		Pl_it get_player(std::string &name);
};

#endif	// include SCOREBOARD_H
