# 🌲 Octree: Estructura de Datos Jerárquica para Partición Espacial Tridimensional

**Proyecto para el curso Algoritmos y Estructura de Datos, UTEC**

### Integrantes
* **Eliseo David Velasquez Diaz** (202410184)
* **Jean Luka Terrazo Santiago** (202410422)

---

## 💡 Objetivo del Proyecto

Este proyecto implementa la estructura de datos **Octree** en C++ para gestionar y consultar eficientemente grandes volúmenes de puntos en un espacio tridimensional. El Octree optimiza operaciones de búsqueda y consulta por rango al particionar el espacio recursivamente en ocho octantes, demostrando una complejidad algorítmica superior al enfoque lineal (Naive).

## 📊 Aplicación y Resultados Clave del Benchmark

El proyecto incluye una demostración de la **utilidad práctica** del Octree con dos componentes principales:

### 1. Benchmark de Consulta por Rango
Se ejecutó una comparación de rendimiento sobre **100,000 puntos** aleatorios entre el Octree y el algoritmo Naive ($O(n)$) para la consulta por rango:

| Algoritmo | Tiempo de Ejecución (ms) | Complejidad Teórica |
| :--- | :--- | :--- |
| **Octree** | 0 ms | $O(\sqrt[3]{n}+k)$ |
| **Naive** | 2 ms | $O(n)$ |
| **Speedup (en N=100K)** | **2.0x más rápido** | — |

### 2. Simulación Gráfica ASCII (Demostración Creativa)
El código incluye una función que imprime una simulación en la consola (ASCII) para visualizar cómo el Octree adapta su subdivisión a la densidad de los puntos. Las zonas con alta concentración de datos se subdividen recursivamente, demostrando la naturaleza **adaptativa** de la estructura.

## 🛠️ Cómo Compilar y Ejecutar

Este proyecto está configurado para ser compilado utilizando **CMake**.

### Requisitos

* Compilador C++ moderno (C++17 o superior, ej. g++).
* CMake (versión 3.10 o superior).

### Instrucciones de Compilación

1.  **Clonar el Repositorio:**
    ```bash
    git clone [https://github.com/eliseodavidv/octree_proyecto.git](https://github.com/eliseodavidv/octree_proyecto.git)
    cd octree_proyecto
    ```

2.  **Crear Directorio de Compilación y Configurar CMake:**
    ```bash
    mkdir build
    cd build
    cmake ..
    ```

3.  **Compilar el Proyecto:**
    ```bash
    cmake --build .
    ```

4.  **Ejecutar la Simulación y el Benchmark:**
    ```bash
    ./octree_proyecto.exe 
    # (El nombre del ejecutable puede ser 'octree_proyecto' o 'a.out' dependiendo del sistema)
    ```

---

## 📄 Documentación Adicional

El informe final completo con el análisis teórico detallado, la complejidad algorítmica y la comparación con estructuras de datos similares se encuentra en el archivo adjunto (`Octree_Informe_Final.pdf` o en el código LaTeX).

---

## 📌 Pasos para Subir el `README.md`

Una vez que hayas guardado el archivo `README.md` en la carpeta `C:\Users\Luka\octree_proyecto`, regresa a la terminal para subirlo:

```bash
git add README.md
git commit -m "Agrega el archivo README.md con la descripcion del proyecto y las instrucciones de compilacion."
git push origin main
