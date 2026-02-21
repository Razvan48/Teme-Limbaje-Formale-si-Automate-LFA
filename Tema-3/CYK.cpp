#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

const int SIGMA = 26;

const int LMAX = 1000;

string linie;
string linieSimplificata;

vector<string> productii[SIGMA];

string CYK[1 + LMAX][1 + LMAX];

string cuvant;

string multimeStanga;
string multimeDreapta;

void eliminaDuplicate(string& s)
{
    if (s.empty())
        return;

    sort(s.begin(), s.end());

    for (int i = 1; i < s.size(); ++i)
        if (s[i] == s[i - 1])
            s[i - 1] = '#';

    int lungimeNoua = 0;

    for (int i = 0; i < s.size(); ++i)
    {
        if (s[i] != '#')
        {
            swap(s[lungimeNoua], s[i]);
            ++lungimeNoua;
        }
    }

    s.resize(lungimeNoua);
}

int main()
{
    ifstream in("input.txt");
    ofstream out("output.txt");

    int nrLinii;
    in >> nrLinii;

    in.get();

    for (int i = 1; i <= nrLinii; ++i)
    {
        getline(in, linie);

        linieSimplificata = "";

        for (int j = 0; j < linie.size(); ++j)
        {
            if (linie[j] != ' ' && linie[j] != '\n')
            {
                linieSimplificata.push_back(linie[j]);
            }
        }

        for (int j = 1; j < linieSimplificata.size(); ++j)
        {
            if (j < linieSimplificata.size() && (('A' <= linieSimplificata[j] && linieSimplificata[j] <= 'Z')
                || ('a' <= linieSimplificata[j] && linieSimplificata[j] <= 'z')))
            {
                productii[linieSimplificata[0] - 'A'].push_back("");

                while (j < linieSimplificata.size() && (('A' <= linieSimplificata[j] && linieSimplificata[j] <= 'Z')
                    || ('a' <= linieSimplificata[j] && linieSimplificata[j] <= 'z')))
                {
                    productii[linieSimplificata[0] - 'A'].back().push_back(linieSimplificata[j]);
                    ++j;
                }

                --j;
            }
        }
    }

    char stareStart;
    in >> stareStart;

    int nrCuvinte;
    in >> nrCuvinte;

    for (int k = 1; k <= nrCuvinte; ++k)
    {
        in >> cuvant;

        for (int i = 1; i <= cuvant.size(); ++i)
            for (int j = 1; j <= i; ++j)
                CYK[i][j] = "";

        /// Linia de jos din matricea algoritmului CYK
        for (int j = 1; j <= cuvant.size(); ++j)
        {
            for (char nonTerminal = 'A'; nonTerminal <= 'Z'; ++nonTerminal)
            {
                bool apareInProductii = false;

                for (int l = 0; l < productii[nonTerminal - 'A'].size() && !apareInProductii; ++l)
                {
                    if (productii[nonTerminal - 'A'][l].size() == 1 &&
                        productii[nonTerminal - 'A'][l][0] == cuvant[j - 1])
                            apareInProductii = true;
                }

                if (apareInProductii)
                    CYK[cuvant.size()][j].push_back(nonTerminal);
            }

            eliminaDuplicate(CYK[cuvant.size()][j]);
        }

        /// Restul liniilor din matricea algoritmului CYK
        for (int linie = (int)cuvant.size() - 1; linie >= 1; --linie)
        {
            int lungimeSubSecventa = (int)cuvant.size() - linie + 1;

            for (int coloana = 1; coloana <= linie; ++coloana)
            {
                for (int mijloc = coloana; mijloc <= coloana + lungimeSubSecventa - 1 - 1; ++mijloc)
                {
                    int lungimeSubSecventaStanga = mijloc - coloana + 1;
                    int lungimeSubSecventaDreapta = (coloana + lungimeSubSecventa - 1) - (mijloc + 1) + 1;

                    int linieSubSecventaStanga = (int)cuvant.size() - lungimeSubSecventaStanga + 1;
                    int linieSubSecventaDreapta = (int)cuvant.size() - lungimeSubSecventaDreapta + 1;

                    multimeStanga = CYK[linieSubSecventaStanga][coloana];
                    multimeDreapta = CYK[linieSubSecventaDreapta][mijloc + 1];

                    for (int i = 0; i < multimeStanga.size(); ++i)
                    {
                        for (int j = 0; j < multimeDreapta.size(); ++j)
                        {
                            for (char nonTerminal = 'A'; nonTerminal <= 'Z'; ++nonTerminal)
                            {
                                bool apareInProductii = false;

                                for (int l = 0; l < productii[nonTerminal - 'A'].size() && !apareInProductii; ++l)
                                {
                                    if (productii[nonTerminal - 'A'][l].size() == 2 &&
                                        productii[nonTerminal - 'A'][l][0] == multimeStanga[i] &&
                                        productii[nonTerminal - 'A'][l][1] == multimeDreapta[j])
                                            apareInProductii = true;
                                }

                                if (apareInProductii)
                                    CYK[linie][coloana].push_back(nonTerminal);
                            }
                        }
                    }
                }

                if (linie == (int)cuvant.size() - 1) /// Linia a doua de jos in sus (indexare de la 1)
                {
                    for (char nonTerminal = 'A'; nonTerminal <= 'Z'; ++nonTerminal)
                    {
                        bool apareInProductii = false;

                        for (int l = 0; l < productii[nonTerminal - 'A'].size() && !apareInProductii; ++l)
                        {
                            if (productii[nonTerminal - 'A'][l].size() == 2 &&
                                productii[nonTerminal - 'A'][l][0] == cuvant[coloana - 1] &&
                                productii[nonTerminal - 'A'][l][1] == cuvant[coloana])
                                    apareInProductii = true;
                        }

                        if (apareInProductii)
                            CYK[linie][coloana].push_back(nonTerminal);
                    }
                }

                eliminaDuplicate(CYK[linie][coloana]);
            }
        }

        bool stareStartInclusa = false;
        for (int i = 0; i < CYK[1][1].size() && !stareStartInclusa; ++i)
            if (CYK[1][1][i] == stareStart)
                stareStartInclusa = true;

        if (stareStartInclusa)
            out << "Cuvantul " << cuvant << " poate fi generat de gramatica data!" << '\n';
        else
            out << "Cuvantul " << cuvant << " NU poate fi generat de gramatica data!" << '\n';

        /// Liniile de mai jos pot fi decomentate pentru a vedea matricea algoritmului.
        /*
        out << "DEBUG" << '\n';

        for (int i = 1; i <= cuvant.size(); ++i)
        {
            for (int j = 1; j <= i; ++j)
            {
                out << CYK[i][j] << "  #  ";
            }
            out << '\n';
        }
        */
    }

    in.close();
    out.close();

    return 0;
}


