#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <string.h>

#define F(f, g, a) (f(a) - g(a)) //макрос, который реализует подстановкой функцию F(x) = f(x) - g(x)
typedef double afunc(double); //имя для функции от одного вещественного аргумента
static long long unsigned num_of_iterations; //переменная, которая хранит в себе число итераций,
//необходимых для приближённого вычисления абсцисс точек пересечения

extern afunc f1, f2, f3, f1der, f2der, f3der, f1der_, f2der_, f3der_; //список внешних функций
double (*funcs[9])(double) = {f1, f2, f3, f1der, f2der, f3der, f1der_, f2der_, f3der_}; //массив указателей на функции

//функция, которая печатает число итераций, потребовавшихся для поиска абсцисс
static void print_num_of_iterations(void){
    printf("%llu - количество итераций\n\n", num_of_iterations);
}

//функция, которая печатает абсциссы точек пересечения кривых
static void print_roots(double a, double b, double c){
    printf("%lf - абсцисса точки пересечения функций f1 и f3\n", a);
    printf("%lf - абсцисса точки пересечения функций f2 и f3\n", b);
    printf("%lf - абсцисса точки пересечения функций f1 и f2\n\n", c);
}

//функция, которая печатает список доступных ключей командной строки
static void print_key_list(void){
    printf("Список доступных ключей:\n");
    printf("--root и -r\n");
    printf("--iterations и -i\n");
    printf("--test-root и -R, аргумент: F1:F2:A:B:E:R\n");
    printf("--test-integral и -I, аргумент: F:A:B:E:R\n");
    printf("--help и -h\n");
}

//функция, которая считает интеграл от функции f(x) с помощью формулы прямоугольников
static double integral(afunc *f, double a, double b, double eps){
    int n = 10; //начальное количество прямоугольников
    double i_n = 0; //интеграл, равный сумме площадей n прямоугольников
    double i_2n = 0; //интеграл, равный сумме площадей 2n прямоугольников
    double p = 1/3.0; //константа, необходимая для применения правила Рунге
    double h = 0; //шаг разбиения
    int first_iteration = 1; //флаг отвечающий за первую итерацию при подсчёте интеграла

    do{
        if(first_iteration){ //если первая итерация, то посчитаем i_n
            h = (b - a)/n;
            for(int i = 0; i < n; i++){
                i_n = h*(f(a + (i + 0.5)*h)); //применяем формулу прямоугольников
            }
            first_iteration = 0; //чтобы снова не пересчитывать интеграл
        } else {
            i_n = i_2n; //иначе приравниваем к i_2n, так как будет равен i_2n на предыдущей итерации цикла
            i_2n = 0; //зануляем, так как будем вычислять заново
        }
        h = (b - a)/(2*n); //шаг разбиения (длина прямоугольника/отрезка)

        for(int i = 0; i < 2*n; i++){
            i_2n += h*(f(a + (i + 0.5)*h)); //применяем формулу прямоугольников
        }

        n *= 2; //увеличиваем число прямоугольников (для повышения точности)
    } while(p*fabs(i_n - i_2n) > eps); //применяем правило Рунге

    return i_2n; //ответ
}

//функция, которая находит абсциссу точки пересечения функции F(x) = f(x) - g(x) с осью OX с помощью метода касательных
static double root(afunc *f, afunc *fder, afunc *fder_, afunc *g, afunc *gder, afunc *gder_, double a, double b, double eps) {
    double c, eps_step;
    if(F(f, g, a)*F(fder_, gder_, a) > 0){ //выбираем сторону приближение
        c = a; //приближение слева
        eps_step = eps;
    } else {
        c = b; //приближение справа
        eps_step = -eps;
    }

    while(F(f, g, c)*F(f, g, (c + eps_step)) > 0){
        c = c - (F(f,g,c)/F(fder, gder, c)); //приближение
        num_of_iterations++; //увеличиваем число итераций
    }

    return c; //ответ
}

//функция, которая парсит аргумент ключа --test-root / -r и тестирует функцию root
static void test_root(const char *arg){ //F1:F2:A:B:E:R
    int num_of_first_func, num_of_second_func; //номера функций для тестирования
    double a, b, eps, answ; //аргументы A, B, E, R соответственно

    sscanf(arg, "%d:%d:%lf:%lf:%lf:%lf", &num_of_first_func, &num_of_second_func, &a, &b, &eps, &answ);//cчитали аргументы
    num_of_first_func--; //уменьшаем номер функции, чтобы удобнее итерироваться по массиву
    num_of_second_func--; //уменьшаем номер функции, чтобы удобнее итерироваться по массиву

    double cur_answ = root(funcs[num_of_first_func], funcs[num_of_first_func+3],
                           funcs[num_of_first_func+6], funcs[num_of_second_func],
                           funcs[num_of_second_func+3], funcs[num_of_second_func+6],
                           a, b, eps); //ответ, который мы получим вызвав функцию root
    double absolute_err = fabs(cur_answ - answ);  //абсолютная ошибка
    double relative_err = fabs(absolute_err / cur_answ); //относительная ошибка

    printf("%lf %lf %lf\n", cur_answ, absolute_err, relative_err);
}

//функция, которая парсит аргумент ключа --test-integral / -I и тестирует функцию integral
static void test_integral(const char *arg){
    int num_of_func; //номер функции для тестирования
    double a, b, eps, answ; //аргументы A, B, E, R соответственно

    sscanf(arg, "%d:%lf:%lf:%lf:%lf", &num_of_func, &a, &b, &eps, &answ); //считали аргументы
    num_of_func--; //уменьшаем номер функции, чтобы удобнее итерироваться по массиву

    double cur_answ = integral(funcs[num_of_func], a, b, eps); //ответ, который мы получим из функции integral

    double absolute_err = fabs(cur_answ - answ); //абсолютная ошибка
    double relative_err = fabs(absolute_err / cur_answ); //относительная ошибка
    printf("%lf %lf %lf\n", cur_answ, absolute_err, relative_err);
}

int main (int argc, char *argv[]) {
    const char* short_options = "hriR:I:"; //список доступных коротких ключей
    struct option long_options[] = { //cписок доступных ключей командной строки
            {"help",          no_argument,       NULL, 'h'},
            {"root",          no_argument ,      NULL, 'r'},
            {"iterations",    no_argument,       NULL, 'i'},
            {"test-root",     required_argument, NULL, 'R'},
            {"test-integral", required_argument, NULL, 'I'},
    };
    int print_answer = 1; //флаг, который отвечает за то, будем ли искать (и печатать) ответ в целом
    double eps1 = 0.00001; //задаём точность вычислений корней
    double a = root(f1, f1der, f1der_, f3, f3der, f3der_, -5, -4, eps1); //абсцисса точки пересечения функций f1 и f3
    double b = root(f3, f3der, f3der_, f2, f2der, f2der_, -2, -1, eps1); //абсцисса точки пересечения функций f2 и f3
    double c = root(f1, f1der, f1der_, f2, f2der, f2der_, -1, -0.1, eps1); //абсцисса точки пересечения функций f1 и f2

    int value = 0; //переменная, которая будет хранить в себе значение long_options[index].val, которое вернёт getopt_long()
    while ((value = getopt_long(argc, argv, short_options, long_options, NULL)) != -1){
        switch (value) {
            case 'R':
                test_root(optarg); //тестируем функцию root
                print_answer = 0; //не выводим ответ, т.к. программа запущена в тестовом режиме
                break;
            case 'I':
                test_integral(optarg); //тестируем функцию integral
                print_answer = 0; //не выводим ответ, т.к. программа запущена в тестовом режиме
                break;
            case 'h':
                print_key_list(); //печать доступных ключей
                print_answer = 0; //не выводим ответ, т.к. программа запущена в справочном режиме
                break;
            case 'i':
                print_num_of_iterations(); //печать числа итераций
                break;
            case 'r':
                print_roots(a, b, c); //печать абсцисс точек пересечения
                break;
            default:
                break;
        }
    }
    double eps2 = 0.00014; //задаём точность вычисления интегралов
    if(print_answer) { //если программа не в тестовом и справочном режиме
        double s1 = integral(f1, a, b, eps2) -
                    integral(f3, a, b, eps2); //площадь фигуры на отрезке [a, b], равна интегралу от функции f1 - f3
        double s2 = integral(f1, b, c, eps2) -
                    integral(f2, b, c, eps2); //площадь фигуры на отрезке [b, c], равна интеграллу от функции f1 - f2
        double s = s1 + s2;  //итоговая площадь фигуры, равная сумме площадей её частей
        printf("Площадь фигуры равна %lf\n", s); //печатаем ответ
    }
    return 0;
}
