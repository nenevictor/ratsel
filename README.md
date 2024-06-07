# Rätsel: simple and unsafe file encryptor!

Простий, не дає 100% безпеки, але дозволяє затягнути час дешифровкою.

Щоб зкомпілювати можна використати команду:

```Makefile
make product
```

Якщо якимось дивним чином потрібна сама команда, то от і вона: 

```Makefile
gcc -std=c99 -Ofast src/main.c src/ratsel.c vendor/hashmap/hashmap.c -o ratsel
```

В проєкті використовував одну дуже хорошу бібліотеку: https://github.com/tidwall/hashmap.c

Для компіляції використав w64devkit: https://github.com/skeeto/w64devkit/releases/tag/v1.23.0