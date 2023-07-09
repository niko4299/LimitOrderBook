#include <memory>
#include <cstdint>
#include <optional>
#include <iostream>

template<typename T>
struct Node{
    T key;
    bool is_black;
    Node<T>* parent;
    Node<T>* left;
    Node<T>* right;

    Node(const T& key,bool is_black = false,Node<T>* parent = nullptr,Node<T>* left  = nullptr,Node<T>* right = nullptr) : key {key}, is_black {is_black},parent{parent},left{left},right{right} {}

    Node(): parent{nullptr},left{nullptr},right{nullptr}{}

    bool operator==(const Node<T>& other){
        return other.key == key;
    }

    bool operator!=(const Node<T>& other){
        return other.key != key;
    }
};

template<class RBTree>
class ReverseRBTreeIterator{
    public:
        using ValueType = typename RBTree::ValueType;
        using PointerType = ValueType*;
        using RefrenceType = ValueType&;

    public:
        ReverseRBTreeIterator(Node<ValueType>* ptr, RBTree* rb_tree): m_Ptr(ptr), rb_tree(rb_tree){}

        ReverseRBTreeIterator& operator++(){
            m_Ptr = rb_tree->predecessor(m_Ptr);

            return *this;
        }
        
        ReverseRBTreeIterator operator++(int){
            ReverseRBTreeIterator iterator = *this;
            ++(*this);

            return iterator;
        }


        ReverseRBTreeIterator& operator--(){
            m_Ptr = rb_tree->successor(m_Ptr);

            return *this;
        }
        
        ReverseRBTreeIterator operator--(int){
            ReverseRBTreeIterator iterator = *this;
            --(*this);

            return iterator;
        }

        PointerType operator->(){
            return &m_Ptr->key;
        }

        RefrenceType operator*(){
            return m_Ptr->key;
        }

        inline bool valid(){
            return m_Ptr != rb_tree->get_nil();
        }

        bool operator==(const ReverseRBTreeIterator& other) const{
            return m_Ptr == other.m_Ptr;
        }

        bool operator!=(const ReverseRBTreeIterator& other) const{
            return m_Ptr != other.m_Ptr;
        }

        bool operator>(const ReverseRBTreeIterator& other) const{
                        return m_Ptr->key > other.m_Ptr->key;
        }

    private:
        Node<ValueType>* m_Ptr;

        RBTree* rb_tree;
};

template<class RBTree>
class ForwardRBTreeIterator{
    public:
        using ValueType = typename RBTree::ValueType;
        using PointerType = ValueType*;
        using RefrenceType = ValueType&;

    public:
        ForwardRBTreeIterator(Node<ValueType>* ptr,RBTree* rb_tree): m_Ptr(ptr), rb_tree(rb_tree){}

        ForwardRBTreeIterator& operator++(){
            m_Ptr = rb_tree->successor(m_Ptr);

            return *this;
        }
        
        ForwardRBTreeIterator operator++(int){
            ForwardRBTreeIterator iterator = *this;
            ++(*this);

            return iterator;
        }


        ForwardRBTreeIterator& operator--(){
            m_Ptr = rb_tree->predecessor(m_Ptr);

            return *this;
        }
        
        ForwardRBTreeIterator operator--(int){
            ForwardRBTreeIterator iterator = *this;
            --(*this);

            return iterator;
        }

        PointerType operator->(){
            return &m_Ptr->key;
        }

        RefrenceType operator*(){
            return m_Ptr->key;
        }

        inline bool valid(){
            return m_Ptr != rb_tree->get_nil();
        }

        bool operator==(const ForwardRBTreeIterator& other) const{
            return m_Ptr == other.m_Ptr;
        }

        bool operator!=(const ForwardRBTreeIterator& other) const{
            return m_Ptr != other.m_Ptr;
        }

        bool operator>(const ForwardRBTreeIterator& other) const{
                        return m_Ptr->key > other.m_Ptr->key;
        }

    private:
        Node<ValueType>* m_Ptr;

        RBTree* rb_tree;
};

template <typename T>
class RBTree{
    public:
        using ValueType = T;
        using ForwardIterator = ForwardRBTreeIterator<RBTree<T>>;
        using ReverseIterator = ReverseRBTreeIterator<RBTree<T>>;

    public:
     RBTree(){
        _nil = new Node<T>();
        _nil->is_black = true;
        _root = _nil;
        _first = _root;
        _last = _root;
        _size = 0;
     }

     ~RBTree(){
        clear(_root);
        if(!_nil){
            delete _nil;
        }
     }

     const Node<T>* get_root() noexcept{
        if(_root == _nil){
            return _nil;
        }else{
            return _root;
        }
     }

     inline const std::optional<T> first() noexcept{
        if(_first == _nil){
            return std::nullopt;
        }

        return _first->key;
     }

     inline const std::optional<T> last() noexcept{
        if(_last == _nil){
            return std::nullopt;
        }

        return _last->key;
     }

     const std::optional<T> find(const T& value) noexcept{
        auto found_node = search_tree(_root,value);
        if(found_node == _nil){
            return std::nullopt;
        }

        return found_node->key;
     }

     bool contains(const T& value) noexcept {
        return find(value).has_value(); 
     }

      void insert(const T& value){
        Node<T>* parent = _nil;
        Node<T>* current = _root;

        while (current != _nil) {
            parent = current;
            if (value < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        
        auto new_node = new Node(value,false,parent,_nil,_nil);
        if (parent == _nil) {
            _root = new_node;
            _first = new_node;
            _last = new_node;
        } else if (new_node->key < parent->key) {
            parent->left = new_node;
        } else {
            parent->right = new_node;
        }

        if (_first->left != _nil){
            _first = _first->left;
        }

        if(_last->right != _nil){
            _last = _last->right;
        }

        rebalance_insert(new_node);
        _size++;
     };

     std::uint64_t size() const {
        return _size;
     }

    bool remove(const T& key) noexcept {
        auto node = search_tree(_root,key);
        if(node == _nil) return false;
        remove(node);
        _size--;
        return true;
    }

    void clear() noexcept{
        clear(_root);
        _root = _nil;
    }

    Node<T>* successor(Node<T>* node) noexcept {
        if(node->right != _nil){
            return most_left(node->right);
        }

        auto successor_node = node->parent;
        while(successor_node && successor_node != _nil && node == successor_node->right){
            node = successor_node;
            successor_node = successor_node->parent;
        }

        return successor_node;
    }

    Node<T>* predecessor(Node<T>* node) noexcept {
        if(node->left != _nil){
            return most_right(node->left);
        }
        
        auto predecessor_node = node->parent;
        while(predecessor_node != _nil && node == predecessor_node->left){
            node = predecessor_node;
            predecessor_node = predecessor_node->parent;
        }

        return predecessor_node;
    }


    void pre_order_print(){
        pre_order_print(_root);
    }

    void in_order_print(){
        in_order_print(_root);
    }

    void post_order_print(){
        post_order_print(_root);
    }

    ForwardIterator begin(){
        return ForwardRBTreeIterator{_first, this};
    }

    ForwardIterator end(){
        return ForwardRBTreeIterator{_last, this};
    }

    ReverseIterator rbegin(){
        return ReverseIterator{_last, this};
    }

    ReverseIterator rend(){
        return ReverseIterator{_first,this};
    }

    std::pair<ForwardIterator,ForwardIterator> range(T from, T to){
        return {lower_bound(from),upper_bound(to)};
    }

    ForwardIterator lower_bound(T& from){
        auto result = _last;
        auto node = _root;

        while(node != _nil){
            if (node->key < from ){
                node = node->right;
            } else {
                result = node;
                node = node->left;
            }
        }

        return ForwardIterator{result,this};
    }

    ForwardIterator upper_bound(T& to){
        auto result = _last;
        auto node = _root;

        while(node != _nil){

            if (node->key <= to){
                node = node->right;
            } else {
                result = node;
                node = node->left;
            }
        }

        return ForwardIterator{result,this};
    }

    Node<T>* get_nil(){
            return _nil;
        }
    
    private:
    void pre_order_print(Node<T>* node) noexcept{
        if(node != _nil){
            std::cout<<node->key<<" ";
            pre_order_print(node->left);
            pre_order_print(node->right);
        }
    }
    void in_order_print(Node<T>* node) noexcept{
        if(node != _nil){
            in_order_print(node->left);
            std::cout<<node->key<<" ";
            in_order_print(node->right);
        }
     }

     void post_order_print(Node<T>* node) noexcept{
        if(node != _nil){
            post_order_print(node->left);
            post_order_print(node->right);
            std::cout<<node->key<<" ";
        }
     }

    constexpr inline void rotate_right(Node<T>* node) noexcept {
		auto child_left = node->left;
		node->left = child_left->right;
		if (child_left->right != _nil) {
			child_left->right->parent = node;
		}

		child_left->parent = node->parent;
		if (node->parent == _nil) {
			_root = child_left;
		} else if (node == node->parent->right) {
			node->parent->right = child_left;
		} else {
			node->parent->left = child_left;
		}

		child_left->right = node;
		node->parent = child_left;
    }

    constexpr inline void rotate_left(Node<T>* node) noexcept {
		auto child_right = node->right;
		node->right = child_right->left;

		if (child_right->left != _nil) {
			child_right->left->parent = node;
		}

		child_right->parent = node->parent;
		if (node->parent == _nil) {
			_root = child_right;
		} else if (node == node->parent->left) {
			node->parent->left = child_right;
		} else {
			node->parent->right = child_right;
		}

		child_right->left = node;
		node->parent = child_right;
    }
    
    constexpr void transplant(Node<T>* u, Node<T>* v) noexcept {
        if(u->parent == _nil){
            _root = v;
        }else if (u == u->parent->left){
            u->parent->left = v;
        }else{
            u->parent->right = v;
        }

        v->parent = u->parent;  
    }

    constexpr void remove(Node<T>* node) noexcept {
        if(node == _first){
            if(node->right != _nil){
                _first = node->right;
            }else{
                _first = node->parent;
            }

        }else if(node == _last){
            if(node->left != _nil){
                _last = node->left;
            }else{
                _last = node->parent;
            }
        }


        auto original_color = node->is_black;
        Node<T>* fix_node = _nil;
        if(node->left == _nil){
            fix_node = node->right;
            transplant(node,node->right);
        }else if(node->right == _nil){
            fix_node = node->left;
            transplant(node,node->left);
        }else{
            auto pom = most_left(node->right);
            original_color = pom->is_black;
            fix_node = pom->right;
            if(pom->parent == node){
                fix_node->parent = pom;
            }else{
                transplant(pom,pom->right);
                pom->right = node->right;
                pom->right->parent = pom;
            }
            
            transplant(node,pom);
            pom->left = node->left;
            pom->left->parent = pom; 
            pom->is_black = node->is_black;
        }

        delete node;
        
        if(original_color){
            rebalance_delete(fix_node);
        }
    }

     Node<T>* search_tree(Node<T>* node,const T& value) noexcept {
        	while (node != _nil && value != node->key){
                if (value < node->key){
                    node = node->left;
                } else {
                    node = node->right;
                }
            }

            return node;
     }

    constexpr void rebalance_insert(Node<T>* node) noexcept {
		Node<T>* u = _nil;
		while (node != _root && !node->parent->is_black) {
			if (node->parent == node->parent->parent->right) {
				u = node->parent->parent->left;
                if (!u->is_black) {
					u->is_black = true;
					node->parent->is_black = true;
					node->parent->parent->is_black = false;
					node = node->parent->parent;
				} else {
					if (node == node->parent->left) {
						node = node->parent;
						rotate_right(node);
					}
					node->parent->is_black = true;
					node->parent->parent->is_black = false;
					rotate_left(node->parent->parent);
				}
			} else {
				u = node->parent->parent->right;

				if (!u->is_black) {
					u->is_black = true;
					node->parent->is_black = true;
					node->parent->parent->is_black = false;
					node = node->parent->parent;	
				} else {
					if (node == node->parent->right) {
						node = node->parent;
						rotate_left(node);
					}
					node->parent->is_black = true;
					node->parent->parent->is_black = false;
					rotate_right(node->parent->parent);
				}
			}
		}
        _root->is_black = true;
     }
    
    constexpr void rebalance_delete(Node<T>* node) noexcept{
        while(node != _root && node->is_black){
            if(node == node->parent->left){
                auto parent_right = node->parent->right;
                if(!parent_right->is_black){
                    parent_right->is_black = true;
                    node->parent->is_black = false;
                    rotate_left(node->parent);
                    parent_right = node->parent->right;
                }

                if(parent_right->left->is_black && parent_right->right->is_black){
                    parent_right->is_black = false;
                    node = node->parent;
                }else{
                    if(parent_right->right->is_black){
                        parent_right->left->is_black = true;
                        parent_right->is_black = false;
                        rotate_right(parent_right);
                        parent_right = node->parent->right;
                    }

                    parent_right->is_black = node->parent->is_black;
                    node->parent->is_black = true;
                    parent_right->right->is_black = true;
                    rotate_left(node->parent);
                    node = _root;
                }
            } else{
                auto parent_left = node->parent->left;
                if(!parent_left->is_black){
                    parent_left->is_black = true;
                    node->parent->is_black = false;
                    rotate_right(node->parent);
                    parent_left = node->parent->left;
                }

                if(parent_left->right->is_black && parent_left->left->is_black){
                    parent_left->is_black = false;
                    node = node->parent;
                }else{
                    if(parent_left->left->is_black){
                        parent_left->right->is_black = true;
                        parent_left->is_black = false;
                        rotate_left(parent_left);
                        parent_left = node->parent->left;
                    }

                    parent_left->is_black = node->parent->is_black;
                    node->parent->is_black = true;
                    parent_left->left->is_black = true;
                    rotate_right(node->parent);
                    node = _root;
                }
            }
        }
        node->is_black = true;
    }

    Node<T>* most_left(Node<T>* node)noexcept {
        while(node->left != _nil) {
		   node = node->left;
	    }

	    return node;
    }

    Node<T>* most_right(Node<T>* node) noexcept {
        while(node->right != _nil) {
		   node = node->right;
	    }

	    return node;
    }

    void clear(Node<T>* node) noexcept{
        if(node == _nil){
            return;
        }

        clear(node->left);
        clear(node->right);
        delete node;
        _size--;
    }

     Node<T>* _nil;
     Node<T>* _root;
     Node<T>* _first;
     Node<T>* _last;
     std::uint64_t _size;
};