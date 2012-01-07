/*
       (x,y) y - >
	  x  0 U 0
	  |  L X R
	  v  0 D 0
*/

#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;

#define FOR(a,b,c)	for (int a = int(b); a < int(c); a++)
#define RFOR(a,b,c)	for (int a = int(b); a > int(c); a--)
#define GO_L(i, j, x, y)	for (int i = x, j = y-1; j > -1; j--)
#define GO_R(i, j, x, y)	for (int i = x, j = y+1; j < BS; j++)
#define GO_U(i, j, x, y)	for (int i = x-1, j = y; i > -1; i--)
#define GO_D(i, j, x, y)	for (int i = x+1, j = y; i < BS; i++)
#define GO_UL(i, j, x, y)	for (int i = x-1, j = y-1; i > -1 && j > -1; i--, j--)
#define GO_UR(i, j, x, y)	for (int i = x-1, j = y+1; i > -1 && j < BS; i--, j++)
#define GO_DL(i, j, x, y)	for (int i = x+1, j = y-1; i < BS && j > -1; i++, j--)
#define GO_DR(i, j, x, y)	for (int i = x+1, j = y+1; j < BS && i < BS; i++, j++)

enum	{ B, W, N };
const char out[] = {'X', 'O', '-'};
const int NIL = -1;
const int BOARD_SIZE = 8, BS = BOARD_SIZE;
const int po3[] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441, 1594323, 4782969, 14348907, 43046721, 129140163};

struct Move
{
    int x, y, who;

    Move();
    Move(const Move& M);
    Move(int a, int b, int w);
};

struct Board
{
    vector <unsigned long long> A;					//bits denote cells on the board.
    vector <int> score;					//W, B
    
    Board();
    Board(const Board& B);
    Board(const vector <unsigned long long>& B);
    Board(const vector <vector <int> >& B);

    inline int operator()(int x, int y);

    bool valid(Move M);
    void flip(int x, int y);
    void place(Move M);
    void place(int x, int y, int who);
    vector <int> calc_score();
    vector <unsigned long long> b_to_ll(const vector <vector <int> >& B);
    vector <Move> all_moves(int who);
    bool move_exists(int who);
    bool game_over();
    
    Board try_move(Move M);
    void make_move(Move M);
    
    void print(int w);
};


/*******************************************************************Constructors***********************************************/

Move::Move()	{    x = y = -1; who = N;	}

Move::Move(const Move& M)	{	x = M.x; y = M.y; who = M.who;	}

Move::Move(int a, int b, int w)	{	x = a;   y = b;   who = w;	}

Board::Board()	{    A.resize(2); score.resize(2); place(BS/2-1,BS/2-1,W); place(BS/2,BS/2,W); place(BS/2-1,BS/2,B); place(BS/2,BS/2-1,B); 	}

Board::Board(const Board& B)	{	A = B.A; score = B.score;	}

Board::Board(const vector <unsigned long long>& B)	{	A = B;  score = calc_score();	}

Board::Board(const vector <vector <int> >& B)	{    A = b_to_ll(B); score = calc_score();	}

/******************************************************************Operators*************************************************/

inline int Board::operator()(int x, int y)
{
    return ( (A[W] & 1ll<<(BS*x+y)) ? W : ( (A[B] & 1ll<<(BS*x+y)) ? B : N ) );
}

/*****************************************************************Functions*************************************************/

bool Board::valid(Move M)
{
    if ( M.x<0 || M.x>BS-1 || M.y<0 || M.y>BS-1 || (M.who != W && M.who != B) )	{ cerr<<"Exception called from \"bool Board::valid(int,int)\" : Invalid Move!!\n"<<M.x<<' '<<M.y<<endl; exit(1); }
    if ( (*this)(M.x,M.y) != N )	return false;

    //bool ret = false;
    int other = 1-M.who, t;
    GO_L(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other){ if ( j != M.y-1 && t == M.who)	return true;	break;	}	//ret = true;	break; }
    GO_R(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other){ if ( j != M.y+1 && t == M.who)	return true;	break;	}	//ret = true;	break; }
    GO_U(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other){ if ( i != M.x-1 && t == M.who)	return true;	break;	}	//ret = true;	break; }
    GO_D(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other){ if ( i != M.x+1 && t == M.who)	return true;	break;	}	//ret = true;	break; }
    GO_UL(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other){ if ( i != M.x-1 && t == M.who)	return true;	break;	}	//ret = true;	break; }
    GO_UR(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other){ if ( i != M.x-1 && t == M.who)	return true;	break;	}	//ret = true;	break; }
    GO_DL(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other){ if ( i != M.x+1 && t == M.who)	return true;	break;	}	//ret = true;	break; }
    GO_DR(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other){ if ( i != M.x+1 && t == M.who)	return true;	break;	}	//ret = true;	break; }

    return false;	//return ret;
}

void Board::flip(int x, int y)
{
    if ( x<0 || x>BS-1 || y<0 || y>BS-1 )	{ cerr<<"Exception called from \"void Board::flip(int,int)\" : Invalid Position!!\n"<<x<<' '<<y<<endl; exit(1); }

    int t = (*this)(x,y);
    if ( t != W && t != B)	{ cerr<<"Exception called from \"void Board::flip(int,int)\" : Could not flip!!\n"<<x<<' '<<y<<endl; exit(1); }

    A[W] ^= 1ll<<(BS*x+y);
    A[B] ^= 1ll<<(BS*x+y);
    
    score[t]--;
    score[1-t]++;
}

void Board::place(Move M)	{	place(M.x, M.y, M.who);	}

void Board::place(int x, int y, int who)
{
    if ( x<0 || x>BS-1 || y<0 || y>BS-1 )	{ cerr<<"Exception called from \"void Board::place(int,int,int)\" : Invalid Position!!\n"<<x<<' '<<y<<endl; exit(1); }
    
    int t = (*this)(x,y);
    if ( t != N)	{ cerr<<"Exception called from \"void Board::place(int,int,int)\" : Could not place!!\n"<<t<<endl; exit(1); }

    A[who] |= 1ll<<(BS*x+y);
    score[who]++;
}

vector <int> Board::calc_score()
{
    vector <int> ret(2,0);

    FOR(i,0,A.size())
	FOR(j,0,BS*BS)
	    ret[i] += (A[i] & (1ll<<j)) ? 1 : 0;

    return ret;
}

vector <unsigned long long> Board::b_to_ll(const vector <vector <int> >& B)
{
    if (B.size() != BS)		{ cerr<<"Exception called from \"vector <int> b_to_int(vector <vector <int> >)\" : Wrong size!!\n"; exit(1);	}
    
    vector <unsigned long long> ret(3, 0);
    FOR(i,0,BS)
	FOR(j,0,BS)
	    ret[B[i][j]] |= 1ll<<(i*BS+j);

    ret.resize(2);
    return ret;
}

bool Board::move_exists(int who)
{
    FOR(i,0,BS)
	FOR(j,0,BS)
	    if ( (*this)(i,j) == N)
		if ( valid(Move(i,j,who)) )	return true;
    
    return false;
}

bool Board::game_over()
{
    FOR(i,0,BS)
	FOR(j,0,BS)
	    if ( (*this)(i,j) == N)
	    {
		if ( valid(Move(i,j,B)) )	return false;
		if ( valid(Move(i,j,W)) )	return false;
	    }
    return true;
}
	    
Board Board::try_move(Move M)
{
    if ( M.x<0 || M.x>BS-1 || M.y<0 || M.y>BS-1 || (M.who != W && M.who != B) || (*this)(M.x,M.y) != N )	{ cerr<<"Exception called from \"Board Board::try_move(Move)\" : Invalid Move!!\n"<<M.x<<' '<<M.y<<endl; exit(1); }
         

    Board B(*this);
    int other = 1-M.who, t;
    bool ok = false;
    
    GO_L(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other)
			{
			    if (t == M.who)
				GO_R(k,l,i,j)	{	if (l == M.y) break; B.flip(k,l); ok = true;	}
			    break;
			}

    GO_R(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other)
			{
			    if (t == M.who)
				GO_L(k,l,i,j)	{	if (l == M.y) break; B.flip(k,l); ok = true;	}
			    break;
			}

    GO_U(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other)
			{
			    if (t == M.who)
				GO_D(k,l,i,j)	{	if (k == M.x) break; B.flip(k,l); ok = true;	}
			    break;
			}

    GO_D(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other)
			{
			    if (t == M.who)
				GO_U(k,l,i,j)	{	if (k == M.x) break; B.flip(k,l); ok = true;	}
			    break;
			}

    GO_UL(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other)
			{
			    if (t == M.who)
				GO_DR(k,l,i,j)	{	if (l == M.y) break; B.flip(k,l); ok = true;	}
			    break;
			}

    GO_UR(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other)
			{
			    if (t == M.who)
				GO_DL(k,l,i,j)	{	if (l == M.y) break; B.flip(k,l); ok = true;	}
			    break;
			}

    GO_DL(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other)
			{
			    if (t == M.who)
				GO_UR(k,l,i,j)	{	if (l == M.y) break; B.flip(k,l); ok = true;	}
			    break;
			}

    GO_DR(i,j,M.x,M.y)	if ( (t = (*this)(i,j)) != other)
			{
			    if (t == M.who)
				GO_UL(k,l,i,j)	{	if (l == M.y) break; B.flip(k,l); ok = true;	}
			    break;
			}

    if (ok)	B.place(M);
    else	{ cerr<<"Exception called from \"Board Board::try_move(Move)\" : Can't play there.\n"; exit(1); }

    return B;
}

void Board::make_move(Move M)	{	(*this) = try_move(M);	}

void Board::print(int w)
{
    cout<<endl<<"  ";
    FOR(i,0,BS)		cout<<char('a'+i)<<' ';cout<<endl;
    FOR(i,0,BS)
    {cout<<(i+1)<<' ';
	FOR(j,0,BS)
	{
	    if (w == 0 || w == 1)
	    {
		if (valid(Move(i,j,w)))	cout<<'*'<<' ';
		else			cout<<out[(*this)(i,j)]<<' ';
	    }
	    else
	    cout<<out[(*this)(i,j)]<<' ';
	}
	cout<<endl;
    }
    cout<<"B: "<<score[0]<<"\tW: "<<score[1]<<endl;
    cout<<endl;
}

vector <Move> Board::all_moves(int who)
{
    vector <Move> ret;
    Move t;
    FOR(i,0,BS)
	FOR(j,0,BS)
	    if (valid(t = Move(i,j,who)))	ret.push_back(t);

    return ret;
}

/*
int main()
{
    Board A;
    A.place(4,4,W);
    A.place(3,4,B);
    A.place(3,3,W);
    A.place(4,3,B);
    
    A.print();
    int x, y;
    vector <Move> M;
    for (int i = 0; i < 10; i++)
    {
	M = A.all_moves(i%2);
	FOR(j,0,M.size())	cout<<M[j].x<<' '<<M[j].y<<'\t';cout<<endl;
	cin>>x>>y;
	A.make_move(Move(x,y,i%2));
        A.print();
    }
    return 0;
}
*/