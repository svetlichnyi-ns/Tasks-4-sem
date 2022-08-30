So as to run the program, enter the following into console:  
g++ -c -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -Wconversion -Wunreachable-code -g -Wuninitialized -fsanitize=address main.cpp  
g++ -c -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -Wconversion -Wunreachable-code -g -Wuninitialized -fsanitize=address source.cpp  
g++ main.cpp source.cpp -lpthread -o output  
./output
