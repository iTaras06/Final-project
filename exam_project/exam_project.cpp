#include <iostream>
#include <cstring>

using namespace std;

const int SIZE = 10; //размер игрового поля 10х10

//символы, обозначающие состояние клеток
const char EMPTY = '.'; //для пустых клеток без кораблей
const char SHIP = '#'; //обозначает корабль
const char MISS = 'o'; //отображает промах (мимо)
const char HIT = 'x';  //означает попадание по кораблю

//глобальная переменная для режима стрельбы компьютера
bool useSmartAI = false; //false - рандомная стрельба, true - интеллектуальный режим

struct Player //структура игрока
{
    char field[SIZE][SIZE]; //поле 10х10 - хранит корабли игрока
    char enemyView[SIZE][SIZE]; //поле обзора противника
    int shipsAlive; //количество целых кораблей игрока (изначально 10)
    bool isHuman; //тип игрока (true - человек, false - компьютер)
    const char* name; //указатель на строку с именем игрока (Human, Computer_1,  Computer_2)
};





// Очищает игровое поле, заполняя все клетки точками (символом '.'),
// который обозначает пустую клетку

void initField(char field[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            field[i][j] = EMPTY; //заполняет массив поля точками '.'
        }
    }
}





// Выводит игровое поле в виде таблицы: верхний ряд - буквы столбцов (A-J)
// левый столбец - номера строк (1-10), в клетках - состояние (пусто, корабль, попал, мимо)

void printField(const char field[SIZE][SIZE]) //входной параметр: двумерный массив символов
{                                             //(поле, которое надо напечатать
    cout << "   "; //начальный отступ перед А
    for (char c = 'A'; c < 'A' + SIZE; ++c) //цикл печатает заголовки столбцов
    {
        cout << c << ' '; //печатает A-J
    }
    cout << endl;

    for (int i = 0; i < SIZE; ++i) //цикл печатает номера строк
    {
        if (i + 1 < 10)
        {
            cout << " "; //выравнивает числа по ширине
        }
        cout << i + 1 << " "; //выводит номер строки


        for (int j = 0; j < SIZE; ++j)
        {
            cout << field[i][j] << ' '; //печатает символ из массива field[i][j], 
            //соответствующий состоянию клетки ('.', '#', 'o', 'x')
        }
        cout << endl;
    }
}






// Проверяет области вокруг корабля на пересечение с другими кораблями

bool isValidPlacement(char field[SIZE][SIZE], int row, int col, int len, bool horizontal)
{
//field[SIZE][SIZE] - поле игрока (двумерный массив 10х10);  row,col - начальная координата
//размещения корабля (первая палуба);  len - длина корабля (1,2,3,4);  
//horizontal = true, если корабль размещается горизонтально,
//horizontal = false, если корабль размещается вертикально;
    for (int i = -1; i <= len; ++i) //проверяем не только клетки, занятые кораблем, но и область
                                    //вокруг него (слева, справа, сверху, снизу, по диагонали)
    {
        for (int j = -1; j <= 1; ++j) // проверяем клетки сбоку от каждой позиции корабля
        {   //вычисление координат: если корабль горизонтальный, то (r = row + j) - двигаемся
            //по вертикали (вверх/вниз), (c = col + i) - вдоль палубы корабля (горизонтально);
            //если корабль вертикальный, то (r = row + i) - вдоль палубы корабля (вертикально),
            //(c = col + j) - двигаемся по горизонтали (влево/вправо)
            int r = row + (horizontal ? j : i);
            int c = col + (horizontal ? i : j);

            if (r >= 0 && r < SIZE && c >= 0 && c < SIZE) //проверка на выход за границу поля
            {
                if (field[r][c] == SHIP) //если рядом другой корабль (соседняя клетка
                 //содержит '#', то размещать корабль нельзя
                {
                    return false;   //ф-ция возвращает false, если размещать корабль
                                    //в этой клетке нельзя
                }
            }
        }
    }
    return true; //ф-ция возвращает true, если размещение корректно (корабль не пересекается
                 //и не соприкасается с другими кораблями)
}





// Размещает корабль на игровом поле рандомно, отмечая символом '#' 

void placeShip(char field[SIZE][SIZE], int len)
{   //field[SIZE][SIZE] - двумерный массив 10х10 - игровое поле, на котором надо
    //разместить корабль;  len - длина корабля (1,2,3,4)

    int row, col; //координаты начала корабля (первой палубы)
    bool horizontal; //horizontal = true, если корабль будет размещен горизонтально;
    //horizontal = fals, eсли корабль будет размещен вертикально
    do
    {
        horizontal = rand() % 2; //рандомное размещение корабля:
        if (horizontal)          //горизонтально или вертикально
        {
            row = rand() % SIZE; //рандомные координаты корабля
            col = rand() % (SIZE - len + 1);
        }
        else
        {
            row = rand() % (SIZE - len + 1);
            col = rand() % SIZE;
        }
    } while (!isValidPlacement(field, row, col, len, horizontal)); //проверка допустимости:
    //если выбранное место занято или слишком близко к другому кораблю, то повторяем генерацию

    for (int i = 0; i < len; ++i) //размещает корабль на поле по выбранным координатам 
    {   //проходим по всем палубам корабля len раз
        if (horizontal) //для горизонтального размещения
        {
            field[row][col + i] = SHIP;
        }
        else // для вертикального размещения
        {
            field[row + i][col] = SHIP;
        }
    }
}





// Автоматически размещает все корабли игрока на поле

void autoPlaceFleet(Player& player) //ф-ция принимает по ссылке player - структуру игрока

{
    initField(player.field); //вызов ф-ции  initField, чтобы заполнить все клетки поля символом '.'
                            //очищает игровое поле перед размещением
    int ships[4] = { 4, 3, 2, 1 }; //массив с длинами всех типов кораблей (размеры кораблей)
    int counts[4] = { 1, 2, 3, 4 };// количество каждого типа корабля
    for (int t = 0; t < 4; ++t) //цикл по всем типам кораблей (всего 4 типа)
    {
        for (int i = 0; i < counts[t]; ++i) //вложенный цикл размещает counts[t] штук кораблей
                                            // длины ships[t]
        {
            placeShip(player.field, ships[t]); //вызов функции, которая автоматически размещает 
                    //один корабль указанной длины на поле player.field, соблюдая правила
        }                  
    }
    player.shipsAlive = 10; //после размещения всех кораблей у игрока их 10.
    //В процессе игры при потоплении кораблей это число будет уменьшаться.
}





// Корабли размещает человек вручную

void manualPlaceFleet(Player& player) // ф-ция принимает по ссылке структуру player
//и размещает корабли вручную на его поле
{
    initField(player.field); //очищает поле игрока, заполняя его символами '.'
    int ships[4] = { 4, 3, 2, 1 }; //длины кораблей
    int counts[4] = { 1, 2, 3, 4 }; //сколько кораблей каждого размера нужно разместить

    for (int t = 0; t < 4; ++t) //цикл по типам кораблей (по размеру), от большого к маленькому
    {
        for (int i = 0; i < counts[t]; ++i) //внутренний цикл размещает нужное количество
            //кораблей текущего типа ships[t]
        {
            bool placed = false;
            while (!placed) //пока корабль не удстся корректно разместить - повторять ввод
            {
                printField(player.field); //показывает текущее состояние поля перед каждым размещением
                
                //инструкция пользователю: ввести координаты начальной клетки и ориентацию корабля
                cout << "Place a ship of size " << ships[t]
                    << " - enter coordinate and location (H-horizontally, V-vertically) "
                    << "\nof first deck (example, B3 H): ";
                char input[10];
                char dir;
                cin >> input >> dir; // input - координата вида В3;  dir - направление Н или 

                int row = atoi(input + 1); //преобразует строку, содержащую число, в int
                int col = toupper(input[0]) - 'A'; //преобразует букву в верхний регистр
                row--; //уменьшаем, т.к. строки считаются с 0, а игрок вводит с 1
                bool horizontal = (toupper(dir) == 'H'); //определяем направление (горизонтальное 
                                                         //или вертикальное размещение

                if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) //проверка: находится ли
                                                        //начальная точка внутри игрового поля
                {
                    cout << "Invalid coordinates. Try again." << endl; //если за границами - 
                                                       //сообщение об ошибке и повтор попытки
                    continue;
                }

                if ((horizontal && col + ships[t] > SIZE) || (!horizontal && row + ships[t] > SIZE))
                {  //проверка, помещается ли корабль в поле в выбранном направлении
                    cout << "Ship does not fit on the board. Try again." << endl;
                    continue; // если не помещается - то ошибка и новый ввод
                }

                if (!isValidPlacement(player.field, row, col, ships[t], horizontal))
                {   // проверка, можно ли разместить корабль в этом месте, не нврушая правил
                    cout << "You cannot place a ship on another ship" //если правила нарушаются,
                                                           //выводим сообщение и просим повторить
                        << "\n or too close to one.Try again." << endl;
                    continue;
                }

                // размещение корабля
                for (int k = 0; k < ships[t]; ++k) //если всё в порядке - размещаем корабль
                                                    //символами '#' на нужных клетках поля
                {
                    if (horizontal) player.field[row][col + k] = SHIP;
                    else player.field[row + k][col] = SHIP; //размещение корабля
                }

                placed = true; //помечаем, что корабль успешно размещен - переходим
                                //к следующему кораблю
            }
        }
    }

    player.shipsAlive = 10; //устанавливаем значение - 10 кораблей на поле
}



// Интеллектуальный режим стрельбы - ищет клетки вокруг 'x' и стреляет туда
// Если таких нет, то действунт рандомно

void smartShot(Player& shooter, int& row, int& col) 
{
    for (int i = 0; i < SIZE; ++i)     //поиск клеток рядом с попаданиями 'x'
    {
        for (int j = 0; j < SIZE; ++j) 
        {
            if (shooter.enemyView[i][j] == HIT) //HIT = 'x' 
            {//если найдена клетка, где уже было попадание, пытаемся стрелять рядом
                int dx[] = { -1, 1, 0, 0 }; //массивы для проверки четырех направлений:
                int dy[] = { 0, 0, -1, 1 }; //вверх, вниз, влево, вправо  
                for (int d = 0; d < 4; ++d) 
                {//вычисляем координаты соседних клеток по каждому направлению
                    int ni = i + dx[d], nj = j + dy[d];
                    if (ni >= 0 && ni < SIZE && nj >= 0 && nj < SIZE && //проверяем, что соседняя клетка внутри
                        shooter.enemyView[ni][nj] == EMPTY)  //поля и по ней еще не стреляли - EMPTY = '.'
                    {
                        row = ni; col = nj; //запмсываем координаты подходящей цели в row, col 
                        return;
                    }
                }
            }
        }
    }
    do 
    {//если вокруг 'x' нет пустых клеток, то делаем рандомный выстрел
        row = rand() % SIZE;
        col = rand() % SIZE;
    } while (shooter.enemyView[row][col] == HIT || shooter.enemyView[row][col] == MISS);
}







// Проверяет, потоплен ли корабль после попадания

bool isShipSunk(char field[SIZE][SIZE], int row, int col)
{
    for (int d = -1; d <= 1; d += 2)//проверка по двум направлениям: -1 влево и ввех,
    {                                                            //   1 вправо и вниз
        for (int i = 1; i < SIZE; ++i) //проверка по вертикали
        {
            int r = row + d * i, c = col;
            if (r < 0 || r >= SIZE) break; //если выходим за границы поля - остановить цикл
            if (field[r][c] == SHIP) return false;//если рядом непораженная часть корабля, значит он не потоплен
            if (field[r][c] != HIT) break;        //если нет попадания, остановить проверку
        }
        for (int i = 1; i < SIZE; ++i) //проверка по горизонтали
        {//в каждой строке проверяем столбцы слева и справа от col
            int r = row, c = col + d * i;
            if (c < 0 || c >= SIZE) break; //если выходим за границы - остановить цмкл
            if (field[r][c] == SHIP) return false;//если находим часть корабля, значит но не потоплен
            if (field[r][c] != HIT) break; //остановить проверку, если не попал
        }
    }
    return true;//если нет уцелевших частей(палуб) корабля, значит он потоплен
}





// Меняет символ 'x' на '#' когда корабль потоплен

void markSunkShip(Player& shooter, char field[SIZE][SIZE], int row, int col)
{
    for (int d = -1; d <= 1; d += 2) //проход по вертикали вверх(-1) и вниз(1) 
    {
        for (int i = 0; i < SIZE; ++i) // i определяет смещение от подбитой 
        {                              // клетки(row,col) вверх и вниз
            int r = row + d * i; //вычисляется строка на смещении i вверх или вниз
            
            if (r < 0 || r >= SIZE) //проверка выхода за границы поля
            {   
                break; //если вышли - прекращаем цикл
            }
            if (field[r][col] == HIT) //если в клетке противника было попадание - 'x'
            {
                   shooter.enemyView[r][col] = SHIP; //меняем в enemyView стрелявшего с 'x' на '#' 
            }
            else break; //в противном случае в клетке не 'x', 
        }               //значит это конец корабля и останавливаем цикл
    }
    for (int d = -1; d <= 1; d += 2) //проход по горизонтали влево(-1) и вправо(1) 
    {
        for (int i = 0; i < SIZE; ++i) 
        {
            int c = col + d * i;//вычисляем координату столбца с учетом смещения i
            if (c < 0 || c >= SIZE) //проверка выхода за границы по горизонтали
            {
                break;
            }
            if (field[row][c] == HIT) //если в клетке противника было попадание - 'x'
            {
                shooter.enemyView[row][c] = SHIP;//меняем в enemyView стрелявшего с 'x' на '#' 
            }
            else break; //в противном случае в клетке не 'x', 
        }               //значит это конец корабля и останавливаем цикл
    }
    shooter.enemyView[row][col] = SHIP; //обновляем центральную клетку попадания, а именно
}                                       //устанавливаем её тоже как '#'






// Анализирует выстрел игрока (попал - 'x', мимо - 'o')

bool makeShot(Player& shooter, Player& target, int row, int col)
{
    if (target.field[row][col] == SHIP) //попал ли в корабль (если в клетке '#', значит в неё попали) 
    {
        target.field[row][col] = HIT; //отмечает попадание в цель - 'x'
        shooter.enemyView[row][col] = HIT; //отмечает попадание на поле стрелявшего - 'x'
        
        //проверка, потоплен ли корабл
        if (isShipSunk(target.field, row, col)) //если все части корабля поражены, то корабль потоплен
        {
            target.shipsAlive--; //уменьшаем счетчик целых кораблей
            
            //вызов ф-ции markSunkShip, чтобы пометить весь корабль символами '#'
            markSunkShip(shooter, target.field, row, col); //меняем 'x' на '#', если корабль потоплен
            cout << "\n Sunk!" << endl; //выводим сообщение, если корабль потоплен
        }
        else
        {
            cout << "\n Hit!" << endl; //выводим, если только попал в корабль, но не потопил его
        }
        return true; //возврат результата, если было попадпние
    }
    else if (target.field[row][col] == EMPTY) //если в клетке точка, значит мимо
    {   //обновляем поля
        target.field[row][col] = MISS; //помечаем клетку 'o' у противника, когда мимо
        shooter.enemyView[row][col] = MISS; //помечаем клетку 'o' у стрелка, когда мимо
        cout << "\n Miss!" << endl; //выводим сообщение, когда мимо
        
        return false; //возврат результата, если мимо
    }
    return false; //ход передаётся другому игроку
}




// Считывает координаты клетки (выстрела) от человека или команды для паузы, выхода, рестарта
// Возвращает true если ввод корректный
bool getCoords(int& row, int& col)
{
    char input[10]; // объявляется массив символов для ввода
    cout << "Enter coordinates (example, B5), or p - for pause, " //пояснения, как вводить 
        << "\ne - for exit, r - for restart): ";                  //координаты или команды
    cin >> input;//запрашивает координаты клетки и записывает в переменную input

    // Команды для паузы, выхода и рестарта
    if (strcmp(input, "p") == 0 || strcmp(input, "P") == 0)
    {
        row = col = -888; //спец-код для паузы
        return true;
    }
    if (strcmp(input, "e") == 0 || strcmp(input, "E") == 0)
    {
        exit(0); // полностью завершает игру
    }
    if (strcmp(input, "r") == 0 || strcmp(input, "R") == 0)
    {

        row = col = -999; //спец-код для рестарта
        return true;
    }

    // Проверка ввода координат
    if (strlen(input) < 2) //если введено меньше 2 символов - это ошибка
    {
        row = col = -1; //сигнал об ошибке
        return true;
    }

    col = toupper(input[0]) - 'A'; //преобразует букву в верхний регистр //получаем букву столбца
    row = atoi(input + 1) - 1;//преобразует строку, содержащую число, в int //получаем номер строки

    if (col < 0 || col >= SIZE || row < 0 || row >= SIZE) //проверка координаты на правильный диапазон
    {
        row = col = -1; //ошибка - если координаты выходят за пределы поля
    }

    return true;
}





// Управляет ходом всей игры: стрельба по очереди; вывод результатов выстрелов;
// завершение игры, если у игрока не осталось кораблей; вывод имени победителя 

void playGame(Player& p1, Player& p2) //принимает двух игроков по ссылке
{
    bool turn = rand() % 2; // выбрать рандомно, кто ходит первым
                            // (turn == 1 -> p1, turn == 0 -> p2
    
    while (p1.shipsAlive > 0 && p2.shipsAlive > 0) //продолжить игру, пока есть хотя бы
    {                                              //один живой корабль у обоих игроков
        Player& current = turn ? p1 : p2; //определить хто ходит 
        Player& target = turn ? p2 : p1; //определить цель
       
        cout << "Turn: " << current.name << endl; //показывает, чей сейчас ход
        printField(current.enemyView); //показать клетку, куда стрелял игрок

        bool repeat; //нужен для повторного хода, если игрок попал
        do
        {
            int row, col;
            if (current.isHuman)           //если играет человек, 
            {
                if (!getCoords(row, col)) //то запросить координаты
                {
                    repeat = true;
                    continue;
                }
                if (row == -888) //спец-код при паузе
                {
                    cout << "Game paused. Press Enter to continue...\n";
                    cin.ignore();//очистка буфера перед вводом строк для удаления лишнего \n
                    cin.get();
                    repeat = true;
                    continue;
                }
                if (row == -999 || col == -999) //  обработка рестарта
                {
                    return;
                }

                if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) //неверные координаты
                { //если координаты выходят за границы поля, просим ввести заново
                    cout << "Invalid coordinates. Try again.\n";
                    repeat = true;
                    continue;
                }
            }

            //если играет компьютер с компьютером, то сгенерировать координаты
            else
            {
                if (useSmartAI) //если выбран интеллектувльный режим
                {
                    smartShot(current, row, col); // вызов ф-ции интеллектуального выстрела
                }                                 // стратегия стрельбы рядом с 'x'
                else //в противном случае стреляе рандомно
                {
                    row = rand() % SIZE;
                    col = rand() % SIZE;

                    //избегает повторных выстрелов по 'x' и 'o'
                    while (current.enemyView[row][col] == HIT || current.enemyView[row][col] == MISS)
                    {
                        row = rand() % SIZE;
                        col = rand() % SIZE;
                    }
                }
                //показываем координаты выстрела компьютера
                cout << current.name << " fires at " << char('A' + col) << row + 1 << endl;
            }

            //вызов ф-ции совершения выстрела
            repeat = makeShot(current, target, row, col); //сделать выстрел
        } while (repeat && p1.shipsAlive > 0 && p2.shipsAlive > 0); //пока repeat == true,
          //то попал и ход продолжается, если мимо repeat == false и ход передается
        turn = !turn; //поменять игрока
    }

    cout << "Game over! "; //конец игры и объявление победителя
    if (p1.shipsAlive > 0)
    {
        cout << "Winner: " << p1.name << endl;
    }
    else
    {
        cout << "Winner: " << p2.name << endl;
    }
}






// Спрашивает режим игры (человек-компьютер или компьютер-компьютер),
// расставляет корабли, запускает игру

int main()
{
    srand(time(0));

    while (true) //игра будет перезапускаться после рестарта
    {
        Player player1, player2; //создаем двух игроков - объекты структуры Player 

        //спрашиваем у пользователя режим игры для противников
        cout << "Select mode:\n1. Human vs Computer\n2. Computer vs Computer\n ";
        int mode;
        cin >> mode; //считать режим игры: человек-компьютер или компьютер-компьютер
        cin.ignore(); //очистка буфера перед вводом строк для удаления лишнего \n

        //спрашиваем у пользователя режим стрельбы для компьютера
        cout << "Computer shooting mode:\n1. Random Shot\n2. Intellectual Game\n ";
        int aiMode;
        cin >> aiMode;
        cin.ignore(); //очистка буфера перед вводом строк для удаления лишнего \n
        useSmartAI = (aiMode == 2); //если выбран 2 режим, то useSmartAI = true

        if (mode == 1) //если выбран режим человек против компьютера, то 
        { 
            //настройка имён и типов игроков
            player1.name = "Human";
            player1.isHuman = true;
            player2.name = "Computer";
            player2.isHuman = false;

            //предлагается расставить корабли вручную или автоматически
            cout << "Do you want to place your fleet manually? (y/n): ";
            char choice;
            cin >> choice;
            cin.ignore(); //очистка буфера перед вводом строк для удаления лишнего \n

            //в зависимости от выбора вызывается соответствубщая функция
            if (choice == 'y' || choice == 'Y')
            {
                manualPlaceFleet(player1); //вызов ф-ции для ручной расстановки кораблей
            }
            else
            {
                autoPlaceFleet(player1); //вызов ф-ции для автоматической расстановки кораблей
            }
            autoPlaceFleet(player2); //компьютеру корабли расставляются всегда автоматически
        }
        else if (mode == 2) //если выбран режим компьютер против компьютера
        {
            //устанавливаются имена и роли обоих игроков
            player1.name = "Computer_1";
            player1.isHuman = false;
            player2.name = "Computer_2";
            player2.isHuman = false;

            autoPlaceFleet(player1); //автоматическое размещение кораблей первого игрока
            autoPlaceFleet(player2); //автоматическое размещение кораблей второго игрока
        }
        else //если пользователь ввёл что-то кроме 1 или 1, будет выведено сообщение об ошибке
        {
            cout << "Invalid coordinates. Try again.\n";
        }

        initField(player1.enemyView); //очистка поля выстрелов первого игрока
        initField(player2.enemyView); //очистка поля выстрелов второго игрока

        playGame(player1, player2); // если restart — цикл начнётся заново
    }

    return 0;
}
