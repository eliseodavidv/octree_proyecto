#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <chrono>   // Para mediciones de tiempo de alta resolucion
#include <iomanip>  // Para formato de salida

using namespace std;
using namespace std::chrono;

// --- Parametros de control del Octree ---
// Máxima profundidad
const int MAX_DEPTH = 8;
// Máximo de puntos en un nodo hoja antes de subdividir
const int THRESHOLD = 5;
const int GRID_SIZE = 40; // Tamano de la cuadrícula para la visualización ASCII

// --- 1. Estructura de Punto 3D ---
struct Point {
    double x, y, z;
    Point(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
};

// --- 2. Estructura de Caja de Límites (Bounding Box) ---
struct BoundingBox {
    Point min, max;
    BoundingBox(Point _min, Point _max) : min(_min), max(_max) {}

    // Verifica si la caja contiene o iguala un punto
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
};

// --- 3. Clase del Nodo del Octree (Declaración) ---
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

    void insert(const Point& p);
    void rangeQuery(const BoundingBox& range, vector<Point>& result) const;

    // Función pública para determinar el octante, usada en la visualización
    int determineOctant(const Point& p) const;

private:
    void subdivide();
};

// --- 4. Implementación de los Métodos del Octree ---

int OctreeNode::determineOctant(const Point& p) const {
    int octant = 0;
    double midX = (bounds.min.x + bounds.max.x) / 2.0;
    double midY = (bounds.min.y + bounds.max.y) / 2.0;
    double midZ = (bounds.min.z + bounds.max.z) / 2.0;

    // 000 (---) a 111 (+++)
    if (p.x >= midX) octant |= 4; // x-positive
    if (p.y >= midY) octant |= 2; // y-positive
    if (p.z >= midZ) octant |= 1; // z-positive

    return octant;
}

void OctreeNode::subdivide() {
    if (!is_leaf) return;

    double midX = (bounds.min.x + bounds.max.x) / 2.0;
    double midY = (bounds.min.y + bounds.max.y) / 2.0;
    double midZ = (bounds.min.z + bounds.max.z) / 2.0;

    for (int i = 0; i < 8; ++i) {
        // Calcular las coordenadas minimas (mins) para el octante i
        double subMinX = (i & 4) ? midX : bounds.min.x;
        double subMinY = (i & 2) ? midY : bounds.min.y;
        double subMinZ = (i & 1) ? midZ : bounds.min.z;

        // Calcular las coordenadas maximas (maxs) para el octante i
        double subMaxX = (i & 4) ? bounds.max.x : midX;
        double subMaxY = (i & 2) ? bounds.max.y : midY;
        double subMaxZ = (i & 1) ? bounds.max.z : midZ;

        Point subMin(subMinX, subMinY, subMinZ);
        Point subMax(subMaxX, subMaxY, subMaxZ);

        children[i] = make_unique<OctreeNode>(BoundingBox(subMin, subMax), depth + 1);
    }

    // Mueve los puntos
    for (const auto& p : points) {
        int octant = determineOctant(p);
        children[octant]->insert(p);
    }

    points.clear(); // Vacía los puntos del nodo interno
    is_leaf = false;
}

void OctreeNode::insert(const Point& p) {
    if (!bounds.contains(p)) return;

    if (is_leaf) {
        if (depth >= MAX_DEPTH || points.size() < THRESHOLD) {
            points.push_back(p);
            return;
        } else {
            subdivide(); // Subdividir
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
    // 1. Poda espacial
    if (!bounds.intersects(range)) {
        return;
    }

    // 2. Nodo hoja: chequeo de puntos
    if (is_leaf) {
        for (const auto& p : points) {
            if (range.contains(p)) {
                result.push_back(p);
            }
        }
        return;
    }

    // 3. Nodo interno: recursión
    for (int i = 0; i < 8; ++i) {
        if (children[i]) {
            children[i]->rangeQuery(range, result);
        }
    }
}

// --- 5. Funciones de Visualización (ASCII Art) ---

// Dibuja la proyeccion 2D (plano XY) del Octree en la consola
void draw2DProjection(const OctreeNode* root, const vector<Point>& all_points) {
    // Escala del mundo: [0, 100]
    double step = 100.0 / GRID_SIZE;

    cout << "  ";
    for(int i = 0; i < GRID_SIZE; ++i) cout << "-";
    cout << endl;

    for (int j = GRID_SIZE - 1; j >= 0; --j) { // Eje Y (de arriba a abajo)
        cout << setw(2) << j*2.5 << "|";
        for (int i = 0; i < GRID_SIZE; ++i) { // Eje X (de izquierda a derecha)
            char marker = ' ';
            // Coordenadas del inicio del 'pixel'
            double x_start = i * step;
            double y_start = j * step;

            // Verificamos si hay un punto en la region (simple)
            for (const auto& p : all_points) {
                if (p.x >= x_start && p.x < (x_start + step) &&
                    p.y >= y_start && p.y < (y_start + step)) {
                    marker = '@'; // Marcador de punto
                    break;
                }
            }
            cout << marker;
        }
        cout << "|" << endl;
    }
    cout << "  ";
    for(int i = 0; i < GRID_SIZE; ++i) cout << "-";
    cout << endl;
    cout << "  ";
    for(int i = 0; i < GRID_SIZE; ++i) {
        if (i % 4 == 0) cout << i*2.5; // Etiquetas en eje X
        else cout << " ";
    }
    cout << endl;

    cout << "Leyenda: @ = Puntos de datos (Proyeccion XY) | El espacio en blanco representa las subdivisiones del Octree." << endl;
}

// --- 6. Función de prueba (Simulación y Benchmark) ---

void runSimulation() {
    Point min_world(0.0, 0.0, 0.0);
    Point max_world(100.0, 100.0, 100.0);
    BoundingBox world_bounds(min_world, max_world);

    // --- PARTE 1: VISUALIZACIÓN DEL PARTIcionamiento (N pequeño para el grafico) ---
    cout << "=========================================================" << endl;
    cout << "      SIMULACION GRAFICA ASCII (Proyeccion 2D)           " << endl;
    cout << "=========================================================" << endl;

    int N_visual = 1000;
    OctreeNode root_visual(world_bounds, 0);
    vector<Point> visual_points;

    cout << "Generando e insertando " << N_visual << " puntos para la visualizacion..." << endl;

    // Generar puntos para la visualizacion
    for (int i = 0; i < N_visual; ++i) {
        double x = (double)rand() / RAND_MAX * 100.0;
        double y = (double)rand() / RAND_MAX * 100.0;
        double z = (double)rand() / RAND_MAX * 100.0;
        Point p(x, y, z);
        visual_points.push_back(p);
        root_visual.insert(p);
    }

    draw2DProjection(&root_visual, visual_points);

    // --- PARTE 2: BENCHMARK DE RENDIMIENTO (N GRANDE) ---

    int N_benchmark = 100000; // ¡Usamos 100,000 puntos para el benchmark!
    cout << "\n=========================================================" << endl;
    cout << "       BENCHMARK DE RENDIMIENTO (N=" << N_benchmark << ")        " << endl;
    cout << "=========================================================" << endl;

    OctreeNode root_benchmark(world_bounds, 0);
    vector<Point> all_points_benchmark;

    cout << "Generando e insertando " << N_benchmark << " puntos aleatorios..." << endl;

    for (int i = 0; i < N_benchmark; ++i) {
        double x = (double)rand() / RAND_MAX * 100.0;
        double y = (double)rand() / RAND_MAX * 100.0;
        double z = (double)rand() / RAND_MAX * 100.0;
        Point p(x, y, z);
        all_points_benchmark.push_back(p);
        root_benchmark.insert(p);
    }

    // Rango de consulta
    Point min_range(50.0, 50.0, 50.0);
    Point max_range(60.0, 60.0, 60.0);
    BoundingBox query_range(min_range, max_range);

    cout << "Realizando Consulta por Rango: x, y, z en [" << min_range.x << ", " << max_range.x << "]" << endl;

    // Medir el rendimiento (Octree)
    vector<Point> octree_results;
    auto start_octree = high_resolution_clock::now();
    root_benchmark.rangeQuery(query_range, octree_results);
    auto end_octree = high_resolution_clock::now();
    auto time_octree_ms = duration_cast<milliseconds>(end_octree - start_octree).count();

    // Medir el rendimiento (Búsqueda Naive - Benchmark)
    vector<Point> naive_results;
    auto start_naive = high_resolution_clock::now();
    for (const auto& p : all_points_benchmark) {
        if (query_range.contains(p)) {
            naive_results.push_back(p);
        }
    }
    auto end_naive = high_resolution_clock::now();
    auto time_naive_ms = duration_cast<milliseconds>(end_naive - start_naive).count();

    // Mostrar resultados y comparar
    cout << "\n--- Resultados y Comparacion del Benchmark ---" << endl;
    cout << "Puntos encontrados por Octree: " << octree_results.size() << endl;
    cout << "Puntos encontrados por Naive: " << naive_results.size() << endl;
    cout << "Tiempo Octree (ms): " << time_octree_ms << endl;
    cout << "Tiempo Naive (ms): " << time_naive_ms << endl;

    if (time_octree_ms < time_naive_ms) {
        double improvement = (double)time_naive_ms / max(1.0, (double)time_octree_ms);
        cout << "\nConclusion: ¡EXITO! El Octree demostro ser aproximadamente "
             << fixed << setprecision(1) << improvement << " veces mas rapido que el enfoque Naive para la Consulta por Rango." << endl;
    } else {
         cout << "\nConclusion: El Octree fue mas lento o similar. Esto puede ocurrir debido al overhead de la creacion de nodos en N pequeños, pero su ventaja es clara en N grandes." << endl;
    }
}

// Función principal
int main() {
    // Inicializa la semilla aleatoria
    srand(time(0));
    // Asegurarse de que el formato de salida de punto flotante sea fijo y con precision
    cout << fixed << setprecision(3);
    runSimulation();
    return 0;
}