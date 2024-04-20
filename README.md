# Uchat

Uchat is a messaging application developed in C using GTK+3 and SQLite3. It allows users to communicate with each other in real-time through text messages.

## Installation

To run the project, make sure that `make` and `clang` are installed on your system. Then, simply call `make` in the terminal to compile the project.
```bash
make
```

> [!WARNING] 
> Libraries are preinstalled, they were compiled for MacOS, so it will run ONLY on MacOS. Development was on different OS's, so for Linux based systems you should:
> - edit [server/inc/server.h](nikFinogenov/uchate/server/inc/server.h): comment preinstalled ssl and sqlite3 libraries and uncomment system analogues of them
> - edit [server/Makefile](nikFinogenov/uchate/server/Makefile): add flags `-lsqlite3`,  `-lssl`, `-lcrypto` to compilation proccess, and also remove paths of preinstalled libraries.
> So your compilation should look like this `@clang -lpthread -lssl -lcrypto -lsqlite3 $(FLAGS) $(DFLAGS) $(OBJS) $(LMXA) -o ../$(NAME)`

## Usage

After compiling the project you will see two files, run the 'uchat_server' with port as argument first, than run 'uchat' with ip and port as an arguments, port must be the same as server started with.

```bash
./uchat_server 7777
./uchat 127.0.0.1 7777
```

## Contributors

Uchat is written by a team of 4 developers, with 3 actively contributing to the project and 1 super lazy contributor:

- [Nikita Finogenov](https://github.com/nikFinogenov)
- [Maksym Bratash](https://github.com/kitska)
- [Dmytro Dolhii](https://github.com/DMYTRO-DOLHII)
- [Andrew Yakimov](https://github.com/WoCCeR) (he did something, for sure)

## Thanks for Support

We would like to express our gratitude to the following individuals for their support and feedback during the development of Uchat:

- Dmitriy ['erter'](https://steamcommunity.com/profiles/76561199014577745) Avdeed 
- Vladislav ['Champion'](https://steamcommunity.com/profiles/76561199022185683) Chubukin

Your contributions have been invaluable to the project's success.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
