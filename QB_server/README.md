QB server

This is just an instruction about the framework of the QB server and some other files in QB_server



questions_c/py.json: They are the question files that should contain all the questions and answers of one type of programming language.

test_template_c/py: These are folders which contain the test cases of the coding question in the question file. the name of the test_template should be the same to the coding question's id.

config.ini: this is the configuration file containing the programming language of the questions, the name of the question file, the folder of test_template, and the last server_port that opens for communication.



you should be able to create qbserver by make. and follow the instruction in the main README file to set it up for running.



You need to notice that the server cannot stay up forever, it will be closed by OS if it runs too long. But this process usually can stay around 15-20mins. If you wish to make it longer, you must set up a daemon process to keep the server running. 

