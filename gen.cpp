#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
using namespace std;

/*
Player create(Player A, Player B)
{
    Player P(A.id, A.depth, A.who);
    FOR(i,0,(A.Wt).size())
	FOR(j,0,(A.Wt)[i].size())
	    (P.Wt)[i][j] = ( (A.Wt)[i][j] + (B.Wt)[i][j] ) /2;
    FOR(i,0,BS*BS-4)
    {
	double norm = 0;
	FOR(j,0,NS)	norm += (P.Wt)[j][i]*(P.Wt)[j][i];
	norm = sqrt(norm);
	FOR(j,0,NS)	(P.Wt)[j][i] /= norm;
    }

    return P;
}

Player mutate(Player P)
{
    FOR(i,0,(P.Wt).size())
	FOR(j,1,(P.Wt)[i].size())
	    if (j >= 0 && j%1 == 0)
		//(P.Wt)[i][j] = (P.Wt)[i][j] * ( 1 + double(rand()%201-100)/1000 );
		(P.Wt)[i][j] = (P.Wt)[i][j] * ( 1 + double(rand()%201-100)/1000 );
	    else
		(P.Wt)[i][j] = (P.Wt)[i][j-1];

    FOR(i,0,BS*BS-4)
    {
	double norm = 0;
	FOR(j,0,NS)	norm += (P.Wt)[j][i]*(P.Wt)[j][i];
	norm = sqrt(norm);
	FOR(j,0,NS)	(P.Wt)[j][i] /= norm;
    }
    
    return P;
}
    
Player evolve(vector <Player> P, int G)
{
    vector <Player> T;
    vector <int> sc;
    int res, siz, siz2;
    
    FOR(i,0,P.size())	P[i].depth = 1;
    FOR(g,0,G)
    {
	siz = P.size();
	for (siz2 = 2; siz2*siz2 < siz; siz2++);
	//siz2 = siz/2;
	sc.clear();
	sc.resize(P.size(), 0);
	FOR(i,0,P.size())
	    FOR(j,0,P.size())
	    {
		res = Referee().play_game(P[i], P[j]);
		if (res == 0)	sc[i]++;
		else
		if (res == 1)	sc[j]++;
	    }

	FOR(i,0,sc.size())	cout<<sc[i]<<' ';cout<<endl;

	T.clear();
	int best, bind;
	FOR(k,0,siz2)
	{
	    best = bind = -1;
	    FOR(i,0,P.size())
	    	if (sc[i] > best)
		{
		    best = sc[i];
		    bind = i;
		}
	    T.push_back(P[bind]);
	    sc[bind] = -1;
	}


	//FOR(k,0,2)
	FOR(i,0,siz2)
	    FOR(j, i+1, siz2)
		T.push_back(mutate(create(T[i], T[j])));
	//FOR(i,0,siz2)	T.push_back(mutate(T[i]));
	//T.resize(min(siz, int(T.size())));
	P = T;

	//FOR(i,0,P.size())
	{//for (int j =0 ; j < BS*BS-4; j += 5)
	//cerr<<(P[i].Wt)[SCORE][j]<<'\t';
	//cerr<<endl;
	}//cerr<<endl;

    }


//FOR(i,0,P.size())
//FOR(j,0,BS*BS-4)
//for (int j =0 ; j < BS*BS-4; j += 5)
//cerr<<(P[0].Wt)[SCORE][j]<<'\t';
//cerr<<endl;
    return P[0];
}
*/


const double crossover_rate = 0.8, mutation_rate = 0.1;
bool elitist = true;

vector <Player> crossover(Player A, Player B)
{
	vector <Player> ret(2);


	int cross_point = rand() % int(A.Wt[0].size());
	cross_point /= 5;
	cross_point *= 5;
	//cerr<<"CROSS: "<<cross_point<<endl;
	FOR(i, 0, BS*BS-4)			//Note that the current bounds might be tailored to be used by the current main.cpp. Remember to make this independent.
		if (i >= cross_point)
				FOR(j, 0, NS)
					swap(A.Wt[j][i], B.Wt[j][i]);

	ret[0] = A;
	ret[1] = B;

	return ret;
	
}

void mutate(Player &A)
{
	double del, norm;
	
	FOR(i, 0, NS)
		FOR(j, 0, BS*BS-4)
		{
			del = (2*sqrt(double((rand()+10000)%10000)) - 100.0)/500.0;
			//cerr<<"DEL: "<<del<<'\t';
			/**/A.Wt[i][j] += A.Wt[i][j]*del;
			
		}

	FOR(i, 0, BS*BS-4)
	{
		norm = 0.0;
		FOR(j, 0, NS)	norm += A.Wt[j][i]*A.Wt[j][i];
		norm = sqrt(norm);
		FOR(j, 0, NS)	A.Wt[j][i] /= norm;
	}
		//cerr<<endl;
}

bool operator<(pair <int, Player> A, pair <int, Player> B)
{
	return (A.first < B.first);
}

void sort_players(vector <Player> &P, vector <int> &score)
{
	int pop = P.size();
	
	vector <pair <int, pair <int, Player> > > to_sort;
	FOR(i, 0, pop)	to_sort.push_back(make_pair(score[i], make_pair(i, P[i])));
	
	sort(to_sort.begin(), to_sort.end());
	
	P.clear();
	score.clear();
	RFOR(i, pop-1, -1)
	{
		P.push_back(to_sort[i].second.second);
		score.push_back(to_sort[i].first);
	}
}

vector <Player> new_gen(vector <Player> P, vector <int> score)		//score needs to be sorted in decreasing order.
{
	vector <Player> P2;
	int pop = P.size();
	int retain = (1.0 - crossover_rate) * double(pop);

	sort_players(P, score);
	/**/cerr<<"E:\n";
	if (elitist)
		FOR(i, 0, retain)
		{
			for (int j = 0; j < BS*BS-4; j += 5)	cerr<<P[i].Wt[SCORE][j]<<'\t';
			cerr<<endl;
			P2.push_back(P[i]);
		}
	/**/cerr<<"F:\n";
	int sum = 0;
	FOR(i, 0, score.size())	sum += score[i];
	
	while (P2.size() < pop)
	{
		Player p1, p2;
		vector <Player> newplayers;
		int pick1, pick2, sum2 = 0;
		pick2 = pick1 = rand()%sum;
		while (pick2 == pick1)	pick2 = rand()%sum;

		FOR(i, 0, pop)
		{
			sum2 += score[i];
			if (sum2 > pick1)	{p1 = P[i]; pick1 = INF; cerr<<P[i].Wt[0][0]<<' ';}
			if (sum2 > pick2)	{p2 = P[i]; pick2 = INF; cerr<<P[i].Wt[0][0]<<' ';}
		}
		cerr<<endl;
		newplayers = crossover(p1, p2);
		P2.push_back(newplayers[0]);
		P2.push_back(newplayers[1]);
		
	}

	if (!elitist)	FOR(i, 0, retain)	mutate(P2[i]);
	FOR(i, retain, P2.size())	mutate(P2[i]);

	return P2;

}

Player evolve(vector <Player> P, int G)
{
	vector <Player> T = P;
	vector <int> score;
	int result, pop;
	
	FOR(i, 0, P.size())	P[i].depth = 0;

	FOR(g, 0, G)
	{
		P = T;
		pop = P.size();

		score.clear();
		score.resize(pop, 0);
		
		FOR(i, 0, pop)
			FOR(j, 0, pop)
			{
				result = Referee().play_game(P[i], P[j]);

				if (result == 0)	score[i]++;
				if (result == 1)	score[j]++;
			}

		T = new_gen(P, score);
		
	



	/**/FOR(i,0,T.size())
	{/**/for (int j =0 ; j < BS*BS-4; j += 5)
	/**/cerr<<(T[i].Wt)[SCORE][j]<<'\t';
	/**/cerr<<endl;
	}/**/cerr<<endl;

	}

	sort_players(P, score);

	//FOR(i,0,P.size())
	//FOR(j,0,BS*BS-4)
	/**/FOR(k, 0, NS){
	/**/for (int j =0 ; j < BS*BS-4; j += 5)
	/**/cerr<<(P[0].Wt)[k][j]<<'\t';
	/**/cerr<<endl;}


	




	return P[0];

}
