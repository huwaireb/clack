#include "stack.hh"

template <typename T>
Stack<T>::Stack() : head(nullptr), count(0) {}

template <typename T>
Stack<T>::~Stack() {
  while (this->head) {
    Node* next = this->head->next;
    delete head;
    this->head = next;
  }
  count = 0;
}

template <typename T>
bool Stack<T>::isEmpty() const {
  return this->head == nullptr;
}

template <typename T>
std::optional<T> Stack<T>::pop() {
  if (this->head == nullptr) return std::nullopt;

  T data = std::move(this->head->data);
  Node* next = this->head->next;
  delete head;
  this->head = next;
  count--;

  return data;
}

template <typename T>
bool Stack<T>::push(const T& item) {
  Node* new_node = new Node{item, this->head};
  this->head = new_node;
  count++;
  return true;
}

template <typename T>
bool Stack<T>::push(T&& item) {
  Node* new_node = new Node{std::move(item), this->head};
  this->head = new_node;
  count++;
  return true;
}

template <typename T>
std::optional<T> Stack<T>::top() const {
  if (this->head == nullptr) return std::nullopt;
  return this->head->data;
}

template <typename T>
size_t Stack<T>::size() const {
  return count;
}
