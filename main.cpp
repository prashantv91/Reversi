#include <iostream>
#include <cstdlib>
#include "referee.cpp"
#include "gen.cpp"
#include <ctime>
#include <cmath>
#include <iomanip>
using namespace std;

int main()
{
    vector <vector <double> > Wt(NS);
    vector <Player> P(25, Player(COMP, 0, W));
	int c = 0;

	srand(time(NULL));
    FOR(p,0,P.size())
    {
	
		(P[p].Wt)[SCORE][0] = double(rand()%100);//20;				//Ideally, the divisions shouldn't matter.
		(P[p].Wt)[STABILITY][0] = double(rand()%100);//5;
		(P[p].Wt)[MOBILITY][0] = double(rand()%100);//5;
		(P[p].Wt)[FRONTIER][0] = double(rand()%100);//5;
		(P[p].Wt)[PARITY][0] = double(rand()%100);

		double norm = 0.0;
		FOR(j,0,5)	norm += (P[p].Wt)[j][0]*(P[p].Wt)[j][0];

		norm = sqrt(norm);
		FOR(j,0,5)	(P[p].Wt)[j][0] /= norm;

		FOR(i,1,BS*BS-4)
		{
			if (i > 0 && i%5 == 0)
			{/**/c++;
				(P[p].Wt)[0][i] = double(rand()%100) + 1;
				(P[p].Wt)[1][i] = double(rand()%100) + 1;
				(P[p].Wt)[2][i] = double(rand()%100) + 1;
				(P[p].Wt)[3][i] = double(rand()%100) + 1;
				(P[p].Wt)[4][i] = double(rand()%100) + 1;

				/*(P[p].Wt)[0][i] = (P[p].Wt)[0][i-1] + (P[p].Wt)[0][0]*(double(rand()%201-100)/500.0);//*(1 + double(rand()%201-100)/100.0);				//NS = 5
				(P[p].Wt)[1][i] = (P[p].Wt)[1][i-1] + (P[p].Wt)[1][0]*(double(rand()%201-100)/500.0);//*(1 + double(rand()%201-100)/100.0);
				(P[p].Wt)[2][i] = (P[p].Wt)[2][i-1] + (P[p].Wt)[2][0]*(double(rand()%201-100)/500.0);//*(1 + double(rand()%201-100)/100.0);
				(P[p].Wt)[3][i] = (P[p].Wt)[3][i-1] + (P[p].Wt)[3][0]*(double(rand()%201-100)/500.0);//*(1 + double(rand()%201-100)/100.0);
				(P[p].Wt)[4][i] = (P[p].Wt)[4][i-1] + (P[p].Wt)[4][0]*(double(rand()%201-100)/500.0);//*(1 + double(rand()%201-100)/100.0);
				*/

				double norm = 0.0;
				FOR(j,0,5)	norm += (P[p].Wt)[j][i]*(P[p].Wt)[j][i];

				norm = sqrt(norm);
				FOR(j,0,5)	(P[p].Wt)[j][i] /= norm;

			}
			else
			{
				(P[p].Wt)[0][i] = (P[p].Wt)[0][i-1];
				(P[p].Wt)[1][i] = (P[p].Wt)[1][i-1];
				(P[p].Wt)[2][i] = (P[p].Wt)[2][i-1];
				(P[p].Wt)[3][i] = (P[p].Wt)[3][i-1];
				(P[p].Wt)[4][i] = (P[p].Wt)[4][i-1];
			}
		}
    }
    cerr<<setprecision(2)<<c<<endl;
    
/**/FOR(i,0,P.size())
{/**/for (int j =0 ; j < BS*BS-4; j += 5)
/**/cerr<<(P[i].Wt)[SCORE][j]<<'\t';
/**/cerr<<endl;}cerr<<endl;
/**/cerr<<"AA\n";


    Player P1 = evolve(P,15);
	cerr<<"A\n";
	P1.depth = 0;//3;

	FOR(i, 0, P.size())
	{cerr<<"B ";
	Player P2(P[i]);cerr<<"C ";
		//Player P2 = evolve(P,8);
		P2.depth = 0;//3;cerr<<"A\n";
		//Player P2(HUMAN);
	
		Referee R;cerr<<"D ";
		cout<<out[R.play_game(P1,P2)]<<' ';cerr<<"E\n";
	}
	return 0;
}