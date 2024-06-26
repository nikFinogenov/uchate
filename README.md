# Uchat

Uchat is a messaging application developed in C using GTK+3 and SQLite3. It allows users to communicate with each other in real-time through text messages.

## Installation

To run the project, make sure that `make`, `clang`, and `sqlite3` are installed on your system. Then, simply call `make` in the terminal to compile the project.

```bash
make
```

## Usage

After compiling the project you will see two files, run the `uchat_server` with port as an argument first, than run `uchat` with ip and port as an arguments, port must be the same as the server started with.

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
