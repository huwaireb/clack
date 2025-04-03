#pragma once

#include <optional>

template <typename T>
class Stack {
 private:
  struct Node {
    T data;
    Node* next;
  };

  Node* head;
  size_t count;

 public:
  Stack();
  ~Stack();

  bool isEmpty() const;
  std::optional<T> pop();
  bool push(const T& item);
  bool push(T&& item);
  std::optional<T> top() const;
  size_t size() const;
};
