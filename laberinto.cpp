#include <iostream>
#include <vector>
#include <functional>
#include <queue>    // para usar cola (queue)
#include <utility>  // para usar pair
#include <cstdlib> // rand
#include <ctime>   // time
#include <algorithm> // shuffle
#include <random>
#include <chrono>

using namespace std;

// Declaración de funciones (solo nombres por ahora)
void crearLaberinto(vector<vector<char>>& laberinto);
void mostrarLaberinto(const vector<vector<char>>& laberinto);
void resolverBFS(vector<vector<char>>& laberinto);
void reconstruirCamino(
    vector<vector<char>>& laberinto,
    vector<vector<pair<int, int>>>& padre,
    pair<int, int> inicio,
    pair<int, int> fin
);
void mostrarLaberintoResuelto(const vector<vector<char>>& laberinto);

int main(int argc, char* argv[]) {
    int filas, columnas;
    int fila_ingres, columna_ingres;

    filas = 11;
    columnas = 11;

    if (argc == 3) {
        fila_ingres = atoi(argv[1]);
        columna_ingres = atoi(argv[2]);
        if (fila_ingres%2==1 && columna_ingres%2==1) {
            filas = fila_ingres;
            columnas = columna_ingres;
        } else {
            cout << "Por favor, ingrese dimensiones impares\n";
            return 1;
        }
    } else if (argc != 1) {
        cout << "Uso: ./laberinto filas columnas\n";
        return 1;
    }

    vector<vector<char>> laberinto(filas, vector<char>(columnas));

    auto inicio_total = chrono::high_resolution_clock::now();

    auto inicio_gen = chrono::high_resolution_clock::now();
    crearLaberinto(laberinto);
    auto fin_gen = chrono::high_resolution_clock::now();

    mostrarLaberinto(laberinto);

    auto inicio_res = chrono::high_resolution_clock::now();
    resolverBFS(laberinto);
    auto fin_res = chrono::high_resolution_clock::now();

    mostrarLaberintoResuelto(laberinto);
    auto fin_total = chrono::high_resolution_clock::now();

    chrono::duration<double> tiempo_gen = fin_gen - inicio_gen;
    chrono::duration<double> tiempo_res = fin_res - inicio_res;
    chrono::duration<double> tiempo_total = fin_total - inicio_total;

    cout << "\n--- TIEMPOS ---\n";
    cout << "Generacion: " << tiempo_gen.count() << " s\n";
    cout << "Resolucion: " << tiempo_res.count() << " s\n";
    cout << "Total: " << tiempo_total.count() << " s\n";

    return 0;
}

void resolverBFS(vector<vector<char>>& laberinto) {
    int filas = laberinto.size();
    int columnas = laberinto[0].size();

    pair<int, int> inicio = {0, 0};
    pair<int, int> fin = {filas - 1, columnas - 1};

    queue<pair<int, int>> q;
    vector<vector<bool>> visitado(filas, vector<bool>(columnas, false));
    vector<vector<pair<int, int>>> padre(
        filas, vector<pair<int, int>>(columnas, {-1, -1})
    );

    q.push(inicio);
    visitado[0][0] = true;

    int dx[4] = {-1, 1, 0, 0}; // arriba, abajo
    int dy[4] = {0, 0, -1, 1}; // izquierda, derecha

    while (!q.empty()) {
        pair<int, int> actual = q.front();
        q.pop();

        int x = actual.first;
        int y = actual.second;

        if (x == fin.first && y == fin.second) {
            reconstruirCamino(laberinto, padre, inicio, fin);
            return;
        }

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx >= 0 && nx < filas &&
                ny >= 0 && ny < columnas &&
                !visitado[nx][ny] &&
                laberinto[nx][ny] != '#') {
                q.push({nx, ny});
                visitado[nx][ny] = true;
                padre[nx][ny] = {x, y};
            }
        }
    }

    cout << "No hay solucion" << endl;
}

void reconstruirCamino(vector<vector<char>>& laberinto, vector<vector<pair<int, int>>>& padre,
    pair<int, int> inicio, pair<int, int> fin) {
    pair<int, int> actual = fin;
    while (actual != inicio) {
        int x = actual.first;
        int y = actual.second;
        if (laberinto[x][y] != 'S' && laberinto[x][y] != 'E') {
            laberinto[x][y] = '.';
        }
        actual = padre[x][y];
    }
}

void crearLaberinto(vector<vector<char>>& laberinto) {
    int filas = laberinto.size();
    int columnas = laberinto[0].size();

    // Inicializar todo como muro
    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
            laberinto[i][j] = '#';

    // Semilla aleatoria
    random_device rd;
    mt19937 g(rd());

    // Función DFS (backtracking)
    function<void(int,int)> dfs = [&](int x, int y) {
        laberinto[x][y] = '*';

        // Direcciones
        vector<pair<int,int>> dirs = {{-1,0}, {1,0}, {0,-1}, {0,1}};

        // Mezclar direcciones (aleatoriedad)
        shuffle(dirs.begin(), dirs.end(), g);

        for (auto d : dirs) {
            int nx = x + d.first * 2;
            int ny = y + d.second * 2;
            if (nx >= 0 && nx < filas &&
                ny >= 0 && ny < columnas &&
                laberinto[nx][ny] == '#') {
                // Romper pared intermedia
                laberinto[x + d.first][y + d.second] = '*';
                dfs(nx, ny);
            }
        }
    };

    // Empezar desde (0,0)
    dfs(0,0);

    // Asegurar inicio y fin
    laberinto[0][0] = 'S';
    laberinto[filas-1][columnas-1] = 'E';
}

void mostrarLaberinto(const vector<vector<char>>& laberinto) {
    int filas = laberinto.size();
    int columnas = laberinto[0].size();
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++)
            cout << laberinto[i][j] << " ";
        cout << endl;
    }
}

void mostrarLaberintoResuelto(const vector<vector<char>>& laberinto) {
    cout << "\nLaberinto resuelto:\n";
    mostrarLaberinto(laberinto);
}
