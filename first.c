#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// структура определяет адрес сервера
struct sockaddr_in server;

int main(int argc, char *argv[]) {
    // переменные для проверки результатов
    long send_res, recv_res, connect_res;
    int cs, // сокет для соединения с сервером
    start, // начальный индекс текста
    len, // длина текста, который нужно зашифровать
    i; // без комментариев
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2])); // argv[2] - порт
    // создаем новый сокет, который используется для соединения с сервером
    cs = socket(AF_INET, SOCK_STREAM, 0);
    if (cs < 0) {
        exit(1); // ошибка создания сокета
    }
    // конвертируем IP-адрес в двоичный формат
    inet_pton(AF_INET, argv[1], &server.sin_addr); // argv[1] = IP
    // устанавливаем соединение с сервером
    connect_res = (connect(cs, (struct sockaddr *) &server, sizeof(server)));
    if (connect_res < 0) {
        exit(1); // ошибка соединения
    }
    // получаем данные от сервера - начальный индекс
    recv_res = recv(cs, &start, sizeof(start), 0);
    if (recv_res < 0) {
        perror("Не удалось получить начальный индекс :(");
        exit(1); // ошибка получания индекса
    }
    // получает данные от сервера - длинну текста
    recv_res = recv(cs, &len, sizeof(len), 0);
    if (recv_res < 0) {
        perror("Не удалось получить длинну текста :(");
        exit(1); // ошибка получения длинны
    }
    char *text[len], // исходный текст
    *encrypted_text[len]; // зашифрованный текст
    // получает данные от сервера - сам текст
    recv_res = recv(cs, &text, len, 0);
    if (recv_res < 0) {
        perror("Не удалось получить текст :(");
        exit(1); // ошибка получения текста
    }
    // шифруем текст просто сдвигая каждый символ
    for (i = 0; i < len; i++) {
        encrypted_text[i] = text[i] + 2;
    }
    // отправляет данные на сервер.
    send_res = send(cs, encrypted_text, len, 0);
    if (send_res < 0) {
        perror("Не удалось отправить зашифрованный текст :(");
        exit(1); // ошибка отправки текста
    }
    // закрываем сокет
    close(cs);
    return 0;
}
