# ssh-client

**ssh-client** — это простой консольный SSH-клиент на C++20, который позволяет подключаться к удалённому серверу по SSH, выполнять команды и получать их вывод.

## Возможности

- подключение к SSH-серверу по логину и паролю;
- выполнение команд на удалённой машине;
- получение stdout ответа команды;
- автоматическое закрытие соединения при завершении работы;
- сборка из исходников и установка через DEB-пакет.

## Требования

### Для сборки

- CMake 3.26+
- C++20

````bash
apt install -y build-essential cmake dpkg-dev
````

## Сборка из исходников

```bash 
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
cmake --build . -j$(nproc)
cmake --install .
```

## Сборка DEB

Пакет собирается через CPack:

```bash 
cd build cpack -G DEB
apt install -y ./ssh-client_<версия>_amd64.deb
```

После установки будут размещены:

- бинарник: `/usr/bin/ssh-client`

