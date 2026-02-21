#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>

using namespace std;

struct Tranzitie
{
    int dest;
    char lit;

    Tranzitie() = default;

    Tranzitie(int dest, char lit):
        dest(dest), lit(lit) {};
};

const int NMAX = 50000;

int n;
int stare[1 + NMAX];
unordered_map<int, int> hm;

int m;
vector<Tranzitie> graf[1 + NMAX];

int S;
int nrF;
int stareFin[1 + NMAX];

int NrCuv;
string cuv;

string cuvPart[1 + NMAX]; ///cuvPart[i] = partea ramasa din cuvantul initial atunci cand algoritmul a intrat in nodul i
vector<int> predecesor[1 + NMAX]; ///predecesor[i] = lista nodurilor din care am venit in nodul i (e o lista pentru ca putem ajunge intr-un nod de mai multe ori si din mai multi predecesori distincti)
///ultimul element din lista predecesor[i] este cel mai recent predecesor din care am venit in nodul i (ne folosim de acest lucru pentru a reface un lant de solutie)

queue<int> q;

string nextCuv;
string auxiliar;

vector<int> lantNoduriSolutie;

///Vom codifica lambda ca fiind "L" in fisierul de intrare.

int main()
{
    ifstream in("input.txt");
    ofstream out("output.txt");

    in >> n;
    for (int i = 1; i <= n; i++)
        in >> stare[i];

    for (int i = 1; i <= n; i++) ///Am hash-uit cele n stari la primele n numere, [1, 2, ..., n].
        hm[stare[i]] = i;
    ///In cod ne vom referi mereu la stare[i] ca fiind hash-ul sau, hm[stare[i]].

    in >> m;
    for (int i = 1; i <= m; i++)
    {
        int x, y;
        char l;
        in >> x >> y >> l;
        graf[hm[x]].emplace_back(y, l);
    }

    in >> S;
    
    in >> nrF;
    for (int i = 1; i <= nrF; i++)
    {
        in >> stareFin[i];
    }

    in >> NrCuv;
    for (int j = 1; j <= NrCuv; j++)
    {
        in >> cuv;
        
        if (cuv == "L") ///Cuvantul citit este cuvantul vid. In acest caz cuvantul este acceptat de NFA <=> starea initiala e si stare finala.
        {
            bool accept = false;
            
            for (int i = 1; i <= nrF; i++)
            {
                if (stareFin[i] == S)
                {
                    accept = true;
                    break;
                }
            }
            
            if (accept)
            {
                out << "DA\n";
                out << "Lantul solutiei:\n";
                out << S << '\n';
            }
            else
            {
                out << "NU\n";
            }
        }
        else
        {
            for (int i = 1; i <= n; i++)
            {
                cuvPart[i] = "$";
                predecesor[i].clear();
            }

        	while (!q.empty())
        	{
           		q.pop();
        	}
    
            cuvPart[hm[S]] = cuv;
            q.push(S);
            
            /*
            Cum vom trata ciclurile cu muchii ce contin doar cuvantul vid/lambda:
            Permitem ca algoritmul sa treaca prin aceeasi stare de mai multe ori, dar la fiecare trecere
            cuvantul partial ramas trebuie sa fie mai mic in lungime decat cuvantul partial de la
            trecerea anterioara prin acelasi nod.
            */
    
            while (!q.empty())
            {
                int stareCurenta = q.front();
                q.pop();
                auxiliar = cuvPart[hm[stareCurenta]];
    
                if (cuvPart[hm[stareCurenta]] != "") ///In acest if incercam sa ne deplasam doar pe muchiile ce nu contin cuvantul vid/lambda.
                {
                    char urmLitera = cuvPart[hm[stareCurenta]][0];
    
                    for (int i = 0; i < graf[hm[stareCurenta]].size(); i++)
                    {
                        if (graf[hm[stareCurenta]][i].lit == cuvPart[hm[stareCurenta]][0]) ///Aici tratam toate muchiile ce nu au cuvantul vid/lambda pe ele.
                        {
                            nextCuv = "";
                            for (int k = 1; k < cuvPart[hm[stareCurenta]].size(); k++)
                                nextCuv.push_back(cuvPart[hm[stareCurenta]][k]);
                                
                            if (cuvPart[hm[graf[hm[stareCurenta]][i].dest]] == "$" || nextCuv.size() < cuvPart[hm[graf[hm[stareCurenta]][i].dest]].size()) ///Ne ducem in alt nod doar daca nu am fost in nodul acela (adica avem "$" salvat acolo) sau daca venim cu un cuvant partial mai scurt.
                            {
                                cuvPart[hm[graf[hm[stareCurenta]][i].dest]] = nextCuv;
                                predecesor[hm[graf[hm[stareCurenta]][i].dest]].push_back(stareCurenta);
                                q.push(graf[hm[stareCurenta]][i].dest);
                            }
                        }
                    }
                }
                
                nextCuv = auxiliar;
                
                ///Mai incercam sa ne extindem si pe muchiile cu lambda pe ele.
                for (int i = 0; i < graf[hm[stareCurenta]].size(); i++)
                {
                    if (graf[hm[stareCurenta]][i].lit == 'L') ///Am codificat cuvantul vid/lambda ca fiind "L" in fisierul de intrare.
                    {
                        if (cuvPart[hm[graf[hm[stareCurenta]][i].dest]] == "$" || nextCuv.size() < cuvPart[hm[graf[hm[stareCurenta]][i].dest]].size()) ///Ne ducem in alt nod doar daca nu am fost in nodul acela (adica avem "$" salvat acolo) sau daca venim cu un cuvant partial mai scurt.
                        {
                            cuvPart[hm[graf[hm[stareCurenta]][i].dest]] = nextCuv;
                            predecesor[hm[graf[hm[stareCurenta]][i].dest]].push_back(stareCurenta);
                            q.push(graf[hm[stareCurenta]][i].dest);
                        }
                    }
                }
            }
    
            bool cuvantAcceptat = false;
            int stareCurentaInLant = -1;
    
            for (int i = 1; i <= nrF; i++)
            {
                if (cuvPart[hm[stareFin[i]]] == "")
                {
                    out << "DA\n";
                    cuvantAcceptat = true;
                    stareCurentaInLant = stareFin[i];
                    break;
                }
            }
    
            if (!cuvantAcceptat)
                out << "NU\n";
                
            if (cuvantAcceptat)
            {
                lantNoduriSolutie.clear();
                
                lantNoduriSolutie.push_back(stareCurentaInLant);
                
                while (!predecesor[hm[stareCurentaInLant]].empty())
                {
                    int aux = stareCurentaInLant;
                    stareCurentaInLant = predecesor[hm[stareCurentaInLant]].back();
                    predecesor[hm[aux]].pop_back();
                    
                    lantNoduriSolutie.push_back(stareCurentaInLant);
                }
                
                out << "Lantul solutiei:\n";
                for (int i = (int)lantNoduriSolutie.size() - 1; i >= 0; i--)
                    out << lantNoduriSolutie[i] << ' ';
                out << '\n';
            }
        }
    }

    in.close();
    out.close();

    return 0;
}
