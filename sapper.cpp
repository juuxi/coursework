#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <time.h>
#include <iostream>

const char BOMB[] = "\U0001f4a3";
const char FLOOR[] = "\U00002b1c";
const char CLOSED_FLOOR[] = "\U0001f533";
const char SMILE[] = "\U0001f610";
const char FLAG[] = "\U0001f6a9";

#define BOMB_NUM 999

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

    void print_first() 
    {
        if(is_empty()) return;
        Node* p = first;
        std::cout << p->val.x;
        std::cout << p->val.y;
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
        printf("Список удален");
    }
};

struct preMovement{
    int x;
    int y;
    int value;
};

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

int symbol_scanning(int overfmax, int overfmin) //эта функция занимается посимвольной читкой введенного числа
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
                    else if(counter != 0) 
                    {
                        if(num < overfmin) 
                        {
                        printf("Число меньше минимального (%d). Повторите попытку\n", overfmin);
                        num = 0;
                        clean_stdin();
                        counter = 0;
                        }
                        else
                        {
                        count = true; 
                        break;
                        }
                    } //если это перенос строки не первым же символом - все круто, прерываем цикл
                    else {printf("Вы ввели неверный символ, повторите попытку\n");} //если первым - выдаем ошибку
                }
                else{
                if (num*10+c-'0' > overfmax) {overf = true; num = 0; clean_stdin();} 
                else  
                num = num*10+c-'0';
                }

                counter++;

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
        if(pitch[i+1][j] != BOMB_NUM) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != BOMB_NUM) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM) pitch[i][j+1]++;
        if(pitch[i-1][j+1] != BOMB_NUM) pitch[i-1][j+1]++;
        if(pitch[i-1][j] != BOMB_NUM) pitch[i-1][j]++;
        if(pitch[i-1][j-1] != BOMB_NUM) pitch[i-1][j-1]++;
        if(pitch[i][j-1] != BOMB_NUM) pitch[i][j-1]++;
        if(pitch[i+1][j-1] != BOMB_NUM) pitch[i+1][j-1]++;
    }

    if(i == 0 && j == 0)
    {
        if(pitch[i+1][j] != BOMB_NUM) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != BOMB_NUM) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM) pitch[i][j+1]++;
    }
    if(i == n-1 && j == 0)
    {
        if(pitch[i-1][j] != BOMB_NUM) pitch[i-1][j]++;
        if(pitch[i-1][j+1] != BOMB_NUM) pitch[i-1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM) pitch[i][j+1]++;
    }
    if(i == n-1 && j == m-1)
    {
        if(pitch[i][j-1] != BOMB_NUM) pitch[i][j-1]++;
        if(pitch[i-1][j-1] != BOMB_NUM) pitch[i-1][j-1]++;
        if(pitch[i-1][j] != BOMB_NUM) pitch[i-1][j]++;
    }
    if(i == 0 && j == m-1)
    {
        if(pitch[i+1][j] != BOMB_NUM) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != BOMB_NUM) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM) pitch[i][j+1]++;
    }

    if(i == 0 && j != 0 && j != m-1)
    {
        if(pitch[i][j-1] != BOMB_NUM) pitch[i][j-1]++;
        if(pitch[i+1][j-1] != BOMB_NUM) pitch[i+1][j-1]++;
        if(pitch[i+1][j] != BOMB_NUM) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != BOMB_NUM) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM) pitch[i][j+1]++;
    }
    if(i == n-1 && j != 0 && j != m-1)
    {
        if(pitch[i][j+1] != BOMB_NUM) pitch[i][j+1]++;
        if(pitch[i-1][j+1] != BOMB_NUM) pitch[i-1][j+1]++;
        if(pitch[i-1][j] != BOMB_NUM) pitch[i-1][j]++;
        if(pitch[i-1][j-1] != BOMB_NUM) pitch[i-1][j-1]++;
        if(pitch[i][j-1] != BOMB_NUM) pitch[i][j-1]++;
    }
    if(j == 0 && i != 0 && i != n-1)
    {
        if(pitch[i+1][j] != BOMB_NUM) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != BOMB_NUM) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != BOMB_NUM) pitch[i][j+1]++;
        if(pitch[i-1][j+1] != BOMB_NUM) pitch[i-1][j+1]++;
        if(pitch[i-1][j] != BOMB_NUM) pitch[i-1][j]++;
    }
    if(j == m-1 && i != 0 && i != n-1)
    {
        if(pitch[i-1][j] != BOMB_NUM) pitch[i-1][j]++;
        if(pitch[i-1][j-1] != BOMB_NUM) pitch[i-1][j-1]++;
        if(pitch[i][j-1] != BOMB_NUM) pitch[i][j-1]++;
        if(pitch[i+1][j-1] != BOMB_NUM) pitch[i+1][j-1]++;
        if(pitch[i+1][j] != BOMB_NUM) pitch[i+1][j]++;
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
        if(pitch[o][p] != BOMB_NUM)
        {
            pitch[o][p] = BOMB_NUM;
            mark_around(pitch, n, m, o, p);
            bombCounter++;
        }
    }

    /* for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            if(pitch[i][j] > 0)
            printf("%2d", pitch[i][j]);

            if(pitch[i][j] == -1) 
            printf("%3s", BOMB);

            if(pitch[i][j] == 0)
            printf("%3s", FLOOR);
        }
        printf("\n");
    } */
    //if(err == true) printf("Введен неверный символ\n");
}

void printPitch(int pitch[20][20], int n, int m)
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            if(pitch[i][j] == -1) 
            printf("%3s", BOMB);

            if(pitch[i][j] == 0)
            printf("%3s", FLOOR);

            if(pitch[i][j] == 100)
            printf("%3s", FLAG);

            if(pitch[i][j] == 300)
            printf("%3s", SMILE);

            if(pitch[i][j] > 998)
            printf("%3s", CLOSED_FLOOR);

            if(pitch[i][j] > 0 && pitch[i][j] < 10)
            printf("%2d", pitch[i][j]);
        }
        printf("\n");
    }
}

void digging(int pitch[20][20], int n, int m, int ySmile, int xSmile, int &value);

void movingSmile(int pitch[20][20], int n, int m, int ySmile, int xSmile)
{
    preMovement preM;
    preM.x = xSmile;
    preM.y = ySmile;
    int value = pitch[ySmile][xSmile];
    pitch[ySmile][xSmile] = 300;
    bool endd = false;
    printPitch(pitch, n, m);
    while(true)
    {
    printf("Нажимайте клавиши w, a, s, d для управления смайликом и p чтобы открыть поле под собой\n");
    disable_waiting_for_enter();
    char movement = getchar();
    restore_terminal_settings();
    switch(movement)
    {
        case 'w': 
        pitch[ySmile][xSmile] = value;
        if(ySmile != 0) ySmile -= 1;
        value = pitch[ySmile][xSmile];
        pitch[ySmile][xSmile] = 300;
        break;

        case 'a':
        pitch[ySmile][xSmile] = value;
        if(xSmile != 0) xSmile -= 1;
        value = pitch[ySmile][xSmile];
        pitch[ySmile][xSmile] = 300;
        break;

        case 's':
        pitch[ySmile][xSmile] = value;
        if(ySmile != n-1) ySmile += 1;
        value = pitch[ySmile][xSmile];
        pitch[ySmile][xSmile] = 300;
        break;

        case 'd':
        pitch[ySmile][xSmile] = value;
        if(xSmile != m-1) xSmile += 1;
        value = pitch[ySmile][xSmile];
        pitch[ySmile][xSmile] = 300;
        break;

        case 'p':
        digging(pitch, n, m, ySmile, xSmile, value);
        if(value == -1) endd = true;
        break;

        case 10: endd = true; break;
    }
    system("clear");
    printPitch(pitch, n, m);
    if(endd == true) break;
    }
}

void digging(int pitch[20][20], int n, int m, int ySmile, int xSmile, int &value)
{
    switch(value)
    {
        case 999:
        printf("Игра окончена\n");
        pitch[ySmile][xSmile] = value;
        for(int i = 0; i < n; ++i)
        {
            for(int j = 0; j < m; j++)
            if(pitch[i][j] > 998) pitch[i][j] -= 1000;
        }
        printPitch(pitch, n, m);
        break;

        case 1000: 
        break;
    }
    if(value > 1000 && value < 1010)
        value -=1000;
}

int main()
{
    bool isIntoBlock = false;
    int pitch[20][20];
    int n = 0, m = 0, pos = 1, numberOfBombs = 0;
    int xSmile = 0, ySmile = 0;
    char menuCatcher, checkArrow;
    int endd[4] = {0, 0, 0, 0};
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
    if(pos == 6) break;
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
            n = symbol_scanning(20);
            system("clear");
            break;

            case 2: 
            printf("Введите длину\n");
            m = symbol_scanning(20);
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
            movingSmile(pitch, n, m, ySmile, xSmile);
            /* getchar();
            system("clear"); */
            }
            break;

            case 5: printf("Таблица рекордов в разработке\n"); break;
        }
    }
    }
    return 0;
}