# Simple Web Server in C

This project is a simple web server written in C for educational purposes. It's capable of serving static files, including HTML and text files, from its running directory. The server listens on port 8080 by default but can be adapted to use any port.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

The web server is designed to run on a Linux system with GCC installed for compilation. Ensure you have `gcc` and `make` installed on your system:

`bash
sudo apt-get update
sudo apt-get install build-essential
`

### Installing

A step-by-step series of examples that tell you how to get a development env running.

1. Clone the repository or download the source code.
2. Navigate to the project directory.
3. Use the provided Makefile to compile the server:

`bash
make
`

### Running the server

After compilation, you can start the server with the following command:

\```bash
./webserver
\```

The server will start on port 8080. You can test it by accessing `http://localhost:8080/test.html` from your web browser, where `test.html` is a file in the server's directory.

### Deployment

To deploy the server on a live system, ensure that the port you intend to use is open and not blocked by your firewall.

## Built With

* [GCC](https://gcc.gnu.org/) - The GNU Compiler Collection
* Make - A tool which controls the generation of executables and other non-source files of a program from the program's source files.

