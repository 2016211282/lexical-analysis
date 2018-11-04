#include<iostream>
#include<string>
#include<vector>
#include<fstream>
using namespace std;

enum STATE{
    START, WORD, NUM, 
    POINT, DECIMAL, INDEX, 
    E_POSITIVE, E_NEGITIVE, SMALL, 
    BIG, SLASH, NOTE, 
    ADD, SUB, EQUAL, 
    NOT, AND, OR,
    DOUBLE_QUOTE
};

STATE state = START; 
int keyWord = 0;     //count the number of keyWords    
int words = 0;       //count the number of all words
int row = 0, line = 0;
int maxRow = 0;
char ch;        //the word read now
string str;     //the string read now

vector<string> buffer;
vector<string> table;  //word table defined by users

const int keyWordNum = 32;
string keyWordTable[keyWordNum] = {
"char"     , "double"   , "enum"   , "float"  , "int" , 
"short"    , "long"     , "signed" , "struct" , "union" , 
"unsigned" , "void"     , "for"    , "do"     , "while" , 
"break"    , "continue" , "if"     , "else"   , "goto" , 
"switch"   , "case"     , "default", "return" , "auto" ,
"extem"    , "register" , "static" , "const"  , "sizeof" , 
"typedef"  , "volatile" }; 

void lexAnalysis();
void readFile();
void outputBuffer();   //test

void getChar();
void getN();
void cat();    //put ch into str
bool isLetter();
bool isDigit();
void retract();   //go back one word
bool isKeyWord();
bool isInTable();
void tableInsert();
void skip();    //while reading the note //, skip to next row

/* output */
void outputTable();
void outputNum();
void outputKey();
void outputOP(string );
void outputCount();
void outputString();

/* error */
void errorWord();
void errorQuote();

int main()
{
    readFile();
    //outputBuffer();
    lexAnalysis();
    return 0;
}

void lexAnalysis()
{
    state = START;
    row = 0;
    line = 0;
    while(row <= maxRow){
        switch(state){
            case START:
                str.clear();
                getChar();   //get ch
                getN();      //skip blank and \n
                cat();
                if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')){
					state = WORD;
				}
				else if(ch >= '0' && ch <= '9'){
					state = NUM;
				}
				else{
					switch(ch){
                    case '_':
                        state = WORD;
                        break;
                    case '<':
                        state = SMALL;
                        break;
                    case '>':
                        state = BIG;
                        break;
                    case '/':
                        state = SLASH;
                        break;
                    case '+':
                        state = ADD;
                        break;
                    case '-':
                        state = SUB;
                        break;
                    case '=':
                        state = EQUAL;
                        break;
                    case '!':
                        state = NOT;
                        break;
                    case '&':
                        state = AND;
                        break;
                    case '|':
                        state = OR;
                        break;
                    case '"':
                    	state = DOUBLE_QUOTE;
                    	break;
                    
					case '\0':
						outputOP("");
                    	break;
                	case '#':
                		outputOP("");
                        break;
                    case '%':
                		outputOP("");
                        break;
                    case '*':
                		outputOP("");
                        break;
                    case '[':
                		outputOP("");
                        break;
                    case ']':
                		outputOP("");
                        break;
                    case ',':
                		outputOP("");
                        break;
                    case '?':
                		outputOP("");
                        break;
                    case ';':
                		outputOP("");
                        break;
                    case '(':
                		outputOP("");
                        break;
                    case ')':
                		outputOP("");
                        break;
                    case '{':
                		outputOP("");
                        break;
                    case '}':
                		outputOP("");
                        break;
                    
                    default:
						errorWord();
                        break;
                    } // end switch
                } // end if
                break;
            
            case WORD:
                getChar();
                if(isLetter() || isDigit() || ch == '_'){
                    state = WORD;
                    cat();
                }
                else{
                    retract();
                    state = START;
                    if(isKeyWord()){
                        keyWord++;
                        outputKey();
                    }
                    else{
                        if(!isInTable()){
                            tableInsert();
                        }
                        outputTable();
                    }
                }
                break;
            
            case NUM:
                getChar();
                if(isDigit()){
                    state = NUM;
                    cat();
                }
                else if(ch == '.'){
                    state = POINT;
                    cat();
                }
                else if(ch == 'e'){
                    state = INDEX;
                    cat();
                }
                else{
                    retract();
                    state = START;
                    outputNum();
                }
                break;

            case POINT:
                getChar();
                if(isDigit()){
                    state = POINT;
                    cat();
                }
                else if(ch == 'e'){
                    state = INDEX;
                    cat();
                }
                else{
                    retract();
                    state = START;
                    outputNum();
                }
                break;

            case DECIMAL:
                getChar();
                if(isDigit()){
                    cat();
                    state = DECIMAL;
                }
                else if(ch == 'e'){
                    state = INDEX;
                    cat();
                }
                else{
                    retract();
                    state = START;
                    outputNum();
                }
                break;

            case INDEX:
                getChar();
                if(ch == '+'){
                    cat();
                    state = E_POSITIVE;
                }
                else if(ch == '-'){
                    cat();
                    state = E_NEGITIVE;
                }
                else{
                    retract();
                    state = START;
                    outputNum();
                }
                break;

            case E_POSITIVE:
                getChar();
                if(isDigit()){
                    cat();
                    state = E_POSITIVE;
                }
                else{
                    retract();
                    state = START;
                    outputNum();
                }
                break;

            case E_NEGITIVE:
                getChar();
                if(isDigit()){
                    cat();
                    state = E_NEGITIVE;
                }
                else{
                    retract();
                    state = START;
                    outputNum();
                }
                break;

            case SMALL:
                getChar();
                if(ch == '='){
                    cat();
                    outputOP("LE");
                }
                else{
                    retract();
                    outputOP("LT");
                }
                state = START;
                break;

            case BIG:
                getChar();
                if(ch == '='){
                    cat();
                    outputOP("GE");
                }
                else{
                    retract();
                    outputOP("GT");
                }
                state = START;
                break;

            case SLASH:
                getChar();
                if(ch == '/'){
                    cat();
                    state = START;
                    skip();
                }
                else if(ch == '*'){
                    cat();
                    state = NOTE;
                }
                else{
                    retract();
                    state = START;
                    outputOP("");
                }
                break;
            
            case NOTE:
                getChar();
                if(ch == '*'){
                    getChar();
                    if( ch == '/' )
                        state = START;
                    else
                        retract();
                }
                else if(ch == '"'){
                    while(ch != '"')
                        getChar();
                }
                break;

            case ADD:
                getChar();
                if(ch == '+' || ch == '='){
                    cat();
                    state = START;
                    outputOP("");
                }
                else{
                    retract();
                    state = START;
                    outputOP("");
                }
                break;

            case SUB:
                getChar();
                if(ch == '-' || ch == '='){
                    cat();
                    state = START;
                    outputOP("");
                }
                else{
                    retract();
                    state = START;
                    outputOP("");
                }
                break;

            case EQUAL:
                getChar();
                if(ch == '='){
                    cat();
                    state = START;
                    outputOP("");
                }
                else{
                    retract();
                    state = START;
                    outputOP("");
                }
                break;

            case NOT:
                getChar();
                if(ch == '='){
                    cat();
                    state = START;
                    outputOP("");
                }
                else{
                    retract();
                    state = START;
                    outputOP("");
                }
                break;

            case AND:        //when ch == '&'
				getChar();
                if(ch == '&'){
                    cat();
                    state = START;
                    outputOP("");
                }
                else{
                    retract();
                    state = START;
                    outputOP("");
                }
                break;
                
            case DOUBLE_QUOTE:
            	getChar();
            	if(ch == '\n'){
            		state = START;
            		errorQuote();
            	}
            	else if(ch == '"'){
            		cat();
            		outputString();
            		state = START;
            	}
            	else{
            		cat();
            	}
            	break;
            
            default:       //when ch == '|'
				getChar();
                if(ch == '|'){
                    cat();
                    state = START;
                    outputOP("");
                }
                else{
                    retract();
                    state = START;
                    outputOP("");
                }
                break;
                
        } //end switch

    } //end while
    
	outputCount();
}


void readFile()
{
    ifstream infile;
    infile.open("test2.c");
    if(!infile)
        cout<<"Can't open this file !"<<endl;
    char c;
    string temp = "";
    buffer.push_back(temp);
    for(row = 0, line = 0; !infile.eof() ; ){
        infile.get(c);
        buffer[row] += c;
        if( c == '\n' ){
            buffer.push_back(temp);
			row++;
            line = 0;
        }
        else{
            line++;
        }
    }
    maxRow = row - 1;
    infile.close();
}


/*void outputBuffer()
{
    for(row = 0, line = 0; row < maxRow ; ){
        if(buffer[row][line] == '\n'){
            cout<<endl;
            row++;
            line = 0;
        }
        else{
            cout<<buffer[row][line];
            line++;
        }
    }
}*/

void getChar()
{
	ch = buffer[row][line];
	if(ch != '\n' && ch != ' ' && state != NOTE)
		words++;
	if(ch == '\n' || ch == '\0'){
		row++;
		line = 0;
	}
	else
		line++;
}

void getN()
{
	while(ch == ' ' || ch == '\n')
		getChar();
}

void retract()
{
	words = words - 2;
	if(line == 0){
		row--;
		while(buffer[row][line] != '\n')
			getChar();
	}
	else if(line == 1){
		line--;
		ch = '\n';
	}
	else{
		line = line -2;
		getChar();
	}
}

void skip()
{
	row++;
	line = 0;
}

void cat()
{
	str += ch;
}
	
bool isLetter()
{
	if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return true;
	else
		return false;
}

bool isDigit()
{
	if(ch >= '0' && ch <= '9')
		return true;
	else
		return false;
}
	

/* output */
void outputKey()
{
	cout<<str<<"  keyWord"<<endl;
}
void outputTable()
{
	cout<<str<<"  tableWord"<<endl;
}
void outputNum()
{
	cout<<str<<"  num"<<endl;
}
void outputOP(string s)
{
	if(str[0] != '	'){
		cout<<str<<"  operator  ";
		if(s.size() != 0)
			cout<<s<<endl;
		else
			cout<<"NULL"<<endl;
	}
}
void outputString()
{
	cout<<str<<"  string"<<endl;
}
void outputCount()
{
	cout<<endl;
	cout<<"Counting information : "<<endl;
	cout<<"rows: "<<maxRow<<endl;
	cout<<"keyWords: "<<keyWord<<endl;
	words--;    //sub the last getChar()
	cout<<"All words: "<<words<<endl;
}


bool isKeyWord()
{
	for(int i = 0 ; i < keyWordNum ; i++){
		if(str == keyWordTable[i])
			return true;
	}
	return false;
}
	
void tableInsert()
{
	table.push_back(str);
}
	
bool isInTable()
{
	for(int i = 0 ; i < table.size() ; i++){
		if(str == table[i])
			return true;
	}
	return false;
}


/* error */
void errorWord()
{
	cout<<ch<<"    word error in row "<<row<<endl;
}
void errorQuote()
{
	str += '"';
	cout<<str<<"  warning: a double quote is forgotton in row "<<row<<endl;
}
	



