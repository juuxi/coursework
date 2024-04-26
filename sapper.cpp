#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <time.h>
#include <iostream>
#include <time.h>

const char BOMB[] = "\U0001f4a3";
const char FLOOR[] = "\U00002b1c";
const char CLOSED_FLOOR[] = "\U0001f533";
const char SMILE[] = "\U0001f610";
const char FLAG[] = "\U0001f6a9";

#define BOMB_NUM_CL 999
#define BOMB_NUM_OP -1
#define SMILE_NUM 300
#define FLOOR_NUM 0
#define WIN_END 1
#define LOSE_END 0
#define QUIT_END 81

static struct termios oldt;

void restore_terminal_settings(void)
{
tcsetattr(0, TCSANOW, &oldt); /* Apply saved settings */
}

void disable_waiting_for_enter(void)
{
struct termios newt;

/* Make terminal read 1 char at a time */
tcgetattr(0, &oldt); /* Save terminal settings */
newt = oldt; /* Init new settings */
newt.c_lflag &= ~(ICANON | ECHO); /* Change settings */
tcsetattr(0, TCSANOW, &newt); /* Apply settings */
atexit(restore_terminal_settings); /* Make sure settings will be restored when program ends */
}

struct coordinates {
    int x;
    int y;
};

struct NodeC{
    char symb;
    NodeC* next;
};

struct MyStack{
    NodeC* first;
    MyStack()
    {
        first = nullptr;
    }

    void get_all(char movements[40]) 
    {
        int i = 0;
        NodeC* curr = first;
        while(curr != nullptr)
        {
            movements[i] = curr->symb;
            curr = curr->next;
            i++;
        }
    }

    void remove_first()
    {
        if(first == nullptr) return;
        NodeC* p = first;
        first = p->next;
        delete p;
        p = nullptr;
    }

    void push_front(char _symb)
    {
        if(first == nullptr)
        {
            first = new NodeC;
            first->next = nullptr;
            first->symb = _symb;
            return;
        }
        NodeC* curr = first;
        first = new NodeC;
        first->next = curr;
        first->symb = _symb;
    }

    void remove_stack()
    {
        while(first != nullptr)
        {
            NodeC* p = first;
            first = p->next;
            delete p;
            p = nullptr;
        }
    }
};

struct Node{
    coordinates val;
    Node* next;
    Node(coordinates _val) 
    {
        val = _val;
        next = nullptr;
    }
};

struct MyList{
    Node* first;
    Node* last;
    MyList()
    {
        first = nullptr;
        last = nullptr;
    }

    bool is_empty()
    {
        return first == nullptr;
    }

    void push_back(coordinates _val)
    {
        Node* p = new Node(_val);
        if(is_empty())
        {
            first = p;
            last = p;
            return;
        }
        last->next = p;
        last = p;
    }

    Node* get_first() 
    {
        if(is_empty()) return nullptr;
        return first;
    }

    void remove_first()
    {
        if(is_empty()) return;
        Node* p = first;
        first = p->next;
        delete p;
        p = nullptr;
    }

    void remove_list()
    {
        while(!is_empty())
        {
            Node* p = first;
            first = p->next;
            delete p;
            p = nullptr;
        }
    }
};

struct preMovement{
    int x;
    int y;
    int value;
};

    MyStack stack;

void clean_stdin(void) //очистка потока ввода, применяется там где без нее scanf начнет читать всякие необработанные символы
{
int c;
do {
c = getchar();
} while (c != '\n' && c != EOF);
}

int symbol_scanning(int overfmax) //эта функция занимается посимвольной читкой введенного числа
{
    bool inputfailure = false, count = false, r = false, overf = false;
    int num = 0, counter = 0; //отобрать возможность сразу нажать "энтр"
    while(true){
                overf = false;
                inputfailure = false;
                char c = '\0';
                scanf("%c", &c);
                if (c < '0' || c > '9')   //если прочитанный символ - не цифра, лезем проверять что же это              
                {
                    if (c != '\n') {printf("Вы ввели неверный символ, повторите попытку\n"); clean_stdin();} // если это не перенос строки - кидаем ошибку
                    else if(counter != 0) {count = true; break;} //если это перенос строки не первым же символом - все круто, прерываем цикл
                    else {printf("Вы ввели неверный символ, повторите попытку\n");} //если первым - выдаем ошибку
                }
                else{
                if (num*10+c-'0' > overfmax) {overf = true; num = 0; clean_stdin(); r = true;} //если дописывание полученной цифры к имеющемуся (см. ниже)
                else r = false;  //числу не выйдет за установленные нами рамки (число, которое мы указали при вызове функции) - записываем его (см. ниже) 
                if(r == false)   //если же выйдет - выходим из цикла с ошибкой overf
                num = num*10+c-'0';
                }
                if(overf == true) printf("Число превысило максимальный размер (%d). Повторите попытку\n", overfmax);
                counter++;
    }
    return num;
}

int symbol_scanning(int overfmax, int overfmin) 
{
    bool inputfailure = false, count = false, r = false, overf = false;
    int num = 0, counter = 0; 
    while(true)
    {
        overf = false;
        inputfailure = false;
        char c = '\0';
        scanf("%c", &c);
        if (c < '0' || c > '9')        
        {
            if (c != '\n') 
            {
                printf("Вы ввели неверный символ, повторите попытку\n"); 
                clean_stdin();
            } 
            else 
            {
                if(num < overfmin) 
                {
                    printf("Число меньше минимального (%d). Повторите попытку\n", overfmin);
                    inputfailure = true;
                    num = 0;
                    counter = 0;
                }
                else
                {
                    count = true; 
                    break;
                }
            } 
        }
        else
        {
            if (num*10+c-'0' > overfmax) 
            {
                overf = true; 
                inputfailure = true;
                num = 0; 
                counter = 0; 
                clean_stdin();
            } 
            else  
                num = num*10+c-'0';
        }

        if(!inputfailure) counter++;

        if(overf == true) 
        {
            printf("Число превысило максимальный размер (%d). Повторите попытку\n", overfmax);
            counter = 0;
        }
    }
    return num;
}

void mark_around(int pitch[20][20], int n, int m, int i, int j)
{
    if(i != n-1 && i != 0 && j != 0 && j != m-1)
    {
        if(pitch[i+1][j] != BOMB_NUM_CL) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != BOMB_NUM_CL) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM_CL) pitch[i][j+1]++;
        if(pitch[i-1][j+1] != BOMB_NUM_CL) pitch[i-1][j+1]++;
        if(pitch[i-1][j] != BOMB_NUM_CL) pitch[i-1][j]++;
        if(pitch[i-1][j-1] != BOMB_NUM_CL) pitch[i-1][j-1]++;
        if(pitch[i][j-1] != BOMB_NUM_CL) pitch[i][j-1]++;
        if(pitch[i+1][j-1] != BOMB_NUM_CL) pitch[i+1][j-1]++;
    }

    if(i == 0 && j == 0)
    {
        if(pitch[i+1][j] != BOMB_NUM_CL) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != BOMB_NUM_CL) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM_CL) pitch[i][j+1]++;
    }
    if(i == n-1 && j == 0)
    {
        if(pitch[i-1][j] != BOMB_NUM_CL) pitch[i-1][j]++;
        if(pitch[i-1][j+1] != BOMB_NUM_CL) pitch[i-1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM_CL) pitch[i][j+1]++;
    }
    if(i == n-1 && j == m-1)
    {
        if(pitch[i][j-1] != BOMB_NUM_CL) pitch[i][j-1]++;
        if(pitch[i-1][j-1] != BOMB_NUM_CL) pitch[i-1][j-1]++;
        if(pitch[i-1][j] != BOMB_NUM_CL) pitch[i-1][j]++;
    }
    if(i == 0 && j == m-1)
    {
        if(pitch[i+1][j] != BOMB_NUM_CL) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != BOMB_NUM_CL) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM_CL) pitch[i][j+1]++;
    }

    if(i == 0 && j != 0 && j != m-1)
    {
        if(pitch[i][j-1] != BOMB_NUM_CL) pitch[i][j-1]++;
        if(pitch[i+1][j-1] != BOMB_NUM_CL) pitch[i+1][j-1]++;
        if(pitch[i+1][j] != BOMB_NUM_CL) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != BOMB_NUM_CL) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM_CL) pitch[i][j+1]++;
    }
    if(i == n-1 && j != 0 && j != m-1)
    {
        if(pitch[i][j+1] != BOMB_NUM_CL) pitch[i][j+1]++;
        if(pitch[i-1][j+1] != BOMB_NUM_CL) pitch[i-1][j+1]++;
        if(pitch[i-1][j] != BOMB_NUM_CL) pitch[i-1][j]++;
        if(pitch[i-1][j-1] != BOMB_NUM_CL) pitch[i-1][j-1]++;
        if(pitch[i][j-1] != BOMB_NUM_CL) pitch[i][j-1]++;
    }
    if(j == 0 && i != 0 && i != n-1)
    {
        if(pitch[i+1][j] != BOMB_NUM_CL) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != BOMB_NUM_CL) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM_CL) pitch[i][j+1]++;
        if(pitch[i-1][j+1] != BOMB_NUM_CL) pitch[i-1][j+1]++;
        if(pitch[i-1][j] != BOMB_NUM_CL) pitch[i-1][j]++;
    }
    if(j == m-1 && i != 0 && i != n-1)
    {
        if(pitch[i-1][j] != BOMB_NUM_CL) pitch[i-1][j]++;
        if(pitch[i-1][j-1] != BOMB_NUM_CL) pitch[i-1][j-1]++;
        if(pitch[i][j-1] != BOMB_NUM_CL) pitch[i][j-1]++;
        if(pitch[i+1][j-1] != BOMB_NUM_CL) pitch[i+1][j-1]++;
        if(pitch[i+1][j] != BOMB_NUM_CL) pitch[i+1][j]++;
    }
}

void creating(int pitch[20][20], int n, int m, int numOfBombs)
{   
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            pitch[i][j] = 1000;
        }
    }
    srand(time(NULL)); 
    int bombCounter = 0;
    for(int i = 0; bombCounter < numOfBombs; i++)
    {
        int o = rand() % n;
        int p = rand() % m;
        if(pitch[o][p] != BOMB_NUM_CL)
        {
            pitch[o][p] = BOMB_NUM_CL;
            mark_around(pitch, n, m, o, p);
            bombCounter++;
        }
    }
}

void printPitch(int pitch[20][20], int n, int m)
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            if(pitch[i][j] == BOMB_NUM_OP) 
            printf("%3s", BOMB);

            if(pitch[i][j] == FLOOR_NUM)
            printf("%3s", FLOOR);

            if(pitch[i][j] > 1098 || (pitch[i][j] > 99 && pitch[i][j] < 110))
            printf("%3s", FLAG);

            if(pitch[i][j] == SMILE_NUM)
            printf("%3s", SMILE);

            if(pitch[i][j] > 998 && pitch[i][j] < 1098)
            printf("%3s", CLOSED_FLOOR);

            if(pitch[i][j] > 0 && pitch[i][j] < 10)
            printf("%2d", pitch[i][j]);
        }
        printf("\n");
    }
}

void digging(int pitch[20][20], int n, int m, int ySmile, int xSmile, int &value, int &numOfEmpty);
void diggingReverse(int pitch[20][20], int n, int m, int ySmile, int xSmile, int &value, int &numOfEmpty);

int movingSmile(int pitch[20][20], int n, int m, int& ySmile, int& xSmile, int numOfBombs, char movements[40], int& value, bool reverse)
{
    int endofmove = LOSE_END;
    preMovement preM;
    int numOfEmpty = m * n, i = 0;
    preM.x = xSmile;
    preM.y = ySmile;
    if(!reverse) value = pitch[ySmile][xSmile];
    pitch[ySmile][xSmile] = SMILE_NUM;
    bool endd = false;
    if(reverse) printf("\n");
    printPitch(pitch, n, m);
    while(true)
    {
        if(!reverse) printf("Нажимайте клавиши w, a, s, d для управления смайликом и p чтобы открыть поле под собой.\nДля выхода нажмите Q\n");
        char movement;
        if(!reverse)
        {
            disable_waiting_for_enter();
            movement = getchar();
            restore_terminal_settings();
        }
        else 
        {
            movement = movements[i];
            
            switch (movement)
            {
                case 'w': movement = 's'; break;
                case 'a': movement = 'd'; break;
                case 's': movement = 'w'; break;
                case 'd': movement = 'a'; break;
            }
            i++;
        }

        if(reverse) 
        {
            disable_waiting_for_enter();
            getchar();
            restore_terminal_settings();
        }

        switch(movement)
        {
            case 'w': 
            pitch[ySmile][xSmile] = value;
            if(ySmile != 0) ySmile -= 1;
            value = pitch[ySmile][xSmile];
            pitch[ySmile][xSmile] = SMILE_NUM;
            if(!reverse) stack.push_front('w');
            break;

            case 'a':
            pitch[ySmile][xSmile] = value;
            if(xSmile != 0) xSmile -= 1;
            value = pitch[ySmile][xSmile];
            pitch[ySmile][xSmile] = SMILE_NUM;
            if(!reverse) stack.push_front('a');
            break;

            case 's':
            pitch[ySmile][xSmile] = value;
            if(ySmile != n-1) ySmile += 1;
            value = pitch[ySmile][xSmile];
            pitch[ySmile][xSmile] = SMILE_NUM;
            if(!reverse) stack.push_front('s');
            break;

            case 'd':
            pitch[ySmile][xSmile] = value;
            if(xSmile != m-1) xSmile += 1;
            value = pitch[ySmile][xSmile];
            pitch[ySmile][xSmile] = SMILE_NUM;
            if(!reverse) stack.push_front('d');
            break;

            case 'm':
            if (value > 1098 || (value > 99 && value < 110)) value -= 100;
            else value += 100;
            if(!reverse) stack.push_front('m');
            break;

            case 'p':
            if(!reverse)
            {
                digging(pitch, n, m, ySmile, xSmile, value, numOfEmpty);
                if(value == 999) 
                {
                    endd = true;
                    printf("Нажмите любую клавишу для продолжения\n");
                    disable_waiting_for_enter();
                    getchar();
                    restore_terminal_settings();
                }
                if(numOfEmpty == numOfBombs)
                {
                    endd = true; 
                    endofmove = WIN_END;
                }
                stack.push_front('p');
            }
            else diggingReverse(pitch, n, m, ySmile, xSmile, value, numOfEmpty);
            break;

            case 'Q': endd = true; endofmove = QUIT_END; break;
        }
        system("clear");
        printPitch(pitch, n, m);
        if(movements[i] == '\0' && reverse) break;
        if(endd == true) 
            break;
    }
    return endofmove;
}

void digging(int pitch[20][20], int n, int m, int ySmile, int xSmile, int &value, int &numOfEmpty)
{
    if(value == 999 || value == 1099)
    {
        pitch[ySmile][xSmile] = value;
        for(int i = 0; i < n; ++i)
        {
            for(int j = 0; j < m; j++)
            {
            if(pitch[i][j] > 998) pitch[i][j] -= 1000;
            if(pitch[i][j] == 100 || pitch[i][j] == 99 || pitch[i][j] > 100 && pitch[i][j] < 110) pitch[i][j] -= 100;
            }
        }
        system("clear");
        printPitch(pitch, n, m);
        printf("Вы проиграли\n");
    }

    if(value == 1000 || value == 1100)
    {
        MyList list;
        coordinates coord;
        coord.y = ySmile;
        coord.x = xSmile;
        list.push_back(coord);
        Node* checker;
        if(value == 1000) value -= 1000;
        if(value == 1100) value -= 1100;
        numOfEmpty--;
        int toCheck = 4;
        int nChecker = 0;
        while(true)
        {
            for(int i = 0; i < toCheck; ++i)
            {
                checker = list.get_first();
                if(checker != nullptr)
                {                    
                    if(checker->val.y != 0)
                    {
                    if(pitch[checker->val.y-1][checker->val.x] == 1000)
                    {
                        pitch[checker->val.y-1][checker->val.x] -= 1000;
                        numOfEmpty--;
                        coord.y = checker->val.y-1;
                        coord.x = checker->val.x;
                        list.push_back(coord);
                        nChecker++;
                    }
                    if(pitch[checker->val.y-1][checker->val.x] > 1000)
                    {
                        pitch[checker->val.y-1][checker->val.x] -= 1000;
                        numOfEmpty--;
                    }
                    }

                    if(checker->val.y != n - 1)
                    {
                    if(pitch[checker->val.y+1][checker->val.x] == 1000)
                    {
                        pitch[checker->val.y+1][checker->val.x] -= 1000;
                        numOfEmpty--;
                        coord.y = checker->val.y+1;
                        coord.x = checker->val.x;
                        list.push_back(coord);
                        nChecker++;
                    }
                    if(pitch[checker->val.y+1][checker->val.x] > 1000)
                    {
                        pitch[checker->val.y+1][checker->val.x] -= 1000;
                        numOfEmpty--;
                    }
                    }

                    if(checker->val.x != 0)
                    {
                    if(pitch[checker->val.y][checker->val.x-1] == 1000)
                    {
                        pitch[checker->val.y][checker->val.x-1] -= 1000;
                        numOfEmpty--;
                        coord.y = checker->val.y;
                        coord.x = checker->val.x-1;
                        list.push_back(coord);
                        nChecker++;
                    }
                    if(pitch[checker->val.y][checker->val.x-1] > 1000)
                    {
                        pitch[checker->val.y][checker->val.x-1] -= 1000;
                        numOfEmpty--;
                    }
                    }

                    if(checker->val.x != m - 1)
                    {
                    if(pitch[checker->val.y][checker->val.x+1] == 1000)
                    {
                        pitch[checker->val.y][checker->val.x+1] -= 1000;
                        numOfEmpty--;
                        coord.y = checker->val.y;
                        coord.x = checker->val.x+1;
                        list.push_back(coord);
                        nChecker++;
                    }
                    if(pitch[checker->val.y][checker->val.x+1] > 1000)
                    {
                        pitch[checker->val.y][checker->val.x+1] -= 1000;
                        numOfEmpty--;
                    }
                    }

                    list.remove_first();
                }
            }
            if(nChecker == 0) break;
            else toCheck = nChecker;
            nChecker = 0;
        }
    }
    
    if(value > 1000 && value < 1010)
    {
        value -=1000;
        numOfEmpty--;
    }    
    if(value > 1100 && value < 1110)
    {
        value -= 1100;
        numOfEmpty--;
    }
}

void diggingReverse(int pitch[20][20], int n, int m, int ySmile, int xSmile, int &value, int &numOfEmpty)
{
    if(value > 0 && value < 10)
    {
        value +=1000;
        numOfEmpty++;
    }  
}

void newRecord(float _time, char _name[25])
{
    char name[10][25];
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 25; j++)
            name[i][j] = 32;
    }
    float time[10];
    int counter = 0, num = -1;
    FILE* in;
    FILE* out;
    in = fopen("table.txt", "r");
    if (!in)
    {
        printf("Ошибка при открытии файла\n"); 
        return;
    }
    for (int i = 0 ; i < 10; ++i)
    {
        if (!fscanf(in,"%d %s %f", &counter, &*(name[i]), &time[i]))
        break;
    }
    fclose(in);
    for (int i = 0; i < counter; ++i)
    {
        if(_time < time[i])
        {
            num = i;
            break;
        }
    }
    if (num == -1 && counter == 10)
    {
        printf("Ваше время медленнее 10-го места списка\n");
        return;
    }
    if (counter == 10)
    {
    for(int j = counter-1; j > num; --j)
    {
        for(int i = 0; i < 25; ++i)
        {
            name[j][i] = name[j-1][i];
        }   
        time[j] = time[j-1];
    }
    for(int i = 0; i < 25; ++i)
    {
        name[num][i] = _name[i];
    }     
    time[num] = _time;
    }

    if(counter != 10)
    {
        if (num != -1)
        {
            for(int j = counter; j > num; --j)
            {
                for(int i = 0; i < 25; ++i)
                {
                    name[j][i] = name[j-1][i];
                }   
                time[j] = time[j-1];
            }
            for(int i = 0; i < 25; ++i)
            {
                name[num][i] = _name[i];
            }     
            time[num] = _time;
        }
        else
        {
            num = counter;
            for(int i = 0; i < 25; ++i)
            {
                name[num][i] = _name[i];
            }     
            time[num] = _time;
        }
        counter++;
    }

    out = fopen("table.txt", "w");
    if (!out)
    {
        printf("Ошибка при открытии файла\n"); 
        return;
    }
    for (int i = 0 ; i < counter; ++i)
    {
        fprintf(out, "%d %s %2.1f\n", i+1, name[i], time[i]);
    }
    fclose(out);
}

void getRecord()
{
    FILE* in;
    int counter = 0;
    char name[10][25];
    float time[10];
    in = fopen("table.txt", "r");
    if (!in)
    {
        printf("Ошибка при открытии файла\n"); 
        getchar();
        return;
    }
    for (int i = 0 ; i < 10; ++i)
    {
        if (!fscanf(in,"%d %s %f", &counter, &*(name[i]), &time[i]))
        break;
    }
    fclose(in);
    for(int i = 0; i < counter; i++)
        printf("%d %s %.1f\n", i+1, name[i], time[i]);
    printf("\n");
}

int main()
{
    system("clear");
    char name[25];
    char movements[40];
    for(int i = 0; i < 40; i++)
        movements[i] = '\0';
    bool isIntoBlock = false, endd = false;
    int pitch[20][20];
    int n = 0, m = 0, pos = 1, numberOfBombs = 0, endofgame, value = 0;
    int xSmile = 0, ySmile = 0;
    char menuCatcher, checkArrow;
    //int endd[4] = {0, 0, 0, 0};
    while(true)
    {
    if (pos == 1) printf("1. Ввод высоты поля <--\n"); else printf("1. Ввод высоты поля\n");
    if (pos == 2) printf("2. Ввод длины поля <--\n"); else printf("2. Ввод длины поля\n");
    if (pos == 3) printf("3. Выбор числа бомб <--\n"); else printf("3. Выбор числа бомб \n");
    if (pos == 4) printf("4. Запуск игры  <--\n"); else printf("4. Запуск игры \n");
    if (pos == 5) printf("5. Вывод таблицы рекордов <--\n"); else printf("5. Вывод таблицы рекордов\n");
    if (pos == 6) printf("6. Выход из программы <--\n"); else printf("6. Выход из программы\n");
    printf ("\nВысота поля = %d Длина поля = %d Число бомб = %d\n\n", n, m, numberOfBombs);
    if(pos != 6) printf ("Выберите пункт меню нажатием стрелочек вверх-вниз или нажатием соответствующей цифры\n");
    //if(pos == 6) break;
    disable_waiting_for_enter();
    menuCatcher = getchar();
    restore_terminal_settings();
    switch(menuCatcher)
    {
        case 27: 
        system("clear");
        if (getchar() == '[')
        checkArrow = getchar();
        switch(checkArrow)
        {
            case 'A':
            if (pos != 1) pos--;
            else printf("Невозможно сместить стрелочку в указанном направлении\n\n");
            break;
            case 'B':
            if (pos != 5) pos++;
            else printf("Невозможно сместить стрелочку в указанном направлении\n\n");
            break;
        }
        break;
        case 49: pos = 1; break;
        case 50: pos = 2; break;
        case 51: pos = 3; break;
        case 52: pos = 4; break;
        case 53: pos = 5; break;
        case 54: pos = 6; break;
        case 10: isIntoBlock = true; break;
        default: printf("Input failure\n\n"); break;
    }

    system("clear");

    if(isIntoBlock)
    {
        switch(pos)
        {
            case 1: 
            printf("Введите высоту\n");
            n = symbol_scanning(20, 5);
            system("clear");
            break;

            case 2: 
            printf("Введите длину\n");
            m = symbol_scanning(20, 5);
            system("clear");
            break;

            case 3: 
            if(!n || !m) printf("Сначала введите высоту и длину\n\n");
            else
            {
            printf("Введите число бомб, максимальное число бомб - %d, минимальное - 4\n", n*m/3);
            numberOfBombs = symbol_scanning(n*m/3, 4);
            system("clear");
            }
            break;

            case 4: 
            if(!n || !m || !numberOfBombs) printf("Сначала введите высоту, длину и число бомб\n\n");
            else 
            {
                creating(pitch, n, m, numberOfBombs);
                ySmile = n / 2; xSmile = m / 2;
                struct timespec begin;
                timespec_get(&begin, TIME_UTC);
                endofgame = movingSmile(pitch, n, m, ySmile, xSmile, numberOfBombs, movements, value, false);
                struct timespec end;
                timespec_get(&end, TIME_UTC);
                float duration = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

                if(endofgame == WIN_END)
                {
                    printf("Введите свой псевдоним\n");
                    char c;
                    for(int i = 0; i < 25; ++i) 
                    {
                        scanf("%c", &c);
                        if(c == '\n') 
                        {
                            name[i] = '\0'; 
                            break;
                        }
                        name[i] = c;
                    }
                    printf("Running time is %2.1f\n", duration);
                    newRecord(duration, name);
                    getchar();
                }

                system("clear");
                if(endofgame == WIN_END) printf("Вы победили!\n\n");
                if(endofgame == LOSE_END) printf("Вы проиграли\n\n");
                if(endofgame == QUIT_END) printf("Вы вышли из игры\n\n");
                char reverse = '\0';
                while(reverse != 'y' && reverse != 'n')
                {
                    printf("Хотите ли вы посмотреть запись игры [y/n]? ");
                    disable_waiting_for_enter();
                    reverse = getchar();
                    restore_terminal_settings();
                    if (reverse == 'y') 
                    {
                        stack.get_all(movements);
                        movingSmile(pitch, n, m, ySmile, xSmile, numberOfBombs, movements, value, true);
                        printf("Просмотр перемотки завершен нажмите Q для выхода в меню\n");
                        char endOfReverse = '\0';
                        while(endOfReverse != 'Q')
                        {
                            disable_waiting_for_enter();
                            endOfReverse = getchar();
                            restore_terminal_settings();
                        }
                        system("clear");
                        stack.remove_stack();
                    }
                    if (reverse == 'n') 
                    {
                        stack.remove_stack();
                        break;
                    }
                    printf("\n");
                }
            }
            break;

            case 5: getRecord(); break;
            case 6: endd = true; break;
        }
    }
    isIntoBlock = false;
    if(endd == true) break;
    }
    stack.remove_stack();
    return 0;
}