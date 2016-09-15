// uscheme.cpp

#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <unistd.h>
#include <cctype>

using namespace std;

// Globals ---------------------------------------------------------------------

bool BATCH = false;
bool DEBUG = false;

// Structures ------------------------------------------------------------------

struct Node {
    Node(string value, Node *left=nullptr, Node *right=nullptr);
    ~Node();

    string value;
    Node * left;
    Node * right;

    friend ostream &operator<<(ostream &os, const Node &n);
};

Node::Node(string v, Node *l, Node *r): value(v), left(l), right(r){
}

Node::~Node(){
    
}

ostream &operator<<(ostream &os, const Node &n) {
    // print current node
    cout << "(Node: value=" << n.value;


    // recursively call on left child
    if(n.left){
        cout << ", left=" << *(n.left);
    }

    // recursively call on right child
    if(n.right){
        cout << ", right=" << *(n.right);
    }

    cout << ")";
    return os;
}

// Parser ----------------------------------------------------------------------

string parse_token(istream &s) {
    string token="";
    char c1;

    // removes whitespace
    c1=s.peek();
    while( isspace(c1) ){
        c1=s.get();
        c1=s.peek();
    }

    int ascii_c1=(int)c1;
    // check if parathenses or operator
    if( c1=='(' || c1==')' || c1=='+' || c1=='-' || c1=='*' || c1=='/' ){
        token=s.get();
    }// check if chracter is a digit
    else if( (ascii_c1>=48) && (ascii_c1<58) ){
        c1=s.get();
        token+=c1;
        c1=s.peek();
        ascii_c1=(int)c1;
        while( (ascii_c1>=48) && (ascii_c1<58) ){
                c1=s.get();
                token+=c1;
                c1=s.peek();
                ascii_c1=(int)c1;
        }
    }

    return token;
}

Node *parse_expression(istream &s) {
    string token = parse_token(s);

    Node* left=nullptr;
    Node* right=nullptr;
   
    if((token=="") || (token==")")){
        return nullptr;
    }

    if(token=="("){
        token = parse_token(s);
        left  = parse_expression(s);
        if(left){
            right = parse_expression(s);
        }
        if(right){
            parse_token(s);
        }
    }

    return new Node(token, left, right);
}

// Interpreter -----------------------------------------------------------------

void evaluate_r(const Node *n, stack<int> &s) {
}

int evaluate(const Node *n) {
    return 0;
}

// Main execution --------------------------------------------------------------

int main(int argc, char *argv[]) {
    string line;
    int c;

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

        if (!getline(cin, line)) {
            break;
        }

        if (DEBUG) { cout << "LINE: " << line << endl; }

        stringstream s(line);
        Node *n = parse_expression(s);
        if (DEBUG) { cout << "TREE: " << *n << endl; }

        cout << evaluate(n) << endl;

        delete n;
    }

    return 0;
}

// vim: set sts=4 sw=4 ts=8 expandtab ft=cpp:
