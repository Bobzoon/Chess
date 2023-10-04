#include <iostream>
#include <string>
#include <cmath>

//-1 and 1 are used instead of 0 and 1 to denote direction across the board
const int BLACK = -1;
const int WHITE = 1;

class ChessBoard;
class King;
void newGame(ChessBoard* chessBoard);
bool onBoard(std::string location);
bool onBoard(int position);
bool check4checkmate(ChessBoard* chessBoard, King* atRisk);
void printBoard(int turn, ChessBoard* chessBoard);

class chessPiece{
protected:
    std::string name;
    int color;
    int position;
    bool hasMoved = false;
public:
    chessPiece(std::string name, std::string coord);
    chessPiece(const chessPiece& piece): name(piece.name), color(piece.color), position(piece.position) {}
    std::string getName() {return name;}
    int getColor() {return color;}
    int getPos() {return position;}
    void setPos(int pos) {position = pos;}
    bool getHasMoved() {return hasMoved;}
    void setHasMoved(bool move) {hasMoved = move;}
    //Move makes sure a move is theoretically legal for any piece (i.e starts and ends on the board and doesn't attack it's own color, etc.) and performs the movement
    virtual bool move(ChessBoard* chessBoard, int destination, int& turn);
    //isLegal makes sure that a move is legal for that piece (i.e. the destination is legal for that piece in that position)
    virtual bool isLegal(int destination, ChessBoard* chessBoard) =0;
    virtual bool isPathClear(chessPiece* board[], int destination);
};

class King: public chessPiece{
public:
    King(std::string name, std::string coord): chessPiece::chessPiece(name, coord) {}
    King(const chessPiece& piece): chessPiece::chessPiece(piece) {}
    virtual bool move(ChessBoard* chessBoard, int destination, int& turn);
    virtual bool isLegal(int destination, ChessBoard* chessBoard);
};

class Queen: public chessPiece{
public:
    Queen(std::string name, std::string coord): chessPiece::chessPiece(name, coord) {}
    Queen(const chessPiece& piece): chessPiece::chessPiece(piece) {}
    virtual bool isLegal(int destination, ChessBoard* chessBoard);
};

class Rook: public chessPiece{
public:
    Rook(std::string name, std::string coord): chessPiece::chessPiece(name, coord) {}
    Rook(const chessPiece& piece): chessPiece::chessPiece(piece) {}
    virtual bool isLegal(int destination, ChessBoard* chessBoard);
};

class Bishop: public chessPiece{
public:
    Bishop(std::string name, std::string coord): chessPiece::chessPiece(name, coord) {}
    Bishop(const chessPiece& piece): chessPiece::chessPiece(piece) {}
    virtual bool isLegal(int destination, ChessBoard* chessBoard);
};

class Knight: public chessPiece{
public:
    Knight(std::string name, std::string coord): chessPiece::chessPiece(name, coord) {}
    Knight(const chessPiece& piece): chessPiece::chessPiece(piece) {}
    virtual bool isLegal(int destination, ChessBoard* chessBoard);
};

class Pawn: public chessPiece{
public:
    Pawn(std::string name, std::string coord): chessPiece::chessPiece(name, coord) {}
    Pawn(const chessPiece& piece): chessPiece::chessPiece(piece) {}
    virtual bool isLegal(int destination, ChessBoard* chessBoard);
    virtual bool move(ChessBoard* chessBoard, int destination, int& turn);
    virtual bool isPathClear(chessPiece* board[], int destination);
    void promote(ChessBoard* chessBoard);
};

class ChessBoard{
private:
    chessPiece* board[64] {};
    King* kings[2]; //black and white kings in that order
    int movements[32][1];
    Pawn* enpassantable = nullptr;
public:
    ChessBoard() {}
    void addPiece(chessPiece* newPiece) {board[newPiece->getPos()] = newPiece;}
    chessPiece** getBoard() {return board;}
    King** getKings() {return kings;}
    Pawn* getEnpassantable() {return enpassantable;}
    void setEnpassantable(Pawn* piece) {enpassantable = piece;}
    chessPiece* isThreatened(int position, int color, bool includeKings = true);
    chessPiece* isThreatened(chessPiece* piece);
    friend class chessPiece;
    friend class King;
    friend class Queen;
    friend class Rook;
    friend class Bishop;
    friend class Knight;
    friend class Pawn;
};

chessPiece::chessPiece(std::string name, std::string coord): name(name){
    if('a' <= name[0] && name[0] <= 'z')
        color = BLACK;
    else if('A' <= name[0] && name[0] <= 'Z')
        color = WHITE;
    position = coord[0] - 'a' + ((coord[1] - '0') - 1) * 8;
}

bool chessPiece::isPathClear(chessPiece* board[], int destination){
    int tempPos = position, colDir, rowDir;
    colDir = (position % 8 != destination % 8? (destination % 8 - position % 8) / abs(destination % 8 - position % 8): 0);
    rowDir = (position / 8 != destination / 8? (destination / 8 - position / 8) / abs(destination / 8 - position / 8): 0);
    
    while (tempPos + (8 * rowDir) + (colDir) != destination){
        tempPos += (8 * rowDir + colDir);
        if(board[tempPos] != nullptr)
            return false;
    }
    return true;
}

bool chessPiece::move(ChessBoard* chessBoard, int destination, int& turn){
    chessPiece** positions = chessBoard->getBoard();
    King** kings = chessBoard->getKings();
    if(color != turn){
        std::cout<<"It is "<<(turn == WHITE? "white's " : "black's ")<<"turn\n";
        return false;
    }

    if(positions[destination] != nullptr){
        if(positions[destination]->color == color){
            std::cout<<"You cannot attack your own piece\n";
            return false;
        }
    }
    
    if(name[1] != 'n' && name[1] != 'N'){
        if(!isPathClear(positions, destination)){
            std::cout<<"Not a legal move\n";
            return false;
        }
    }

    if(!isLegal(destination, chessBoard)){
        std::cout<<"Not a legal move\n";
        return false;
    }

    //Test if the move places your own king in check
    chessPiece* temp = positions[destination];
    positions[destination] = this;
    positions[position] = nullptr;
    if(chessBoard->isThreatened(kings[int(0.5 * turn + 0.5)])){
        std::cout<<"You cannot put your own king in check.\n";
        positions[position] = this; //undo the illegal move
        positions[destination] = temp;
        return false;
    }

    delete temp;
    positions[destination] = this;

    position = destination;
    chessBoard->enpassantable = nullptr;
    hasMoved = true;
    return true;
}

chessPiece* ChessBoard::isThreatened(chessPiece* piece){return isThreatened(piece->getPos(), piece->getColor() * -1);}

chessPiece* ChessBoard::isThreatened(int position, int threatColor, bool includeKings){
    int knPosArr[8] = {-17, -15, -10, -6, 6, 10, 15, 17};
    int dirs[8] = {-9, -8, -7, -1, 1, 7, 8, 9};
    int knPos, dir, offset; 
    for(int i = 0; i < 8; i++){
        //Check Knight Positions
        knPos = knPosArr[i];
        if(onBoard(position + knPos)){
            if(board[position + knPos] != nullptr){
                if(board[position + knPos]->getColor() == threatColor && board[position + knPos]->isLegal(position, this))
                    return board[position + knPos];
            }
        }
        //Check rank, file, and diagonal positions
        dir = dirs[i];
        offset = dir;
        while(onBoard(position + dir)){
            if(board[position + dir] != nullptr){
                if(board[position + dir]->getColor() == threatColor && board[position + dir]->isLegal(position, this))
                    if(includeKings)
                        return board[position + dir];
                    else
                        break;
                break;
            }
            dir += offset;
        }
    }
    return nullptr;
}

bool Knight::isLegal(int destination, ChessBoard* chessBoard){
    if(abs(destination % 8 - position % 8) == 2 && abs(destination / 8 - position / 8) == 1 ||
    abs(destination % 8 - position % 8) == 1 && abs(destination / 8 - position / 8) == 2)
        return true;
    return false;
}

bool Bishop::isLegal(int destination, ChessBoard* chessBoard){
    if(abs(destination % 8 - position % 8) == abs(destination / 8 - position / 8))
        return true;
    return false;
}

bool Rook::isLegal(int destination, ChessBoard* chessBoard){
    if(destination % 8 - position % 8 == 0 || destination / 8 - position / 8 == 0){
        return true;
    }
    return false;
}

bool Queen::isLegal(int destination, ChessBoard* chessBoard){
    if(destination % 8 - position % 8 == 0 || destination / 8 - position / 8 == 0 || abs(destination % 8 - position % 8) == abs(destination / 8 - position / 8))
        return true;
    return false;
}

bool King::move(ChessBoard* chessBoard, int destination, int& turn){
    chessPiece** positions = chessBoard->getBoard();
    King** kings = chessBoard->getKings();
    if(color != turn){
        std::cout<<"It is "<<(turn == WHITE? "white's " : "black's ")<<"turn\n";
        return false;
    }

    if(positions[destination] != nullptr){
        if(positions[destination]->getColor() == color){
            std::cout<<"You cannot attack your own piece\n";
            return false;
        }
    }

    if(!isLegal(destination, chessBoard)){
        std::cout<<"Not a legal move\n";
        return false;
    }

    //Check if the move endangers the king
    chessPiece* temp = positions[destination];
    positions[destination] = this;
    positions[position] = nullptr;
    if(chessBoard->isThreatened(destination, color * -1)){
        std::cout<<"You cannot put your own king in check.\n";
        positions[position] = this; //undo the illegal move
        positions[destination] = temp;
        return false;
    }

    delete temp;
    positions[destination] = this;

    position = destination;
    chessBoard->enpassantable = nullptr;
    hasMoved = true;

    //When castling, deal with the castle's movement
    if(abs(destination - position) == 2){
        int moveDir = (destination > position? 1 : -1);
        int castlePos = (destination > position? position + 3 : position - 4);
        positions[position + moveDir] = positions[castlePos];
        positions[castlePos]->setHasMoved(true);
        positions[castlePos]->setPos(position + moveDir);
        positions[castlePos] = nullptr;
    }
    return true;
}

bool King::isLegal(int destination, ChessBoard* chessBoard){
    if(abs(destination % 8 - position % 8) == 1 || abs(destination / 8 - position / 8) == 1){
        return true;
    }
    else if((destination % 8 == 6 || destination % 8 == 2) && !hasMoved){ //castling
        chessPiece** board = chessBoard->board;
        int castlePos = (destination % 8 == 6? position + 3 : position - 4);
        if(board[castlePos]->getHasMoved())
            return false;
        if(!isPathClear(board, castlePos))
            return false;
        int moveDir = (destination % 8 == 6? 1 : -1);
        for(int i = 0; i <= 2; i++){
            if(chessBoard->isThreatened(position + i * moveDir, color * -1))
                return false;
        }
        return true;
    }
    return false;
}

//When not attacking, pawn needs every square including the one it lands on to be clear
bool Pawn::isPathClear(chessPiece* board[], int destination){
    int tempPos = position + 8 * color;
    while((destination - tempPos) * color >= 0){ //Until destination has been checked
        if(board[tempPos] != nullptr)
            return false;
        tempPos += 8 * color;
    }
    return true;
}

bool Pawn::move(ChessBoard* chessBoard, int destination, int& turn){
    chessPiece** positions = chessBoard->getBoard();
    King** kings = chessBoard->getKings();
    if(color != turn){
        std::cout<<"It is "<<(turn == WHITE? "white's " : "black's ")<<"turn\n";
        return false;
    }

    if(positions[destination] != nullptr){
        if(positions[destination]->getColor() == color){
            std::cout<<"You cannot attack your own piece\n";
            return false;
        }
    }

    if(!isLegal(destination, chessBoard)){
        std::cout<<"Not a legal move\n";
        return false;
    }   
    
    //Requiring a clear path is only relevant for pawns when not attacking
    //Becasue pawns can't skip squares while attacking
    if(position % 8 == destination % 8){
        if(!isPathClear(positions, destination)){
            std::cout<<"Not a legal move\n";
            return false;
        }
    }

    if(destination / 8 == color * 2 + position / 8)
        chessBoard->enpassantable = this;
    else
        chessBoard->enpassantable = nullptr;

    chessPiece* temp = positions[destination];
    positions[destination] = this;
    positions[position] = nullptr;
    if(chessBoard->isThreatened(kings[int(0.5 * turn + 0.5)])){
        std::cout<<"You cannot put your own king in check.\n";
        positions[position] = this; //undo the illegal move
        positions[destination] = temp;
        return false;
    }
    delete temp;
    position = destination;

    if(destination / 8 == 0 || destination / 8 == 7)
        promote(chessBoard);
    return true;
}

bool Pawn::isLegal(int destination, ChessBoard* chessBoard){
    chessPiece** board = chessBoard->board;
    if(destination == 8 * color + position){ //Normal movement
        hasMoved = true;
        return true;
    }
    else if(hasMoved == false && destination / 8 == color * 2 + position / 8){ //Double move
        hasMoved = true;
        return true;
    }
    else if(destination / 8 == color + position / 8 && abs(destination % 8 - position % 8) == 1 && board[destination] != nullptr){ //Normal attack
        hasMoved = true;
        return true;
    }
    else if((destination / 8 == color + position / 8) && (abs(destination % 8 - position % 8) == 1) 
    && (board[destination + 8 * -1 * color] != nullptr) && (chessBoard->enpassantable == board[destination + 8 * -1 * color])){ //En passant
        delete board[destination + 8 * -1 * color];
        board[destination + 8 * -1 * color] = nullptr;
        return true;
    }
    return false;
}

void Pawn::promote(ChessBoard* chessBoard){
    char selection;
    std::cout<<"Select one to promote your pawn to: Queen(Q), Rook(R), Bishop(B), Knight(N)\n";
    std::cin>>selection;
    Pawn* temp = this;

    char row = (position / 8) + '0' + 1;
    char column = (position % 8) + 'a';
    std::string coord;
    coord += column;
    coord += row;
    std::string newName;
    newName += name[0];
    
    switch (selection){
    case 'Q':
        chessBoard->board[position] = new Queen(newName + "Q", coord);
        delete temp;
        break;
    case 'R':
        chessBoard->board[position] = new Rook(newName + "R", coord);
        delete temp;
        break;
    case 'B':
        chessBoard->board[position] = new Bishop(newName + "B", coord);
        delete temp;
        break;
    case 'N':
        chessBoard->board[position] = new Knight(newName + "N", coord);
        delete temp;
        break;
    default:
        std::cout<<"Invalid input\n";
        promote(chessBoard);
        return;
    }
    return;
}

int main()
{
    std::cout<<"Welcome to chess!\n"
    <<"Pieces other than pawns are represented with two letters. The first is either q or k, indicating if it started on the queen or king side of the board.\n"
    <<"Uppercase first letters represent white's pieces, lowercase black's.\n"
    <<"The second letter is one of the following: R, N, B, Q, or K for rook, knight, bishop, queen and king respectively.\n"
    <<"Pawns are represented as P1 - P8 using the same capitalization scheme, or p followed by the letter representing the new piece following promotion.\n"
    <<"To move, enter the position of the piece you want to move, e.g. b5, using lowercase letters"
    <<" followed after a space by the position you want the piece to move to.\n"
    <<"As an example, white might make the opening move e2 e4.\n"
    <<"Castling is indicated from the king's point of view.\n"
    <<"Stalemates are not implemented.\n";
    int turn = WHITE;
    ChessBoard* chessBoard = new ChessBoard();
    std::string src, dest;
    newGame(chessBoard);
    while(true){
        printBoard(turn, chessBoard);
        std::cin>>src>>dest;
        int srcPos = src[0] - 'a' + ((src[1] - '0') - 1) * 8;
        int destPos = dest[0] - 'a' + ((dest[1] - '0') - 1) * 8;
        if(!onBoard(src) || !onBoard(dest)){
            std::cout<<"Invalid address entered\n";
            continue;
        }
        if(srcPos == destPos || chessBoard->getBoard()[srcPos] == nullptr){
            std::cout<<"A piece must move every turn\n";
            continue;
        }
        if(!chessBoard->getBoard()[srcPos]->move(chessBoard, destPos, turn))
            continue;
        if(check4checkmate(chessBoard, chessBoard->getKings()[int(-0.5 * turn + 0.5)])){
            std::cout<<(turn == WHITE? "WHITE" : "BLACK")<<" WINS!\n";
            break;
        }
        turn = turn * -1;
    }
    return 0;
}

void newGame(ChessBoard* chessBoard){
    Rook* QR = new Rook("QR", "a1");
    chessBoard->addPiece(QR);
    Knight* QN = new Knight("QN", "b1");
    chessBoard->addPiece(QN);
    Bishop* QB = new Bishop("QB", "c1");
    chessBoard->addPiece(QB);
    Queen* QQ = new Queen("QQ", "d1");
    chessBoard->addPiece(QQ);
    King* KK = new King("KK", "e1");
    chessBoard->addPiece(KK);
    Bishop* KB = new Bishop("KB", "f1");
    chessBoard->addPiece(KB);
    Knight* KN = new Knight("KN", "g1");
    chessBoard->addPiece(KN);
    Rook* KR = new Rook("KR", "h1");
    chessBoard->addPiece(KR);
    Pawn* P1 = new Pawn("P1", "a2");
    chessBoard->addPiece(P1);
    Pawn* P2 = new Pawn("P2", "b2");
    chessBoard->addPiece(P2);
    Pawn* P3 = new Pawn("P3", "c2");
    chessBoard->addPiece(P3);
    Pawn* P4 = new Pawn("P4", "d2");
    chessBoard->addPiece(P4);
    Pawn* P5 = new Pawn("P5", "e2");
    chessBoard->addPiece(P5);
    Pawn* P6 = new Pawn("P6", "f2");
    chessBoard->addPiece(P6);
    Pawn* P7 = new Pawn("P7", "g2");
    chessBoard->addPiece(P7);
    Pawn* P8 = new Pawn("P8", "h2");
    chessBoard->addPiece(P8);
    
    Rook* qR = new Rook("qR", "a8");
    chessBoard->addPiece(qR);
    Knight* qN = new Knight("qN", "b8");
    chessBoard->addPiece(qN);
    Bishop* qB = new Bishop("qB", "c8");
    chessBoard->addPiece(qB);
    Queen* qQ = new Queen("qQ", "d8");
    chessBoard->addPiece(qQ);
    King* kK = new King("kK", "e8");
    chessBoard->addPiece(kK);
    Bishop* kB = new Bishop("kB", "f8");
    chessBoard->addPiece(kB);
    Knight* kN = new Knight("kN", "g8");
    chessBoard->addPiece(kN);
    Rook* kR = new Rook("kR", "h8");
    chessBoard->addPiece(kR);
    Pawn* p1 = new Pawn("p1", "a7");
    chessBoard->addPiece(p1);
    Pawn* p2 = new Pawn("p2", "b7");
    chessBoard->addPiece(p2);
    Pawn* p3 = new Pawn("p3", "c7");
    chessBoard->addPiece(p3);
    Pawn* p4 = new Pawn("p4", "d7");
    chessBoard->addPiece(p4);
    Pawn* p5 = new Pawn("p5", "e7");
    chessBoard->addPiece(p5);
    Pawn* p6 = new Pawn("p6", "f7");
    chessBoard->addPiece(p6);
    Pawn* p7 = new Pawn("p7", "g7");
    chessBoard->addPiece(p7);
    Pawn* p8 = new Pawn("p8", "h7");
    chessBoard->addPiece(p8);
    
    chessBoard->getKings()[0] = kK;
    chessBoard->getKings()[1] = KK;
}

bool onBoard(std::string location){
    if('a' <= location[0] && location[0] <= 'h' && '1' <= location[1] && location[1] <= '8' && location.length() == 2)
        return true;
    return false;
}

bool onBoard(int position){
    if(0 <= position && position <= 63)
        return true;
    return false;
}

bool check4checkmate(ChessBoard* chessBoard, King* atRisk){
    chessPiece** board = chessBoard->getBoard();
    chessPiece* threatening = chessBoard->isThreatened(atRisk);
    if(!threatening)
        return false;
    int surroundings[8] = {-9, -8, -7 ,-1, 1, 7, 8, 9};
    for(int dir: surroundings){
        int check = atRisk->getPos() + dir;
        if(onBoard(check)){
            if((board[check] == nullptr || board[check]->getColor() != atRisk->getColor()) && !chessBoard->isThreatened(check, atRisk->getColor() * -1))
                return false;
        }
    }
    int tempPos = atRisk->getPos(), colDir, rowDir;
    colDir = (tempPos % 8 != threatening->getPos() % 8? (threatening->getPos() % 8 - tempPos % 8) / abs(threatening->getPos() % 8 - tempPos % 8): 0);
    rowDir = (tempPos / 8 != threatening->getPos() / 8? (threatening->getPos() / 8 - tempPos / 8) / abs(threatening->getPos() / 8 - tempPos / 8): 0);
    while (board[tempPos] != threatening){
        tempPos += (8 * rowDir + colDir);
        if(chessBoard->isThreatened(tempPos, atRisk->getColor(), false))
            return false;
    }
    return true;
}

void printBoard(int turn, ChessBoard* chessBoard){
    std::cout<<std::endl;
    for(char letter = -3.5 * turn + 100.5; 'a' <= letter && letter <= 'h'; letter += turn)
        std::cout<<"\t"<<letter;
        std::cout<<std::endl;

    for(int i = 3.5 * turn + 4.5; 1 <= i && i <= 8; i += turn * -1){
        std::cout<<i<<"\t";
        for(int j = 8 * i - (3.5 * turn + 4.5); 8 * (i - 1) <= j && j < 8 * i; j += turn){
            if(chessBoard->getBoard()[j] != nullptr)
                std::cout<<chessBoard->getBoard()[j]->getName()<<"\t";
            else
                std::cout<<"--\t";
        }
        std::cout<<i<<std::endl;
    }

    for(char letter = -3.5 * turn + 100.5; letter >= 'a' && letter <= 'h'; letter += turn)
        std::cout<<"\t"<<letter;
    std::cout<<std::endl;
    return;
}