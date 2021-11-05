#pragma once
#include "Utility.h"
#include "Lock.h"

namespace Threading {
	namespace DataStructures {

		template<typename InnerNodeType>
		class ConcurrentQueue {
		private:

			size_t q_size;

			struct Node	{
				InnerNodeType element;
				Node* next;
				Node(InnerNodeType _elem, Node* _next = nullptr) : element(_elem), next(_next) { }
			};

			Node* m_head, *m_tail;
			MutexLock m_hLock, m_tLock;

			ConcurrentQueue(const ConcurrentQueue&) = delete;
			ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

		public:
			ConcurrentQueue()	{
				Node* dummy = new Node(nullptr, nullptr);
				m_tail = dummy;
				m_head = dummy;
				q_size = 0;
			}

			~ConcurrentQueue() {
				while (Pop());
				delete m_head;
			}

			bool Empty() {
				if (m_head->next != nullptr)
					return false;
				return true;
			}

			size_t Size() {
				return q_size;
			}

			void Push(InnerNodeType item) {
				Node* node = new Node(item);
				m_tLock.Lock();
				{
					m_tail->next = node;
					m_tail = node;
				}
				q_size++;
				m_tLock.Unlock();
			}

			InnerNodeType Pop() {
				Node* node;
				InnerNodeType value;
				m_hLock.Lock();
				{
					node = m_head;
					Node* newHead = node->next;
					if (newHead == nullptr)
					{
						m_hLock.Unlock();
						return nullptr;
					}
					value = newHead->element;
					m_head = newHead;
				}
				q_size--;
				m_hLock.Unlock();
				delete node;
				return value;
			}

		};
	}
}