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

vector<int> lantSol;

int main()
{
    ifstream in("input.txt");
    ofstream out("output.txt");

    in >> n;
    for (int i = 1; i <= n; i++)
        in >> stare[i];

    for (int i = 1; i <= n; i++) ///Am hash-uit cele n stari la primele n numere, [1, 2, ..., n].
        hm[stare[i]] = i;
    ///In cod ne vom referi la stare[i] ca fiind hash-ul sau, hm[stare[i]].

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
        int indexInCuv = 0;
        
        lantSol.clear();

        int stareCurenta = S;
        
        lantSol.push_back(stareCurenta);

        while (indexInCuv < cuv.size())
        {
            bool muchieGasita = false;
            
            for (int i = 0; i < graf[hm[stareCurenta]].size(); i++) ///Cautam tranzitia corecta din starea in care ne aflam.
            {
                ///Automatul este determinist => exista maxim o muchie cu o litera x ce pleaca dintr-un nod.
                if (graf[hm[stareCurenta]][i].lit == cuv[indexInCuv])
                {
                    stareCurenta = graf[hm[stareCurenta]][i].dest;
                    muchieGasita = true;
                    lantSol.push_back(stareCurenta);
                    break;
                }
            }
            
            if (!muchieGasita)
            {
                stareCurenta = -1;
                break;
            }

            indexInCuv++;
        }

        bool cuvantAcceptat = false;

        for (int i = 1; i <= nrF; i++) ///Verificam daca ultima stare in care am ajuns cu acest cuvant este una finala.
        {
            if (stareCurenta == stareFin[i])
            {
                out << "DA\n";
                cuvantAcceptat = true;
                break;
            }
        }

        if (!cuvantAcceptat)
            out << "NU\n";
            
        if (cuvantAcceptat)
        {
            out << "Lantul solutiei:\n";
            for (int i = 0; i < lantSol.size(); i++)
                out << lantSol[i] << ' ';
            out << '\n';
        }
    }

    in.close();
    out.close();

    return 0;
}

