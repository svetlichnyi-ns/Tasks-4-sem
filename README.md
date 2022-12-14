Задача 2.5. Вычисление интеграла численными методами  
Исходные данные: функция одной переменной, отрезок интегрирования, количество вычислителей.  
Цель: написать параллельную программу для вычисления интеграла от данной функции по данному отрезку.  
Программа должна считать функцию, отрезок интегрирования, количество вычислителей, посчитать интеграл в несколько вычислителей, вывести результат.  
Предполагаемый алгоритм распараллеливания: использовать параллелизм по отрезку. Каждый вычислитель считает интеграл по своей части отрезка с помощью одного из стандартных методов (трапеции, Симпсона, Ньютона-Лейбница и т.д.). Результаты отдельных вычислителей суммируются.  
Численный метод выбирается преподавателем.  

Задача 2.6. Вычисление интеграла методом Монте-Карло  
Исходные данные: функция одной переменной, отрезок интегрирования, количество вычислителей.  
Цель: написать параллельную программу для вычисления интеграла от данной функции по данному отрезку методом Монте-Карло.  
Программа должна считать функцию, отрезок интегрирования, количество вычислителей, посчитать интеграл в несколько вычислителей, вывести результат.  
Предполагаемый алгоритм распараллеливания:  
a) Использовать параллелизм по отрезку. Каждый вычислитель считает интеграл по своей части отрезка. Результаты отдельных вычислителей суммируются.  
b) Использовать параллелизм по количеству испытаний в методе Монте-Карло. Необходимое количество испытаний делится между N вычислителями. Результаты испытаний обобщаются.
