## (Insecure) Guessing Game

#### Game: Guess a random number. Your score is determined by the number of times your were consecutively within 35 steps of the random numbers generated.

Vulnerabilities:
- Storing sensitive info in data section of executable
- Shell injection/privilege escalation (due to poor mediation)
- Integer overflow to get past whitelist check
