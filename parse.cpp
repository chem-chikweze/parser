/* Complete recursive descent parser for the calculator language.
   Builds on figure 2.16 in the text.  Prints a trace of productions
   predicted and tokens matched.  Does no error recovery: prints
   "syntax error" and dies on invalid input.
   Michael L. Scott, 2008-2022.
*/

#include <iostream>
#include <tuple>
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::tie;

#include "scan.hpp"

// const char* names[] = {"read", "write", "id", "literal", "gets", "add",
//                        "sub", "mul", "div", "lparen", "rparen", "eof"};

const char* names[] =  {"int", "id", "gets", "real", "trunc", "lparen", "rparen", "float", 
            "read", "write", "if", "then", "end", "while", "do", "i_num", "r_num", 
            "equal", "noequal", "less", "greater", "less_or_equal", "greater_or_equal",
            "add", "sub", "mul", "div", "comma", "eof"
            };

class parser {
    token next_token;
    string token_image;
    scanner s;

    void error () {
        cerr << "syntax error ffffff" << endl;
        exit (1);
    }

    void errors () {
        cerr << "syntax" << endl;
        exit (1);
    }

    void match (token expected) {
        if (next_token == expected) {
            cout << "matched " << names[next_token];
            // if (next_token == t_id || next_token == t_i_num || next_token == t_r_num)
            //     cout << ": " << token_image;
            cout << endl;
            tie(next_token, token_image) = s.scan ();
        }
        else{
            cout << "Umatch Syntax with " ;
             error ();
        }
    }

public:

    parser() {
        tie(next_token, token_image) = s.scan ();
    }

    void program () {
        switch (next_token) {
            case t_int:
            case t_real:
            case t_trunc:
            case t_float:
            case t_id:
            case t_read:
            case t_write:
            case t_if:
            case t_while:
            case t_eof:
                cout << "predict program --> stmt_list eof" << endl;
                stmt_list ();
                match (t_eof);
                break;
            default: error ();
        }
    }

private:

    void stmt_list () {
        switch (next_token) {
            case t_int:
            case t_real:
            case t_trunc:
            case t_float:
            case t_id:
            case t_read:
            case t_write:
            case t_if:
            case t_while:
                cout << "predict stmt_list --> stmt ; stmt_list" << endl;
                stmt ();
                match(t_comma);
                stmt_list ();
                break;
            case t_end:
            case t_eof:
                cout << "predict stmt_list --> epsilon" << endl;
                break;          // epsilon production
            default: error();
        }
    }

    void stmt () {
        switch (next_token) {
            case t_int:
                cout << "predict stmt --> int id gets expr" << endl;
                match(t_int);
                match (t_id);
                match (t_gets);
                expr ();
                break;
            case t_real:
                cout << "predict stmt --> real id gets expr " << endl;
                match(t_real);
                match (t_id);
                match (t_gets);
                expr ();
                break;
            case t_trunc:
                cout << "predict stmt --> trunc lparen expr rparen" << endl;
                match(t_trunc);
                match (t_lparen);
                expr ();
                match (t_rparen);
                break;
            case t_float:
                cout << "predict stmt --> float lparen expr rparen" << endl;
                match(t_float);
                match (t_lparen);
                expr ();
                match (t_rparen);
                break;
            case t_id:
                cout << "predict stmt --> id gets expr" << endl;
                match (t_id);
                match (t_gets);
                expr ();
                break;
            case t_read:
                cout << "predict stmt --> read type id" << endl;
                match (t_read);
                type();
                match (t_id);
                break;
            case t_write:
                cout << "predict stmt --> write expr" << endl;
                match (t_write);
                expr ();
                break;
            case t_if:
                cout << "predict stmt --> if condition then stmt_list end " << endl;
                match (t_if);
                condition();
                match(t_then);
                stmt_list();
                match(t_end);
                break;
            case t_while:
                cout << "predict stmt --> while condition do stmt_list end" << endl;
                match (t_while);
                condition();
                match(t_do);
                stmt_list();
                match(t_end);
                break;
            default: error ();
        }
    }

    void type () {
        switch (next_token) {
            case t_int:
                cout << "predict type --> int" << endl;
                match(t_int);
                break;
            case t_real:
                cout << "predict type --> real" << endl;
                match(t_real);
                break;
            case t_id:
            case t_eof:
                cout << "predict type --> epsilon" << endl;
                break;          // epsilon production
            default: error ();
        }
    }

    void condition () {
        switch (next_token) {
            case t_lparen:
            case t_id:
            case t_i_num:
            case t_r_num:
                cout << "predict condition --> expr ro expr" << endl;
                expr();
                ro();
                expr();
                break;
            default: error ();
        }
    }

    void expr () {
        switch (next_token) {
            case t_lparen:
            case t_id:
            case t_i_num:
            case t_r_num:
                cout << "predict expr --> term term_tail" << endl;
                term();
                term_tail();
                break;
            // follow E
            default: error ();
        }
    }

    void term_tail () {
        switch (next_token) {
            case t_add:
            case t_sub:
                cout << "predict term_tail --> add_op term term_tail" << endl;
                add_op ();
                term ();
                term_tail ();
                break;
            case t_comma:
            case t_equal:
            case t_not_equal:
            case t_greater:
            case t_less:
            case t_greater_or_equal:
            case t_less_or_equal:
            case t_rparen:
            case t_do:
            case t_then:
            case t_eof:
                cout << "predict term_tail --> epsilon" << endl;
                break;          // epsilon production
            default: error ();
        }
    }

    void term () {
        switch (next_token) {
            case t_lparen:
            case t_id:
            case t_i_num:
            case t_r_num:
                cout << "predict term --> factor factor_tail" << endl;
                factor ();
                factor_tail ();
                break;
            default: error ();
        }
    }

    void factor_tail () {
        switch (next_token) {
            case t_mul:
            case t_div:
                cout << "predict factor_tail --> mul_op factor factor_tail"
                     << endl;
                mul_op ();
                factor ();
                factor_tail ();
                break;
            case t_add:
            case t_sub:
            // case t_eof:
            case t_comma:
            case t_rparen:
            case t_equal:
            case t_not_equal:
            case t_greater:
            case t_less:
            case t_greater_or_equal:
            case t_less_or_equal:
            case t_do:
            case t_then:
                cout << "predict factor_tail --> epsilon" << endl;
                break;          // epsilon production
            default: errors ();
        }
    }

    void factor () {
        switch (next_token) {
            case t_i_num:
                cout << "predict factor --> t_i_num" << endl;
                match (t_i_num);
                break;
            case t_r_num:
                cout << "predict factor --> t_r_num" << endl;
                match (t_r_num);
                break;
            case t_id :
                cout << "predict factor --> id" << endl;
                match (t_id);
                break;
            case t_lparen:
                cout << "predict factor --> lparen expr rparen" << endl;
                match (t_lparen);
                expr ();
                match (t_rparen);
                break;
            default: error ();
        }
    }

    void ro () {
        switch (next_token) {
            case t_equal:
                cout << "predict ro --> equal" << endl;
                match (t_equal);
                break;
            case t_not_equal:
                cout << "predict ro --> not_equal" << endl;
                match (t_not_equal);
                break;
            case t_less:
                cout << "predict ro --> less" << endl;
                match (t_less);
                break;
            case t_greater:
                cout << "predict ro --> greater" << endl;
                match (t_greater);
                break;
            case t_less_or_equal:
                cout << "predict ro --> less_or_equal" << endl;
                match (t_less_or_equal);
                break;
            case t_greater_or_equal:
                cout << "predict ro --> greater_or_equal" << endl;
                match (t_greater_or_equal);
                break;
            default: error ();
        }
    }

    void add_op () {
        switch (next_token) {
            case t_add:
                cout << "predict add_op --> add" << endl;
                match (t_add);
                break;
            case t_sub:
                cout << "predict add_op --> sub" << endl;
                match (t_sub);
                break;
            default: error ();
        }
    }

    void mul_op () {
        switch (next_token) {
            case t_mul:
                cout << "predict mul_op --> mul" << endl;
                match (t_mul);
                break;
            case t_div:
                cout << "predict mul_op --> div" << endl;
                match (t_div);
                break;
            default: error ();
        }
    }
}; // parser

int main () {
    parser p;
    p.program ();
    return 0;
}
