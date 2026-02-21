#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <istream>
#include <ostream>
#include <queue>
#include <algorithm>
#include <string>

using namespace std;

struct Muchie
{
    int y;
    char l;

    Muchie() = default;

    Muchie(int y, char l) : y(y), l(l)
    {

    }
};

class Automat
{
private:

protected:

    static const int NMAX = 50000;

    int n;
    int stare[1 + Automat::NMAX];
    unordered_map<int, int> hm;
    int m;
    vector<Muchie> graf[1 + Automat::NMAX];
    int s;
    int nrF;
    int stareFinala[1 + Automat::NMAX];

public:

    Automat() : n(0)
    {

    }

    friend istream& operator >> (istream& in, Automat& a);
    friend ostream& operator << (ostream& out, Automat& a);
};

istream& operator >> (istream& in, Automat& a)
{
    a.hm.clear();
    for (int i = 1; i <= a.n; i++)
        a.graf[i].clear();

    in >> a.n;

    for (int i = 1; i <= a.n; i++)
    {
        in >> a.stare[i];
        a.hm[a.stare[i]] = i;
    }

    in >> a.m;

    for (int i = 1; i <= a.m; i++)
    {
        int x, y;
        char l;

        in >> x >> y >> l;

        a.graf[a.hm[x]].emplace_back(y, l);
    }

    in >> a.s;

    in >> a.nrF;

    for (int i = 1; i <= a.nrF; i++)
    {
        in >> a.stareFinala[i];
    }

    return in;
}

ostream& operator << (ostream& out, Automat& a)
{
    out << a.n << '\n';

    for (int i = 1; i <= a.n; i++)
    {
        out << a.stare[i] << ' ';
    }
    out << '\n';

    out << '\n';

    out << a.m << '\n';

    for (int i = 1; i <= a.n; i++)
    {
        for (int j = 0; j < a.graf[a.hm[a.stare[i]]].size(); j++)
        {
            out << a.stare[i] << ' ';
            out << a.graf[a.hm[a.stare[i]]][j].y << ' ';
            out << a.graf[a.hm[a.stare[i]]][j].l << '\n';
        }
    }

    out << '\n';

    out << a.s << '\n';

    out << '\n';

    out << a.nrF << '\n';

    for (int i = 1; i <= a.nrF; i++)
    {
        out << a.stareFinala[i] << ' ';
    }

    out << '\n';

    return out;
}

class NFA;

class DFA : public Automat
{
private:

    bool abort;
    bool vizitat[1 + Automat::NMAX];

    void dfs(int nod);
    void eliminaNod(int nod);
    bool esteNodFinal(int nod) const;

    vector<Muchie> aux[1 + Automat::NMAX];

    void unificaNoduri(int a, int b);
    void eliminaMuchiiDuplicate(int nod);

    vector<vector<int>> partitie;
    vector<vector<int>> auxiliar;

    void completeaza(const vector<char>& alfabet);

    bool suntNoduriEchivalente(int a, int b, const vector<char>& alfabet);

    void eliminaNoduriInutile();

protected:

public:

    DFA() = default;

    bool accepta(const string& s);
    void minimizeaza();

    void copiazaNFA(const NFA& b);

    ~DFA() = default;
};

bool DFA::accepta(const string& str)
{
    int index = 0;
    int stareCurenta = this->s;
    this->abort = false;

    while (index < str.size() && !this->abort)
    {
        bool muchieGasita = false;

        for (int i = 0; i < this->graf[this->hm[stareCurenta]].size() && !muchieGasita; i++)
        {
            if (this->graf[this->hm[stareCurenta]][i].l == str[index])
            {
                muchieGasita = true;
                stareCurenta = this->graf[this->hm[stareCurenta]][i].y;
            }
        }

        if (!muchieGasita)
        {
            this->abort = true;
        }

        index++;
    }

    if (this->abort)
    {
        return false;
    }
    else
    {
        bool acceptat = false;

        for (int i = 1; i <= this->nrF && !acceptat; i++)
        {
            if (stareCurenta == this->stareFinala[i])
            {
                acceptat = true;
            }
        }

        return acceptat;
    }
}

void DFA::eliminaNod(int nod)
{
    for (int i = 1; i <= this->n; i++)
    {
        for (int j = 0; j < this->graf[this->hm[this->stare[i]]].size(); j++)
        {
            if (this->graf[this->hm[this->stare[i]]][j].y == nod)
            {
                swap(this->graf[this->hm[this->stare[i]]][j],
                     this->graf[this->hm[this->stare[i]]].back()
                     );
                this->graf[this->hm[this->stare[i]]].pop_back();
                j--;

                this->m--;
            }
        }
    }

    for (int i = 1; i <= this->nrF; i++)
    {
        if (this->stareFinala[i] == nod)
        {
            swap(this->stareFinala[i], this->stareFinala[this->nrF]);
            this->nrF--;
            i--;
        }
    }

    for (int i = 1; i <= this->n; i++)
    {
        if (this->stare[i] == nod)
        {
            this->m -= this->graf[this->hm[this->stare[i]]].size();
            this->graf[this->hm[this->stare[i]]].clear();
            swap(this->stare[i], this->stare[this->n]);
            this->n--;
            i--;
        }
    }

    for (int i = 1; i <= this->n; i++)
    {
        this->aux[i].clear();
    }

    for (int i = 1; i <= this->n; i++)
    {
        for (int j = 0; j < this->graf[this->hm[this->stare[i]]].size(); j++)
        {
            this->aux[i].emplace_back(this->graf[this->hm[this->stare[i]]][j].y,
                                      this->graf[this->hm[this->stare[i]]][j].l
                                      );
        }
        this->graf[this->hm[this->stare[i]]].clear();
    }

    for (int i = 1; i <= this->n; i++)
    {
        for (int j = 0; j < this->aux[i].size(); j++)
        {
            this->graf[i].emplace_back(this->aux[i][j].y, this->aux[i][j].l);
        }
    }

    this->hm.clear();
    for (int i = 1; i <= this->n; i++)
    {
        this->hm[this->stare[i]] = i;
    }
}

void DFA::dfs(int nod)
{
    this->vizitat[this->hm[nod]] = true;

    for (int i = 0; i < this->graf[this->hm[nod]].size(); i++)
    {
        if (!this->vizitat[this->hm[this->graf[this->hm[nod]][i].y]])
        {
            this->dfs(this->graf[this->hm[nod]][i].y);
        }
    }
}

void DFA::eliminaMuchiiDuplicate(int nod)
{
    for (int i = this->graf[this->hm[nod]].size() - 1; i >= 0; i--)
    {
        for (int j = i - 1; j >= 0; j--)
        {
            if (this->graf[this->hm[nod]][i].y == this->graf[this->hm[nod]][j].y &&
                this->graf[this->hm[nod]][i].l == this->graf[this->hm[nod]][j].l
                )
            {
                this->m--;
                swap(this->graf[this->hm[nod]][i], this->graf[this->hm[nod]].back());
                this->graf[this->hm[nod]].pop_back();
                break;
            }
        }
    }
}

void DFA::unificaNoduri(int a, int b)
{
    if (a == b)
        return;

    if (b == this->s) /// Daca merge-uim ceva ce implica nodul sursa, atunci nodul sursa nu trebuie pierdut.
        swap(a, b);

    /// Merge-uim pe b la a.

    bool vaFiStareFinala = this->esteNodFinal(a) | this->esteNodFinal(b);

    for (int i = 1; i <= this->n; i++)
    {
        for (int j = 0; j < this->graf[i].size(); j++)
        {
            if (this->graf[i][j].y == b)
            {
                this->m++;
                this->graf[i].emplace_back(a, this->graf[i][j].l);
            }
        }
    }

    for (int i = 1; i <= n; i++)
    {
        this->eliminaMuchiiDuplicate(this->stare[i]);
    }

    bool esteANodFinal = this->esteNodFinal(a);

    this->eliminaNod(b);

    if (vaFiStareFinala && !esteANodFinal)
    {
        this->nrF++;
        this->stareFinala[this->nrF] = a;
    }
}

bool DFA::esteNodFinal(int nod) const
{
    for (int i = 1; i <= this->nrF; i++)
    {
        if (nod == this->stareFinala[i])
        {
            return true;
        }
    }

    return false;
}

void DFA::completeaza(const vector<char>& alfabet)
{
    int nodNou;

    for (int i = 1; i <= this->n; i++)
    {
        nodNou = max(nodNou, this->stare[i]);
    }
    nodNou++;

    bool esteComplet = true;

    for (int i = 1; i <= this->n; i++)
    {
        if (this->graf[this->hm[this->stare[i]]].size() < alfabet.size() && esteComplet)
        {
            esteComplet = false;

            this->n++;
            this->stare[this->n] = nodNou;
            this->hm[this->stare[this->n]] = this->n;
        }

        if (!esteComplet)
        {
            for (int j = 0; j < alfabet.size(); j++)
            {
                bool literaGasita = false;

                for (int k = 0; k < this->graf[this->hm[this->stare[i]]].size() && !literaGasita; k++)
                {
                    if (this->graf[this->hm[this->stare[i]]][k].l == alfabet[j])
                    {
                        literaGasita = true;
                    }
                }

                if (!literaGasita)
                {
                    this->m++;

                    this->graf[this->hm[this->stare[i]]].emplace_back(this->stare[this->n], alfabet[j]);
                }
            }
        }
    }
}

bool DFA::suntNoduriEchivalente(int a, int b, const vector<char>& alfabet)
{
    for (int i = 0; i < alfabet.size(); i++)
    {
        int vecin1 = -1;
        int vecin2 = -1;

        for (int j = 0; j < this->graf[this->hm[a]].size() && vecin1 == -1; j++)
        {
            if (this->graf[this->hm[a]][j].l == alfabet[i])
            {
                vecin1 = this->graf[this->hm[a]][j].y;
            }
        }

        for (int j = 0; j < this->graf[this->hm[b]].size() && vecin2 == -1; j++)
        {
            if (this->graf[this->hm[b]][j].l == alfabet[i])
            {
                vecin2 = this->graf[this->hm[b]][j].y;
            }
        }

        int indexVecin1 = -1;
        int indexVecin2 = -1;

        for (int j = 0; j < this->partitie.size() && indexVecin1 == -1; j++)
        {
            for (int k = 0; k < this->partitie[j].size() && indexVecin1 == -1; k++)
            {
                if (this->partitie[j][k] == vecin1)
                {
                    indexVecin1 = j;
                }
            }
        }

        for (int j = 0; j < this->partitie.size() && indexVecin2 == -1; j++)
        {
            for (int k = 0; k < this->partitie[j].size() && indexVecin2 == -1; k++)
            {
                if (this->partitie[j][k] == vecin2)
                {
                    indexVecin2 = j;
                }
            }
        }

        if (indexVecin1 != indexVecin2)
            return false;
    }

    return true;
}

void DFA::eliminaNoduriInutile()
{
    for (int i = 1; i <= this->n; i++)
    {
        this->vizitat[i] = false;
    }

    this->dfs(this->s);

    vector<int> noduriCeVorFiEliminate;

    for (int i = 1; i <= this->n; i++)
    {
        if (!this->vizitat[i])
        {
            noduriCeVorFiEliminate.emplace_back(this->stare[i]); /// Eliminam nodurile izolate de sursa.
        }
    }

    for (int i = 0; i < noduriCeVorFiEliminate.size(); i++) /// Eliminam nodurile izolate de sursa.
        this->eliminaNod(noduriCeVorFiEliminate[i]);

    /// Eliminam si nodurile ce nu duc intr-o stare finala.

    noduriCeVorFiEliminate.clear();

    for (int i = 1; i <= this->n; i++)
    {
        if (this->stare[i] != this->s && !this->esteNodFinal(this->stare[i]))
        {
            for (int j = 1; j <= this->n; j++)
            {
                this->vizitat[j] = false;
            }

            this->dfs(this->stare[i]);

            bool atingeOStareFinala = false;

            for (int j = 1; j <= this->nrF && !atingeOStareFinala; j++)
            {
                if (this->vizitat[this->hm[this->stareFinala[j]]])
                {
                    atingeOStareFinala = true;
                }
            }

            if (!atingeOStareFinala)
            {
                noduriCeVorFiEliminate.emplace_back(this->stare[i]); /// Eliminam nodurile ce nu duc intr-o stare finala.
            }
        }
    }

    for (int i = 0; i < noduriCeVorFiEliminate.size(); i++) /// Eliminam nodurile ce nu duc intr-o stare finala.
        this->eliminaNod(noduriCeVorFiEliminate[i]);
}

void DFA::minimizeaza()
{
    this->eliminaNoduriInutile();

    /// Identificam alfabetul.

    vector<char> alfabet;

    for (int i = 1; i <= this->n; i++)
    {
        for (int j = 0; j < this->graf[this->hm[this->stare[i]]].size(); j++)
        {
            char litera = this->graf[this->hm[this->stare[i]]][j].l;

            bool dejaInAlfabet = false;

            for (int k = 0; k < alfabet.size() && !dejaInAlfabet; k++)
            {
                if (alfabet[k] == litera)
                {
                    dejaInAlfabet = true;
                }
            }

            if (!dejaInAlfabet)
            {
                alfabet.push_back(litera);
            }
        }
    }

    /// Ne asiguram ca DFA-ul este complet.
    this->completeaza(alfabet);

    /// Algoritmul in sine
    for (int i = 0; i < this->partitie.size(); i++)
        this->partitie[i].clear();
    this->partitie.clear();

    this->partitie.emplace_back();
    this->partitie.emplace_back();

    for (int i = 1; i <= this->n; i++)
    {
        if (this->esteNodFinal(this->stare[i]))
        {
            this->partitie[1].emplace_back(this->stare[i]);
        }
        else
        {
            this->partitie[0].emplace_back(this->stare[i]);
        }
    }

    if (this->partitie[1].empty())
        this->partitie.pop_back();
    if (this->partitie[0].empty())
    {
        swap(this->partitie[0], this->partitie.back());
        this->partitie.pop_back();
    }

    int numarulDePartitiiAnterior = -1;

    while (numarulDePartitiiAnterior != this->partitie.size())
    {
        for (int i = 0; i < this->auxiliar.size(); i++)
            this->auxiliar[i].clear();
        this->auxiliar.clear();

        numarulDePartitiiAnterior = this->partitie.size();

        for (int i = 0; i < this->partitie.size(); i++)
        {
            if (!this->partitie[i].empty())
            {
                vector<vector<int>> auxiliarSubMultime;

                auxiliarSubMultime.emplace_back();
                auxiliarSubMultime.back().emplace_back(this->partitie[i][0]);

                for (int j = 1; j < this->partitie[i].size(); j++)
                {
                    int indexApartenenta = -1;

                    for (int k = 0; k < auxiliarSubMultime.size() && indexApartenenta == -1; k++)
                    {
                        if (this->suntNoduriEchivalente(this->partitie[i][j], auxiliarSubMultime[k][0], alfabet))
                        {
                            indexApartenenta = k;
                        }
                    }

                    if (indexApartenenta == -1)
                    {
                        auxiliarSubMultime.emplace_back();
                        auxiliarSubMultime.back().emplace_back(partitie[i][j]);
                    }
                    else
                        auxiliarSubMultime[indexApartenenta].emplace_back(partitie[i][j]);
                }

                for (int j = 0; j < auxiliarSubMultime.size(); j++)
                {
                    if (!auxiliarSubMultime[j].empty())
                    {
                        this->auxiliar.emplace_back();

                        for (int k = 0; k < auxiliarSubMultime[j].size(); k++)
                        {
                            this->auxiliar.back().emplace_back(auxiliarSubMultime[j][k]);
                        }
                    }
                }
            }
        }

        for (int i = 0; i < this->partitie.size(); i++)
            this->partitie[i].clear();
        this->partitie.clear();

        for (int i = 0; i < this->auxiliar.size(); i++)
        {
            if (!this->auxiliar[i].empty())
            {
                this->partitie.emplace_back();

                for (int j = 0; j < this->auxiliar[i].size(); j++)
                {
                    this->partitie.back().emplace_back(this->auxiliar[i][j]);
                }
            }
        }
    }

    for (int i = 0; i < this->partitie.size(); i++)
    {
        for (int j = 1; j < this->partitie[i].size(); j++)
        {
            if (this->partitie[i][j] == this->s)
                swap(this->partitie[i][0], partitie[i][j]);

            this->unificaNoduri(this->partitie[i][0], this->partitie[i][j]);
        }
    }

    this->eliminaNoduriInutile();
}

struct SuperMuchie
{
    int y;
    char l;

    SuperMuchie() = default;

    SuperMuchie(int y, char l) : y(y), l(l)
    {

    }
};


struct SuperNod
{
    vector<int> noduri;

    vector<SuperMuchie> vecini;

    SuperNod() = default;

    bool operator == (SuperNod& b)
    {
        if (this->noduri.size() != b.noduri.size())
            return false;

        if (this->noduri.empty())
            return true;

        sort(this->noduri.begin(), this->noduri.end());
        sort(b.noduri.begin(), b.noduri.end());

        for (int i = 0; i < this->noduri.size(); i++)
        {
            if (this->noduri[i] != b.noduri[i])
            {
                return false;
            }
        }

        return true;
    }

    bool operator != (SuperNod& b)
    {
        if (this->noduri.size() != b.noduri.size())
            return true;

        if (this->noduri.empty())
            return false;

        sort(this->noduri.begin(), this->noduri.end());
        sort(b.noduri.begin(), b.noduri.end());

        for (int i = 0; i < this->noduri.size(); i++)
        {
            if (this->noduri[i] != b.noduri[i])
            {
                return true;
            }
        }

        return false;
    }

    SuperNod& operator = (const SuperNod& b)
    {
        this->noduri.clear();
        this->vecini.clear();

        for (int i = 0; i < b.noduri.size(); i++)
        {
            this->noduri.emplace_back(b.noduri[i]);
        }

        for (int i = 0; i < b.vecini.size(); i++)
        {
            this->vecini.emplace_back(b.vecini[i].y, b.vecini[i].l);
        }

        return *this;
    }

    void eliminaDuplicatele()
    {
        if (!this->noduri.empty())
        {
            sort(this->noduri.begin(), this->noduri.end());

            vector<int> aux;
            aux.emplace_back(this->noduri[0]);

            for (int i = 1; i < this->noduri.size(); i++)
            {
                if (this->noduri[i] != this->noduri[i - 1])
                {
                    aux.emplace_back(this->noduri[i]);
                }
            }

            this->noduri.clear();

            for (int i = 0; i < aux.size(); i++)
            {
                this->noduri.emplace_back(aux[i]);
            }
        }
    }
};

class NFA : public Automat
{
private:

    bool stareAtinsa[1 + Automat::NMAX];
    queue<pair<int, int>> coada;
    vector<SuperNod> superNoduri;
    queue<int> coadaDFA;

    friend class DFA;

protected:

public:

    NFA() = default;

    bool accepta(const string& s);
    void transformaInDFA();

    ~NFA() = default;
};

void DFA::copiazaNFA(const NFA& b)
{
    for (int i = 1; i <= this->n; i++)
    {
        this->graf[i].clear();
    }

    this->n = b.n;
    for (int i = 1; i <= this->n; i++)
    {
        this->stare[i] = b.stare[i];
    }

    for (int i = 1; i <= this->n; i++)
    {
        for (int j = 0; j < b.graf[i].size(); j++)
        {
            this->graf[i].emplace_back(b.graf[i][j].y, b.graf[i][j].l);
        }
    }

    hm.clear();
    for (int i = 1; i <= this->n; i++)
    {
        this->hm[this->stare[i]] = i;
    }

    this->m = b.m;

    this->s = b.s;

    this->nrF = b.nrF;

    for (int i = 1; i <= this->nrF; i++)
    {
        this->stareFinala[i] = b.stareFinala[i];
    }
}

bool NFA::accepta(const string& str)
{
    for (int i = 1; i <= this->n; i++)
        this->stareAtinsa[this->hm[this->stare[i]]] = false;

    this->coada.emplace(this->s, 0);

    while (!this->coada.empty())
    {
        int stareCurenta = this->coada.front().first;
        int index = this->coada.front().second;
        this->coada.pop();

        if (index == str.size())
        {
            this->stareAtinsa[this->hm[stareCurenta]] = true;
        }
        else
        {
            for (int i = 0; i < this->graf[this->hm[stareCurenta]].size(); i++)
            {
                if (this->graf[this->hm[stareCurenta]][i].l == str[index])
                {
                    this->coada.emplace(this->graf[this->hm[stareCurenta]][i].y, index + 1);
                }
            }
        }
    }

    bool acceptat = false;

    for (int i = 1; i <= this->nrF && !acceptat; i++)
    {
        if (this->stareAtinsa[this->hm[this->stareFinala[i]]])
        {
            acceptat = true;
        }
    }

    return acceptat;
}

void NFA::transformaInDFA()
{
    /// Identificam alfabetul.
    vector<char> alfabet;

    for (int i = 1; i <= this->n; i++)
    {
        for (int j = 0; j < this->graf[this->hm[this->stare[i]]].size(); j++)
        {
            char litera = this->graf[this->hm[this->stare[i]]][j].l;

            bool dejaInAlfabet = false;

            for (int k = 0; k < alfabet.size() && !dejaInAlfabet; k++)
            {
                if (alfabet[k] == litera)
                {
                    dejaInAlfabet = true;
                }
            }

            if (!dejaInAlfabet)
            {
                alfabet.push_back(litera);
            }
        }
    }

    /// Adaugam in coada superNodul format doar din starea initiala.

    this->superNoduri.clear();

    SuperNod superNodInitial;
    superNodInitial.noduri.push_back(this->s);
    this->superNoduri.push_back(superNodInitial);
    this->coadaDFA.push(0);

    /// Algoritmul in sine
    while (!this->coadaDFA.empty())
    {
        int superNod = this->coadaDFA.front();
        this->coadaDFA.pop();

        for (int i = 0; i < alfabet.size(); i++)
        {
            SuperNod superNodVecin;
            char literaCautata = alfabet[i];

            for (int j = 0; j < this->superNoduri[superNod].noduri.size(); j++)
            {
                int nodCrt = this->superNoduri[superNod].noduri[j];

                for (int k = 0; k < this->graf[this->hm[nodCrt]].size(); k++)
                {
                    if (this->graf[this->hm[nodCrt]][k].l == literaCautata)
                    {
                        superNodVecin.noduri.emplace_back(this->graf[this->hm[nodCrt]][k].y);
                    }
                }
            }

            superNodVecin.eliminaDuplicatele();

            bool superStareNoua = true;
            int IDvecin;

            for (int i = 0; i < this->superNoduri.size() && superStareNoua; i++)
            {
                if (this->superNoduri[i] == superNodVecin)
                {
                    IDvecin = i;
                    superStareNoua = false;
                }
            }

            if (superStareNoua)
            {
                this->superNoduri.push_back(superNodVecin);
                IDvecin = this->superNoduri.size() - 1;
                this->coadaDFA.push(IDvecin);
            }

            this->superNoduri[superNod].vecini.emplace_back(IDvecin, literaCautata);
        }
    }

    /// Modificam toata structura automatului acum.

    for (int i = 1; i <= n; i++)
    {
        this->graf[this->hm[this->stare[i]]].clear();
    }

    this->m = 0;

    for (int i = 0; i < this->superNoduri.size(); i++)
    {
        for (int j = 0; j < this->superNoduri[i].vecini.size(); j++)
        {
            this->m++;
            this->graf[i + 1].emplace_back(this->superNoduri[i].vecini[j].y, this->superNoduri[i].vecini[j].l);
        }
    }

    hm.clear();

    for (int i = 0; i < this->superNoduri.size(); i++)
    {
        hm[i] = i + 1;
    }

    this->s = 0;

    vector<int> noileStariFinale;

    for (int i = 0; i < this->superNoduri.size(); i++)
    {
        bool esteStareFinala = false;

        for (int j = 1; j <= this->nrF && !esteStareFinala; j++)
        {
            for (int k = 0; k < this->superNoduri[i].noduri.size() && !esteStareFinala; k++)
            {
                if (this->superNoduri[i].noduri[k] == this->stareFinala[j])
                {
                    esteStareFinala = true;
                }
            }
        }

        if (esteStareFinala)
        {
            noileStariFinale.emplace_back(i);
        }
    }

    this->nrF = noileStariFinale.size();

    for (int i = 1; i <= this->nrF; i++)
    {
        this->stareFinala[i] = noileStariFinale[i - 1];
    }

    this->n = this->superNoduri.size();

    for (int i = 1; i <= this->n; i++)
    {
        this->stare[i] = i - 1;
    }
}

int main()
{
    ifstream in("input.txt");
    ofstream out("output.txt");

    char tipCerinta = 'a'; /// 'a' pentru ca programul sa rezolve cerinta a) si 'b' pentru ca programul sa rezolve cerinta b)

    if (tipCerinta == 'a')
    {
        NFA* nfa = new NFA();
        in >> (*nfa);

        nfa->transformaInDFA();

        out << (*nfa) << '\n';

        delete nfa;
    }
    else
    {
        DFA* dfa = new DFA();
        in >> (*dfa);

        dfa->minimizeaza();

        out << (*dfa) << '\n';

        delete dfa;
    }

    in.close();
    out.close();

    return 0;
}


