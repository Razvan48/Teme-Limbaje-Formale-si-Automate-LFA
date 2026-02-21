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

queue<int> q;

string nextCuv;

vector<int> lantNoduriSolutie;

vector<int> predecesor[1 + NMAX]; ///predecesor[i] = lista nodurilor din care am venit in nodul i (e o lista pentru ca putem ajunge intr-un nod de mai multe ori si din mai multi predecesori distincti)
///ultimul element din lista predecesor[i] este cel mai recent predecesor din care am venit in nodul i (ne folosim de acest lucru pentru a reface un lant de solutie)

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
        in >> stareFin[i];

    in >> NrCuv;
    for (int j = 1; j <= NrCuv; j++)
    {
        in >> cuv;

        for (int i = 1; i <= n; i++)
        {
            predecesor[i].clear();
            cuvPart[i] = "$";
        }
        
        while (!q.empty())
        {
            q.pop();
        }

        cuvPart[hm[S]] = cuv;
        q.push(S);

        while (!q.empty())
        {
            int stareCurenta = q.front();
            q.pop();

            if (cuvPart[hm[stareCurenta]] != "")
            {
                char urmLitera = cuvPart[hm[stareCurenta]][0];

                for (int i = 0; i < graf[hm[stareCurenta]].size(); i++)
                {
                    if (graf[hm[stareCurenta]][i].lit == cuvPart[hm[stareCurenta]][0])
                    {
                        nextCuv = "";
                        for (int k = 1; k < cuvPart[hm[stareCurenta]].size(); k++)
                            nextCuv.push_back(cuvPart[hm[stareCurenta]][k]);
                        predecesor[hm[graf[hm[stareCurenta]][i].dest]].push_back(stareCurenta);
                        cuvPart[hm[graf[hm[stareCurenta]][i].dest]] = nextCuv;
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

    in.close();
    out.close();

    return 0;
}
