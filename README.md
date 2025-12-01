# Octree - Proyecto de Algoritmos y Estructura de Datos

**Universidad de Ingenieria y Tecnologia (UTEC)**

## Integrantes
* Eliseo David Velasquez Diaz (Código: 202410184)
* Jean Luka Terrazo Santiago (Código: 202410422)

---

## Descripción

Este proyecto implementa un Octree, una estructura de datos jerárquica que particiona el espacio tridimensional en 8 octantes. El Octree permite realizar búsquedas espaciales eficientes, reduciendo la complejidad de O(n) del método naive a O(∛n + k) donde k es el número de puntos en el rango consultado.

## Funcionalidades

El proyecto incluye:

- Implementación completa del Octree con subdivisión adaptativa
- Benchmark de rendimiento comparando Octree vs búsqueda lineal
- Sistema de validación automática de correctitud
- Pruebas con casos extremos (octree vacío, puntos en esquinas, alta densidad)
- Modo interactivo para insertar puntos y realizar consultas
- Visualización ASCII de la proyección 2D del espacio

## Compilación

### Requisitos
- Compilador C++ con soporte para C++17 o superior
- CMake 3.10 o superior (opcional)

### Opción 1: Compilación directa

Windows:
```bash
g++ -std=c++17 -O2 main.cpp -o octree_demo.exe
```

Linux/Mac:
```bash
g++ -std=c++17 -O2 main.cpp -o octree_demo
```

### Opción 2: Con CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Uso

Ejecutar el programa:
```bash
./octree_demo.exe    # Windows
./octree_demo        # Linux/Mac
```

El programa muestra un menú con 6 opciones:

1. **Demo básica**: Inserta 1000 puntos aleatorios y muestra la visualización
2. **Benchmark**: Prueba con 10K, 50K, 100K y 200K puntos, comparando tiempos
3. **Validación**: Verifica que el Octree devuelve los mismos resultados que búsqueda lineal
4. **Casos borde**: Prueba situaciones extremas
5. **Modo interactivo**: Permite insertar puntos y hacer consultas personalizadas
6. **Demo completa**: Ejecuta todos los escenarios

## Resultados de Benchmarks

Pruebas con consultas por rango en espacio [0,100]³:

| N Puntos | Tiempo Octree | Tiempo Naive | Mejora |
|----------|---------------|--------------|--------|
| 10,000   | 0.05 ms       | 0.50 ms      | 10x    |
| 50,000   | 0.30 ms       | 2.80 ms      | 9.3x   |
| 100,000  | 0.60 ms       | 5.50 ms      | 9.2x   |
| 200,000  | 1.20 ms       | 11.00 ms     | 9.2x   |

## Complejidad

- **Inserción**: O(log n) en promedio
- **Consulta por rango**: O(∛n + k) donde k es el número de puntos encontrados
- **Espacio**: O(n)

## Estructura del Proyecto

```
octree_proyecto/
├── main.cpp           # Implementación del Octree
├── CMakeLists.txt     # Configuración CMake
├── compile.bat        # Script de compilación para Windows
└── README.md          # Este archivo
```

## Parámetros Configurables

En main.cpp se pueden ajustar:
- `MAX_DEPTH = 8`: Profundidad máxima del árbol
- `THRESHOLD = 5`: Número máximo de puntos por nodo antes de subdividir
- `GRID_SIZE = 40`: Tamaño de la visualización ASCII

## Aplicaciones

El Octree se utiliza en:
- Ray tracing y renderizado 3D
- Detección de colisiones en motores de física
- Sistemas de información geográfica (GIS)
- Simulaciones científicas
- Planificación de rutas en robótica

## Notas

- Los colores ANSI funcionan mejor en Windows Terminal o PowerShell 7+
- El benchmark usa microsegundos para mediciones precisas
- La validación garantiza que el Octree devuelve exactamente los mismos puntos que la búsqueda lineal

---

*Proyecto desarrollado para el curso de Algoritmos y Estructura de Datos - UTEC 2025*
