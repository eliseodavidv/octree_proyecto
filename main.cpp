#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace std::chrono;

// =============================================================================
// CODIGOS ANSI PARA COLORES EN CONSOLA
// =============================================================================
// Los colores ANSI funcionan en terminales modernas (Windows Terminal, PowerShell 7+)
// Si no funcionan, simplemente se ignoran
void enableANSI() {
    // En Windows 10+ los colores ANSI funcionan por defecto en Windows Terminal
    // No necesitamos configuración especial
}

namespace Color {
    const string RESET   = "\033[0m";
    const string RED     = "\033[31m";
    const string GREEN   = "\033[32m";
    const string YELLOW  = "\033[33m";
    const string BLUE    = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN    = "\033[36m";
    const string WHITE   = "\033[37m";
    const string BOLD    = "\033[1m";
    const string BG_BLUE = "\033[44m";
}

// =============================================================================
// PARAMETROS DE CONFIGURACION DEL OCTREE
// =============================================================================
const int MAX_DEPTH = 8;              // Maxima profundidad del arbol
const int THRESHOLD = 5;              // Maximo de puntos antes de subdividir
const int GRID_SIZE = 40;             // Tamano de la cuadricula ASCII

// =============================================================================
// ESTRUCTURA DE PUNTO 3D
// =============================================================================
struct Point {
    double x, y, z;
    Point(double _x = 0.0, double _y = 0.0, double _z = 0.0)
        : x(_x), y(_y), z(_z) {}

    bool operator==(const Point& other) const {
        const double EPSILON = 1e-9;
        return abs(x - other.x) < EPSILON &&
               abs(y - other.y) < EPSILON &&
               abs(z - other.z) < EPSILON;
    }
};

// =============================================================================
// CAJA DE LIMITES (BOUNDING BOX)
// =============================================================================
struct BoundingBox {
    Point min, max;

    BoundingBox(Point _min, Point _max) : min(_min), max(_max) {}

    // Verifica si la caja contiene un punto (inclusive)
    bool contains(const Point& p) const {
        return (p.x >= min.x && p.x <= max.x &&
                p.y >= min.y && p.y <= max.y &&
                p.z >= min.z && p.z <= max.z);
    }

    // Verifica si dos Bounding Boxes se intersecan
    bool intersects(const BoundingBox& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x &&
                min.y <= other.max.y && max.y >= other.min.y &&
                min.z <= other.max.z && max.z >= other.min.z);
    }

    double volume() const {
        return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
    }
};

// =============================================================================
// CLASE NODO DEL OCTREE
// =============================================================================
class OctreeNode {
public:
    BoundingBox bounds;
    vector<Point> points;
    unique_ptr<OctreeNode> children[8];
    bool is_leaf;
    int depth;

    OctreeNode(const BoundingBox& b, int d) : bounds(b), is_leaf(true), depth(d) {
        for (int i = 0; i < 8; ++i) children[i] = nullptr;
    }

    // Complejidad: O(log n) promedio, O(n) peor caso
    void insert(const Point& p);

    // Complejidad: O(cbrt(n) + k) donde k es el numero de puntos en el rango
    void rangeQuery(const BoundingBox& range, vector<Point>& result) const;

    // Determina en que octante (0-7) esta un punto
    int determineOctant(const Point& p) const;

    // Obtiene estadisticas del arbol
    void getStats(int& totalNodes, int& leafNodes, int& maxDepth, int& totalPoints) const;

private:
    void subdivide();
};

// =============================================================================
// IMPLEMENTACION DE METODOS DEL OCTREE
// =============================================================================

int OctreeNode::determineOctant(const Point& p) const {
    int octant = 0;
    double midX = (bounds.min.x + bounds.max.x) / 2.0;
    double midY = (bounds.min.y + bounds.max.y) / 2.0;
    double midZ = (bounds.min.z + bounds.max.z) / 2.0;

    // Codificacion binaria: bit 2 = x, bit 1 = y, bit 0 = z
    if (p.x >= midX) octant |= 4;
    if (p.y >= midY) octant |= 2;
    if (p.z >= midZ) octant |= 1;

    return octant;
}

void OctreeNode::subdivide() {
    if (!is_leaf) return;

    double midX = (bounds.min.x + bounds.max.x) / 2.0;
    double midY = (bounds.min.y + bounds.max.y) / 2.0;
    double midZ = (bounds.min.z + bounds.max.z) / 2.0;

    // Crear 8 nodos hijos
    for (int i = 0; i < 8; ++i) {
        double subMinX = (i & 4) ? midX : bounds.min.x;
        double subMinY = (i & 2) ? midY : bounds.min.y;
        double subMinZ = (i & 1) ? midZ : bounds.min.z;

        double subMaxX = (i & 4) ? bounds.max.x : midX;
        double subMaxY = (i & 2) ? bounds.max.y : midY;
        double subMaxZ = (i & 1) ? bounds.max.z : midZ;

        Point subMin(subMinX, subMinY, subMinZ);
        Point subMax(subMaxX, subMaxY, subMaxZ);

        children[i] = make_unique<OctreeNode>(BoundingBox(subMin, subMax), depth + 1);
    }

    // Redistribuir puntos a los hijos
    for (const auto& p : points) {
        int octant = determineOctant(p);
        children[octant]->insert(p);
    }

    points.clear();
    is_leaf = false;
}

void OctreeNode::insert(const Point& p) {
    if (!bounds.contains(p)) return;

    if (is_leaf) {
        if (depth >= MAX_DEPTH || points.size() < THRESHOLD) {
            points.push_back(p);
            return;
        } else {
            subdivide();
        }
    }

    if (!is_leaf) {
        int octant = determineOctant(p);
        if (children[octant]) {
            children[octant]->insert(p);
        }
    }
}

void OctreeNode::rangeQuery(const BoundingBox& range, vector<Point>& result) const {
    // Poda espacial: si no hay interseccion, retornar inmediatamente
    if (!bounds.intersects(range)) {
        return;
    }

    if (is_leaf) {
        for (const auto& p : points) {
            if (range.contains(p)) {
                result.push_back(p);
            }
        }
        return;
    }

    // Recursion en nodos hijos
    for (int i = 0; i < 8; ++i) {
        if (children[i]) {
            children[i]->rangeQuery(range, result);
        }
    }
}

void OctreeNode::getStats(int& totalNodes, int& leafNodes, int& maxDepth, int& totalPoints) const {
    totalNodes++;
    if (depth > maxDepth) maxDepth = depth;

    if (is_leaf) {
        leafNodes++;
        totalPoints += points.size();
    } else {
        for (int i = 0; i < 8; ++i) {
            if (children[i]) {
                children[i]->getStats(totalNodes, leafNodes, maxDepth, totalPoints);
            }
        }
    }
}

// =============================================================================
// FUNCIONES DE UTILIDAD Y VISUALIZACION
// =============================================================================

void printHeader(const string& title) {
    cout << "\n" << Color::BOLD << Color::CYAN;
    cout << "=========================================================" << endl;
    cout << "  " << title << endl;
    cout << "=========================================================" << Color::RESET << endl;
}

void printSubHeader(const string& title) {
    cout << "\n" << Color::YELLOW << Color::BOLD << ">>> " << title << Color::RESET << endl;
}

void printSuccess(const string& message) {
    cout << Color::GREEN << "[✓] " << message << Color::RESET << endl;
}

void printInfo(const string& message) {
    cout << Color::BLUE << "[i] " << message << Color::RESET << endl;
}

void printWarning(const string& message) {
    cout << Color::YELLOW << "[!] " << message << Color::RESET << endl;
}

void printError(const string& message) {
    cout << Color::RED << "[✗] " << message << Color::RESET << endl;
}

// Visualizacion 2D mejorada con estadisticas
void draw2DProjection(const OctreeNode* root, const vector<Point>& all_points) {
    double step = 100.0 / GRID_SIZE;

    printSubHeader("PROYECCION 2D (Plano XY)");

    // Estadisticas del octree
    int totalNodes = 0, leafNodes = 0, maxDepth = 0, totalPoints = 0;
    root->getStats(totalNodes, leafNodes, maxDepth, totalPoints);

    cout << Color::CYAN << "Nodos totales: " << totalNodes
         << " | Hojas: " << leafNodes
         << " | Profundidad max: " << maxDepth
         << " | Puntos: " << totalPoints << Color::RESET << endl;

    cout << "\n  " << Color::WHITE;
    for(int i = 0; i < GRID_SIZE; ++i) cout << "-";
    cout << Color::RESET << endl;

    for (int j = GRID_SIZE - 1; j >= 0; --j) {
        cout << Color::WHITE << setw(3) << (int)(j * 2.5) << "|" << Color::RESET;

        for (int i = 0; i < GRID_SIZE; ++i) {
            char marker = ' ';
            double x_start = i * step;
            double y_start = j * step;

            int count = 0;
            for (const auto& p : all_points) {
                if (p.x >= x_start && p.x < (x_start + step) &&
                    p.y >= y_start && p.y < (y_start + step)) {
                    count++;
                }
            }

            if (count > 0) {
                if (count == 1) {
                    cout << Color::GREEN << "." << Color::RESET;
                } else if (count <= 3) {
                    cout << Color::YELLOW << "o" << Color::RESET;
                } else {
                    cout << Color::RED << "@" << Color::RESET;
                }
            } else {
                cout << " ";
            }
        }
        cout << Color::WHITE << "|" << Color::RESET << endl;
    }

    cout << "  ";
    for(int i = 0; i < GRID_SIZE; ++i) cout << "-";
    cout << endl;

    cout << "  ";
    for(int i = 0; i < GRID_SIZE; ++i) {
        if (i % 10 == 0) cout << (int)(i * 2.5);
        else if (i % 10 == 1 && i > 1) cout << " ";
        else cout << " ";
    }
    cout << endl;

    cout << "\n" << Color::CYAN << "Leyenda: "
         << Color::GREEN << "." << Color::RESET << "=1 punto  "
         << Color::YELLOW << "o" << Color::RESET << "=2-3 puntos  "
         << Color::RED << "@" << Color::RESET << "=4+ puntos" << endl;
}

// Valida que dos vectores contengan los mismos puntos
bool validateResults(const vector<Point>& v1, const vector<Point>& v2) {
    if (v1.size() != v2.size()) return false;

    vector<Point> sorted1 = v1;
    vector<Point> sorted2 = v2;

    auto comparator = [](const Point& a, const Point& b) {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        return a.z < b.z;
    };

    sort(sorted1.begin(), sorted1.end(), comparator);
    sort(sorted2.begin(), sorted2.end(), comparator);

    for (size_t i = 0; i < sorted1.size(); ++i) {
        if (!(sorted1[i] == sorted2[i])) return false;
    }

    return true;
}

// =============================================================================
// ESCENARIOS DE DEMOSTRACION
// =============================================================================

void scenario1_BasicDemo() {
    printHeader("ESCENARIO 1: DEMOSTRACION BASICA");

    Point min_world(0.0, 0.0, 0.0);
    Point max_world(100.0, 100.0, 100.0);
    BoundingBox world_bounds(min_world, max_world);

    int N = 1000;
    OctreeNode root(world_bounds, 0);
    vector<Point> points;

    printInfo("Insertando " + to_string(N) + " puntos aleatorios...");

    for (int i = 0; i < N; ++i) {
        double x = (double)rand() / RAND_MAX * 100.0;
        double y = (double)rand() / RAND_MAX * 100.0;
        double z = (double)rand() / RAND_MAX * 100.0;
        Point p(x, y, z);
        points.push_back(p);
        root.insert(p);
    }

    printSuccess("Puntos insertados correctamente");
    draw2DProjection(&root, points);
}

void scenario2_PerformanceBenchmark() {
    printHeader("ESCENARIO 2: BENCHMARK DE RENDIMIENTO");

    Point min_world(0.0, 0.0, 0.0);
    Point max_world(100.0, 100.0, 100.0);
    BoundingBox world_bounds(min_world, max_world);

    vector<int> testSizes = {10000, 50000, 100000, 200000};

    cout << Color::BOLD << "\nPrueba de escalabilidad con diferentes tamanos de datos:\n" << Color::RESET;
    cout << setw(12) << "N" << setw(15) << "Octree (ms)" << setw(15) << "Naive (ms)"
         << setw(15) << "Speedup" << setw(15) << "Puntos" << endl;
    cout << string(70, '-') << endl;

    for (int N : testSizes) {
        OctreeNode root(world_bounds, 0);
        vector<Point> all_points;

        // Generar puntos
        for (int i = 0; i < N; ++i) {
            double x = (double)rand() / RAND_MAX * 100.0;
            double y = (double)rand() / RAND_MAX * 100.0;
            double z = (double)rand() / RAND_MAX * 100.0;
            Point p(x, y, z);
            all_points.push_back(p);
            root.insert(p);
        }

        // Rango de consulta
        Point min_range(40.0, 40.0, 40.0);
        Point max_range(60.0, 60.0, 60.0);
        BoundingBox query_range(min_range, max_range);

        // Benchmark Octree
        vector<Point> octree_results;
        auto start_octree = high_resolution_clock::now();
        root.rangeQuery(query_range, octree_results);
        auto end_octree = high_resolution_clock::now();
        auto time_octree = duration_cast<microseconds>(end_octree - start_octree).count();

        // Benchmark Naive
        vector<Point> naive_results;
        auto start_naive = high_resolution_clock::now();
        for (const auto& p : all_points) {
            if (query_range.contains(p)) {
                naive_results.push_back(p);
            }
        }
        auto end_naive = high_resolution_clock::now();
        auto time_naive = duration_cast<microseconds>(end_naive - start_naive).count();

        double speedup = (double)time_naive / max(1.0, (double)time_octree);

        cout << setw(12) << N
             << setw(15) << fixed << setprecision(2) << time_octree / 1000.0
             << setw(15) << time_naive / 1000.0
             << setw(14) << setprecision(1) << speedup << "x"
             << setw(15) << octree_results.size() << endl;
    }
}

void scenario3_ValidationTest() {
    printHeader("ESCENARIO 3: VALIDACION DE CORRECTITUD");

    Point min_world(0.0, 0.0, 0.0);
    Point max_world(100.0, 100.0, 100.0);
    BoundingBox world_bounds(min_world, max_world);

    int N = 50000;
    OctreeNode root(world_bounds, 0);
    vector<Point> all_points;

    printInfo("Generando " + to_string(N) + " puntos de prueba...");

    for (int i = 0; i < N; ++i) {
        double x = (double)rand() / RAND_MAX * 100.0;
        double y = (double)rand() / RAND_MAX * 100.0;
        double z = (double)rand() / RAND_MAX * 100.0;
        Point p(x, y, z);
        all_points.push_back(p);
        root.insert(p);
    }

    // Probar varios rangos de consulta
    vector<pair<Point, Point>> test_ranges = {
        {Point(40, 40, 40), Point(60, 60, 60)},
        {Point(0, 0, 0), Point(25, 25, 25)},
        {Point(75, 75, 75), Point(100, 100, 100)},
        {Point(25, 25, 25), Point(75, 75, 75)},
        {Point(45, 45, 45), Point(55, 55, 55)}
    };

    bool all_passed = true;

    for (size_t i = 0; i < test_ranges.size(); ++i) {
        BoundingBox range(test_ranges[i].first, test_ranges[i].second);

        vector<Point> octree_result;
        vector<Point> naive_result;

        root.rangeQuery(range, octree_result);

        for (const auto& p : all_points) {
            if (range.contains(p)) {
                naive_result.push_back(p);
            }
        }

        bool passed = validateResults(octree_result, naive_result);

        cout << "Prueba " << (i + 1) << " - Rango ["
             << (int)test_ranges[i].first.x << "-" << (int)test_ranges[i].second.x << "]: ";

        if (passed) {
            printSuccess("CORRECTO (" + to_string(octree_result.size()) + " puntos)");
        } else {
            printError("FALLO (Octree: " + to_string(octree_result.size()) +
                      ", Naive: " + to_string(naive_result.size()) + ")");
            all_passed = false;
        }
    }

    cout << "\n";
    if (all_passed) {
        printSuccess("TODAS LAS PRUEBAS PASARON - Implementacion correcta!");
    } else {
        printError("Algunas pruebas fallaron - Revisar implementacion");
    }
}

void scenario4_EdgeCases() {
    printHeader("ESCENARIO 4: CASOS BORDE Y EXTREMOS");

    Point min_world(0.0, 0.0, 0.0);
    Point max_world(100.0, 100.0, 100.0);
    BoundingBox world_bounds(min_world, max_world);

    // Test 1: Octree vacio
    {
        printSubHeader("Test 1: Consulta en Octree vacio");
        OctreeNode root(world_bounds, 0);
        vector<Point> result;
        BoundingBox range(Point(40, 40, 40), Point(60, 60, 60));
        root.rangeQuery(range, result);

        if (result.empty()) {
            printSuccess("Octree vacio retorna 0 puntos");
        } else {
            printError("Error: Octree vacio retorno " + to_string(result.size()) + " puntos");
        }
    }

    // Test 2: Puntos en las esquinas
    {
        printSubHeader("Test 2: Puntos en esquinas del espacio");
        OctreeNode root(world_bounds, 0);
        vector<Point> corners = {
            Point(0, 0, 0), Point(100, 0, 0), Point(0, 100, 0), Point(0, 0, 100),
            Point(100, 100, 0), Point(100, 0, 100), Point(0, 100, 100), Point(100, 100, 100)
        };

        for (const auto& p : corners) {
            root.insert(p);
        }

        printSuccess("Insertadas 8 esquinas correctamente");

        // Consulta que deberia incluir todas las esquinas
        vector<Point> result;
        root.rangeQuery(world_bounds, result);

        if (result.size() == 8) {
            printSuccess("Consulta completa retorno todas las esquinas");
        } else {
            printError("Error: Se esperaban 8 puntos, se obtuvieron " + to_string(result.size()));
        }
    }

    // Test 3: Alta densidad localizada
    {
        printSubHeader("Test 3: Alta densidad en region localizada");
        OctreeNode root(world_bounds, 0);

        int dense_points = 10000;
        for (int i = 0; i < dense_points; ++i) {
            double x = 49.0 + (double)rand() / RAND_MAX * 2.0;  // [49, 51]
            double y = 49.0 + (double)rand() / RAND_MAX * 2.0;
            double z = 49.0 + (double)rand() / RAND_MAX * 2.0;
            root.insert(Point(x, y, z));
        }

        int totalNodes = 0, leafNodes = 0, maxDepth = 0, totalPoints = 0;
        root.getStats(totalNodes, leafNodes, maxDepth, totalPoints);

        cout << Color::CYAN << "  Puntos insertados: " << totalPoints << endl;
        cout << "  Nodos creados: " << totalNodes << endl;
        cout << "  Profundidad maxima: " << maxDepth << Color::RESET << endl;

        printSuccess("Octree manejo correctamente alta densidad localizada");
    }
}

void scenario5_Interactive() {
    printHeader("ESCENARIO 5: MODO INTERACTIVO");

    Point min_world(0.0, 0.0, 0.0);
    Point max_world(100.0, 100.0, 100.0);
    BoundingBox world_bounds(min_world, max_world);
    OctreeNode root(world_bounds, 0);
    vector<Point> all_points;

    bool running = true;

    while (running) {
        cout << "\n" << Color::BOLD << Color::MAGENTA;
        cout << "======== MENU INTERACTIVO ========" << endl;
        cout << "1. Insertar puntos aleatorios" << endl;
        cout << "2. Insertar punto manual" << endl;
        cout << "3. Realizar consulta por rango" << endl;
        cout << "4. Ver estadisticas del Octree" << endl;
        cout << "5. Visualizar proyeccion 2D" << endl;
        cout << "6. Limpiar Octree" << endl;
        cout << "0. Volver al menu principal" << endl;
        cout << "==================================" << Color::RESET << endl;
        cout << "Opcion: ";

        int opcion;
        cin >> opcion;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            printError("Opcion invalida");
            continue;
        }

        switch(opcion) {
            case 1: {
                int n;
                cout << "Cantidad de puntos a insertar: ";
                cin >> n;

                if (n > 0 && n <= 1000000) {
                    printInfo("Insertando " + to_string(n) + " puntos...");
                    auto start = high_resolution_clock::now();

                    for (int i = 0; i < n; ++i) {
                        double x = (double)rand() / RAND_MAX * 100.0;
                        double y = (double)rand() / RAND_MAX * 100.0;
                        double z = (double)rand() / RAND_MAX * 100.0;
                        Point p(x, y, z);
                        all_points.push_back(p);
                        root.insert(p);
                    }

                    auto end = high_resolution_clock::now();
                    auto time_ms = duration_cast<milliseconds>(end - start).count();

                    printSuccess("Puntos insertados en " + to_string(time_ms) + " ms");
                } else {
                    printError("Cantidad invalida");
                }
                break;
            }

            case 2: {
                double x, y, z;
                cout << "Ingrese coordenadas (x y z): ";
                cin >> x >> y >> z;

                if (x >= 0 && x <= 100 && y >= 0 && y <= 100 && z >= 0 && z <= 100) {
                    Point p(x, y, z);
                    all_points.push_back(p);
                    root.insert(p);
                    printSuccess("Punto (" + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ") insertado");
                } else {
                    printError("Coordenadas fuera de rango [0, 100]");
                }
                break;
            }

            case 3: {
                double x1, y1, z1, x2, y2, z2;
                cout << "Ingrese rango minimo (x y z): ";
                cin >> x1 >> y1 >> z1;
                cout << "Ingrese rango maximo (x y z): ";
                cin >> x2 >> y2 >> z2;

                BoundingBox range(Point(x1, y1, z1), Point(x2, y2, z2));
                vector<Point> result;

                auto start = high_resolution_clock::now();
                root.rangeQuery(range, result);
                auto end = high_resolution_clock::now();
                auto time_us = duration_cast<microseconds>(end - start).count();

                printSuccess("Consulta completada en " + to_string(time_us) + " microsegundos");
                printInfo("Puntos encontrados: " + to_string(result.size()));

                if (result.size() <= 10) {
                    for (const auto& p : result) {
                        cout << "  (" << p.x << ", " << p.y << ", " << p.z << ")" << endl;
                    }
                }
                break;
            }

            case 4: {
                int totalNodes = 0, leafNodes = 0, maxDepth = 0, totalPoints = 0;
                root.getStats(totalNodes, leafNodes, maxDepth, totalPoints);

                cout << "\n" << Color::CYAN << Color::BOLD << "ESTADISTICAS DEL OCTREE:" << Color::RESET << endl;
                cout << Color::CYAN;
                cout << "  Total de nodos: " << totalNodes << endl;
                cout << "  Nodos hoja: " << leafNodes << endl;
                cout << "  Nodos internos: " << (totalNodes - leafNodes) << endl;
                cout << "  Profundidad maxima: " << maxDepth << endl;
                cout << "  Puntos almacenados: " << totalPoints << endl;
                cout << "  Factor de ramificacion promedio: "
                     << fixed << setprecision(2)
                     << (leafNodes > 0 ? (double)totalPoints / leafNodes : 0.0) << Color::RESET << endl;
                break;
            }

            case 5: {
                if (all_points.empty()) {
                    printWarning("No hay puntos para visualizar");
                } else {
                    draw2DProjection(&root, all_points);
                }
                break;
            }

            case 6: {
                root = OctreeNode(world_bounds, 0);
                all_points.clear();
                printSuccess("Octree limpiado");
                break;
            }

            case 0: {
                running = false;
                break;
            }

            default:
                printError("Opcion invalida");
        }
    }
}

// =============================================================================
// MENU PRINCIPAL
// =============================================================================

void showMainMenu() {
    cout << "\n" << Color::BOLD << Color::BG_BLUE << Color::WHITE;
    cout << "                                                           " << endl;
    cout << "  OCTREE - ESTRUCTURA DE DATOS ESPACIAL JERARQUICA 3D     " << endl;
    cout << "  Universidad de Ingenieria y Tecnologia (UTEC)           " << endl;
    cout << "                                                           " << Color::RESET << endl;

    cout << "\n" << Color::BOLD << Color::MAGENTA;
    cout << "================ MENU PRINCIPAL ================" << endl;
    cout << "1. Demo basica con visualizacion" << endl;
    cout << "2. Benchmark de rendimiento" << endl;
    cout << "3. Test de validacion y correctitud" << endl;
    cout << "4. Tests de casos borde" << endl;
    cout << "5. Modo interactivo" << endl;
    cout << "6. Ejecutar DEMO COMPLETA (para exposicion)" << endl;
    cout << "0. Salir" << endl;
    cout << "================================================" << Color::RESET << endl;
    cout << "\nSeleccione una opcion: ";
}

// =============================================================================
// FUNCION PRINCIPAL
// =============================================================================

int main() {
    // Habilitar colores ANSI en Windows
    enableANSI();

    // Inicializar semilla aleatoria
    srand(static_cast<unsigned>(time(0)));

    // Configurar precision de salida
    cout << fixed << setprecision(3);

    // Banner inicial
    cout << Color::BOLD << Color::CYAN;
    cout << R"(
   ___   ____ _____ ____  _____ _____
  / _ \ / ___|_   _|  _ \| ____| ____|
 | | | | |     | | | |_) |  _| |  _|
 | |_| | |___  | | |  _ <| |___| |___
  \___/ \____| |_| |_| \_\_____|_____|
    )" << Color::RESET << endl;

    cout << Color::YELLOW << "Proyecto: Algoritmos y Estructura de Datos" << endl;
    cout << "Autores: Eliseo Velasquez & Jean Terrazo" << Color::RESET << endl;

    bool running = true;

    while (running) {
        showMainMenu();

        int opcion;
        cin >> opcion;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            printError("Opcion invalida");
            continue;
        }

        switch(opcion) {
            case 1:
                scenario1_BasicDemo();
                break;
            case 2:
                scenario2_PerformanceBenchmark();
                break;
            case 3:
                scenario3_ValidationTest();
                break;
            case 4:
                scenario4_EdgeCases();
                break;
            case 5:
                scenario5_Interactive();
                break;
            case 6:
                printHeader("DEMO COMPLETA - PRESENTACION");
                printInfo("Ejecutando todos los escenarios...\n");
                scenario1_BasicDemo();
                scenario2_PerformanceBenchmark();
                scenario3_ValidationTest();
                scenario4_EdgeCases();
                printSuccess("\nDEMO COMPLETA FINALIZADA");
                break;
            case 0:
                cout << "\n" << Color::GREEN << "Gracias por usar el proyecto Octree!" << Color::RESET << endl;
                running = false;
                break;
            default:
                printError("Opcion invalida. Intente nuevamente.");
        }
    }

    return 0;
}
