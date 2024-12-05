#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <iomanip>

using namespace std;

const string file = "graph.txt";
const string file2 = "graph2.txt";

struct Edge {
    int src, dest, weight;
};

class Graph {
public:
    vector<Edge> edges;  // Список рёбер
    vector<string> vertices; // Список всех вершин и их индексов

    vector<vector<pair<int, int>>> adjacencyList;

    void addEdge(const string& src, const string& dest, int weight) {
        int srcIndex = getVertexIndex(src);
        int destIndex = getVertexIndex(dest);
        edges.push_back({ srcIndex, destIndex, weight });
    }

    void generateAdjacencyList() {
        adjacencyList.clear();
        adjacencyList.resize(vertices.size());

        for (const Edge& edge : edges) {
            adjacencyList[edge.src].push_back({ edge.dest, edge.weight });
            adjacencyList[edge.dest].push_back({ edge.src, edge.weight });
        }
    }

    void printAdjacencyList() {
        generateAdjacencyList();
        for (int i = 0; i < adjacencyList.size(); ++i) {
            cout << vertices[i] << ": ";
            for (const auto& neighbor : adjacencyList[i]) {
                cout << "(" << vertices[neighbor.first] << ", " << neighbor.second << ") ";
            }
            cout << endl;
        }
    }

    vector<vector<int>> generateIncidenceMatrix() {
        // Пустая матрица (строки - вершины, стоблцы - рёбра)
        vector<vector<int>> incidenceMatrix(vertices.size(), vector<int>(edges.size(), 0));

        for (int i = 0; i < edges.size(); ++i) {
            const Edge& edge = edges[i];
            incidenceMatrix[edge.src][i] = edge.weight;
            incidenceMatrix[edge.dest][i] = edge.weight;
        }

        return incidenceMatrix;
    }

    void printIncidenceMatrix() {
        vector<vector<int>> incidenceMatrix = generateIncidenceMatrix();

        cout << "\nМатрица инцидентности:" << endl;
        cout << "      ";
        for (int i = 0; i < edges.size(); ++i)
            cout << " E" << i + 1 << "";
        cout << endl;

        for (int i = 0; i < incidenceMatrix.size(); ++i) {
            cout << setw(3) << vertices[i] << " | ";
            for (int j = 0; j < incidenceMatrix[i].size(); ++j) {
                cout << setw(2) << incidenceMatrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    void readFromFile(const string& filename) {
        ifstream file(filename);
        string line;

        if (getline(file, line)) {
            istringstream iss(line);
            string vertex;
            while (iss >> vertex) {
                addVertex(vertex);
            }
        }
        int rowIndex = 0;
        while (getline(file, line)) {
            istringstream iss(line);
            int weight;
            for (int colIndex = 0; colIndex < vertices.size(); ++colIndex) {
                iss >> weight;
                if (rowIndex < colIndex && weight > 0) {
                    addEdge(vertices[rowIndex], vertices[colIndex], weight);
                }
            }
            ++rowIndex;
        }

        file.close();
    }

    void sortEdges() {
        sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
            return a.weight < b.weight;
            });
    }

    void printEdges() const {
        for (const Edge& edge : edges) {
            cout << vertices[edge.src] << " - " << vertices[edge.dest] << ": " << edge.weight << endl;
        }
    }

    void addVertex(const string& vertex) {
        if (find(vertices.begin(), vertices.end(), vertex) == vertices.end()) {
            vertices.push_back(vertex);
        }
    }

    int getVertexIndex(const string& vertex) {
        auto it = find(vertices.begin(), vertices.end(), vertex);
        if (it == vertices.end()) {
            addVertex(vertex);
            return vertices.size() - 1;
        }
        return distance(vertices.begin(), it);
    }

    void findMinSpanningTree() {
        sortEdges();
        int numVertices = vertices.size();
        vector<int> parent(numVertices, -1);
        vector<Edge> minSpanningTree;
        int numEdgesAdded = 0;

        for (Edge edge : edges) {
            int srcParent = -1;
            int destParent = -1;

            int src = edge.src;
            int dest = edge.dest;

            while (parent[src] != -1)
                src = parent[src];
            while (parent[dest] != -1)
                dest = parent[dest];
            srcParent = src;
            destParent = dest;

            if (srcParent != destParent) {
                minSpanningTree.push_back(edge);
                numEdgesAdded++;
                parent[destParent] = srcParent;
            }

            if (numEdgesAdded == numVertices - 1)
                break;
        }

        cout << "\nМинимальное остовное дерево:" << endl;
        int totalWeight = 0;
        for (Edge edge : minSpanningTree) {
            cout << edge.src << " -- " << edge.dest << "  вес: " << edge.weight << endl;
            totalWeight += edge.weight;
        }

        cout << "суммарный вес: " << totalWeight << endl;
    }

    void DFS(int startIndex, vector<bool>& visited) { // в глубину (смотрим всю ветку до конца)
        cout << vertices[startIndex] << " ";
        visited[startIndex] = true;
        for (const Edge& edge : edges) {
            if (edge.src == startIndex && !visited[edge.dest]) {
                DFS(edge.dest, visited);
            }
        }
    }
    
    void BFS(int startIndex) { // в ширину (смотрим всех соседей)
        vector<bool> visited(vertices.size(), false);
        queue<int> q;
        q.push(startIndex);
        visited[startIndex] = true;

        while (!q.empty()) {
            int current = q.front();
            q.pop();
            cout << vertices[current] << " ";

            for (const Edge& edge : edges) {
                if (edge.src == current && !visited[edge.dest]) {
                    q.push(edge.dest);
                    visited[edge.dest] = true;
                }
            }
        }
    }
};

int main() {
    Graph graph;
    graph.readFromFile(file);

    cout << "Рёбра графа до сортировки:" << endl;
    graph.printEdges();

    cout << "\nСортировка рёбер по весу:" << endl;
    graph.sortEdges();
    graph.printEdges();

    cout << "\nСписок смежности:" << endl;
    graph.printAdjacencyList();

    graph.printIncidenceMatrix();

    graph.findMinSpanningTree();

    cout << "\nОбход в глубину (DFS):" << endl;
    vector<bool> visited(graph.vertices.size(), false);
    graph.DFS(graph.getVertexIndex("A"), visited);

    cout << "\n\nОбход в ширину (BFS):" << endl;
    graph.BFS(graph.getVertexIndex("A"));

    return 0;
}