#include <iostream>
#include <queue>
#include <cstdlib>
#include "base.cpp"
using namespace std;

const double INF = 1E9;
const int NS = 5;		//Number of strategy factors.
enum	{ SCORE, STABILITY, MOBILITY, FRONTIER, PARITY };		//Fill this.
enum	{ COMP, HUMAN };

int cnt = 0, cnt2 = 0;

struct Player
{
    int id, who, depth, move;				//id - COMP/ HUMAN, who - colour, depth - of search, move - number of move.
    vector <vector <double> > Wt;
    
    //Player();
    Player(int i, int d, int w);
    Player(int i, int d, int w, vector <vector <double> > wt);
    Player(const Player& P);

    Move play(Board Bo);
    double search(Board Bo, int w, double a, double b, int d);	//Alpha-Beta. a - assured
    double eval(Board Bo);

    
    
};

/*******************************************************************Constructors***********************************************/


Player::Player(int i = COMP, int d = 0, int w = B)	{	id = i; who = w; depth = d; move = w; Wt.resize(NS); FOR(i,0,NS) Wt[i].resize(BS*BS-4, 0.0);	}

Player::Player(int i, int d, int w, vector <vector <double> > wt)	{	id = i; who = w; depth = d; move = w; Wt = wt;	}

Player::Player(const Player& P)	{	id = P.id; who = P.who; depth = P.depth; Wt = P.Wt; move = P.move;	}


/*****************************************************************Functions*************************************************/


Move Player::play(Board Bo)
{
    if (id == HUMAN)
    {
		char x, y;
		Move ret;
		do
		{
			do{    cin>>y>>x;	}while (y > 'h' || y < 'a' || x > '8' || x < '1');
			ret = Move((x-'1'), (y-'a'), who);
		} while (!Bo.valid(ret));
		move+=2;
		return ret;
    }

    if (move > BS*BS-14)	depth = (BS*BS-4) - move;		//50 taken as endgame.
    vector <Move> moves = Bo.all_moves(who);
    double best = (-1.0)*INF, temp;
    Move ret = moves[0];
    //cnt = cnt2 = 0;
    move++;

	FOR(i,0,moves.size())
		if ((temp = (-1)*search( Bo.try_move(moves[i]), (1-who), (-1)*INF, INF, (depth) ) ) >= best)
		{
			if (temp == best && rand()%2)	continue;
			best = temp;
			ret = moves[i];
		}
    move--;
    move+=2;
    //cout<<cnt<<' '<<cnt2<<endl;
    //cout<<out[who]<<": "<<char('a'+ret.y)<<(ret.x+1)<<endl;
    return ret;
}

double Player::search(Board Bo, int w, double a, double b, int d)
{
    if (d == 0)
    {
		if (!(Bo.move_exists(w)) && !(Bo.move_exists(1-w)))	{ /*move--;*/ return ( (Bo.score[w] > Bo.score[1-w]) ? INF : ((Bo.score[w] < Bo.score[1-w]) ? (-1)*INF : (-1)*INF+1 ) ); }	
		double E = eval(Bo);
		return ((w == who) ? E : (-1)*E);
    }
    
    move++;
    vector <Move> moves = Bo.all_moves(w);

	if (moves.empty())
    {
		if (!(Bo.move_exists(1-w)))	{ move--; return ( (Bo.score[w] > Bo.score[1-w]) ? INF : ((Bo.score[w] < Bo.score[1-w]) ? (-1)*INF : (-1)*INF+1 ) ); }	//A tie is given very low value. We like to win. :)
		else				{ move--; return (-1)*search( Bo, 1-w, (-1)*b, (-1)*a, d-1 ); }
    }
    //cnt2 += moves.size();

	FOR(i,0,moves.size())
    {/**/cnt++;
		a = max(a, (-1)*search( Bo.try_move(moves[i]), 1-w,(-1)*b, (-1)*a, d-1));
		if (a >= b)	{ move--; return a; }
    }

    move--;
    return a;
}

double Player::eval(Board Bo)
{
    double ret = 0.0;
    int scor = 0;
    FOR(i,0,BS)
		FOR(j,0,BS)
			if (Bo(i,j) == who)		scor++;
			else
			if (Bo(i,j) == 1-who)	scor--;

	ret += double(scor)*Wt[SCORE][move];

	int mob = 0;
    FOR(i,0,BS)
		FOR(j,0,BS)
			if (Bo(i,j) == N)
			{
				if (Bo.valid(Move(i,j,who)))	mob++;
				if (Bo.valid(Move(i,j,1-who)))	mob--;
			}

    ret += double(mob)*Wt[MOBILITY][move];
													//Stability finding is TERRIBLE. Correct it.
    vector <vector <bool> > seen;
    vector <vector <int> > neigh;					//no. of surrounding stable discs of same colour.
    queue <Move> Q;
    int t;
    seen.resize(BS);	FOR(i,0,BS)	seen[i].resize(BS, false);
    neigh.resize(BS);	FOR(i,0,BS)	neigh[i].resize(BS, 0);

    neigh[0][0] = neigh[0][BS-1] = neigh[BS-1][0] = neigh[BS-1][BS-1] = 5;
    FOR(i,1,BS-1)	neigh[0][i] = neigh[i][0] = neigh[BS-1][i] = neigh[i][BS-1] = 3;

    if ((t = Bo(0,0)) != N)			{	Q.push(Move(0,0,t)); 		seen[0][0] = true;	}
    if ((t = Bo(0,BS-1)) != N)		{	Q.push(Move(0,BS-1,t)); 	seen[0][BS-1] = true;	}
    if ((t = Bo(BS-1,0)) != N)		{	Q.push(Move(BS-1,0,t)); 	seen[BS-1][0] = true;	}
    if ((t = Bo(BS-1,BS-1)) != N)	{	Q.push(Move(BS-1,BS-1,t)); 	seen[BS-1][BS-1] = true;}

    Move v;
    while (!Q.empty())									//This is REALLY faulty. Doesn't count everything and counts some wrong.
    {											//But shouldn't be very far from the correct method.
		v = Q.front();
		Q.pop();

		if (v.x > 0)	if (Bo(v.x-1, v.y) == v.who)
						{    neigh[v.x-1][v.y]++;
							if (neigh[v.x-1][v.y] >= 4 && !seen[v.x-1][v.y])
							{
							Q.push(Move(v.x-1, v.y, v.who));
							seen[v.x-1][v.y] = true;
							}
						}

		if (v.x < BS-1)	if (Bo(v.x+1, v.y) == v.who)
						{    neigh[v.x+1][v.y]++;
							if (neigh[v.x+1][v.y] >= 4 && !seen[v.x+1][v.y])
							{
							Q.push(Move(v.x+1, v.y, v.who));
							seen[v.x+1][v.y] = true;
							}
						}
		if (v.y > 0)	if (Bo(v.x, v.y-1) == v.who)
						{    neigh[v.x][v.y-1]++;
							if (neigh[v.x][v.y-1] >= 4 && !seen[v.x][v.y-1])
							{
							Q.push(Move(v.x, v.y-1, v.who));
							seen[v.x][v.y-1] = true;
							}
						}
		if (v.y < BS-1)	if (Bo(v.x, v.y+1) == v.who)
						{    neigh[v.x][v.y+1]++;
							if (neigh[v.x][v.y+1] >= 4 && !seen[v.x][v.y+1])
							{
							Q.push(Move(v.x, v.y+1, v.who));
							seen[v.x][v.y+1] = true;
							}
						}
    }

    int stab = 0;
    FOR(i,0,BS)
		FOR(j,0,BS)
			if (seen[i][j] && Bo(i,j) == who)	stab++;
			else
			if (seen[i][j] && Bo(i,j) == 1-who)	stab--;

    ret += double(stab)*Wt[STABILITY][move];

	int fron = 0;
    FOR(i,0,BS)
		FOR(j,0,BS)
		{
			if (i>0)	if (Bo(i-1,j) == N && !seen[i][j])	{ fron += (Bo(i,j) == who) ? -1 : 1; continue; }
			if (i<BS-1)	if (Bo(i+1,j) == N && !seen[i][j])	{ fron += (Bo(i,j) == who) ? -1 : 1; continue; }
			if (j>0)	if (Bo(i,j-1) == N && !seen[i][j])	{ fron += (Bo(i,j) == who) ? -1 : 1; continue; }
			if (j<BS-1)	if (Bo(i,j+1) == N && !seen[i][j])	{ fron += (Bo(i,j) == who) ? -1 : 1; continue; }
		}

    ret += double(fron)*Wt[FRONTIER][move];

	int par = -1;					//There must be an odd number of odd regions.
    int cnt = 0;
    
    seen.clear();
	seen.resize(BS);	FOR(i,0,BS)	seen[i].resize(BS, false);
    FOR(i,0,BS)
		FOR(j,0,BS)
			if (Bo(i,j) == N && !seen[i][j])
			{
				Q.push(Move(i,j,N));
				seen[i][j] = true;
				cnt = 1;
				while (!Q.empty())
				{
					v = Q.front();
					Q.pop();

					if (v.x>0)		if (Bo(v.x-1,v.y) == N && !seen[v.x-1][v.y])	{	Q.push(Move(v.x-1, v.y, N));	seen[v.x-1][v.y] = true;	cnt++;	}
					if (v.x<BS-1)	if (Bo(v.x+1,v.y) == N && !seen[v.x+1][v.y])	{	Q.push(Move(v.x+1, v.y, N));	seen[v.x+1][v.y] = true;	cnt++;	}
					if (v.y>0)		if (Bo(v.x,v.y-1) == N && !seen[v.x][v.y-1])	{	Q.push(Move(v.x, v.y-1, N));	seen[v.x][v.y-1] = true;	cnt++;	}
					if (v.y<BS-1)	if (Bo(v.x,v.y+1) == N && !seen[v.x][v.y+1])	{	Q.push(Move(v.x, v.y+1, N));	seen[v.x][v.y+1] = true;	cnt++;	}
				}
				if (cnt%2)	par *= -1;
			}

	if (move%2 != who)	par *= -1;
    ret += double(par)*Wt[PARITY][move];

	return ret;
}

