Master [![Build Status](https://travis-ci.org/balancedbanana/balancedbanana.svg?branch=master)](https://travis-ci.org/balancedbanana/balancedbanana)[![Coverage Status](https://coveralls.io/repos/github/balancedbanana/balancedbanana/badge.svg?branch=master)](https://coveralls.io/github/balancedbanana/balancedbanana?branch=master)

# Dev Setup
## Ubuntu 18.04
- `git clone https://github.com/balancedbanana/balancedbanana.git`
- `cd balancedbanana`
- `pip install --user cpp-coveralls`
- `sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test`
- `sudo apt-get -q update`
- `sudo apt-get -y libssl-dev qtbase5-dev make g++-9 libqt5sql5-mysql mysql-server tar wget sshpass mailutils docker`
- `wget https://github.com/Kitware/CMake/releases/download/v3.16.2/cmake-3.16.2-Linux-x86_64.tar.gz`
- `tar -xf cmake-3.16.2-Linux-x86_64.tar.gz`
- Setup Database once if mysql was never installed before
    - `sudo systemctl start mysql`
    - `cat balancedbanana.sql | sudo mysql`
    - `echo "ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'password';"$'\n'"ALTER USER 'root'@'localhost' IDENTIFIED BY 'qwer1234';"$'\n'"FLUSH PRIVILEGES;" | sudo mysql`
- Setup Docker for Tests
    - `sudo docker pull centos`
- Building
    - `CXX=/usr/bin/g++-9 CC=/usr/bin/gcc-9 ./cmake-3.16.2-Linux-x86_64/bin/cmake -DCOVERAGE=1 .`
    - `./cmake-3.16.2-Linux-x86_64/bin/cmake --build .`


Externe Abhängigkeiten:

 - QT5:
	Bibliothek für Netzwerkübertragung und Datenbanken. Wird verwendet, um die drei Programme (Client, Scheduler und Worker) miteinander kommunizieren lassen zu können. Wird außerdem dazu verwendet die MySQL Datenbank auf dem Scheduler zu verwalten und von innerhalb des Codes SQL Anfragen zu ermöglichen.

 - CLI11:
	Bibliothek zum einlesen der Befehlszeile. Wird verwendet um die Argumente beim Aufruf der drei Programme (Client, Scheduler und Worker) die der Benutzer angegeben hat einzulesen.

 - OpenSSL:
	Bibliothek zur Verschlüsselung von Daten. Wird dazu verwendet einen Benutzer mithilfe eines Public-Private-Key Verfahrens mit dem Scheduler zu verbinden und zu identifizieren, wenn sich der selbe Benutzer erneut im System anmeldet.

 - GTest:
	Bibliothek zum Testen von c und c++ Code. Wird verwendet, um die einzelnen Bestandteile der Programme zu testen.

 - SimpleHTTPServer:
	Selbstentwickelte Bibliothek, die das Aufsetzten und verwalten eines HTTP Servers ermöglicht. Wird verwendet, um den HTTP Server zu verwalten, der auf HTTP Anfragen bezüglich des Systemstatus (z.B. Auslastung der Arbeiter Rechner, ...) antwortet.

 - Docker:
	Bibliothek zum kapseln von Prozessen in Containern. Wird dazu verwendet, die Benutzer Aufträge voneinender zu trennen und die gesetzten Grenzen für die Hardwareressourcen einzubehalten.

 - Cereal:
	Bibliothek

 - criu:
	Bibliothek

 - python:
	Programmiersprache für Scripts. Wird verwendet, um gewisse Tests auszuführen, die Netzwerkfunktionalität erfordern.
