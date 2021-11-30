# pluginscalc
  
Калькулятор с плагинами (динамически загружаемые библиотеки).  
Подключаемые функции калькулятора должны находиться в папке `./plugins/`.  
Каждый плагин должен содержать структуру для подключения плагина в калькулятор:  
```
struct Info_calc_func
```  
Структура содержит имя плагина (отображается в меню калькулятора) и имя функции динамически загружаемой библиотеки.  
Пример:  
```
struct Info_calc_func info_calc_func = {
    "Сумма (a + b)",
    "my_add"
};
```
