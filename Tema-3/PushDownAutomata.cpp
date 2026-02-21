#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>

using namespace std;

/// Automat Push-Down Determinist, unde un cuvant este acceptat doar daca ajunge intr-o stare finala si stiva a fost vidata exact o data la starea imediat anterioara starii finale.

const int NR_STARI_MAX = 200000; /// numarul maxim de stari din graf

struct Muchie
{
    int y;
    char litera; /// litera de pe muchie

    char varfStiva; /// ce trebuie sa fie in varful stivei (la ce dam pop)
    string adaugaStiva; /// ce adaugam pe stiva

    Muchie() = default;

    Muchie(int y, char litera, char varfStiva, string adaugaStiva) :
        y(y), litera(litera), varfStiva(varfStiva), adaugaStiva(adaugaStiva) {};
};

int nrStari;

unordered_map<int, int> hm;

int nrMuchii;

vector<Muchie> graf[1 + NR_STARI_MAX];

int stareStart;

int nrStariFinale;

bool stareFinala[1 + NR_STARI_MAX];

char caracterSpecialStiva;

set<string> vizitat[1 + NR_STARI_MAX];

string conversieNumarInSir(int x)
{
    string sol = "";

    if (x < 0)
    {
        x = -x;
        sol += "-";
    }

    if (x == 0)
        sol += "0";

    vector<char> cifre;

    while (x > 0)
    {
        cifre.push_back((char)((x % 10) + '0'));
        x /= 10;
    }

    for (int i = (int)cifre.size() - 1; i >= 0; --i)
        sol.push_back(cifre[i]);

    return sol;
}

string sirStare(int stareCurenta, int indexInCuvant, const vector<char>& stiva)
{
    string sol = "";

    sol += conversieNumarInSir(stareCurenta);
    sol += "#";
    sol += conversieNumarInSir(indexInCuvant);
    sol += "#";

    for (int i = (int)stiva.size() - 1; i >= 0; --i)
        sol.push_back(stiva[i]);

    return sol;
}

bool cuvantInLimbaj(string& cuvant)
{
    for (int i = 1; i <= nrStari; ++i)
        vizitat[i].clear();

    int indexInCuvant = 0;

    int stareCurenta = stareStart;

    vector<char> stiva;
    stiva.emplace_back(caracterSpecialStiva);
    bool stivaVidata = false;

    if (cuvant == "L") /// cuvantul primit este cuvantul vid
    {
        indexInCuvant = 1;

        while (!stivaVidata)
        {
            bool tranzitieGasita = false;

            for (int i = 0; i < graf[hm[stareCurenta]].size() && !tranzitieGasita; ++i)
            {
                if (graf[hm[stareCurenta]][i].litera == 'L' && graf[hm[stareCurenta]][i].varfStiva == stiva.back() && vizitat[hm[graf[hm[stareCurenta]][i].y]].find(sirStare(stareCurenta, indexInCuvant, stiva)) == vizitat[hm[graf[hm[stareCurenta]][i].y]].end())
                {
                    vizitat[hm[graf[hm[stareCurenta]][i].y]].insert(sirStare(stareCurenta, indexInCuvant, stiva));

                    tranzitieGasita = true;

                    stiva.pop_back();

                    if (graf[hm[stareCurenta]][i].adaugaStiva != "L")
                        for (int j = (int)graf[hm[stareCurenta]][i].adaugaStiva.size() - 1; j >= 0; --j)
                            stiva.push_back(graf[hm[stareCurenta]][i].adaugaStiva[j]);

                    stareCurenta = graf[hm[stareCurenta]][i].y;

                    if (stiva.empty())
                        stivaVidata = true;
                }
            }

            if (!tranzitieGasita)
                break;
        }
    }
    else /// cuvantul primit nu este vid
    {
        while (!stivaVidata)
        {
            bool tranzitieGasita = false;

            if (indexInCuvant < cuvant.size()) /// tranzitii ce nu au lambda pe muchie
            {
                for (int i = 0; i < graf[hm[stareCurenta]].size() && !tranzitieGasita; ++i)
                {
                    if (graf[hm[stareCurenta]][i].litera != 'L' && graf[hm[stareCurenta]][i].varfStiva == stiva.back() && graf[hm[stareCurenta]][i].litera == cuvant[indexInCuvant] && vizitat[hm[graf[hm[stareCurenta]][i].y]].find(sirStare(stareCurenta, indexInCuvant, stiva)) == vizitat[hm[graf[hm[stareCurenta]][i].y]].end())
                    {
                        vizitat[hm[graf[hm[stareCurenta]][i].y]].insert(sirStare(stareCurenta, indexInCuvant, stiva));

                        ++indexInCuvant;

                        tranzitieGasita = true;

                        stiva.pop_back();

                        if (graf[hm[stareCurenta]][i].adaugaStiva != "L")
                            for (int j = (int)graf[hm[stareCurenta]][i].adaugaStiva.size() - 1; j >= 0; --j)
                                stiva.push_back(graf[hm[stareCurenta]][i].adaugaStiva[j]);

                        stareCurenta = graf[hm[stareCurenta]][i].y;

                        if (stiva.empty())
                            stivaVidata = true;
                    }
                }
            }

            for (int i = 0; i < graf[hm[stareCurenta]].size() && !tranzitieGasita; ++i) /// lambda-tranzitiile
            {
                if (graf[hm[stareCurenta]][i].litera == 'L' && graf[hm[stareCurenta]][i].varfStiva == stiva.back() && vizitat[hm[graf[hm[stareCurenta]][i].y]].find(sirStare(stareCurenta, indexInCuvant, stiva)) == vizitat[hm[graf[hm[stareCurenta]][i].y]].end())
                {
                    vizitat[hm[graf[hm[stareCurenta]][i].y]].insert(sirStare(stareCurenta, indexInCuvant, stiva));

                    tranzitieGasita = true;

                    stiva.pop_back();

                    if (graf[hm[stareCurenta]][i].adaugaStiva != "L")
                        for (int j = (int)graf[hm[stareCurenta]][i].adaugaStiva.size() - 1; j >= 0; --j)
                            stiva.push_back(graf[hm[stareCurenta]][i].adaugaStiva[j]);

                    stareCurenta = graf[hm[stareCurenta]][i].y;

                    if (stiva.empty())
                        stivaVidata = true;
                }
            }

            if (!tranzitieGasita)
                break;
        }
    }

    return stareFinala[hm[stareCurenta]] && stivaVidata && indexInCuvant == cuvant.size();
}

int main()
{
    ifstream in("input.txt");
    ofstream out("output.txt");

    in >> nrStari;

    for (int i = 1; i <= nrStari; ++i)
    {
        int stare;
        in >> stare;

        hm[stare] = i;
    }

    in >> caracterSpecialStiva;

    in >> nrMuchii;

    for (int j = 1; j <= nrMuchii; ++j)
    {
        int x, y;
        char litera;

        char varfStiva;
        string adaugaStiva;

        in >> x >> y >> litera >> varfStiva >> adaugaStiva;

        graf[hm[x]].emplace_back(y, litera, varfStiva, adaugaStiva);
    }

    in >> stareStart;

    in >> nrStariFinale;

    for (int i = 1; i <= nrStariFinale; ++i)
    {
        int stare;
        in >> stare;

        stareFinala[hm[stare]] = true;
    }

    int nrCuvinte;
    in >> nrCuvinte;

    for (int j = 1; j <= nrCuvinte; ++j)
    {
        string cuvant;
        in >> cuvant;

        if (cuvantInLimbaj(cuvant))
            out << "Cuvantul " << cuvant << " este in limbaj!" << '\n';
        else
            out << "Cuvantul " << cuvant << " NU este in limbaj!" << '\n';
    }

    in.close();
    out.close();

    return 0;
}


