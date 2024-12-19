#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <stack>
#include <fstream>

using namespace std;


struct Order {
    string type;  // Тип заказа: "sale" или "purchase"
    int city;     // Город, куда отправляется заказ
    string product; // Тип товара: "phone", "tablet", "computer"
};

class Manager {
public:
    Manager(int city) : city(city) {}
    int getCity() const { return city; }
private:
    int city; // Город, в котором находится менеджер
};
class Garage {
public:
    Garage(int city) : city(city) {}
    int getCity() const { return city; }
private:
    int city; // Город, в котором находится гараж
};
class Phone {
public:
    Phone(int city) : city(city) {}
    int getCity() const { return city; }
private:
    int city; // Город, в котором находится гараж
};
class Computer {
public:
    Computer(int city) : city(city) {}
    int getCity() const { return city; }
private:
    int city; // Город, в котором находится гараж
};
class Tablet {
public:
    Tablet(int city) : city(city) {}
    int getCity() const { return city; }
private:
    int city; // Город, в котором находится гараж
};
class Graph {
public:
    Graph(int vertices);
    void addEdge(int u, int v, int weight);
    pair<int, vector<int>> dijkstra(int start, int end); // Возвращает пару (вес пути, путь)
    void showPath(const Manager& manager, const Garage& garage, const Phone& phone, const Computer& computer, const Tablet& tablet, int destination, const string& productType);
    void readGraph(const string& filename);
    vector<Order> readOrders(const string& filename);
private:
    int vertices;
    vector<vector<pair<int, int>>> adjList; // Список смежности: (город, вес)
};

Graph::Graph(int vertices) : vertices(vertices) {
    adjList.resize(vertices);
}

void Graph::addEdge(int u, int v, int weight) {
    adjList[u].emplace_back(v, weight);
    adjList[v].emplace_back(u, weight); // Если граф неориентированный
}

pair<int, vector<int>> Graph::dijkstra(int start, int end) {
    vector<int> dist(vertices, numeric_limits<int>::max());
    vector<int> prev(vertices, -1);
    vector<bool> visited(vertices, false);

    dist[start] = 0;
    for (int i = 0; i < vertices - 1; ++i) {
        int minDist = numeric_limits<int>::max();
        int minIndex = -1;
        for (int j = 0; j < vertices; ++j) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                minIndex = j;
            }
        }
        if (minIndex == -1) break; // Все вершины были посещены
        visited[minIndex] = true;
        for (const auto& neighbor : adjList[minIndex]) {
            int v = neighbor.first;
            int weight = neighbor.second;
            if (!visited[v] && dist[minIndex] + weight < dist[v]) {
                dist[v] = dist[minIndex] + weight;
                prev[v] = minIndex; // Запоминаем предшественника
            }
        }
    }
    // Восстанавливаем путь
    vector<int> path;
    for (int at = end; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());
    return { dist[end], path }; // Возвращаем вес и путь
}

void Graph::showPath(const Manager& manager, const Garage& garage, const Phone& phone, const Computer& computer, const Tablet& tablet, int destination, const string& productType) {
    int managerCity = manager.getCity();
    int garageCity = garage.getCity();
    int warehouseCity = -1;
    // Определяем склад в зависимости от типа товара
    if (productType == "phone") {
        warehouseCity = phone.getCity(); // Предположим, что склад телефонов в городе 3
    }
    else if (productType == "computer") {
        warehouseCity = computer.getCity(); // Склад компьютеров в городе 4
    }
    else if (productType == "tablet") {
        warehouseCity = tablet.getCity(); // Склад планшетов в городе 5
    }
    else {
        cout << "Неверный вид товара." << endl;
        return;
    }
    // Находим путь от менеджера до склада
    auto resultToWarehouse = dijkstra(garageCity, warehouseCity);
    // Находим путь от склада до конечной точки
    auto resultToDestination = dijkstra(warehouseCity, destination);
    if (resultToWarehouse.first != numeric_limits<int>::max() && resultToDestination.first != numeric_limits<int>::max()) {
        int totalDistance = resultToWarehouse.first + resultToDestination.first;
        cout << "Общее расстояние через склад: " << totalDistance << endl;
        cout << "Путь от менеджера до склада: ";
        for (int city : resultToWarehouse.second) {
            cout << city << " ";
        }
        cout << endl;
        cout << "Путь от склада до конечной точки: ";
        for (int city : resultToDestination.second) {
            cout << city << " ";
        }
        cout << endl;
    }
    else {
        cout << "Нет доступного пути." << endl;
    }
}

void Graph::readGraph(const string& filename) {
    ifstream file(filename); // Открываем файл
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл: " << filename << endl;
        return;
    }

    file >> vertices; // Считываем количество вершин
    adjList.clear();  // Очищаем предыдущие данные
    adjList.resize(vertices);

    int u, v, weight;
    while (file >> u >> v >> weight) {
        addEdge(u, v, weight); // Добавляем ребро в граф
    }

    file.close(); // Закрываем файл
    cout << "Данные графа успешно загружены из файла: " << filename << endl;
}

vector<Order> Graph::readOrders(const string& filename) {
    vector<Order> orders;
    ifstream file(filename); // Открываем файл
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл: " << filename << endl;
        return orders;
    }

    string type, product;
    int city;
    while (file >> type >> city >> product) {
        if (type != "sale" && type != "purchase") {
            cerr << "Ошибка: неверный тип заказа в файле: " << type << endl;
            continue;
        }
        orders.push_back({ type, city, product }); // Добавляем заказ в список
    }

    file.close(); // Закрываем файл
    cout << "Заказы успешно загружены из файла: " << filename << endl;
    return orders;
}

int main() {
    // Создаем граф
    Graph g(6); // 6 вершин: менеджер, гараж, 3 склада и одна цель
    g.addEdge(0, 1, 10); // Менеджер - Гараж
    g.addEdge(0, 2, 3);  // Менеджер - Склад телефонов
    g.addEdge(1, 2, 1);  // Гараж - Склад телефонов
    g.addEdge(1, 3, 2);  // Гараж - Склад компьютеров
    g.addEdge(2, 3, 8);  // Склад телефонов - Склад компьютеров
    g.addEdge(2, 4, 2);  // Склад телефонов - Склад планшетов
    g.addEdge(3, 4, 7);  // Склад компьютеров - Склад планшетов
    // Создаем экземпляры классов
    Manager manager(0); // Менеджер в координатах 0
    Garage garage(1);   // Гараж в координатах 1
    Phone phone(3); // Менеджер в координатах 0
    Computer computer(1);
    Tablet tablet(4);
    // Пример использования функции
    g.showPath(manager, garage, phone, computer, tablet, 4, "computer"); // Найти путь от менеджера через склад компьютеров до города 4
    return 0;
}
