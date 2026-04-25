# ssh-client

![SSHClient preview](logo/logo.png)

**ssh-client** — это простой консольный SSH-клиент на C++20, который позволяет подключаться к удалённому серверу по SSH,
выполнять команды и получать их вывод.

## Возможности

- подключение к SSH-серверу по логину и паролю;
- выполнение команд на удалённой машине;
- интерактивный режим: команды вводятся в `stdin`, вывод печатается в `stdout`;
- получение stdout ответа команды;
- автоматическое закрытие соединения при завершении работы;
- автодополнение аргументов в `zsh`;
- сборка из исходников и установка через DEB-пакет.

## Требования

### Для сборки

- CMake 3.26+
- C++20
- libssh2
- Boost.Program_options

#### Debian/Ubuntu

```bash
sudo apt install -y build-essential cmake dpkg-dev libssh2-1-dev libboost-program-options-dev
`````

#### macOS/Homebrew

````bash
brew install cmake libssh2 boost pkgconf
````

### Сборка из исходников

#### Linux

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
cmake --build . -j$(nproc)
cmake --install .
```

#### macOS

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(sysctl -n hw.ncpu)
cmake --install .
```

### Сборка DEB

Пакет собирается через CPack:

```bash 
cd build cpack -G DEB
apt install -y ./ssh-client_<версия>_amd64.deb
```

После установки будут размещены:

- `бинарник`: /usr/bin/ssh-client
- `zsh completion`: /usr/share/zsh/vendor-completions/_ssh-client

### Использование

Запуск:

``` bash
ssh-client --host <host> --port <port> --user <username> --pass <password>
```

Пример:

``` bash
ssh-client --host 172.16.0.100 --port 22 --user user --pass 'Pa$$w0rd'
```

После подключения программа перейдёт в интерактивный режим:

``` text
Connected. Type commands, or 'exit'/'quit' to stop.
> whoami
user
> ls -l /tmp
...
> exit
Disconnected.
```

### Аргументы

- `--host` адрес SSH-сервера
- `--port` порт SSH-сервера
- `--user` имя пользователя
- `--pass` пароль пользователя
- `--help` показать справку

