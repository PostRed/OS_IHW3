#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// cтруктура определяет адрес сервера и адрес клиента.
struct sockaddr_in server;

int main(int argc, char *argv[]) {
    int ss, // сокет сервера
    cs, // сокет клиента
    count, // количество процессов-шифровальщиков
    start, // начальный индекс
    len_f, // длина кусочка текста
    len, // длина текста
    i;
    char encrypted_text[10000]; // зашифрованный текст (макс. длинна = 10000)
    socklen_t size_of_client;
    count = rand() % 5 + 1; // количество процессов-шифровальщиков рандомно от 1 до 5
    // создаем новый сокет, который используется для прослушивания подключений от клиентов
    ss = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2])); // argv[2] - порт
    // конвертируем IP-адрес в двоичный формат
    inet_pton(AF_INET, argv[1], &server.sin_addr); // argv[1] = IP
    // связываем сокет с адресом сервера.
    bind(ss, (struct sockaddr *) &server, sizeof(server));
    // начинаем прослушивание подключений от клиентов.
    listen(ss, count);
    len = strlen(argv[3]); // argv[3] - текст
    for (i = 0; i < count; i++) {
        start = (len / count) * i;
        if (i < count - 1) {
            len_f = len / start; // если это последний шифроващик, то нужно учесть что длина текста не будет нацело делиться на количество процессов-шифровальщиков
        } else {
            len_f = len - (len / count) * (count - 1);
        }

        struct sockaddr_in client;
        size_of_client = sizeof(client);
        // принимаем подключение от клиента и создает новый сокет для обмена данными с ним.
        cs = accept(ss, (struct sockaddr *) &client, &size_of_client);
        // отправляем начальный индекс
        send(cs, &start, sizeof(start), 0);
        // отправляем длинну теста
        send(cs, &len_f, sizeof(len_f), 0);
        // отправляем текст
        send(cs, argv[3] + start, len_f, 0);
        // получаем зашифрованный текст
        recv(cs, encrypted_text + start, len_f, 0);
        // закрываем сокет клиента
        close(cs);
    }
    encrypted_text[len] = '\0';
    printf("%s", encrypted_text);
    //  закрываем сокет сервера
    close(ss);
    return 0;
}
