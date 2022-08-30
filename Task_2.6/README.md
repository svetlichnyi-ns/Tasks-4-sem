So as to run the program, enter the following into console:  
g++ -c -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -Wconversion -Wunreachable-code -g -Wuninitialized main.cpp  
g++ -c -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -Wconversion -Wunreachable-code -g -Wuninitialized source.cpp  
g++ main.cpp source.cpp -lpthread -fsanitize=address -o output  
./output
