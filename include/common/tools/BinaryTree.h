// -*- c++ -*-
// $Id: BinaryTree.h,v 1.6 2006/06/05 13:51:17 kasemir Exp $
//
// Please refer to NOTICE.txt,
// included as part of this distribution,
// for legal information.

#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

// tools
#include "tools/GenericException.h"
#include "tools/NoCopy.h"

/// \addtogroup Tools
/// @{

/// Binary tree item. 
/// It holds the full Item as well as left/right pointers.
template<class Item> class BinaryTreeItem {
public:

  /// Constructor
  BinaryTreeItem() {
    _left = 0;
    _right = 0;
  }

  Item            _item;   ///< item 
  BinaryTreeItem  *_left;  ///< left pointer 
  BinaryTreeItem  *_right; ///< right pointer 

private:

    PROHIBIT_DEFAULT_COPY(BinaryTreeItem);

};


/// Sorted binary tree for Items that support
/// the "less than" and "equal" operators.
template<class Item> class BinaryTree {
public:

    BinaryTree()
    {    _root = 0; }
    
    ~BinaryTree()
    {    cleanup(_root); }
    
    /// Add (copy) Item into the tree.
    void add(const Item &item) {
        
      BinaryTreeItem<Item> *n;
      try {
        n = new BinaryTreeItem<Item>;
      } catch (...) {
	throw GenericException(__FILE__, __LINE__,
			       "Cannot allocate new Item");
      }
        
      n->_item = item;
      n->_left = 0;
      n->_right = 0;
      insert(n, &_root);
    }

    /// Try to find item in tree.
    ///
    /// @return  true if found.
    bool find(const Item &item) {
        
      BinaryTreeItem<Item> *n = _root;
        
      while (n) {
	if (item == n->_item) return true;
            
	if (item < n->_item) {
	  n = n->_left;
	} else {
	  n = n->_right;
	}
      }
      return false;
    }

    /// Sorted (inorder) traverse, calling visitor routine on each item.
    void traverse(void (*visit) (const Item &, void *), void *arg=0) {    
      visit_inorder (visit, _root, arg); 
    }

private:

    PROHIBIT_DEFAULT_COPY(BinaryTree);

    BinaryTreeItem<Item>    *_root;

    void insert(BinaryTreeItem<Item> *new_item, BinaryTreeItem<Item> **node) {
      
      if (*node == 0){
	*node = new_item;
      } else if (new_item->_item  <  (*node)->_item) {
	insert(new_item, & ((*node)->_left));
      } else {
	insert(new_item, & ((*node)->_right));
      }
    }

    void visit_inorder(void (*visit) (const Item &, void *),
                       BinaryTreeItem<Item> *node, void *arg) {
      if (node) {
	visit_inorder(visit, node->_left, arg);
	visit(node->_item, arg);
	visit_inorder(visit, node->_right, arg);
      }
    }

    void cleanup(BinaryTreeItem<Item> *node) {        
      if (node){
	cleanup(node->_left);
	cleanup(node->_right);
	delete node;
      }
    }
};


/// @}

#endif

