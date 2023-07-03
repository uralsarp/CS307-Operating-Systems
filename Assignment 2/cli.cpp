#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <vector>
#include <thread>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>

using namespace std;

struct parsedCommand{
 string command;
 string option;
 string inpt;
 string redirection;
 string background;
 string textword;

};


mutex printLock; 

void printThread(int* fd){
    
    printLock.lock();
    cout << "---- " << this_thread::get_id() << endl;
                       
    FILE* file = fdopen(fd[0], "r"); 
    
    char line[256];
    while (fgets(line, sizeof(line), file)) { 
     	cout << line; 
    }
    fclose(file);

    cout << "---- " << this_thread::get_id() << endl;
    fflush(stdout);
    printLock.unlock();
}


int main(){


	ifstream input("commands.txt");       
	string line;
	vector<parsedCommand> commands;
	vector<int> backprocess;
	vector<thread> backthreads;

	
	
	while (getline(input, line)) {
       	 	string redirection="-";
        	string command="";
        	string option="";
        	string inpt="";
        	string background="n";
        	string textword="";

		stringstream ss(line);
		string word;
		bool txtword=false;
		bool comCheck=true;
		while (ss>>word){
			if(comCheck==true){
				command=word;
				comCheck=false;
			}
			else{
				if(command=="wait"){
                                
                        	}

                        	else if(word[0]=='-'){
                                	option=word;
                                	txtword=false;
                        	}
                        	else if(word == "&"){
                                	background="y";
                                	txtword=false;
                        	}
                       	 	else if(word == "<" || word == ">"){
                                	redirection=word;
                                	txtword=true;
                        	}
                        	else if(txtword){
                                	textword=word;
                                	txtword=false;
                        	}
                        	else{
                               		inpt=word;
                               		txtword=false;
                        	}	
			}		
			
			
		}	
		 
		
		parsedCommand parsecommand;
		parsecommand.command=command;
		parsecommand.inpt=inpt;
		parsecommand.option=option;
		parsecommand.redirection=redirection;
		parsecommand.textword=textword;
		parsecommand.background=background;
		
		commands.push_back(parsecommand);
	}
   	input.close();
		

	ofstream output;
        output.open("parse.txt");

	for (int i=0; i<commands.size(); i++){
		output<<"----------"<<endl;
       	 	output<<"Command: "<<commands[i].command<<endl;
        	output<<"Inputs: "<<commands[i].inpt<<endl;
       	 	output<<"Options: "<<commands[i].option<<endl;
        	output<<"Redirection: "<<commands[i].redirection<<endl;
        	output<<"Background Job: "<<commands[i].background<<endl;
        	output<<"----------"<<endl;

		if (commands[i].command=="wait"){
			for (int m=0;m<backprocess.size();m++){
				waitpid(backprocess[m],NULL,0);
			}
			backprocess.clear();

			for (int k=0;k<backthreads.size();k++){
				backthreads[k].join();
			}
			backthreads.clear();
		
		}		

		else if(commands[i].background=="n" && commands[i].redirection=="-"){
                	
			int comchild = fork();
               		if (comchild < 0){
                        	cout<<"fork failed"<<endl;
			}

			else if (comchild == 0){
				string commandString;
				if (commands[i].inpt == ""){
					if (commands[i].option == ""){
						commandString = commands[i].command;
					}
					else{
						commandString = commands[i].command + " " + commands[i].option;
					}
				}
				else{
					if (commands[i].option==""){
						commandString = commands[i].command + " " + commands[i].inpt;
					}
					else{
						commandString = commands[i].command + " " + commands[i].inpt + " " + commands[i].option;
					}
				}
				char *argumentarr[commandString.length()+1];
				stringstream ss(commandString);
    				string word;
				int j=0;
				while (ss >> word && j < commandString.length()) {
        				argumentarr[j] = new char[word.length() + 1];
    					strcpy(argumentarr[j], word.c_str());
    					j++;
				}
				argumentarr[j] = NULL;
				
				execvp(argumentarr[0],argumentarr);		
			}
		
			else{
				waitpid(comchild,NULL,0);
				if (i==(commands.size()-1)){
                        
                        		for (int k=0;k<backthreads.size();k++){
                                		backthreads[k].join();
                        		}
                        		backthreads.clear();

                        		for (int m=0;m<backprocess.size();m++){
                                		waitpid(backprocess[m],NULL,0);
                        		}
                        		backprocess.clear();

                			}

			}
		
		}
		else if(commands[i].redirection!="-"){
			if (commands[i].redirection==">"){
				int comchild = fork();
				if (comchild < 0){
                                	cout<<"fork failed"<<endl;
                        	}

                        	else if (comchild == 0){
                                	close(STDOUT_FILENO);
					open(commands[i].textword.c_str(),O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU);
											
					string commandString;
                                	if (commands[i].inpt == ""){
                                        	if (commands[i].option == ""){
                                                	commandString = commands[i].command;
                                       	 	}
                                       	 	else{
                                               	 	commandString = commands[i].command + " " + commands[i].option;
                                        	}
                                	}
                                	else{
                                        	if (commands[i].option==""){
                                               	 	commandString = commands[i].command + " " + commands[i].inpt;
                                        	}
                                        	else{
                                                	commandString = commands[i].command + " " + commands[i].inpt + " " + commands[i].option;
                                        	}
                                	}
                                	char *argumentarr[commandString.length()+1];
                                	stringstream ss(commandString);
                                	string word;
                                	int j=0;
                                	while (ss >> word && j < commandString.length()) {
                                        	argumentarr[j] = new char[word.length() + 1];
                                        	strcpy(argumentarr[j], word.c_str());
                                        	j++;
                                	}
                                	argumentarr[j] = NULL;	
					execvp(argumentarr[0],argumentarr);
                        	}

                        	else{
					if (commands[i].background=="y"){
						backprocess.push_back(comchild);
					}
					else{
						waitpid(comchild,NULL,0);
					}
					if (i==(commands.size()-1)){
                        
                        			for (int k=0;k<backthreads.size();k++){
                                			backthreads[k].join();
                        			}
                        			backthreads.clear();

                        			for (int m=0;m<backprocess.size();m++){
                                			waitpid(backprocess[m],NULL,0);
                        			}
                        			backprocess.clear();

                			}
                        	}
			}
			else{ //redirection is: <
				int* fd = new int[2];
				pipe(fd);
				int comchild = fork();
                                if (comchild < 0){
                                        cout<<"fork failed"<<endl;
                                }
					
                                else if (comchild == 0){
                                	close(STDIN_FILENO);
                                        open(commands[i].textword.c_str(), O_RDONLY);
					dup2(fd[1],STDOUT_FILENO);
					close(fd[0]);
					close(fd[1]);

					string commandString;
                                        if (commands[i].inpt == ""){
                                                if (commands[i].option == ""){
                                                        commandString = commands[i].command;
                                                }
                                                else{
                                                        commandString = commands[i].command + " " + commands[i].option;
                                                }
                                        }
                                        else{
                                                if (commands[i].option==""){
                                                        commandString = commands[i].command + " " + commands[i].inpt;
                                                }
                                                else{
                                                        commandString = commands[i].command + " " + commands[i].inpt + " " +commands[i].option;
                                                }
                                        }
					
                                        char *argumentarr[commandString.length()+1];
                                        stringstream ss(commandString);
                                        string word;
                                        int j=0;
                                        while (ss >> word && j < commandString.length()) {
                                                argumentarr[j] = new char[word.length() + 1];
                                                strcpy(argumentarr[j], word.c_str());
                                                j++;
                                        }
                                        argumentarr[j] = NULL;

                                        execvp(argumentarr[0],argumentarr);                        
                                }

                                else{
					if (commands[i].background=="y"){
						close(fd[1]);
                                                backprocess.push_back(comchild);
                                        	backthreads.push_back(thread(printThread, fd));
						
					}
                                        else{
						close(fd[1]);
                                                waitpid(comchild,NULL,0);
						backthreads.push_back(thread(printThread, fd));
                                        	
					}

					if (i==(commands.size()-1)){
                        	                for (int k=0;k<backthreads.size();k++){
                                			backthreads[k].join();
                        			}
                        			backthreads.clear();

                        			for (int m=0;m<backprocess.size();m++){
                                			waitpid(backprocess[m],NULL,0);
                        			}
                        			backprocess.clear();

                			}

                                }

			}

		}

		else{ //all other commands 
		   int* fd = new int[2];
		   pipe(fd);
		   int comchild = fork();
		   if (comchild < 0){
		   	cout<<"fork failed"<<endl;
		   }
		   else if (comchild == 0){
        		dup2(fd[1], STDOUT_FILENO);
			close(fd[0]);
			close(fd[1]);
			string commandString;
                        if (commands[i].inpt == ""){
                        	if (commands[i].option == ""){
                                	commandString = commands[i].command;
                                }
                                else{
                                	commandString = commands[i].command + " " + commands[i].option;
                               	}
                        }
                        else{
                        	if (commands[i].option==""){
                                	commandString = commands[i].command + " " + commands[i].inpt;
                                }
                                else{
                                	commandString = commands[i].command + " " + commands[i].inpt + " " + commands[i].option;
                                }
                        }
                        char *argumentarr[commandString.length()+1];
                        stringstream ss(commandString);
                        string word;
                        int j=0;
                        while (ss >> word && j < commandString.length()) {
                        	argumentarr[j] = new char[word.length() + 1];
                                strcpy(argumentarr[j], word.c_str());
                                j++;
                        }
                        argumentarr[j] = NULL;

                        execvp(argumentarr[0],argumentarr);
		   }

		   else{
			if (commands[i].background=="y"){
				close(fd[1]);
				backprocess.push_back(comchild);
                        	backthreads.push_back(thread(printThread, fd));
                        	
			}
                        else{
				close(fd[1]);
				waitpid(comchild,NULL,0);
				backthreads.push_back(thread(printThread, fd));
                        	
			}

			if (i==(commands.size()-1)){

	                        for (int k=0;k<backthreads.size();k++){
        	                        backthreads[k].join();
                	        }
                        	backthreads.clear();

                        	for (int m=0;m<backprocess.size();m++){
                                	waitpid(backprocess[m],NULL,0);
                        	}
                        	backprocess.clear();

                	}
			
		   }
		     	
		}
	
		
	}
	
	output.close();

	return 0;
}
