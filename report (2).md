# Отчет по лабораторной работе №1
## Работа со списками и реляционным представлением данных
## по курсу "Логическое программирование"

### студент: Саженов К.С.

## Результат проверки

| Преподаватель     | Дата         |  Оценка       |
|-------------------|--------------|---------------|
| Сошников Д.В. |              |               |
| Левинская М.А.|              |      4        |

<!-- > *Комментарии проверяющих (обратите внимание, что более подробные комментарии возможны непосредственно в репозитории по тексту программы)* -->


## Введение


В языке программирования Prolog списки представлены двумя элементами: головой и хвостом.
Данное представление позволяет прологу обрабатывать списки с помощью рекурсии, не прибегая к излишне сложным конструкциям. Данный вид списков называется односвязным списком, т.к. у каждого элемента, фактически, есть ссылка только на следующий за ним.

В списках в прологе изменение и добавка элемента занимает `O(1)` времени, в то время как доступ к `n` элементу занимает `O(n)` времени, что довольно долго. Данный "недостаток" имеет место быть из-за того, что списки в прологе являются односвязными списками.

В императивных языках программирования, чаще всего, встроенные списки называются массивами и имеют сложность доступа `O(n)`, в то время как добавление, наоборот, является "долгой" операцией и выполняется за `O(n)` амортизированное. Хотя в некоторых языках также существует и тип данных, называемый `list`, который реализует именно список, правда двусвязный. Примером такого языка является `C++`.

## Задание 1.1: Предикат обработки списка

`mynth0(X, List, R)` - выводит `X`-й элемент списка `List` в переменную `R`, индексируя список с нуля.

Примеры использования:
```prolog
?- mynth0(X, [1, 2, 4, 3, 5], R).
X = 0,
R = 1 ;
X = 1,
R = 2 ;
X = 2,
R = 4 ;
X = R, R = 3 ;
X = 4,
R = 5 ;
false.

?- mynth0(0, [1, 2, 4, 3, 5], R).
R = 1 ;
false.

?- mynth0(X, [1, 3, 4, 5, 2, 2, 3], 2).
X = 4 ;
X = 5 ;
false.
```

Реализация:
```prolog
mynth0(0, [Head|_], Head).
mynth0(N, [_|Tail], Elem) :-
    nonvar(N),
    M is N-1,
    mynth0(M, Tail, Elem).
mynth0(N, [_|Tail], Elem) :-
    var(N),
    mynth0(M, Tail, Elem),
    N is M+1.
```

При индексе 0, возвращаем в ответ первый элемент списка. В ином случае, если мы запросили какой-то индекс(следовательно N не свободная перменная), то считаем индекс на единицу меньше и запускаем предикат рекурсивно, исключая первый элемент из списка(голову).

Также существует второй вариант использования, когда N является свободной переменной. В таком случае, мы проходим от 0(первый предикат), до максимально возможного с помощью подобия dfs на графах(сначала идем в ноль, затем поднимаемся выше и выше).

Отсутствует реализация  предиката через стандартные.

## Задание 1.2: Предикат обработки числового списка

`arithmetic_progression(List)` - возвращает `true` только в случае, если `List` является списком-арифметической прогрессией.

Примеры использования:
```prolog
?- arithmetic_progression([1, 2, 3, 5]).
false.

?- arithmetic_progression([1, 2, 3, 4]).
true.

?- arithmetic_progression([1, 4, 7]).
true.

?- arithmetic_progression([1, 4, 7, 11]).
false.

?- arithmetic_progression([10, 7, 4, 1, -2]).
true.

?- arithmetic_progression([10, 7, 4, 1, 0]).
false.
```

Реализация:
```prolog
arithmetic_progression([], _, _) :- true, !.
arithmetic_progression([X|Tail], Prev, D) :- X0 is Prev + D, X0 = X, arithmetic_progression(Tail, X, D), !.

arithmetic_progression([]) :- true, !.
arithmetic_progression([_]) :- arithmetic_progression([]), !.
arithmetic_progression([X, Y|Tail]) :- D is Y - X, arithmetic_progression([Y|Tail], X, D), !.
```

В данном задании я использовал вспомогательны одноименный предикат, аргументы которого означали сам список, предыдущий элемент и разность арифметической прогрессии соответственно. Пустой список по определению является арифметической прогрессией. Список из одного элемента также является арифметической прогрессией. В списках из двух элементов и больше, считается разность между первыми двумя элементами списка и затем передается управление вспомогательному предикату, отсекая первый элемент, который, в свою очередь, проверяет, является ли текущий элемент суммой предыдущего и разности арифметической прогрессии, а также запускает этот же предикат рекурсивно для оставшегося хвоста списка. Когда список пуст, он всегда является прогрессией.

## Задание 1.3: пример совместного использования предикатов

В качестве примера можно привести такую задачу:
Реализовать предикат, который ищет N-й элемент списка только в том случае, если список является арифметической прогрессией.

Возможное решение:
```prolog
pred(Index, List, Elem) :- mynth0(Index, List, Elem), arithmetic_progression(List).
```

## Задание 2: Реляционное представление данных

Преимузества реляционного представления в целом:
* Удобно хранить данные, менять их и добавлять
* Легко узнавать связи тех или иных объектов
* Требуется меньше преобразований, чтобы отобразить данные в удобочитаемом виде(по отношению к другим представлениям)

Недостатки реляционного представления в целом:
* В некоторых задачах требуется слишком много времени для поиска информации и связей между ними
* При комплексных структурах данных, требуется значительное время, чтобы найти всю информацию об одном объекте
* Довольно трудно хранить неструктурированную информацию(такую как информация в текстах, книгах)
* Не всегда можно создать "универсальное" представление для некоторого вида информации, в котором будет возможен "быстрый" доступ ко всей информации

Преимущества представления, используемого мной:
* Данный вид представления очень удобно использовать, имея уже полный набор данных

Недостатки:
* В то же время, добавлять какое-либо дополнение в уже имеющиеся данные - нетривиальная задача
* Данное представление мне показалось излишне комплексным и имеет некоторые "over-headings"

`average(List, Res)` - считает среднее арифметическое числового списка `List` и записывает результат в `Res`.

Примеры использования:
```prolog
?- average([1, 2, 3, 3, 4, 5], X).
X=3.
```

Реализация:
```prolog
average(List, Res) :- length(List, Len), sum_list(List, Sum), Len > 0, Res is Sum/Len.
```
Данный предикат сначала считает длину списка и сохраняет в переменную `Len` результат, затем суммирует все члены списка в переменную `Sum` и затем, после проверки длины, которая должна быть больше нуля, считается ответ в переменную `Res`(сумму делит на количество элементов).

`scores(Student, Res)` - выводит все оценки указанного студента `Student` в `Res`.

Примеры использования:

```prolog
?- scores('Джаво', Res).
Res = 2 ;
Res = 4 ;
Res = 5 ;
Res = 5 ;
Res = 4 ;
Res = 4 ;
false.

?- scores(X, 2).
X = 'Петровский' ;
X = 'Сидоров' ;
X = 'Джаво' ;
X = 'Петров' ;
X = 'Сиплюсплюсов' ;
X = 'Криптовалютников' ;
X = 'Решетников' ;
X = 'Азурин' ;
X = 'Безумников' ;
X = 'Круглосчиталкин' ;
false.
```

Реализация:
```prolog
scores(Student, Res) :- subject(_, X), member(grade(Student, Res), X).
```

Данный предикат вычленяет список из каждого предмета(без учета самого предмета), затем проверяет, есть ли студент с указанной оценкой в списке предмета с помощью встроенного предиката `member\2`.

`average_score_by_student(Student, Res)` - считает среднюю оценку для каждого студента.

Пример использования:
```prolog
?- average_score_by_student('Джаво', Res).
Res = 4.
```
Реализация:
```prolog
average_score_by_student(Student, Res) :- findall(N, scores(Student, N), L), average(L, Res).
```

Данный предикат находит все оценки у данного студента `Student` и записывает их в список `L`, который затем передается в предикат `average` для подсчета среднего арифметического данного числового списка и записывает ответ в `Res`.

`average_score_every_student(Group, Res)` - считает среднее арифметическое для каждого студента из указанной группы.

Пример использования:
```prolog
?- average_score_every_student(101, Res).
Res = 3.6666666666666665 ;
Res = 3.6666666666666665 ;
Res = 4.5 ;
Res = 3.6666666666666665 ;
Res = 4.333333333333333.
```

Реализация:
```prolog
average_score_every_student(Group, Res) :- group(Group, List), member(S, List), average_score_by_student(S, Res).
```

Данный предикат берет список студентов из указанной группы, а затем проходит по каждому элементу списка(по каждому студенту из группы) и считает среднее значение оценок у студента.

`average_score_by_group(Group, Ans)` - считает среднее арифметическое оценок для данной группы `Group` и записывает ответ в `And`.

Пример использования:
```prolog
?- average_score_by_group(101, Res).
Res = 3.9666666666666663.

?- average_score_by_group(102, Res).
Res = 3.9444444444444446.

?- average_score_by_group(X, Res).
X = 102,
Res = 3.9444444444444446 ;
X = 101,
Res = 3.9666666666666663 ;
X = 104,
Res = 3.8611111111111107 ;
X = 103,
Res = 4.145833333333334.
```

Реализация:
```prolog
average_score_by_group(Group, Ans) :- group(Group, _), findall(Res, average_score_every_student(Group, Res), ListAverages), average(ListAverages, Ans).
```

Данный предикат удостоверяется, что есть указанная группа и, затем считает среднее для каждого студента из группы, занося найденные ответы из предыдущего предиката в список `ListAverages`, а затем считая среднее арифметическое с помощью предиката `average`.

`with_score_by_subject(Sub, Score, Name)` - ищет студента, у которого оценка по предмету `Sub` равна `Score`.

Примеры использования:
```prolog
?- with_score_by_subject('Логическое программирование', 4, Stud).
Stud = 'Петров' ;
Stud = 'Сидоркин' ;
Stud = 'Биткоинов' ;
Stud = 'Сиплюсплюсов' ;
Stud = 'Программиро' ;
Stud = 'Круглосчиталкин' ;
Stud = 'Эксель' ;
Stud = 'Текстописов' ;
false.
```
Реализация:
```prolog
with_score_by_subject(Sub, Score, Name) :- subject(Sub, Students), member(grade(Name, Score), Students).
```

Данный предикат вычленяет список студентов из указанного предмета `Sub`, затем ищет студента, у которого по данному предмету данная оценка `Score` с помощью встроенного предиката `member`, который проверяет принадлежность к списку, и возвращает имя студента в качестве ответа.

`failed_students_by_subject(Sub, Name)` - находит студента, у которого по данному предмету `Sub` оценка 2 и выводит имя студента в переменную `Name`.

Пример использования:
```prolog
?- failed_students_by_subject('Логическое программирование', Ans).
Ans = 'Петровский' ;
Ans = 'Сидоров' ;
Ans = 'Джаво' ;
false.

?- failed_students_by_subject(Sub, 'Петров').
Sub = 'Математический анализ' ;
false.
```

Реализация:
```prolog
failed_students_by_subject(Sub, Name) :- with_score_by_subject(Sub, 2, Name).
```

Данный предикат использует предыдущий предикат, но конкретизирует оценку в виде числа 2.

`list_failed_students_by_subject(Sub, ListNames)` - ищет учеников, которые не сдали указанный предмет `Sub` и записывает их в список `ListNames`.

Пример использования:

```prolog
?- list_failed_students_by_subject(Sub, Studs).
Sub = 'Логическое программирование',
Studs = ['Петровский', 'Сидоров', 'Джаво'] ;
Sub = 'Математический анализ',
Studs = ['Петров'] ;
Sub = 'Функциональное программирование',
Studs = [] ;
Sub = 'Информатика',
Studs = ['Сиплюсплюсов', 'Криптовалютников'] ;
Sub = 'Английский язык',
Studs = ['Решетников', 'Азурин'] ;
Sub = 'Психология',
Studs = ['Безумников', 'Круглосчиталкин'].

?- list_failed_students_by_subject('Психология', Studs).
Studs = ['Безумников', 'Круглосчиталкин'].

?- list_failed_students_by_subject(Sub, []).
Sub = 'Функциональное программирование' ;
false.
```

Реализация:
```prolog
list_failed_students_by_subject(Sub, ListNames) :- subject(Sub, _), findall(Stud, failed_students_by_subject(Sub, Stud), ListNames).
```
Данный предикат удостоверяется, что данный предмет существует, затем ищет всех студентов, которые провалили указанный предмет `Sub` с помощью встроенного предиката `findall` и запоминает результат в перменую `ListNames`.

`with_score_by_group(Group, Score, Name)` - ищет студента, у которого оценка в группе `Group` равна `Score`.

Примеры использования:
```prolog
?- with_score_by_group(101, 3, Name).
Name = 'Петровский' ;
Name = 'Петровский' ;
Name = 'Сидоров' ;
Name = 'Сидоров' ;
Name = 'Мышин' ;
Name = 'Безумников' ;
Name = 'Безумников' ;
Name = 'Густобуквенникова' ;
false.

?- with_score_by_group(Gr, 2, 'Джаво').
Gr = 104 ;
false.
```

Реализация:
```prolog
with_score_by_group(Group, Score, Name) :- group(Group, Students), member(Name, Students), with_score_by_subject(_, Score, Name).
```

Данный предикат использует предикат нахождения студентов по предмету с указанной оценкой `Score` и для улосвия причастности студента к группе, также использует предикат group, а также ищет студента с данным именем `Name` в списке студентов указанной группы.

`failed_students_by_group(Group, Name)` - предикат ищет студентов, проваливших экзамен, в указанной группе `Group`.

Пример использования:
```prolog
?- failed_students_by_group(101, N).
N = 'Петровский' ;
N = 'Сидоров' ;
N = 'Безумников' ;
false.

?- failed_students_by_group(G, 'Петровский').
G = 101 ;
false.
```

Реализация:
```prolog
failed_students_by_group(Group, Name) :- with_score_by_group(Group, 2, Name).
```

Данный предикат просто использует предыдущий, с явным указанием оценки, равной числу 2.

`count_failed_students_by_group(Group, Count)` - считает количество студентов, проваливших экзамены в данной группе `Group`.

Пример использования:
```prolog
?- count_failed_students_by_group(104, C).
C = 2.

?- count_failed_students_by_group(102, C).
C = 3.

?- count_failed_students_by_group(103, C).
C = 2.

?- count_failed_students_by_group(G, 2).
G = 104 ;
G = 103.
```
Реализация:
```prolog
count_failed_students_by_group(Group, Count) :- group(Group, _), setof(Name, failed_students_by_group(Group, Name), Students), length(Students, Count).
```

Данный предикат сначала проверяет наличие указанной группы `Group`, затем с помощью предиката `setof`, ищет всех уникальных студентов, проваливших экзамен в данной группе, и запоминает их в список `Students`, длина которого затем высчитывается предикатом `length` и результат записывается в перменную `Count`.

## Выводы

Данная лабораторная работа помогла мне задуматься о том, что различные структурированные данные могут иметь разное представление в программировании(в программном коде), которые имеют свои достоинства и недостатки. Также данная лабораторная работа помогла мне основательно разобраться с тем, как следует работать со списками в логических языках программирования, таких как Prolog. Также данная лабораторная работа научила меня находить шаблоны запросов к данным, которые представленны не всегда в удобном формате для данного вида запроса. 

Меня заинтересовало то, каким образом данные структурируются в памяти компьютера(не обязательно оперативной) и данная работа побудила меня начать изучать то, как работают реляционные базы данных, а также у меня возник ещё бо́льший интерес в изучении нереляционных баз данных.