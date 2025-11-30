# 🌲 Octree: Estructura de Datos Jerárquica para Partición Espacial Tridimensional

**Proyecto del curso Algoritmos y Estructura de Datos (UTEC)**

El Octree es una estructura de datos jerárquica que representa una partición adaptativa del espacio tridimensional. Su propósito es optimizar operaciones de búsqueda y consulta por rango en grandes volúmenes de datos, una necesidad crítica en campos como los gráficos 3D, la simulación física y el *ray tracing*.

### Integrantes
* **Eliseo David Velasquez Diaz** (Código: 202410184)
* **Jean Luka Terrazo Santiago** (Código: 202410422)

---
## 💡 Funcionalidad y Justificación Teórica

El proyecto demuestra la **eficiencia algorítmica** del Octree ($O(\sqrt[3]{n}+k)$) frente al método Naive ($O(n)$) mediante dos componentes clave:

### 1. Benchmark de Rendimiento Cuantitativo
Se ejecutó una prueba de Consulta por Rango sobre **100,000 puntos** aleatorios para medir el tiempo computacional de ambas implementaciones.

| Algoritmo | Tiempo de Ejecución (ms) | Complejidad |
| :--- | :--- | :--- |
| **Octree** | 0 ms | $O(\sqrt[3]{n}+k)$ |
| **Naive (Fuerza Bruta)** | 2 ms | $O(n)$ |
| **Mejora Observada** | **2.0x más rápido** | — |

### 2. Simulación Gráfica de Partición (Creatividad)
El programa incluye una **simulación gráfica ASCII** en la consola que proyecta los puntos en un plano 2D (XY), mostrando cómo el Octree ajusta su estructura y se subdivide únicamente en las regiones con alta densidad de datos (adaptabilidad espacial).

---
## 🛠️ Instrucciones de Compilación y Ejecución

El proyecto utiliza **CMake** para la gestión de la compilación, asegurando la portabilidad entre diferentes entornos de desarrollo (CLion, Visual Studio Code, etc.).

### Requisitos
* Compilador C++ moderno (C++17 o superior).
* CMake (versión 3.10 o superior).

### Pasos

1.  **Clonar el Repositorio (si es necesario):**
    ```bash
    git clone [https://github.com/eliseodavidv/octree_proyecto.git](https://github.com/eliseodavidv/octree_proyecto.git)
    cd octree_proyecto
    ```

2.  **Preparar y Configurar la Compilación (Usando CMake):**
    ```bash
    mkdir build
    cd build
    cmake ..
    ```

3.  **Compilar el Código Fuente:**
    ```bash
    cmake --build .
    ```

4.  **Ejecutar el Programa:**
    El programa ejecutará automáticamente la visualización y el benchmark, imprimiendo los resultados en la consola.
    ```bash
    ./octree_proyecto.exe 
    # (El nombre del ejecutable puede variar, buscar 'octree_proyecto' en la carpeta 'build')
    ```

---

## 📄 Documentación Adicional

La descripción teórica, el análisis de complejidad formal, los diagramas de flujo y la justificación de la implementación se encuentran detallados en el **Informe Final** del proyecto.
