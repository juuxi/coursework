#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <time.h>

const char BOMB[] = "\U0001f4a3";
const char FLOOR[] = "\U00002b1c";

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
        if(pitch[i+1][j] != -1) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != -1) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != -1) pitch[i][j+1]++;
        if(pitch[i-1][j+1] != -1) pitch[i-1][j+1]++;
        if(pitch[i-1][j] != -1) pitch[i-1][j]++;
        if(pitch[i-1][j-1] != -1) pitch[i-1][j-1]++;
        if(pitch[i][j-1] != -1) pitch[i][j-1]++;
        if(pitch[i+1][j-1] != -1) pitch[i+1][j-1]++;
    }

    if(i == 0 && j == 0)
    {
        if(pitch[i+1][j] != -1) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != -1) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != -1) pitch[i][j+1]++;
    }
    if(i == n-1 && j == 0)
    {
        if(pitch[i-1][j] != -1) pitch[i-1][j]++;
        if(pitch[i-1][j+1] != -1) pitch[i-1][j+1]++;
        if(pitch[i][j+1] != -1) pitch[i][j+1]++;
    }
    if(i == n-1 && j == m-1)
    {
        if(pitch[i][j-1] != -1) pitch[i][j-1]++;
        if(pitch[i-1][j-1] != -1) pitch[i-1][j-1]++;
        if(pitch[i-1][j] != -1) pitch[i-1][j]++;
    }
    if(i == 0 && j == m-1)
    {
        if(pitch[i+1][j] != -1) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != -1) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != -1) pitch[i][j+1]++;
    }

    if(i == 0 && j != 0 && j != m-1)
    {
        if(pitch[i][j-1] != -1) pitch[i][j-1]++;
        if(pitch[i+1][j-1] != -1) pitch[i+1][j-1]++;
        if(pitch[i+1][j] != -1) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != -1) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != -1) pitch[i][j+1]++;
    }
    if(i == n-1 && j != 0 && j != m-1)
    {
        if(pitch[i][j+1] != -1) pitch[i][j+1]++;
        if(pitch[i-1][j+1] != -1) pitch[i-1][j+1]++;
        if(pitch[i-1][j] != -1) pitch[i-1][j]++;
        if(pitch[i-1][j-1] != -1) pitch[i-1][j-1]++;
        if(pitch[i][j-1] != -1) pitch[i][j-1]++;
    }
    if(j == 0 && i != 0 && i != n-1)
    {
        if(pitch[i+1][j] != -1) pitch[i+1][j]++;
        if(pitch[i+1][j+1] != -1) pitch[i+1][j+1]++;
        if(pitch[i][j+1] != -1) pitch[i][j+1]++;
        if(pitch[i-1][j+1] != -1) pitch[i-1][j+1]++;
        if(pitch[i-1][j] != -1) pitch[i-1][j]++;
    }
    if(j == m-1 && i != 0 && i != n-1)
    {
        if(pitch[i-1][j] != -1) pitch[i-1][j]++;
        if(pitch[i-1][j-1] != -1) pitch[i-1][j-1]++;
        if(pitch[i][j-1] != -1) pitch[i][j-1]++;
        if(pitch[i+1][j-1] != -1) pitch[i+1][j-1]++;
        if(pitch[i+1][j] != -1) pitch[i+1][j]++;
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
        if(pitch[o][p] != -1)
        {
            pitch[o][p] = -1;
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
            if(pitch[i][j] > 1000)
            printf("%2d", pitch[i][j]-1000);

            if(pitch[i][j] == -1) 
            printf("%3s", BOMB);

            if(pitch[i][j] == 1000)
            printf("%3s", FLOOR);
        }
        printf("\n");
    }
}

int main()
{
    bool isIntoBlock = false;
    int pitch[20][20];
    int n = 0, m = 0, pos = 1, numberOfBombs = 0;
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
            printPitch(pitch, n, m);
            getchar();
            system("clear");
            }
            break;

            case 5: printf("Таблица рекордов в разработке\n"); break;
        }
    }
    }

    /* for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            switch(pitch[i][j])
            {
            case 0:  printf("%s", "\U0001f9f1"); break;   
            case 10: printf("%s", "\U0001f300"); break;     
            case 20: printf("%s", "\U0001f6a7"); break;
            case 35: printf("%s", "\U0001f600"); break;
            case 70: printf("%s", "\U0001f9f1"); break;
            default: printf("%s", "\U0001f300");
            } 
        }
        printf("\n");
    } */
    return 0;
}