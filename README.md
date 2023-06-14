# CITS3002 Project


**Project Name: Test Server**

This is a project of CITS3002 (sem1 2023), created and built by Daniel Hu (23006364).

 

**Installation and Setup**

This project has been built and tested on Ubuntu22.04 (WSL), to build and run this project you should have python3 and build-essential on your computer. This project uses cJSON Library for JSON parsing.



TM_server needs Flask configparser flask_session to run, the WHL file needed to install them are placed under TM_server/lib, you should be able to install them when you run make under the root of the project file. Then you should be able to use Python3 (3.10) to run app.py and visit 127.0.0.1:5000 on your browser to access the TM_server



QB_server requires cJSON library, but it has already been placed under the header and source file. you should be able to compile it by make. Once you make the qbserver, you will need to make a copy of it, create two separate folders to simulate two QB server, and edit and place the config file under each folder with the executable qbserver. Then create your questions.json and testTemplate folder to fill your questions in. There is an example of how two QB servers should be set up, you can edit it to suit your environment. The entire server will take port 5000, 5002, and 5003 by default. You can change it through config files.



**Third-party Libraries** 

This project utilizes the cjson Flask flask_session configparser, which are subject to the following license terms:

*cJSON*

Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

cJSON is licensed under the MIT License.

*Flask*

Copyright (c) 2012 by Armin Ronacher and contributors.

Some rights reserved.

*flask_session*

Copyright 2014 Pallets Community Ecosystem

flask_session is licensed under the BSD 2-Clause License

*configparser*

Copyright (c) 2015 YANDEX LLC

configparser is licensed under the MIT License.



**Copyright and License**

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

