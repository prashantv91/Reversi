#include <iostream>
#include "player.cpp"
using namespace std;

struct Referee
{
    Board Bo;
    int max_moves;

    Referee();
    Referee(const Referee& R);
    Referee(Board b, int mm);

    int play_game(Player P1, Player P2);

};

/********************************************************************Constructors*************************************************/

Referee::Referee()	{	Bo = Board(); max_moves = BS*BS-4;	}

Referee::Referee(const Referee& R)	{	Bo = R.Bo; max_moves = R.max_moves;	}

Referee::Referee(Board b, int mm = BS*BS-4)	{	Bo = b; max_moves = mm;	}

/*******************************************************************Functions****************************************************/

int Referee::play_game(Player P1, Player P2)
{
    int who = 0;
    Move M;
    Player P[2];
    
    P[0] = Player(P1.id, P1.depth, B, P1.Wt);
    P[1] = Player(P2.id, P2.depth, W, P2.Wt);

    while (!Bo.game_over())
    {
		if (Bo.move_exists(who))
		{
			if (P[0].id == HUMAN || P[1].id == HUMAN)	Bo.print(who);
			M = P[who].play(Bo);
			Bo.make_move(M);

		}
		who = 1-who;
    }
    
    if (P[0].id == HUMAN || P[1].id == HUMAN)	Bo.print(2);
    int ret = (Bo.score[0] > Bo.score[1]) ? 0: ( (Bo.score[0] < Bo.score[1]) ? 1 : 2) ;
    return ret;
}
