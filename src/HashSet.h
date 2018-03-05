
#ifndef HASHSET_H
#define HASHSET_H

#include <vector>
#include <algorithm>
#include <random>
#include <initializer_list>
#include "SList.h"


/*
	THINGS TO DO:
	1. remove dependancy from SList.h
	2. change return value of iterator operator ->
*/
template <typename val_type, 
	typename prehash = std::hash<val_type> >

	class HashSet {

	private:

		typedef typename SList<val_type>::iterator list_iterator;

		
		std::vector< SList<val_type> > set;
		unsigned int elements;
		static const unsigned int starting_size;
		prehash prehasher;
		int rand1, rand2;

		int start_pos;
		int end_pos;

		float load_factor;
	

	private:

		template<typename val_type>
		class forward_iterator :
			public std::iterator<std::forward_iterator_tag, SList<val_type> > {

			friend class HashSet; // to access private constructors
		public:
			forward_iterator() : 
				ptr(nullptr), 
				parent(nullptr),
				pos(-1) 
			{}

			forward_iterator(const forward_iterator* rhs) :
				ptr(rhs->ptr),
				parent(rhs->parent),
				ptr_it(rhs->ptr_it),
				pos(rhs->pos)
			{}
			forward_iterator(const forward_iterator& rhs) :
				ptr(rhs.ptr),
				parent(rhs.parent),
				ptr_it(rhs.ptr_it),
				pos(rhs.pos)
			{}

		private:
			forward_iterator(pointer p, list_iterator liter, HashSet* prnt, long long int pos_) : ptr(p),
				ptr_it(liter),
				parent(prnt),
				pos(pos_)
			{}
			forward_iterator(reference p, list_iterator liter, HashSet* prnt, long long int pos_) :
				ptr(p),
				ptr_it(liter),
				parent(prnt),
				pos(pos_)
			{}

		
		public:
			

			forward_iterator& operator=(forward_iterator rhs) {
				ptr = rhs.ptr;
				ptr_it = rhs.ptr_it;
				parent = rhs.parent;
				pos = rhs.pos;
				return *this;
			}
			forward_iterator operator++() {
				if (ptr == nullptr) {
					throw std::exception("error advancing end iterator");
				}

				if (ptr_it != ptr->end()) {
					++(ptr_it);
				}
				if (ptr_it == ptr->end()) {
					++pos;
					if (pos >= parent->set.size()) {
						ptr = nullptr;
						ptr_it = ptr->end();
						return *this;
					}
					while (parent->set[pos].empty()) {
						++pos;
						if (pos >= parent->set.size()) {
							ptr = nullptr;
							ptr_it = ptr->end();
							return *this;
						}
					}
					ptr = &(parent->set[pos]);
					ptr_it = ptr->begin();
				}

				return *this;
			}

			forward_iterator operator++(int) {
				if (ptr == nullptr) {
					throw std::exception("error advancing end iterator");
				}
				forward_iterator temp(this);
				operator++();
				return temp;
			}
			bool operator ==(const forward_iterator rhs) const {
				return (ptr == rhs.ptr) && (ptr_it == rhs.ptr_it);
			}
			bool operator !=(const forward_iterator rhs) const {
				return !(*this == rhs);
			}
			const val_type& operator*() const {
				if (ptr == nullptr) {
					throw std::exception("error dereferencing an invalid iterator");
				}
				return *ptr_it;
			}
			const val_type& operator->() const {
				if (ptr == nullptr) {
					throw std::exception("error dereferencing an invalid iterator");
				}
				return &(*ptr_it);
			}

		private:
			pointer ptr;
			list_iterator ptr_it;
			HashSet* parent;
			long long int pos;
		};


	public:

		typedef forward_iterator<val_type> iterator;
		typedef forward_iterator<const val_type> const_iterator;

		HashSet(float factor = 0.75f) :
			load_factor(factor),
			elements(0),
			start_pos(2147483647),
			end_pos(-1),
			set(starting_size) {
			if (load_factor >= 1) {
				load_factor = 0.75f; // default value
			}
			std::random_device rd;
			std::mt19937 generator(rd());
			std::uniform_int_distribution<int> U(20, 10000);
			rand1 = U(generator);
			rand2 = U(generator);
		}

		HashSet(const std::initializer_list<val_type>& list)
			: HashSet()

		{
			for (const auto& element : list) {
				insert(element);
			}
		}

		~HashSet() {

		}
		HashSet(const HashSet& oth) :
			set(oth.set),
			elements(oth.elements),
			prehasher(oth.prehasher),
			rand1(oth.rand1),
			rand2(oth.rand2),
			start_pos(oth.start_pos),
			end_pos(oth.end_pos)
		{
			std::random_device rd;
			std::mt19937 generator(rd());
			std::uniform_int_distribution<int> U(20, 10000);
			rand1 = U(generator);
			rand2 = U(generator);
		}
		HashSet(const HashSet&& oth) :
			set(std::move(oth.set)),
			elements(0),
			rand1(0),
			rand2(0),
			start_pos(0),
			end_pos(0),
			load_factor(0)
		{

			std::swap(load_factor, oth.load_factor);
			std::swap(elements, oth.elements);
			std::swap(prehasher, oth.prehasher);
			std::swap(rand1, oth.rand1);
			std::swap(rand2, oth.rand2);
			std::swap(start_pos, oth.start_pos);
			std::swap(end_pos, oth.end_pos);

		}

		HashSet& operator=(const HashSet& rhs) {
			load_factor = rhs.load_factor;
			set = rhs.set;
			elements = rhs.elements;
			starting_size = rhs.starting_size;
			prahasher = rhs.prehasher;
			rand1 = rhs.rand1;
			rand2 = rhs.rand2;
			start_pos = rhs.start_pos;
			end_pos = rhs.end_pos;
		}
		HashSet& operator=(const HashSet&& rhs) {
			set = std::move(rhs.set);
			elements = rhs.elements;
			prahasher = rhs.prehasher;
			rand1 = rhs.rand1;
			rand2 = rhs.rand2;
			start_pos = rhs.start_pos;
			end_pos = rhs.end_pos;
		}

		iterator find(const val_type& value) { 
			long long int prehashKey = prehasher(value);
			long long int trueKey = hash(prehashKey, set.size());

			list_iterator list_iter = set[trueKey].find(value);
			
			if (list_iter == set[trueKey].end()) {
				return end();
			}
			else {
				iterator temp(&set[trueKey], list_iter, this, trueKey);
				return temp;
			}
		}

		iterator begin() {
			if (empty()) {
				return end();
			}
			iterator it(&set[start_pos], set[start_pos].begin(), this, start_pos);
			return it;
		}

		iterator end() {
			return iterator();
		}

		void insert(const val_type& value) { 
			long long int prehashKey = prehasher(value);
			long long int trueKey = hash(prehashKey, set.size());

			if (set[trueKey].find(value) == set[0].end()) {

				update_start_and_end(trueKey);

				set[trueKey].push_front(value);
				++elements;
				grow();
			}
		}
		void insert(const val_type&& value) { 
			long long int prehashKey = prehasher(value);
			long long int trueKey = hash(prehashKey, set.size());

			if (set[trueKey].find(value) == set[0].end()) {

				update_start_and_end(trueKey);
				set[trueKey].push_front(std::move(value));
				++elements;
				grow();
			}
		}
		void insert(const std::initializer_list<val_type>& list) {
			for (const auto& element : list) {
				insert(element);
			}
		}
		void insert(const std::initializer_list<val_type>&& list) {
			for (const auto& element : list) {
				insert(std::move(element));
			}
		}
		

		void erase(const val_type& value) { 
			if (empty()) {
				return;
			}

			long long int prehashKey = prehasher(value);
			long long int trueKey = hash(prehashKey, set.size());
			if (set[trueKey].erase(value)) {
				--elements;
				if (start_pos == trueKey) {
					update_start_pos();
				}
				if (end_pos == trueKey) {
					update_end_pos();
				}
				shrink();
			}
		}

		unsigned int buckets() {
			return (unsigned int)set.size();
		}


		void traverse() {
			for (const auto& bucket : set) {
				bucket.print();
			}
		}

		int size() {
			return elements;
		}

		bool empty() {
			return elements == 0;
		}
		void rehash(unsigned int n) {
			if (n <= set.size()) {
				return;
			}
			start_pos = 2147483647;
			end_pos = -1;
			int possibility = (int)((float)set.size() * 1.15f);
			if (possibility > n) {
				n = possibility;
			}
			std::vector<SList< val_type > > temp(n);
			for (const auto& bucket : set) {
				for (const auto& element : bucket) {

					long long int prehashKey = prehasher(element);
					long long int trueKey = hash(prehashKey, (unsigned int)temp.size());

					update_start_and_end(trueKey);

					temp[trueKey].push_front(element);
				}
			}
			set = std::move(temp);
		}
		void clear() {
			std::vector<SList<val_type>> new_set(starting_size);
			set = std::move(new_set);
			elements = 0;
			start_pos = INT_MAX;
			end_pos = -1;
		}

		void reserve(unsigned int n) {
			if (n < (float)starting_size *load_factor) {
				return;
			}
			unsigned int new_size = (unsigned int)((float)(n+1) / load_factor);
			rehash(new_size);
		}


	private:

		void update_start_and_end(long long int trueKey) {
			if (start_pos > trueKey) {
				start_pos = trueKey;
			}
			if (end_pos < trueKey) {
				end_pos = trueKey;
			}
		}

		void update_start_pos() {
			if (empty()) {
				start_pos = 2147483647;
				return;
			}
			if (elements == 1) {
				start_pos = end_pos;
				return;
			}
			while (start_pos < end_pos) {
				if (set[++start_pos].empty() == false) {
					return;
				}
			}
		}
		void update_end_pos() {
			if (empty()) {
				end_pos = -1;
				return;
			}
			if (elements == 1) {
				end_pos = start_pos;
				return;
			}
			while (end_pos > start_pos) {
				if (set[--end_pos].empty() == false) {
					return;
				}
			}
		}

		


		long long int hash(long long int prehashKey, size_t size) {
			long long int oldKey = prehashKey;
			int prime = 8188057;

			long long int trueKey = (rand1 * oldKey + rand2);
			trueKey %= prime;
			trueKey %= size;
			return trueKey;
		}

		void shrink()  { 
			if (set.size() > starting_size && set.size() >= elements * 4) {
				start_pos = 2147483647;
				end_pos = -1;
				std::vector<SList<val_type>> temp(set.size() / 2);
				for (const auto& bucket : set) {
					for (const auto& element : bucket) {

						long long int prehashKey = prehasher(element);
						long long int trueKey = hash(prehashKey, (unsigned int)temp.size());

						update_start_and_end(trueKey);

						temp[trueKey].push_front(element);
					}
				}
				set = std::move(temp);
			}
		}


		void grow() { 
		
		
			if((float)elements / set.size() >= load_factor) {

				start_pos = 2147483647;
				end_pos = -1;

				std::vector<SList< val_type > > temp(2 * set.size());
				for (const auto& bucket : set) {
					for (const auto& element : bucket) {

						long long int prehashKey = prehasher(element);
						long long int trueKey = hash(prehashKey, (unsigned int)temp.size());

						update_start_and_end(trueKey);

						temp[trueKey].push_front(element);
					}
				}
				set = std::move(temp);
			}
		}
};

template<typename val_type, typename prehash = PreHash<val_type> >
const unsigned int HashSet<val_type, prehash>::starting_size = 20;

#endif
