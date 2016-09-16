// uscheme.cpp

#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <unistd.h>
#include <cctype>
#include <regex>
#include <list> 

using namespace std;

// Globals ---------------------------------------------------------------------
regex num_type("[[:digit:]]+");
regex op_type ("(\\+|-|\\*|/)");
bool BATCH = false;
bool DEBUG = false;

// Structures ------------------------------------------------------------------


//##################     NODE STRUCTURE      #############################
struct Node {

    Node(string value, Node *left=nullptr, Node *right=nullptr);
    ~Node();

    string value;
    Node * left;
    Node * right;

    friend ostream &operator<<(ostream &os, const Node &n);
};

Node::Node(string v, Node *l, Node *r): value(v), left(l), right(r){}
Node::~Node(){}

ostream &operator<<(ostream &os, const Node &n) {
    // print current node
    cout << "(Node: value = " << n.value;

    // recursively call on left child
    if(n.left){
        cout << ", left = " << *(n.left);}

    // recursively call on right child
    if(n.right){
        cout << ", right = " << *(n.right);}

    cout << ")";
    return os;
}

// Creates Tree ----------------------------------------------------------------
class tree
{
    public:
        tree(string root_val = NULL);

        void destroy_tree(Node *leaf);
        ~tree();
        
        void assemble(list<string>*, Node*);
        void evaluate(Node*);
        Node* get_root();

    private:    
        Node *root;
};

//This value is set to NULL if there are no items in the input list
tree::tree(string root_val){
    root = new Node(root_val);
}


//The destructor calls the function "destroy tree" recursively until all the leaves are freed
tree::~tree(){
    destroy_tree(root);}

/*Would be faster to include the leaf!=NULL condition in each of the 
function calls, thereby calling it twice. Only other option is to call both functions and load them into memory, only
to find that the pointers they have been passed as arguments are NULL. Means waiting for the functions to return*/
void tree::destroy_tree(Node *leaf){
  if(leaf!=NULL){
    destroy_tree(leaf->left);
    destroy_tree(leaf->right);
    delete leaf;
  }
}

Node* tree::get_root(){
    return(root);}


//This function sets up the entire tree, given a list of the numbers and operands
void tree::assemble(list<string> *vals, Node *leaf){
    string next = vals -> front();
        //If value in the node is an operator, call recursively on the next 2 numbers
        if( (regex_match(leaf->value, op_type) ) && ( next != "" ) ){
            if(leaf->left == NULL){
                leaf->left=new Node(vals -> front());
                vals -> pop_front();
                assemble(vals, leaf->left);
            }
            if(leaf->right == NULL){
                leaf->right=new Node(vals -> front());
                vals -> pop_front();
                assemble(vals, leaf->right);
            }
        }
}




void tree::evaluate(Node *leaf) {
    if(!( leaf -> left == NULL ) && !( leaf -> right == NULL)){
        //First check the left side, then the right side, to see if there is an operator. Call recursively if so
        if( regex_match(leaf->left->value, op_type) ){
            evaluate(leaf->left);
        }
        if( regex_match(leaf->right->value, op_type) ){
            evaluate(leaf->right);
        }

        //By the time the recursive calls come back, both the left and right should be ints
        int left_val = stoi(leaf->left->value);
        int right_val = stoi(leaf->right->value);
        int product;

        //When evaluating an "OP-node", switch the value of the operation string to find correct product. 
        switch(leaf->value[0]){
            case '+':
                product = left_val + right_val;
                break;
            case '-':
                product = left_val - right_val;
                break;
            case '*':
                product = left_val * right_val;
                break;
            case '/':
                product = left_val / right_val;
                break;
        }   
        leaf -> value =  to_string(product);
    }

}


// Main execution --------------------------------------------------------------

int main(int argc, char *argv[]) {
    int c;
    string line, token, first;
    list<string> inputs;
    tree *mathTree; 

    while ((c = getopt(argc, argv, "bdh")) != -1) {
        switch (c) {
            case 'b': BATCH = true; break;
            case 'd': DEBUG = true; break;
            default:
                cerr << "usage: " << argv[0] << endl;
                cerr << "    -b Batch mode (disable prompt)"   << endl;
                cerr << "    -d Debug mode (display messages)" << endl;
                return 1;
        }
    }

    while (!cin.eof()) {
        if (!BATCH) {
            cout << ">>> ";
            cout.flush();
        }

        // Read the first line of input into "line"
        if (!getline(cin, line)) {break;}
        if (DEBUG) { cout << "Original Line: " << line << endl; }

        size_t found = line.find_first_of("()");

        while (found!=string::npos){
            line.erase(found,1);
            found=line.find_first_of("()",found);
        }


        // Stream "words" one at a time, if they match either of the REGEX, they are pushed into the list 
        stringstream s(line);
        while(s >> token){
            if(regex_match(token,num_type) || regex_match(token,op_type)){
                inputs.push_back(token);
            }
        }


        mathTree = new tree(inputs.front());
        inputs.pop_front();
        if(inputs.front() != ""){
            mathTree -> assemble(&inputs, mathTree -> get_root());
        }
        mathTree -> evaluate(mathTree -> get_root());
        cout << mathTree -> get_root()->value << endl;
        delete mathTree;

    }

    return 0;
}

// vim: set sts=4 sw=4 ts=8 expandtab ft=cpp: