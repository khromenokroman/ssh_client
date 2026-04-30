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

**Опции сборки**

| Опция | По умолчанию | Описание |
|---|---:|---|
| `SSH_CLIENT_BUILD_CLI` | `OFF` | Собирает консольную утилиту `ssh-client` |
| `SSH_CLIENT_BUILD_INSTALL` | `OFF` | Добавляет правила установки через `cmake --install` |
| `SSH_CLIENT_BUILD_DEB` | `OFF` | Добавляет правила сборки DEB-пакета через CPack |

Shared-библиотека `libssh-client` собирается всегда.

Если включена опция `SSH_CLIENT_BUILD_DEB`, то `SSH_CLIENT_BUILD_INSTALL` включается автоматически.


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
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
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

- `бинарник`: `/usr/bin/ssh-client`
- shared-библиотека:
    - `/usr/lib/x86_64-linux-gnu/libssh-client.so.0.1.0`
    - `/usr/lib/x86_64-linux-gnu/libssh-client.so.0`
    - `/usr/lib/x86_64-linux-gnu/libssh-client.so`
- заголовочный файл: `/usr/include/ssh-client/ssh_client.hpp`
- `zsh completion`: `/usr/share/zsh/vendor-completions/_ssh-client`
- CMake package files для `find_package(ssh-client REQUIRED)`:
    - `/usr/lib/x86_64-linux-gnu/cmake/ssh-client/ssh-clientTargets.cmake`
    - `/usr/lib/x86_64-linux-gnu/cmake/ssh-client/ssh-clientTargets-debug.cmake`
    - `/usr/lib/x86_64-linux-gnu/cmake/ssh-client/ssh-clientConfig.cmake`
    - `/usr/lib/x86_64-linux-gnu/cmake/ssh-client/ssh-clientConfigVersion.cmake`

> Примечание: 
> - пути установки зависят от значения `-DCMAKE_INSTALL_PREFIX`. В примерах используется `-DCMAKE_INSTALL_PREFIX=/usr`.
> - nакже путь `/usr/lib/x86_64-linux-gnu` может отличаться в зависимости от архитектуры и дистрибутива.

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

### Использование как библиотеки

После установки dev-пакета библиотеку можно подключить в другом CMake-проекте через `find_package`.

Пример `CMakeLists.txt`:

``` cmake
cmake cmake_minimum_required(VERSION 4.2) 
project(untitled120)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED) 
find_package(ssh-client REQUIRED)
pkg_check_modules(LIBSSH2 REQUIRED libssh2) 

add_executable(${PROJECT_NAME} main.cpp )
target_link_libraries({PROJECT_NAME} PRIVATE
    {LIBSSH2_LIBRARIES} 
    ssh-client::ssh-client 
)
```

Пример `main.cpp`:

````cpp
#include <iostream>
#include <ssh-client/ssh_client.hpp>

int main() {
    std::cout << SshClient("localhost", 22, "roma", "roma").execute("ls -l /tmp1") << std::endl;
    return 0;
}
````
