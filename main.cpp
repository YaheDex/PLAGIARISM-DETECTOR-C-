/*
Este programa compara archivos de texto en una carpeta llamada "dataset"
y genera un archivo HTML que muestra los pares de documentos más similares,
resaltando las subcadenas comunes entre ellos. Utiliza tres métricas para 
evaluar la similitud: 
1. Similitud basada en subcadenas comunes.
2. Distancia de edición (Levenshtein).
3. Contención de Broder.
Complejidad temporal:
- La generación de la matriz de similitud es O(n^2 * m * k), donde n es 
  el número de documentos, m es la longitud promedio de los documentos, 
  y k es la longitud mínima de las subcadenas comunes.
- La distancia de edición tiene una complejidad de O(a * b), donde a y b 
  son las longitudes de los dos textos a comparar.
- La contención de Broder es O(m * n) para calcular las subcadenas.

Complejidad espacial:
- O(n) para almacenar los documentos.
- O(n^2) para la matriz de similitud.
- O(m * n) para las estructuras temporales usadas en la distancia de edición 
  y la contención de Broder.

Ejecución:
Dado el uso de algunas funciones Lambda, el código a ejecutar para el compilado es el siguiente:
    g++ -std=c++17 -lstdc++fs -o plagiarism_detector main.cpp
Probado en un procesador i7 13650hx, tardó cerca de 1 minuto en compilar.  
 */

#include <iostream>          // Librería para flujo de entrada/salida
#include <fstream>           // Librería para leer/escribir archivos
#include <vector>            // Librería para utilizar el contenedor vector
#include <string>            // Librería para manipulación de cadenas de texto
#include <algorithm>         // Librería para algoritmos, como sort
#include <iomanip>           // Librería para controlar la precisión de la salida
#include <filesystem>        // Librería para operaciones con archivos y carpetas (C++17)
#include <unordered_set>     // Librería para el contenedor unordered_set
#include <sstream>           // Librería para usar stringstream

using namespace std;         // Espacio de nombres estándar
namespace fs = std::filesystem; // Alias para filesystem, para simplificar

// Función para leer el contenido completo de un archivo
string readFile(const string &filename) {
    ifstream file(filename);         // Abrimos el archivo de entrada
    stringstream buffer;             // Creamos un buffer de stringstream
    buffer << file.rdbuf();          // Leemos el contenido del archivo en el buffer
    return buffer.str();             // Devolvemos el contenido completo como una cadena
}

// Función para encontrar todas las subcadenas comunes de al menos `minLength` entre dos cadenas
vector<string> findCommonSubstrings(const string &str1, const string &str2, int minLength) {
    unordered_set<string> substrings;          // Set para almacenar subcadenas únicas comunes
    int m = str1.size();                       // Longitud de la primera cadena
    int n = str2.size();                       // Longitud de la segunda cadena
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0)); // Tabla DP para longitudes de subcadenas comunes

    // Llenamos la tabla DP para encontrar todas las subcadenas comunes
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (str1[i - 1] == str2[j - 1]) {      // Comprobamos si los caracteres coinciden
                dp[i][j] = dp[i - 1][j - 1] + 1;   // Extendemos la longitud de la subcadena en la tabla DP
                if (dp[i][j] >= minLength) {       // Si la longitud es mayor o igual a minLength
                    substrings.insert(str1.substr(i - dp[i][j], dp[i][j])); // Insertamos la subcadena común
                }
            }
        }
    }

    return vector<string>(substrings.begin(), substrings.end()); // Devolvemos el set como un vector
}

// Función para calcular la métrica de similitud entre dos cadenas basada en subcadenas comunes
double similarityMetric(const string &str1, const string &str2, int minLength) {
    // Obtenemos todas las subcadenas comunes de longitud >= minLength
    vector<string> commonSubstrings = findCommonSubstrings(str1, str2, minLength); 
    int totalLength = 0;                      // Variable para acumular la longitud total de subcadenas comunes
    for (const auto &substring : commonSubstrings) {
        totalLength += substring.size();      // Sumamos la longitud de cada subcadena a totalLength
    }
    int maxLength = max(str1.size(), str2.size()); // Obtenemos la longitud de la cadena más larga entre ambas
    return static_cast<double>(totalLength) / maxLength; // Calculamos la proporción de similitud
}

// Función para calcular la distancia de edición (Levenshtein)
int editDistance(const string &str1, const string &str2) {
    int m = str1.size();
    int n = str2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 0; i <= m; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i == 0) {
                dp[i][j] = j; // Si str1 está vacío, insertamos todos los caracteres de str2
            } else if (j == 0) {
                dp[i][j] = i; // Si str2 está vacío, eliminamos todos los caracteres de str1
            } else if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1]; // Sin costo si son iguales
            } else {
                dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]}); // Inserción, eliminación o sustitución
            }
        }
    }

    return dp[m][n]; // Devolvemos la distancia de edición
}

// Función para calcular la métrica de contención de Broder
double broderContainment(const string &str1, const string &str2) {
    unordered_set<string> substrings1;
    unordered_set<string> substrings2;

    // Generamos subcadenas de str1
    for (size_t i = 0; i < str1.size(); ++i) {
        for (size_t j = i + 1; j <= str1.size(); ++j) {
            substrings1.insert(str1.substr(i, j - i));
        }
    }

    // Generamos subcadenas de str2
    for (size_t i = 0; i < str2.size(); ++i) {
        for (size_t j = i + 1; j <= str2.size(); ++j) {
            substrings2.insert(str2.substr(i, j - i));
        }
    }

    // Contamos las subcadenas de str1 que están en str2
    int countContained = 0;
    for (const auto &s : substrings1) {
        if (substrings2.count(s)) {
            countContained++;
        }
    }

    return static_cast<double>(countContained) / substrings1.size(); // Proporción de subcadenas contenidas
}

// Función para generar una matriz de similitud para un vector de documentos
vector<vector<double>> generateSimilarityMatrix(const vector<string> &documents, int minLength) {
    int n = documents.size();                 // Número de documentos
    vector<vector<double>> similarityMatrix(n, vector<double>(n, 0.0)); // Matriz n x n inicializada en 0.0

    // Calculamos la similitud para cada par de documentos
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double similarity = similarityMetric(documents[i], documents[j], minLength); // Calculamos similitud
            similarityMatrix[i][j] = similarity;      // Asignamos el valor de similitud en la matriz
            similarityMatrix[j][i] = similarity;      // Reflejamos el valor para hacer simétrica la matriz
        }
    }
    return similarityMatrix; // Devolvemos la matriz de similitud
}

// Función para resaltar subcadenas similares entre dos textos en un formato HTML
string highlightSimilarities(const string &str1, const string &str2, int minLength) {
    // Obtenemos las subcadenas comunes de longitud >= minLength
    vector<string> commonSubstrings = findCommonSubstrings(str1, str2, minLength); 
    string highlightedStr1 = str1;         // Copia de la primera cadena para resaltar
    string highlightedStr2 = str2;         // Copia de la segunda cadena para resaltar

    // Insertamos las etiquetas <mark> alrededor de cada subcadena común en ambas cadenas
    for (const auto &substring : commonSubstrings) {
        size_t pos1 = highlightedStr1.find(substring); // Encontramos la posición en la primera cadena
        size_t pos2 = highlightedStr2.find(substring); // Encontramos la posición en la segunda cadena
        if (pos1 != string::npos) {                     // Si se encuentra la subcadena en la primera cadena
            highlightedStr1.insert(pos1, "<mark>");     // Insertamos la etiqueta de apertura
            highlightedStr1.insert(pos1 + substring.size() + 6, "</mark>"); // Etiqueta de cierre
        }
        if (pos2 != string::npos) {                     // Si se encuentra la subcadena en la segunda cadena
            highlightedStr2.insert(pos2, "<mark>");     // Insertamos la etiqueta de apertura
            highlightedStr2.insert(pos2 + substring.size() + 6, "</mark>"); // Etiqueta de cierre
        }
    }

    // Devolvemos el resultado formateado en HTML
    return "<h3>Texto 1:</h3><p>" + highlightedStr1 + "</p><h3>Texto 2:</h3><p>" + highlightedStr2 + "</p>";
}

// Función para comparar pares de documentos basado en la matriz de similitud
bool comparePairs(const pair<int, int> &a, const pair<int, int> &b, const vector<vector<double>> &similarityMatrix) {
    // Retorna true si la similitud del par `a` es mayor que la del par `b`
    return similarityMatrix[a.first][a.second] > similarityMatrix[b.first][b.second]; 
}

// Función principal
int main() {
    // Vector para almacenar el contenido de todos los documentos de la carpeta "dataset"
    vector<string> documents;
    for (const auto &entry : fs::directory_iterator("dataset")) { // Iteramos cada archivo en la carpeta "dataset"
        documents.push_back(readFile(entry.path().string()));    // Leemos y añadimos el contenido al vector documents
    }

    // Definimos la longitud mínima de subcadenas comunes para la comparación
    int minLength = 5;

    // Generamos la matriz de similitud para todos los pares de documentos
    vector<vector<double>> similarityMatrix = generateSimilarityMatrix(documents, minLength);

    // Vector para almacenar todos los pares posibles de documentos
    vector<pair<int, int>> mostSimilarPairs;
    for (int i = 0; i < documents.size(); ++i) {
        for (int j = i + 1; j < documents.size(); ++j) {
            mostSimilarPairs.push_back({i, j});  // Añadimos cada par único (i, j) al vector
        }
    }

    // Ordenamos los pares de documentos por similitud en orden descendente
    sort(mostSimilarPairs.begin(), mostSimilarPairs.end(), [&](const pair<int, int> &a, const pair<int, int> &b) {
        return comparePairs(a, b, similarityMatrix); // Utilizamos lambda para pasar la matriz de similitud por referencia
    });

    // Creamos un archivo HTML para mostrar los pares de documentos más similares
    ofstream htmlFile("similar_texts.html");   // Creamos el archivo HTML de salida
    htmlFile << "<html><head><title>Textos Más Similares</title></head><body>"; // Encabezado HTML
    htmlFile << "<h1>10 Pares de Textos Más Similares</h1>";                   // Título del reporte de similitud

    // Añadimos los 10 pares de documentos más similares al archivo HTML
    for (int k = 0; k < 10 && k < mostSimilarPairs.size(); ++k) {
        int i = mostSimilarPairs[k].first;     // Índice del primer documento en el par
        int j = mostSimilarPairs[k].second;    // Índice del segundo documento en el par
        double editDist = editDistance(documents[i], documents[j]); // Calculamos la distancia de edición
        double broderCont = broderContainment(documents[i], documents[j]); // Calculamos la contención de Broder

        htmlFile << "<h2>Par " << k + 1 << " (Similitud: " << fixed << setprecision(2) << similarityMatrix[i][j]
                  << ", Distancia de Edición: " << editDist 
                  << ", Contención de Broder: " << fixed << setprecision(2) << broderCont << ")</h2>";
        htmlFile << highlightSimilarities(documents[i], documents[j], minLength); // Resaltamos las secciones comunes
    }

    htmlFile << "</body></html>";              // Cierre del HTML
    htmlFile.close();                          // Cerramos el archivo HTML

    cout << "Archivo HTML generado: similar_texts.html" << endl; // Mensaje de confirmación

    return 0;  // Fin del programa
}
