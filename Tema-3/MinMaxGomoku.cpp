#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

/// Gomoku = X si 0 pe o tabla de 15x15. Castiga cine are primul 5 piese consecutive
/// pe linie/coloana sau pe diagonale (exact 5, nu mai multe).
/// Daca programului ii ia mult sa dea o mutare, micsorati ADANCIME_MINMAX. (Pare ca merge ok pe ADANCIME_MINMAX = 3.)
/// ADANCIME_MINMAX mai mare, programul joaca mai bine, dar ii ia mai mult sa faca o mutare.
/// Am implementat si alpha-beta pruning.

const int lungimeTablaJoc = 15;
const int latimeTablaJoc = 15;

const int ADANCIME_MINMAX = 3;

const int INFINIT = 1e9 + 1;

int numarCeluleLibere = lungimeTablaJoc * latimeTablaJoc;

int tablaJoc[1 + lungimeTablaJoc + 1][1 + latimeTablaJoc + 1];

int IDjucator;

bool randJucator;

struct Mutare
{
    int linie;
    int coloana;

    int scor;

    Mutare() = default;

    Mutare(int linie, int coloana, int scor) :
        linie(linie), coloana(coloana), scor(scor) {};
};

void afiseazaTablaJoc()
{
    for (int j = 0; j <= latimeTablaJoc; ++j)
    {
        cout << j << ' ';
        if (j <= 9)
            cout << ' ';
    }
    cout << '\n';

    for (int i = 1; i <= lungimeTablaJoc; ++i)
    {
        cout << i << ' ';
        if (i <= 9)
            cout << ' ';

        for (int j = 1; j <= latimeTablaJoc; ++j)
        {
            if (tablaJoc[i][j] == -1) /// negru
            {
                cout << 'N' << ' ' << ' ';
            }
            else if (tablaJoc[i][j] == 1) /// alb
            {
                cout << 'A' << ' ' << ' ';
            }
            else
            {
                cout << '.' << ' ' << ' ';
            }
        }
        cout << '\n';
    }
}

int castigatorJoc()
{
    for (int i = 1; i <= lungimeTablaJoc; ++i)
    {
        for (int j = 1; j <= latimeTablaJoc; ++j)
        {
            if (tablaJoc[i][j] == IDjucator)
            {
                /// linie
                if (tablaJoc[i][j - 1] != IDjucator && j + 4 <= latimeTablaJoc && tablaJoc[i][j + 5] != IDjucator &&
                    tablaJoc[i][j + 1] == IDjucator && tablaJoc[i][j + 2] == IDjucator && tablaJoc[i][j + 3] == IDjucator && tablaJoc[i][j + 4] == IDjucator)
                    return IDjucator;

                /// coloana
                if (tablaJoc[i - 1][j] != IDjucator && i + 4 <= lungimeTablaJoc && tablaJoc[i + 5][j] != IDjucator &&
                    tablaJoc[i + 1][j] == IDjucator && tablaJoc[i + 2][j] == IDjucator && tablaJoc[i + 3][j] == IDjucator && tablaJoc[i + 4][j] == IDjucator)
                    return IDjucator;

                /// diagonala 1
                if (tablaJoc[i - 1][j - 1] != IDjucator && i + 4 <= lungimeTablaJoc && j + 4 <= latimeTablaJoc && tablaJoc[i + 5][j + 5] != IDjucator &&
                    tablaJoc[i + 1][j + 1] == IDjucator && tablaJoc[i + 2][j + 2] == IDjucator && tablaJoc[i + 3][j + 3] == IDjucator && tablaJoc[i + 4][j + 4] == IDjucator)
                    return IDjucator;

                /// diagonala 2
                if (tablaJoc[i + 1][j - 1] != IDjucator && i - 4 >= 1 && j + 4 <= latimeTablaJoc && tablaJoc[i - 5][j + 5] != IDjucator &&
                    tablaJoc[i - 1][j + 1] == IDjucator && tablaJoc[i - 2][j + 2] == IDjucator && tablaJoc[i - 3][j + 3] == IDjucator && tablaJoc[i - 4][j + 4] == IDjucator)
                    return IDjucator;
            }
            else if (tablaJoc[i][j] == -IDjucator)
            {
                /// linie
                if (tablaJoc[i][j - 1] != -IDjucator && j + 4 <= latimeTablaJoc && tablaJoc[i][j + 5] != -IDjucator &&
                    tablaJoc[i][j + 1] == -IDjucator && tablaJoc[i][j + 2] == -IDjucator && tablaJoc[i][j + 3] == -IDjucator && tablaJoc[i][j + 4] == -IDjucator)
                    return -IDjucator;

                /// coloana
                if (tablaJoc[i - 1][j] != -IDjucator && i + 4 <= lungimeTablaJoc && tablaJoc[i + 5][j] != -IDjucator &&
                    tablaJoc[i + 1][j] == -IDjucator && tablaJoc[i + 2][j] == -IDjucator && tablaJoc[i + 3][j] == -IDjucator && tablaJoc[i + 4][j] == -IDjucator)
                    return -IDjucator;

                /// diagonala 1
                if (tablaJoc[i - 1][j - 1] != -IDjucator && i + 4 <= lungimeTablaJoc && j + 4 <= latimeTablaJoc && tablaJoc[i + 5][j + 5] != -IDjucator &&
                    tablaJoc[i + 1][j + 1] == -IDjucator && tablaJoc[i + 2][j + 2] == -IDjucator && tablaJoc[i + 3][j + 3] == -IDjucator && tablaJoc[i + 4][j + 4] == -IDjucator)
                    return -IDjucator;

                /// diagonala 2
                if (tablaJoc[i + 1][j - 1] != -IDjucator && i - 4 >= 1 && j + 4 <= latimeTablaJoc && tablaJoc[i - 5][j + 5] != -IDjucator &&
                    tablaJoc[i - 1][j + 1] == -IDjucator && tablaJoc[i - 2][j + 2] == -IDjucator && tablaJoc[i - 3][j + 3] == -IDjucator && tablaJoc[i - 4][j + 4] == -IDjucator)
                    return -IDjucator;
            }
        }
    }

    if (numarCeluleLibere == 0)
        return 2; /// remiza

    return 0;
}

void dateDeIntrareJucator()
{
    cout << "Introduceti linia si coloana unde plasati (indexare de la 1)" << '\n';

    int linie, coloana;
    cin >> linie >> coloana;

    while(!(1 <= linie && linie <= lungimeTablaJoc &&
        1 <= coloana && coloana <= latimeTablaJoc &&
        tablaJoc[linie][coloana] == 0))
    {
        cout << "Coordonate invalide. Introduceti linia si coloana unde plasati (indexare de la 1)" << '\n';

        cin >> linie >> coloana;
    }

    tablaJoc[linie][coloana] = IDjucator;

    --numarCeluleLibere;
}

int atribuieScorConfiguratie() /// Functia euristica, ce atribute un scor aproximativ starii frunza din algoritmul MinMax
{
    int scorConfiguratie = 0;

    for (int i = 1; i <= lungimeTablaJoc; ++i)
    {
        for (int j = 1; j <= latimeTablaJoc; ++j)
        {
            if (tablaJoc[i][j - 1] != IDjucator && j + 4 <= latimeTablaJoc && tablaJoc[i][j + 5] != IDjucator &&
                tablaJoc[i][j] != -IDjucator && tablaJoc[i][j + 1] != -IDjucator && tablaJoc[i][j + 2] != -IDjucator && tablaJoc[i][j + 3] != -IDjucator && tablaJoc[i][j + 4] != -IDjucator)
            {
                int scorCurent = 0;
                if (tablaJoc[i][j] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i][j + 1] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i][j + 2] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i][j + 3] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i][j + 4] == IDjucator)
                    ++scorCurent;

                scorConfiguratie += scorCurent * scorCurent;
            }

            if (tablaJoc[i - 1][j] != IDjucator && i + 4 <= lungimeTablaJoc && tablaJoc[i + 5][j] != IDjucator &&
                tablaJoc[i][j] != -IDjucator && tablaJoc[i + 1][j] != -IDjucator && tablaJoc[i + 2][j] != -IDjucator && tablaJoc[i + 3][j] != -IDjucator && tablaJoc[i + 4][j] != -IDjucator)
            {
                int scorCurent = 0;
                if (tablaJoc[i][j] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 1][j] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 2][j] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 3][j] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 4][j] == IDjucator)
                    ++scorCurent;

                scorConfiguratie += scorCurent * scorCurent;
            }

            if (tablaJoc[i - 1][j - 1] != IDjucator && i + 4 <= lungimeTablaJoc && j + 4 <= latimeTablaJoc && tablaJoc[i + 5][j + 5] != IDjucator &&
                tablaJoc[i][j] != -IDjucator && tablaJoc[i + 1][j + 1] != -IDjucator && tablaJoc[i + 2][j + 2] != -IDjucator && tablaJoc[i + 3][j + 3] != -IDjucator && tablaJoc[i + 4][j + 4] != -IDjucator)
            {
                int scorCurent = 0;
                if (tablaJoc[i][j] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 1][j + 1] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 2][j + 2] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 3][j + 3] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 4][j + 4] == IDjucator)
                    ++scorCurent;

                scorConfiguratie += scorCurent * scorCurent;
            }

            if (tablaJoc[i + 1][j - 1] != IDjucator && i - 4 >= 1 && j + 4 <= latimeTablaJoc && tablaJoc[i - 5][j + 5] != IDjucator &&
                tablaJoc[i][j] != -IDjucator && tablaJoc[i - 1][j + 1] != -IDjucator && tablaJoc[i - 2][j + 2] != -IDjucator && tablaJoc[i - 3][j + 3] != -IDjucator && tablaJoc[i - 4][j + 4] != -IDjucator)
            {
                int scorCurent = 0;
                if (tablaJoc[i][j] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i - 1][j + 1] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i - 2][j + 2] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i - 3][j + 3] == IDjucator)
                    ++scorCurent;
                if (tablaJoc[i - 4][j + 4] == IDjucator)
                    ++scorCurent;

                scorConfiguratie += scorCurent * scorCurent;
            }

            if (tablaJoc[i][j - 1] != -IDjucator && j + 4 <= latimeTablaJoc && tablaJoc[i][j + 5] != -IDjucator &&
                tablaJoc[i][j] != IDjucator && tablaJoc[i][j + 1] != IDjucator && tablaJoc[i][j + 2] != IDjucator && tablaJoc[i][j + 3] != IDjucator && tablaJoc[i][j + 4] != IDjucator)
            {
                int scorCurent = 0;
                if (tablaJoc[i][j] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i][j + 1] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i][j + 2] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i][j + 3] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i][j + 4] == -IDjucator)
                    ++scorCurent;

                scorConfiguratie -= scorCurent * scorCurent;
            }

            if (tablaJoc[i - 1][j] != -IDjucator && i + 4 <= lungimeTablaJoc && tablaJoc[i + 5][j] != -IDjucator &&
                tablaJoc[i][j] != IDjucator && tablaJoc[i + 1][j] != IDjucator && tablaJoc[i + 2][j] != IDjucator && tablaJoc[i + 3][j] != IDjucator && tablaJoc[i + 4][j] != IDjucator)
            {
                int scorCurent = 0;
                if (tablaJoc[i][j] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 1][j] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 2][j] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 3][j] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 4][j] == -IDjucator)
                    ++scorCurent;

                scorConfiguratie -= scorCurent * scorCurent;
            }

            if (tablaJoc[i - 1][j - 1] != -IDjucator && i + 4 <= lungimeTablaJoc && j + 4 <= latimeTablaJoc && tablaJoc[i + 5][j + 5] != -IDjucator &&
                tablaJoc[i][j] != IDjucator && tablaJoc[i + 1][j + 1] != IDjucator && tablaJoc[i + 2][j + 2] != IDjucator && tablaJoc[i + 3][j + 3] != IDjucator && tablaJoc[i + 4][j + 4] != IDjucator)
            {
                int scorCurent = 0;
                if (tablaJoc[i][j] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 1][j + 1] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 2][j + 2] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 3][j + 3] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i + 4][j + 4] == -IDjucator)
                    ++scorCurent;

                scorConfiguratie -= scorCurent * scorCurent;
            }

            if (tablaJoc[i + 1][j - 1] != -IDjucator && i - 4 >= 1 && j + 4 <= latimeTablaJoc && tablaJoc[i - 5][j + 5] != -IDjucator &&
                tablaJoc[i][j] != IDjucator && tablaJoc[i - 1][j + 1] != IDjucator && tablaJoc[i - 2][j + 2] != IDjucator && tablaJoc[i - 3][j + 3] != IDjucator && tablaJoc[i - 4][j + 4] != IDjucator)
            {
                int scorCurent = 0;
                if (tablaJoc[i][j] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i - 1][j + 1] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i - 2][j + 2] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i - 3][j + 3] == -IDjucator)
                    ++scorCurent;
                if (tablaJoc[i - 4][j + 4] == -IDjucator)
                    ++scorCurent;

                scorConfiguratie -= scorCurent * scorCurent;
            }
        }
    }

    return scorConfiguratie;
}

Mutare solutieMinMax(int ID, int adancime, int alfa, int beta) /// Player maximizeaza scorul, programul minimizeaza scorul.
{
    int castigator = castigatorJoc();

    if (castigator == 2)
        return Mutare(-1, -1, 0);
    else if (castigator == IDjucator)
        return Mutare(-1, -1, INFINIT);
    else if (castigator == -IDjucator)
        return Mutare(-1, -1, -INFINIT);

    if (adancime == ADANCIME_MINMAX)
        return Mutare(-1, -1, atribuieScorConfiguratie());

    if (ID == IDjucator) /// maximizam
    {
        int maxim = -INFINIT;
        int linie = -1;
        int coloana = -1;

        bool iesireFor = false;

        for (int i = 1; i <= lungimeTablaJoc && !iesireFor; ++i)
        {
            for (int j = 1; j <= latimeTablaJoc && !iesireFor; ++j)
            {
                if (tablaJoc[i][j] == 0)
                {
                    tablaJoc[i][j] = ID;
                    --numarCeluleLibere;

                    Mutare mutare = solutieMinMax(-ID, adancime + 1, alfa, beta);

                    ++numarCeluleLibere;
                    tablaJoc[i][j] = 0;

                    if (mutare.scor > maxim)
                    {
                        maxim = mutare.scor;
                        linie = i;
                        coloana = j;
                    }

                    beta = max(beta, mutare.scor);

                    if (alfa <= beta)
                        iesireFor = true;
                }
            }
        }

        return Mutare(linie, coloana, maxim);
    }
    else /// minimizam
    {
        int minim = INFINIT;
        int linie = -1;
        int coloana = -1;

        bool iesireFor = false;

        for (int i = 1; i <= lungimeTablaJoc && !iesireFor; ++i)
        {
            for (int j = 1; j <= latimeTablaJoc && !iesireFor; ++j)
            {
                if (tablaJoc[i][j] == 0)
                {
                    tablaJoc[i][j] = ID;
                    --numarCeluleLibere;

                    Mutare mutare = solutieMinMax(-ID, adancime + 1, alfa, beta);

                    ++numarCeluleLibere;
                    tablaJoc[i][j] = 0;

                    if (mutare.scor < minim)
                    {
                        minim = mutare.scor;
                        linie = i;
                        coloana = j;
                    }

                    alfa = min(alfa, mutare.scor);

                    if (alfa <= beta)
                        iesireFor = true;
                }
            }
        }
        return Mutare(linie, coloana, minim);
    }
}

int main()
{
    cout << "Ce culoare este jucatorul? 1 pentru alb, -1 pentru negru (negru mereu incepe jocul)" << '\n';
    cin >> IDjucator;
    while (IDjucator != 1 && IDjucator != -1)
    {
        cout << "Cod invalid. Introduceti 1 pentru alb, -1 pentru negru (negru mereu incepe jocul)" << '\n';
        cin >> IDjucator;
    }

    if (IDjucator == -1)
        randJucator = true;
    else
        randJucator = false;

    system("cls");

    while (castigatorJoc() == 0)
    {
        afiseazaTablaJoc();

        if (randJucator)
            dateDeIntrareJucator();
        else
        {
            Mutare solutie = solutieMinMax(-IDjucator, 1, +INFINIT, -INFINIT);

            tablaJoc[solutie.linie][solutie.coloana] = -IDjucator; /// adica IDprogram

            --numarCeluleLibere;
        }

        randJucator = !randJucator;

        system("cls");
    }

    afiseazaTablaJoc();

    if (castigatorJoc() == 2) /// remiza
        cout << "Remiza!" << '\n';
    else if (castigatorJoc() == IDjucator)
        cout << "Utilizatorul a castigat!" << '\n';
    else
        cout << "Programul a castigat!" << '\n';

    return 0;
}

