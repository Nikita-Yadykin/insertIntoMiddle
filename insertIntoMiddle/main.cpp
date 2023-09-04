#include <iostream>
#include <mutex>

struct Node
{
    int value;
    Node* next;
    std::mutex node_mutex;

    Node(int val) : value(val), next(nullptr) {}
};

class FineGrainedQueue
{
private:
    Node* head;
    Node* tail;
    std::mutex queue_mutex;
    int length; // Добавляем переменную для хранения длины списка

public:
    FineGrainedQueue() : head(nullptr), tail(nullptr), length(0) {} // Инициализируем длину списка как 0

    void insertIntoMiddle(int value, int pos)
    {
        if (pos <= 0)
        {
            // Вставка в начало не поддерживается
            return;
        }

        // Создаем новый узел с заданным значением
        Node* newNode = new Node(value);

        // Захватываем мьютекс очереди
        std::unique_lock<std::mutex> queueLock(queue_mutex);

        if (!head) // Если список пустой, добавляем первый элемент
        {
            head = newNode;
            tail = newNode;
            length = 1; // Устанавливаем длину списка в 1
            return;
        }

        if (pos >= length) // Если pos больше или равно длине списка, вставляем элемент в конец
        {
            std::lock_guard<std::mutex> tailLock(tail->node_mutex);
            tail->next = newNode;
            tail = newNode;
            length++; // Увеличиваем длину списка
        }
        else
        {
            // Если pos меньше длины списка, вставляем элемент в середину
            Node* current = head;
            int currentIndex = 0;
            while (current->next != nullptr && currentIndex < pos - 1)
            {
                current = current->next;
                currentIndex++;
            }
            std::lock_guard<std::mutex> nodeLock(current->node_mutex);
            newNode->next = current->next;
            current->next = newNode;
            length++; // Увеличиваем длину списка
        }
    }

    void printQueue()
    {
        Node* current = head;
        while (current)
        {
            std::cout << current->value << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};

int main()
{
    FineGrainedQueue queue;

    // Вставляем элементы в середину списка
    queue.insertIntoMiddle(1, 0); // Вставить в начало (не сработает)
    queue.insertIntoMiddle(2, 1); // Вставить после 1
    queue.insertIntoMiddle(3, 2); // Вставить после 2
    queue.insertIntoMiddle(4, 1); // Вставить после 1
    queue.insertIntoMiddle(9, 4); // Вставить в конец
    queue.insertIntoMiddle(5, 10); // Вставить в конец (больше длины списка)

    // Выводим элементы очереди
    queue.printQueue();

    return 0;
}
