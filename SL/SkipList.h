
#pragma once
#include "arena.h"
#include "cmath"


template<typename Key, class Comparator>
class SkipList
{
private:

	struct Node;

public:

	explicit SkipList(Comparator cmp, Arena *arena);

	void Insert(Key key);

	bool Contains(Key key);

	class Iterator {
	public:
		 explicit Iterator(const SkipList* list);

		bool Valid();

		Key& key();

		void Next();

		void Prev();

		void Seek(const Key& target);

		void SeekToFirst();

		void SeekToLast();
 
	private:

		const SkipList* list_;
		Node* node_;
	};

private:
	enum { kMaxHeight = 12 };

	Comparator const compare_;

	Arena* const arena_;

	Node* const head_;

	int max_height_;

	int GetMaxHeight();

	//rand rnd_;

	Node* NewNode(const Key& key, int height);

	int RandomHeight();

	bool Equal(const Key& a, const Key& b) const { return (compare_(a, b) == 0); }

	bool KeyIsAfterNode(const Key& key, Node* n);

	Node* FindGreaterOrEqual(const Key& key, Node** prev);

	Node* FindLessThan(const Key& key);
	 

	SkipList(const SkipList&);

	void operator=(const SkipList&);

	
};



template<typename Key, class Comparator>
struct SkipList<Key, Comparator>::Node {

	explicit Node(const Key &k) :key(k) {};

	Key key;

	Node * Next(int n) {
		return next_[n];
	}

	void SetNext(int n, Node *x) {
		next_[n] = x;
	}

private:
	Node * next_[1];

};

template<typename Key, class Comparator>
 SkipList<Key, Comparator>::SkipList(Comparator cmp, Arena *arena) :
	compare_(cmp),
	arena_(arena),
	max_height_(1),
	head_(NewNode(0, kMaxHeight)){
	for (int i = 0; i < kMaxHeight; i++) {
		head_->SetNext(i, nullptr);
	}
}

template<typename Key, class Comparator>
void SkipList<Key, Comparator>::Insert(Key key)
{
	Node * prev[kMaxHeight];
	FindGreaterOrEqual(key, prev);
	int height = RandomHeight();
	Node * newnode = NewNode(key, height);
	int looptime;
	if (height > max_height_) {
		for (int i = max_height_; i < height; i++) {
			newnode->SetNext(i, nullptr);
		}
		looptime = max_height_;
		max_height_ = height;
	}
	else {
		looptime = height;
	}

	for (int i = 0; i < looptime;i++) {
		newnode->SetNext(i, prev[i]->Next(i));
		prev[i]->SetNext(i , newnode);
	}
	


}

template<typename Key, class Comparator>
bool SkipList<Key, Comparator>::Contains(Key key)
{
	return false;
}

template<typename Key, class Comparator>
SkipList<Key, Comparator>::Iterator::Iterator(const SkipList * list)
{
	list_ = list;
	node_ = nullptr;
}

template<typename Key, class Comparator>
bool SkipList<Key, Comparator>::Iterator::Valid()
{
	return node_ != nullptr;
}

template<typename Key, class Comparator>
inline Key & SkipList<Key, Comparator>::Iterator::key()
{
	assert(Valid());
	return node_->key;
}

template<typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::Next()
{
	assert(Valid());
	node_ = node_->Next(0);
}

template<typename Key, class Comparator>
void SkipList<Key, Comparator>::Iterator::Prev()
{
	assert(Valid());
	node_ = list_->FindLessThan(node_->key);
	if (node_ == list_->head_) {
		node_ = nullptr;
	}
}

template<typename Key, class Comparator>
void SkipList<Key, Comparator>::Iterator::Seek(const Key & target)
{
	node_ = list_->FindGreaterOrEqual(target, nullptr);
}


template<typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::SeekToFirst()
{
	node_ = list_->head_->Next(0);
}

template<typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::SeekToLast()
{
	node_ = list_->FindLast();
	if (node_ == list_->head_) {
		node_ = nullptr;
	}
} 

template<typename Key, class Comparator>
inline int SkipList<Key, Comparator>::GetMaxHeight()
{
	return max_height_;
}

template<typename Key, class Comparator> typename SkipList<Key, Comparator>::Node*
SkipList<Key, Comparator>::NewNode(const Key& key, int height) {

	void *mem = arena_->AllocateAligned(sizeof(Node) + sizeof(Node*)*(height - 1));
	return new(mem) Node(key);
}

template<typename Key, class Comparator>
inline int SkipList<Key, Comparator>::RandomHeight()
{
	static const unsigned int kBranching = 4;
	int height = 1;
	while (height < kMaxHeight && ((rand() % kBranching) == 0)) {
		height++;
	}
	assert(height > 0);
	assert(height <= kMaxHeight);
	return height;
}

template<typename Key, class Comparator>
bool SkipList<Key, Comparator>::KeyIsAfterNode(const Key & key, Node * n)
{
	return (n != nullptr) && (compare_(n->key, key) < 0);
}

template<typename Key, class Comparator> typename SkipList<Key, Comparator>::Node * 
SkipList<Key, Comparator>::FindGreaterOrEqual(const Key & key, Node ** prev)
{
	Node *x = head_;
	int level = GetMaxHeight()-1;
	while (true) {
		Node* next = x->Next(level);
		if (KeyIsAfterNode(key, next)) {
			x = next;
		}
		else {
			if (prev != nullptr) prev[level] = x;
			if (level == 0) {
				return next;
			}
			else {
				level--;
			}
		}
	}
	
}

template<typename Key, class Comparator> typename SkipList<Key, Comparator>::Node * 
SkipList<Key, Comparator>::FindLessThan(const Key & key)
{
	Node *x = head_;
	int level = GetMaxHeight() - 1;
	while (true) {
		Node* next = x->Next(level);
		if (KeyIsAfterNode(key, next)) {
			x = next;
		}
		else {
			if (level == 0) {
				return x;
			}
			else {
				level--;
			}
		}
	}
}



/*
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_SKIPLIST_H_
#define STORAGE_LEVELDB_DB_SKIPLIST_H_

// Thread safety
// -------------
//
// Writes require external synchronization, most likely a mutex.
// Reads require a guarantee that the SkipList will not be destroyed
// while the read is in progress.  Apart from that, reads progress
// without any internal locking or synchronization.
//
// Invariants:
//
// (1) Allocated nodes are never deleted until the SkipList is
// destroyed.  This is trivially guaranteed by the code since we
// never delete any skip list nodes.
//
// (2) The contents of a Node except for the next/prev pointers are
// immutable after the Node has been linked into the SkipList.
// Only Insert() modifies the list, and it is careful to initialize
// a node and use release-stores to publish the nodes in one or
// more lists.
//
// ... prev vs. next pointer ordering ...

#include <assert.h>
#include <stdlib.h>

#include "arena.h"




	class Arena;

	template<typename Key, class Comparator>
	class SkipList {
	private:
		struct Node;

	public:
		// Create a new SkipList object that will use "cmp" for comparing keys,
		// and will allocate memory using "*arena".  Objects allocated in the arena
		// must remain allocated for the lifetime of the skiplist object.
		explicit SkipList(Comparator cmp, Arena* arena);

		// Insert key into the list.
		// REQUIRES: nothing that compares equal to key is currently in the list.
		void Insert(const Key& key);

		// Returns true iff an entry that compares equal to key is in the list.
		bool Contains(const Key& key) const;

		// Iteration over the contents of a skip list
		class Iterator {
		public:
			// Initialize an iterator over the specified list.
			// The returned iterator is not valid.
			explicit Iterator(const SkipList* list);

			// Returns true iff the iterator is positioned at a valid node.
			bool Valid() const;

			// Returns the key at the current position.
			// REQUIRES: Valid()
			const Key& key() const;

			// Advances to the next position.
			// REQUIRES: Valid()
			void Next();

			// Advances to the previous position.
			// REQUIRES: Valid()
			void Prev();

			// Advance to the first entry with a key >= target
			void Seek(const Key& target);

			// Position at the first entry in list.
			// Final state of iterator is Valid() iff list is not empty.
			void SeekToFirst();

			// Position at the last entry in list.
			// Final state of iterator is Valid() iff list is not empty.
			void SeekToLast();

		private:
			const SkipList* list_;
			Node* node_;
			// Intentionally copyable
		};

	private:
		enum { kMaxHeight = 12 };

		// Immutable after construction
		Comparator const compare_;
		Arena* const arena_;    // Arena used for allocations of nodes

		Node* const head_;

		// Modified only by Insert().  Read racily by readers, but stale
		// values are ok.
		AtomicPointer max_height_;   // Height of the entire list

		inline int GetMaxHeight() const {
			return static_cast<int>(
				reinterpret_cast<intptr_t>(max_height_.NoBarrier_Load()));
		}

		// Read/written only by Insert().
		

		Node* NewNode(const Key& key, int height);
		int RandomHeight();
		bool Equal(const Key& a, const Key& b) const { return (compare_(a, b) == 0); }

		// Return true if key is greater than the data stored in "n"
		bool KeyIsAfterNode(const Key& key, Node* n) const;

		// Return the earliest node that comes at or after key.
		// Return nullptr if there is no such node.
		//
		// If prev is non-null, fills prev[level] with pointer to previous
		// node at "level" for every level in [0..max_height_-1].
		Node* FindGreaterOrEqual(const Key& key, Node** prev) const;

		// Return the latest node with a key < key.
		// Return head_ if there is no such node.
		Node* FindLessThan(const Key& key) const;

		// Return the last node in the list.
		// Return head_ if list is empty.
		Node* FindLast() const;

		// No copying allowed
		SkipList(const SkipList&);
		void operator=(const SkipList&);
	};

	// Implementation details follow
	template<typename Key, class Comparator>
	struct SkipList<Key, Comparator>::Node {
		explicit Node(const Key& k) : key(k) { }

		Key const key;

		// Accessors/mutators for links.  Wrapped in methods so we can
		// add the appropriate barriers as necessary.
		Node* Next(int n) {
			assert(n >= 0);
			// Use an 'acquire load' so that we observe a fully initialized
			// version of the returned Node.
			return reinterpret_cast<Node*>(next_[n].Acquire_Load());
		}
		void SetNext(int n, Node* x) {
			assert(n >= 0);
			// Use a 'release store' so that anybody who reads through this
			// pointer observes a fully initialized version of the inserted node.
			next_[n].Release_Store(x);
		}

		// No-barrier variants that can be safely used in a few locations.
		Node* NoBarrier_Next(int n) {
			assert(n >= 0);
			return reinterpret_cast<Node*>(next_[n].NoBarrier_Load());
		}
		void NoBarrier_SetNext(int n, Node* x) {
			assert(n >= 0);
			next_[n].NoBarrier_Store(x);
		}

	private:
		// Array of length equal to the node height.  next_[0] is lowest level link.
		AtomicPointer next_[1];
	};

	template<typename Key, class Comparator>
	typename SkipList<Key, Comparator>::Node*
		SkipList<Key, Comparator>::NewNode(const Key& key, int height) {
		char* mem = arena_->AllocateAligned(
			sizeof(Node) + sizeof(AtomicPointer) * (height - 1));
		return new (mem) Node(key);
	}

	template<typename Key, class Comparator>
	inline SkipList<Key, Comparator>::Iterator::Iterator(const SkipList* list) {
		list_ = list;
		node_ = nullptr;
	}

	template<typename Key, class Comparator>
	inline bool SkipList<Key, Comparator>::Iterator::Valid() const {
		return node_ != nullptr;
	}

	template<typename Key, class Comparator>
	inline const Key& SkipList<Key, Comparator>::Iterator::key() const {
		assert(Valid());
		return node_->key;
	}

	template<typename Key, class Comparator>
	inline void SkipList<Key, Comparator>::Iterator::Next() {
		assert(Valid());
		node_ = node_->Next(0);
	}

	template<typename Key, class Comparator>
	inline void SkipList<Key, Comparator>::Iterator::Prev() {
		// Instead of using explicit "prev" links, we just search for the
		// last node that falls before key.
		assert(Valid());
		node_ = list_->FindLessThan(node_->key);
		if (node_ == list_->head_) {
			node_ = nullptr;
		}
	}

	template<typename Key, class Comparator>
	inline void SkipList<Key, Comparator>::Iterator::Seek(const Key& target) {
		node_ = list_->FindGreaterOrEqual(target, nullptr);
	}

	template<typename Key, class Comparator>
	inline void SkipList<Key, Comparator>::Iterator::SeekToFirst() {
		node_ = list_->head_->Next(0);
	}

	template<typename Key, class Comparator>
	inline void SkipList<Key, Comparator>::Iterator::SeekToLast() {
		node_ = list_->FindLast();
		if (node_ == list_->head_) {
			node_ = nullptr;
		}
	}

	template<typename Key, class Comparator>
	int SkipList<Key, Comparator>::RandomHeight() {
		// Increase height with probability 1 in kBranching
		static const unsigned int kBranching = 4;
		int height = 1;
		while (height < kMaxHeight && ((rand() % kBranching) == 0)) {
			height++;
		}
		assert(height > 0);
		assert(height <= kMaxHeight);
		return height;
	}

	template<typename Key, class Comparator>
	bool SkipList<Key, Comparator>::KeyIsAfterNode(const Key& key, Node* n) const {
		// null n is considered infinite
		return (n != nullptr) && (compare_(n->key, key) < 0);
	}

	template<typename Key, class Comparator>
	typename SkipList<Key, Comparator>::Node* SkipList<Key, Comparator>::FindGreaterOrEqual(const Key& key, Node** prev)
		const {
		Node* x = head_;
		int level = GetMaxHeight() - 1;
		while (true) {
			Node* next = x->Next(level);
			if (KeyIsAfterNode(key, next)) {
				// Keep searching in this list
				x = next;
			}
			else {
				if (prev != nullptr) prev[level] = x;
				if (level == 0) {
					return next;
				}
				else {
					// Switch to next list
					level--;
				}
			}
		}
	}

	template<typename Key, class Comparator>
	typename SkipList<Key, Comparator>::Node*
		SkipList<Key, Comparator>::FindLessThan(const Key& key) const {
		Node* x = head_;
		int level = GetMaxHeight() - 1;
		while (true) {
			assert(x == head_ || compare_(x->key, key) < 0);
			Node* next = x->Next(level);
			if (next == nullptr || compare_(next->key, key) >= 0) {
				if (level == 0) {
					return x;
				}
				else {
					// Switch to next list
					level--;
				}
			}
			else {
				x = next;
			}
		}
	}

	template<typename Key, class Comparator>
	typename SkipList<Key, Comparator>::Node* SkipList<Key, Comparator>::FindLast()
		const {
		Node* x = head_;
		int level = GetMaxHeight() - 1;
		while (true) {
			Node* next = x->Next(level);
			if (next == nullptr) {
				if (level == 0) {
					return x;
				}
				else {
					// Switch to next list
					level--;
				}
			}
			else {
				x = next;
			}
		}
	}

	template<typename Key, class Comparator>
	SkipList<Key, Comparator>::SkipList(Comparator cmp, Arena* arena)
		: compare_(cmp),
		arena_(arena),
		// any key will do 
		head_(NewNode(0 , kMaxHeight)),
		max_height_(reinterpret_cast<void*>(1)) {
		for (int i = 0; i < kMaxHeight; i++) {
			head_->SetNext(i, nullptr);
		}
	}

	template<typename Key, class Comparator>
	void SkipList<Key, Comparator>::Insert(const Key& key) {
		// TODO(opt): We can use a barrier-free variant of FindGreaterOrEqual()
		// here since Insert() is externally synchronized.
		Node* prev[kMaxHeight];
		Node* x = FindGreaterOrEqual(key, prev);

		// Our data structure does not allow duplicate insertion
		assert(x == nullptr || !Equal(key, x->key));

		int height = RandomHeight();
		if (height > GetMaxHeight()) {
			for (int i = GetMaxHeight(); i < height; i++) {
				prev[i] = head_;
			}
			//fprintf(stderr, "Change height from %d to %d\n", max_height_, height);

			// It is ok to mutate max_height_ without any synchronization
			// with concurrent readers.  A concurrent reader that observes
			// the new value of max_height_ will see either the old value of
			// new level pointers from head_ (nullptr), or a new value set in
			// the loop below.  In the former case the reader will
			// immediately drop to the next level since nullptr sorts after all
			// keys.  In the latter case the reader will use the new node.
			max_height_.NoBarrier_Store(reinterpret_cast<void*>(height));
		}

		x = NewNode(key, height);
		for (int i = 0; i < height; i++) {
			// NoBarrier_SetNext() suffices since we will add a barrier when
			// we publish a pointer to "x" in prev[i].
			x->NoBarrier_SetNext(i, prev[i]->NoBarrier_Next(i));
			prev[i]->SetNext(i, x);
		}
	}

	template<typename Key, class Comparator>
	bool SkipList<Key, Comparator>::Contains(const Key& key) const {
		Node* x = FindGreaterOrEqual(key, nullptr);
		if (x != nullptr && Equal(key, x->key)) {
			return true;
		}
		else {
			return false;
		}
	}



#endif  // STORAGE_LEVELDB_DB_SKIPLIST_H_
	*/