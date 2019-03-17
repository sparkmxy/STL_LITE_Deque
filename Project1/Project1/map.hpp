/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
>
class map {
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
public:
	typedef sjtu::pair<const Key, T> value_type;

private:
	enum colorT { RED, BLACK };
	struct node {
		value_type *data;
		node *left, *right;
		colorT color;
		node(const value_type& element, node *l = nullptr, node *r = nullptr, colorT c = RED)
			:left(l), right(r), color(c) {
			data = new value_type(element);
		}
		~node() { delete data; }
	};

	class redBlackTree {
		Compare Less;
		size_t _size;
		bool Equal(const Key &lhs,const Key &rhs) const {
			return !(Less(lhs,rhs) || Less(rhs,lhs));
		}
		colorT colorOf(node *&o) {
			if (o == nullptr || o->color == BLACK) return BLACK;
			return RED;
		}
	public:
		node *root;
		size_t size() const { return _size; }
		redBlackTree() { root = nullptr; }
		redBlackTree(const redBlackTree &other) { root = copy(other.root); _size = other._size; }
		~redBlackTree() { clear(root); }
		redBlackTree & operator=(const redBlackTree &other) {
			if (this == &other) return *this;
			this->~redBlackTree();
			root = copy(other.root);
			_size = other._size;
			return *this;
		}

		node *find(const Key &x) const;
		node *maximium(node *o) const {
			while (o->right != nullptr) o = o->right;
			return o;
		}
		node *minimium(node *o) const {
			while (o->left != nullptr) o = o->left;
			return o;
		}
		node *successor(const node *o)const ;
		node *precursor(const node *o)const ;
		
		void insert(const value_type &elem);
		void remove(const Key &x);
	private:
		void swapData(node *t1, node *t2) {
			value_type* temp = t1->data;
			t1->data = t2->data;
			t2->data = temp;
		}
		void clear(node *o);
		node* copy(node *o);
		void LL(node *gp);
		void LR(node *gp);
		void RR(node *gp);
		void RL(node *gp);
		void insertFixUp(node *gp, node *p, node *o);
		void removeFixUp(node *&o, node*&p, node *&c,const Key &del);
	};

	redBlackTree R;
public:

	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
		friend class map<Key,T,Compare>;
	private:
		map *M;
		node *o;
	public:
		bool invalid() { return o == nullptr; }
		iterator() { M = nullptr; o = nullptr; }
		iterator(map *m, node *_o) :M(m), o(_o) {}
		iterator(const iterator &other) :M(other.M),o(other.o){}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		iterator operator++(int) {
			//if (o == nullptr) throw invalid_iterator();
			iterator ret = *this;
			o = M->R.successor(o);
			return ret;
		}

		iterator & operator++() {
			//if (o == nullptr) throw invalid_iterator();
			o = M->R.successor(o);
			return *this;
		}

		iterator operator--(int) {
			iterator ret = *this;
			if (o == nullptr) o = M->R.maximium(M->R.root);
			else o = M->R.precursor(o);
			return ret;
		}

		iterator & operator--() {
			if (o == nullptr) o = M->R.maximium(M->R.root);
			else o = M->R.precursor(o);
			return *this;
		}
		/**
		 * an operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const { return *(o->data); }
		bool operator==(const iterator &rhs) const { return o == rhs.o; }
		bool operator==(const const_iterator &rhs) const { return o == rhs.o; }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const { return o != rhs.o; }
		bool operator!=(const const_iterator &rhs) const { return o != rhs.o; }

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept { return o->data; }
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			const map *M;
			const node *o;
		public:
			const_iterator() { M = nullptr; o = nullptr; }
			const_iterator(const map *m,const node *_o) :M(m), o(_o) {}
			const_iterator(const const_iterator &other):M(other.M),o(other.o){}
			const_iterator(const iterator &other):M(other.M), o(other.o) {}

			const_iterator operator++(int) {
				//if (o == nullptr) throw invalid_iterator();
				const_iterator ret = *this;
				o = M->R.successor(o);
				return ret;
			}

			const_iterator & operator++() {
				//if (o == nullptr) throw invalid_iterator();
				o = M->R.successor(o);
				return *this;
			}

			const_iterator operator--(int) {
				const_iterator ret = *this;
				if (o == nullptr) o = M->R.maximium(M->R.root);
				else o = M->R.precursor(o);
				return ret;
			}

			const_iterator & operator--() {
				if (o == nullptr) o = M->R.maximium(M->R.root);
				else o = M->R.precursor(o);
				return *this;
			}
			bool operator==(const iterator &rhs) const { return o == rhs.o; }
			bool operator==(const const_iterator &rhs) const { return o == rhs.o; }
			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator &rhs) const { return o != rhs.o; }
			bool operator!=(const const_iterator &rhs) const { return o != rhs.o; }
			const value_type & operator*() const { return *(o->data); }
			const value_type* operator->() const noexcept { return o->data; }
	};
	/**
	 * TODO two constructors
	 */
	map() {}
	map(const map &other) :R(other.R){}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
		if (this == &other) return *this;
		R.~redBlackTree();
		R = other.R;
		return *this;
	}
	/**
	 * TODO Destructors
	 */
	~map() {}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
		node *o = R.find(key);
		if (o == nullptr) throw index_out_of_bound();
		return o->data->second;
	}
	const T & at(const Key &key) const {
		node *o = R.find(key);
		if (o == nullptr) throw index_out_of_bound();
		return o->data->second;
	}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
		node *o = R.find(key);
		if (o == nullptr) R.insert(value_type(key,T()));
		return at(key);
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const { return at(key); }
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() { return iterator(this,R.minimium(R.root)); }
	const_iterator cbegin() const { return const_iterator(this, R.minimium(R.root));}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() { return iterator(this, nullptr); }
	const_iterator cend() const { return const_iterator(this, nullptr);}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const { return R.size() == 0; }
	/**
	 * returns the number of elements.
	 */
	size_t size() const { return R.size(); }
	/**
	 * clears the contents
	 */
	void clear() { R.~redBlackTree(); }
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		node *o = R.find(value.first);
		if (o == nullptr) {
			R.insert(value);
			return pair<iterator, bool>(iterator(this,R.find(value.first)),true);
		}
		return pair<iterator, bool>(iterator(this,o),false);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
		if (pos.invalid()) throw invalid_iterator();
		R.remove(pos.o->data->first);
	}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
		if (R.find(key)) return 1;
		return 0;
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
		node *o = R.find(key);
		if (o == nullptr) return end();
		return iterator(this,o);
	}
	const_iterator find(const Key &key) const {
		node *o = R.find(key);
		if (o == nullptr) return cend();
		return const_iterator(this, o);
	}
};

template<class Key,class T,class Compare>
typename map<Key,T,Compare>::node *map<Key, T, Compare>::redBlackTree::copy(node *o) {
	if (o == nullptr) return nullptr;
	node *ret = new node(*(o->data));
	ret->left = copy(o->left);
	ret->right = copy(o->right);
	return ret;
}

template<class Key, class T, class Compare>
void map<Key,T,Compare>::redBlackTree::clear(node *o) {
	if (o->left != nullptr) clear(o->left);
	if (o->right != nullptr) clear(o->right);
	delete o;
}

template<class Key, class T, class Compare>
typename map<Key, T, Compare>::node *map<Key,T,Compare>::redBlackTree::find(const Key &x) const {
	node *o = root;
	while (o!=nullptr) {
		if (Equal(o->data->first,x)) return o;
		if (Less(x,o->data->first)) o = o->left;
		else o = o->right;
	}
	return nullptr;
}

template<class Key, class T, class Compare>
void map<Key,T,Compare>::redBlackTree::insert(const value_type& elem) {
	_size++;
	node *o, *p, *gp;
	if (root == nullptr) {
		std::cout << "Root gernerated.\n";
		root = new node(elem, nullptr, nullptr, BLACK);
		return;
	}
	o = p = gp = root;
	while (true) {
		if (o) {
			if (colorOf(o->left) == RED && colorOf(o->right)==RED) {
				//左右儿子均为红色
				o->color = RED;
				o->left->color = o->right->color = BLACK;
				insertFixUp(gp,p,o);
			}
			gp = p; p = o;
			if (Less(elem.first, o->data->first)) o = o->left;
			else o = o->right;
		}
		else {  // 已经到了叶子节点
			//std::cout << "Found a leaf node.\n";
			o = new node(elem);
			if (Less(elem.first, p->data->first))p->left = o;
			else p->right = o;
			insertFixUp(gp,p,o);
			root->color = BLACK;
			return;
		}
	}
}

template<class Key, class T, class Compare>
void map<Key,T,Compare>::redBlackTree::LL(node *gp) { //左旋并且交换gp 与 p 的颜色
	node *p = gp->left, *o = p->left, *temp = gp->right;
	swapData(gp,p);
	gp->left = o;
	gp->right = p;
	p->left = p->right;
	p->right = temp;
}

template<class Key, class T, class Compare>
void map<Key,T,Compare>::redBlackTree::LR(node *gp) { //右旋+左旋 并交换o与gp的颜色
	node* p = gp->left, *o = p->right, *temp = gp->right;
	swapData(gp, o);
	gp->right = o;
	gp->data = o->data;
	p->right = o->left;
	o->left = o->right;
	o->right = temp;

}
 
template<class Key, class T, class Compare>
void map<Key,T,Compare>::redBlackTree::RR(node *gp) {
	node *p = gp->right, *o = p->right,*temp = gp->left;
	swapData(gp,p);
	gp->right = o;
	gp->left = p;
	p->right = p->left;
	p->left = temp;
}

template<class Key, class T, class Compare>
void map<Key,T,Compare>::redBlackTree::RL(node *gp) {
	node* p = gp->left, *o = p->right, *temp = gp->left;
	swapData(gp, o);
	gp->left = o;
	p->left = o->right;
	p->left = o->right;
	o->right = o->left;
	o->left = temp;
}

template<class Key, class T, class Compare>
void map<Key,T,Compare>::redBlackTree::insertFixUp(node *gp,node* p,node *o) {
	if (p->color == BLACK) return;
	if (p == root) {
		root->color = BLACK;
		return;
	}
	if (gp->left == p)
		if (p->left == o) LL(gp); else LR(gp);
	else
		if (p->right == o)RR(gp); else RL(gp);
}

template<class Key, class T, class Compare>
void map<Key,T,Compare>::redBlackTree::remove(const Key &x){
	_size--;
	node *o, *p, *c; // c 是 o 的兄弟节点
	if (root == nullptr) return;
	if (Equal(root->data->first, x) && root->left == nullptr && root->right == nullptr) {
		delete root;
		root = nullptr;
		return;
	}
	o = p = c = root;
	while (true) {
		removeFixUp(o,p,c,x);
		if (Equal(o->data->first, x) && o->left != nullptr && o->right != nullptr) {
			//被删节点有两个儿子,找替身
			node *shadow = o->right;
			while (shadow->left != nullptr) shadow = shadow->left;
			swapData(o, shadow);
			p = o; 
			o = o->right; 
			c = p->left;
		}
		if (Equal(o->data->first, x)) { //被删节点至多有一个儿子
			delete o;
			if (p->left == o) p->left = nullptr;
			else p->right = nullptr;
			root->color == BLACK; // ???
			return;
		}
		p = o;
		if (Less(x, o->data->first)) o = o->left, c = p->right;
		else o = o->right, c = p->left;	
	}
}

template<class Key, class T, class Compare>
void map<Key,T,Compare>::redBlackTree::removeFixUp(node *&o, node *&p, node *&c,const Key &del) {
	//将o变为红色
	if (o->color == RED) return;
	if (o == root)
		if (o->left != nullptr && o->right != nullptr && o->right->color == o->left->color) {
			o->color = RED;
			o->left->color = c->right->color = BLACK;
			return;
		}
	if (colorOf(o->left) == BLACK && colorOf(o->right) == BLACK) // o 有两个黑色儿子
		if (colorOf(c->left) == BLACK && colorOf(c->right) == BLACK) {
			// c 也有两个黑色儿子
			p->color = BLACK;
			o->color = c->color = RED;
		}
		else {
			// c至少有一个红儿子，按照红儿子相对p的位置分为LL,LR,RR,RL四种情况
			if (p->left == c) {
				if (colorOf(c->left) == RED) {
					c->left->color = BLACK;
					LL(p);
					p = c; //因为LL中事实上交换了p和c的值

				}
				else {
					LR(p);
					p = p->right;
					p->color = BLACK;
				}
			}
			else {
				if (colorOf(c->right) == RED) {
					c->right->color = BLACK;
					RR(p);
					p = c;
				}
				else {
					RL(p);
					p = p->left;
					p->color = BLACK;
				}
			}
		}
	else {
		// o 至少有一个红儿子
		if (Equal(o->data->first,del)) { // o是被删节点
			if (o->left != nullptr &&o->right != nullptr) { //o有两个儿子
				if (o->right->color == BLACK) { //右儿子是黑色。注意此时左儿子必为红色，一次旋转o就变成了红色
					LL(o);
					o = o->right;
				}
				return;
			}
			if (o->left != nullptr) {
				LL(o);
				p = o;
				o = o->right;
			}
			else {
				RR(o);
				p = o;
				o = o->left;
			}
		}
		else {
			p = o;
			if (Less(del, o->data->first)) o = o->left, c = p->right;
			else o = o->right, c = p->left;
			if (o->color == BLACK) {
				//下降一层，o变成了黑色的；通过一次旋转，变成一个新的需要调整的情形，再次调用removeFixUp()即可
				if (c == p->right) RR(p); 
				else LL(p);
				p = c;
				c = (o == p->left ? p->right : p->left);
				removeFixUp(o,p,c,del);
			}
		}
	}
}

template<class Key, class T, class Compare>
typename map<Key,T,Compare>::node *map<Key,T,Compare>::redBlackTree::successor(const node *o)const {
	if (o == nullptr) return nullptr;
	if (o->right != nullptr) return minimium(o->right);
	node *c = root,*last = nullptr;
	while (c != nullptr) {
		if (o == c) return last;
		if (Less(o->data->first, c->data->first)) {
			last = c;
			c = c->left;
		}
		else c = c->right;
	}
}

template<class Key, class T, class Compare>
typename map<Key, T, Compare>::node *map<Key, T, Compare>::redBlackTree::precursor(const node *o)const  {
	if (o == nullptr) return nullptr;
	if (o->left != nullptr) return maximium(o->left);
	node *c = root, *last = nullptr;
	while (c != nullptr) {
		if (c == o) return last;
		if (Less(o->data->first, c->data->first)) c = c->left;
		else {
			last = c;
			c = c->right;
		}
	}
}


} 


#endif
