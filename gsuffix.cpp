#include<iostream>
#include<unordered_map>
#include<cassert>
#include "gsuffix.hpp"


using namespace std;


Node::Node(){
    children = {};
    links = nullptr;
    start = -1;
    end = nullptr;
    inner_end = -1;
    leaf = Leaf();
}


Node::Node(Node *link, int s, int *e){
    links = link;
    children = {};
    start = s;
    end = e;
    inner_end = -1;
    leaf = Leaf();
}

Node::Node(Node *link, int s, int e){
    links = link;
    children = {};
    start = s;
    inner_end = e;
    end = nullptr;
    leaf = Leaf();
}

void Node::attach_root(){
    ap.node = this;
}

void Node::update_active_point(int phase){
    if(ap.node->start == -1 && ap.length > 0){
        // active node is root
        ap.length-=1;
        ap.edge = phase - rsc + 1;
    }  
    if(ap.node->start != -1){
        //ANCFER2C2.
        ap.node = ap.node->links;
    }
}

void Node::update_suffix_link(Node **node){
    if(*node){
        (*node)->links = this;
    }
    *(node) = this;
}


bool Node::walk_down(string word){
    Node* active_node_edge = this->children[word[ap.edge]];
    if(ap.length >= active_node_edge->edge_length()){
        ap.node = active_node_edge;
        ap.length -= active_node_edge->edge_length();
        return true;
    }
    return false;
}


void::Node::add_to_tree(string word){
    for(int i = 0; i<word.length(); i++)
        run_phase(i, word);
    cout<<"The rsc is "<<rsc<<endl;
    this->dfs_leaf(0, word.length());
    this->print_tree(word, "");
}


void Node::insert_inner_node(Node** lcn, int start, int end, int phase, string word){
    Node* node_to_break = ap.node->children[word[ap.edge]];
    Node *inner_node = new Node(this, start, end);                               //create inner_node
    inner_node->children[word[phase]] = new Node(this, phase, &END);            //add new leaf node to inner_node.
    inner_node->children[word[end+1]] = node_to_break;                          //add the remaining of the ex active node child to inner_node
    node_to_break->start = end+1;                                               // update the new start of edge
    ap.node->children[word[ap.edge]] = inner_node;                               //set the active node's active edge to inner_node     
    
    rsc-=1;
    // update last_created_node
    inner_node->update_suffix_link(lcn);
    update_active_point(phase);
}

bool Node::has_edge(char c){
    return (this->children.find(c)) != (this->children.end());
}

int Node::edge_length(){
    int c_start, c_end;
    c_start = this->start;
    if(this->end){
        c_end = *(this->end);
    } else{
        c_end = this->inner_end;
    }
    return c_end - c_start + 1; 
}

void Node::dfs_leaf(int len, int word_size){
    if(this->children.empty()){
        this->leaf = Leaf(0, word_size - len);
    }
    else{
        for(auto [_, tree] : this->children){
            tree->dfs_leaf(len + tree->edge_length(), word_size);
        }
    }
}

void Node::print_tree(string word, string current){
    if(this->children.empty()){
        cout<<current<<"    "<<this->leaf.start<<endl;
    }
    else{
        for(auto [_, tree] : this->children){
            string c;
            c.assign(word, tree->start, tree->edge_length()); 
            tree->print_tree(word, current + c);
        }
    }
}




void Node::run_phase(int phase, string word){
    Node *last_created_node = nullptr;
    END+=1;
    rsc+=1;
    while(rsc){
        if(ap.length > 0){
            if(ap.node->walk_down(word)){
                continue;
            }
            int start_of_edge = ap.node->children[word[ap.edge]]->start;
            int next_index = start_of_edge + ap.length ;
            if(word[next_index] == word[phase]){
                // rule 3
                ap.length+=1;
                ap.node->update_suffix_link(&last_created_node);
                break;
            } else {
                // insert a leaf edge here and update ap according to c1 or c2.
                //rsc-=1; // decrement rsc because adding a new leaf node.
                this->insert_inner_node(&last_created_node, start_of_edge, next_index - 1, phase, word);                                                                   
            }
        }
        else{
            // if ap.length == 0
            ap.edge = phase;            // point at the new character
            if(ap.node->has_edge(word[phase])){
                //rule 3
                ap.length+=1;
                ap.node->update_suffix_link(&last_created_node);
                break;
            } else {
                // insert a leaf edge here and update ap according to c1 or c2.
                Node *leaf = new Node(this, phase, &END);
                ap.node->children[word[phase]] = leaf;
                ap.node->update_suffix_link(&last_created_node);
                rsc-=1;
                update_active_point(phase);
            }
        }
    }
}